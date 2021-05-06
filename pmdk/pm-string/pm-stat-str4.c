#include <stdio.h>
#include <string.h>
#include <libpmemobj.h>                                                              

/*
 Create the pool for this app with:
 pmempool create obj --layout=strstatic <pool-name>
*/

#define LAYOUT_NAME "strstatic"
#define MAX_BUF_LEN 20

POBJ_LAYOUT_BEGIN(string_static);
POBJ_LAYOUT_ROOT(string_static, struct my_root);
POBJ_LAYOUT_END(string_static);

struct my_root {
  char buf[MAX_BUF_LEN];
};


  
int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("usage: %s file-name\n", argv[0]);
    return 1;
  }

  PMEMobjpool *pop = pmemobj_open(argv[1], LAYOUT_NAME);
  if (pop == NULL) {
    perror("pmemobj_open");
    return 1;
  }

  TOID(struct my_root) root = POBJ_ROOT(pop, struct my_root);

	printf("Last string: %s\n", D_RO(root)->buf);

  char buf[MAX_BUF_LEN] = {0};
  if (scanf("%99[0-9a-zA-Z ]", buf) == EOF) {
    fprintf(stderr, "EOF\n");
    return 1;
  }

  TX_BEGIN(pop) {
  
    TX_MEMCPY(D_RW(root)->buf, buf, strlen(buf)+1);

  } TX_END


  pmemobj_close(pop);

  return 0;
}

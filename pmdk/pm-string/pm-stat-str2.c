#include <stdio.h>
#include <string.h>
#include <libpmemobj.h>                                                              

/*
 Create the pool for this app with:
 pmempool create obj --layout=strstatic <pool-name>
*/

#define LAYOUT_NAME "strstatic"
#define MAX_BUF_LEN 20

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

  PMEMoid root = pmemobj_root(pop, sizeof(struct my_root));
  struct my_root *rootp = pmemobj_direct(root);

	printf("Last string: %s\n", rootp->buf);

  char buf[MAX_BUF_LEN] = {0};
  if (scanf("%99[0-9a-zA-Z ]", buf) == EOF) {
    fprintf(stderr, "EOF\n");
    return 1;
  }

  memcpy(rootp->buf, buf, strlen(buf)+1);
  pmemobj_persist(pop, rootp->buf, strlen(buf)+1);


  pmemobj_close(pop);

  return 0;
}

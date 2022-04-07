
#
#

LDFLAGS = -lpmemobj

BINS = linkedlist pm-linkedlist pm-linkedlist-crash pm-linkedlist-crash-logging pm-linkedlist-typed

LINKER=$(CC)

CFLAGS=-std=gnu99

.PHONY:	all clean


all:	$(BINS)

linkedlist.o: linkedlist.c
	$(CC) $(CFLAGS) -c -o $@ $<

pm-linkedlist.o: pm-linkedlist.c
	$(CC) $(CFLAGS) -c -o $@ $<

pm-linkedlist-crash.o: pm-linkedlist.c
	$(CC) $(CFLAGS) -DSIMULATE_CRASH -DNO_LOGGING -c -o $@ $<

pm-linkedlist-crash-logging.o: pm-linkedlist.c
	$(CC) $(CFLAGS) -DSIMULATE_CRASH -c -o $@ $<

pm-linkedlist-typed.o: pm-linkedlist-typed.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINS):	%:	%.o 
	$(LINKER) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(BINS) *.o




CC=gcc
TARGETS = ipmask

all: $(TARGETS)

ipmask: ipmask.o
	$(CC) $^ -o $@

ipmask.o: ipmask.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f ipmask.o
	rm -f $(TARGETS)

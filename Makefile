CFLAGS=-g -Wall
CC=gcc
SRCS=terminal.c tokenizer.c command-block.c linked-list.c support-functions.c process-map.c piping.c handle-job.c
DEPS=tokenizer.h command-block.h linked-list.h process-map.h support-functions.h piping.h handle-job.h
TERM_OBJS=terminal.o tokenizer.o linked-list.o command-block.o process-map.o load-program.o support-functions.o piping.o handle-job.o
LDFLAGS=
LIBS=

all: terminal

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

terminal: $(TERM_OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $(TERM_OBJS)

clean:
	rm -f *.o


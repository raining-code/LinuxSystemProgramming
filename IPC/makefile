CC = gcc
CFLAGS= -std=c99
SRC = $(wildcard *.c)
TARGETS = $(SRC:.c=)
all: $(TARGETS)
%: %.c
	$(CC) $^ -o $@ $(CFLAGS)
.PHONY:clean
clean:
	rm -f $(TARGETS)
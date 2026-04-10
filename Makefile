CC = gcc
CFLAGS = -Wall -D_POSIX_C_SOURCE=200809L

all: myprogram

myprogram: main.c
	$(CC) $(CFLAGS) -o myprogram main.c

clean:
	rm -f myprogram A B C D A.gz B.gz result.txt

.PHONY: all clean

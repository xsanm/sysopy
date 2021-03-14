CC = gcc 
CFLAGS = -Wall -Wextra -O0 -std=c11 -g

static:
	$(CC) $(CFLAGS) -c lib.c -o lib.o
	ar rcs lib.a lib.o

shared:
	$(CC) $(CFLAGS) -fPIC -shared lib.c -o lib_shared.so


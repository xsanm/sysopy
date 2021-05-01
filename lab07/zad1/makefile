CC=gcc -Wall

build:
	$(CC) -o cook cook.c shared.c
	$(CC) -o supplier supplier.c shared.c
	$(CC) -o main main.c shared.c

test:
	./main 5 5

clean:
	rm -f main cook supplier

all: build test clean

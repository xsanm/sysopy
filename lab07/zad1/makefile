CC=gcc -Wall

build:
	touch bake
	touch table
	$(CC) -o cook cook.c shared.c
	$(CC) -o supplier supplier.c shared.c
	$(CC) -o main main.c shared.c

test:
	./main 5 5

clean:
	rm -f main cook supplier
	rm -f bake table

all: build test clean

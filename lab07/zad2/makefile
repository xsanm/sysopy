CC=gcc -Wall

build:
	$(CC) -o cook cook.c shared.c -lrt -lpthread
	$(CC) -o supplier supplier.c shared.c -lrt -lpthread
	$(CC) -o main main.c shared.c -lrt -lpthread

test:
	./main 5 5

clean:
	rm -f bake table

all: build test clean

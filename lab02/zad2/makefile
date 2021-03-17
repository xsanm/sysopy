CC = gcc 
CFLAGS = -Wall -Wextra -O0 -std=c11 -g

compile:
	$(CC) $(CFLAGS) main.c -o prog

clean:
	rm -f prog


small: compile
	./prog A small.txt
	make clean

medium: compile
	./prog A medium.txt
	make clean

big: compile
	./prog A big.txt
	make clean
CC = gcc 
CFLAGS = -Wall -Wextra -O0 -std=c11 -g

compile:
	$(CC) $(CFLAGS) main.c -o prog

clean:
	rm -f prog, a.txt, b.txt, c.txt

run: compile
	./prog

small: compile
	./prog small.txt
	make clean

medium: compile
	./prog medium.txt
	make clean

big: compile
	./prog big.txt
	make clean
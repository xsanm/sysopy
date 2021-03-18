CC = gcc 
CFLAGS = -Wall -Wextra -O0 -std=c11 -g

compile:
	$(CC) $(CFLAGS) main.c -o prog

clean:
	rm -f prog, a_lib.txt, b_lib.txt, c_lib.txt, a_sys.txt, b_sys.txt, c_sys.txt

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
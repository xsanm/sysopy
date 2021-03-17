CC = gcc 
CFLAGS = -Wall -Wextra -O0 -std=c11 -g

compile:
	$(CC) $(CFLAGS) main.c -o prog

clean:
	rm -f prog


small: compile
	./prog small1.txt small2.txt
	make clean

medium: compile
	./prog medium1.txt medium2.txt
	make clean

big: compile
	./prog big1.txt big2.txt
	make clean
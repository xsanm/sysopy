CC = gcc 
CFLAGS = -Wall -Wextra -O0 -std=c11 -g

static:
	$(CC) $(CFLAGS) -c lib.c -o lib.o
	ar rcs lib.a lib.o

shared:
	$(CC) $(CFLAGS) -fPIC -shared lib.c -o lib_shared.so

clean:
	rm -f *.o *test* *.a *.so

zad2_display: clean static
	$(CC) $(CFLAGS) -c main.c -o prog.o
	$(CC) $(CFLAGS) -static prog.o -L. lib.a -o prog
	make display
	make clean

display: 
	./prog create_table 1 merge_files a.txt:b.txt remove_row 0 2 remove_row 0 4
	./prog create_table 2 merge_files f11.txt:f12.txt f21.txt:f22.txt remove_row 0 2 remove_block 1 remove_row 0 3 remove_row 0 4


zad2_tests: clean static
	$(CC) $(CFLAGS) -c main.c -o test.o
	$(CC) $(CFLAGS) -static test.o -L. lib.a -o test
	./test tests > raport2.txt && cat raport2.txt
	make clean


CC = gcc -Wall 


static:
	$(CC) -c lib.c
	ar rcs lib.a lib.o

shared:
	$(CC) -c -fPIC lib.c
	$(CC) -shared -fPIC -o lib.so lib.o

clean:
	rm -f *.o


zad2:
	make clean
	make static
	$(CC) -o test main.c lib.a -I .
	make tests
	make clean

tests:
	./test create_table 1 merge_files a.txt:b.txt remove_row 0 2
	#./test create_table 2 merge_files f11.txt:f12.txt f21.txt:f22.txt remove_row 0 2 remove_block 1 remove_row 0 3 remove_row 0 4
	#./test create_table 3 merge_files f11.txt:f12.txt f21.txt:f22.txt f31.txt:f32.txt

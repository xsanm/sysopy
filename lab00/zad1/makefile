CC = gcc 
CFLAGS = -Wall -O0 -std=c11


static:
	$(CC) $(CFLAGS) -c lib.c -o lib.o
	ar rcs lib.a lib.o

shared:
	$(CC) $(CFLAGS) -c -fPIC lib.c -o lib.o
	$(CC) $(CFLAGS) -shared -fPIC -o lib.so lib.o


clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *test*
	rm -f *merged*


zad2_static:
	make clean
	make static
	$(CC) $(CFLAGS) -c main.c -o test.o
	$(CC) $(CFLAGS) -static test.o lib.a -o test
	#$(CC) $(CFLAGS) -static -llib -L./ -o test main.c
	make tests
	make clean

zad2_shared: 
	make clean
	make shared
	$(CC) $(CFLAGS)  -o test main.c -L. lib.so -Wl,-rpath=.
	make tests
	make clean

tests:
	./test create_table 1 merge_files a.txt:b.txt remove_row 0 2
	#./test create_table 2 merge_files f11.txt:f12.txt f21.txt:f22.txt remove_row 0 2 remove_block 1 remove_row 0 3 remove_row 0 4
	#./test create_table 3 merge_files f11.txt:f12.txt f21.txt:f22.txt f31.txt:f32.txt
	./test tests
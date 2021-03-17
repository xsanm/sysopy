CC = gcc 
CFLAGS = -Wall -Wextra -O0 -std=c11

raport_file_a = results3a.txt
raport_file_b = results3b.txt

static:
	$(CC) $(CFLAGS) -c lib.c -o lib.o
	ar rcs lib.a lib.o

shared:
	$(CC) $(CFLAGS) -fPIC -shared lib.c -o lib_shared.so


clean:
	rm -f *.o *test* *.a *.so


zad3_display: clean static
	$(CC) $(CFLAGS) -c main.c -o prog.o
	$(CC) $(CFLAGS) -static prog.o -L. lib.a -o prog
	make display
	make clean

zad3_static: clean static
	$(CC) $(CFLAGS) -c main.c -o test.o
	$(CC) $(CFLAGS) -static test.o -L. lib.a -o test
	./test tests 
	make clean

zad3_shared: clean shared
	$(CC) $(CFLAGS)  main.c lib_shared.so -L. -Wl,-rpath=. -o test
	./test tests
	make clean

zad3_dynamic: clean shared
	$(CC) $(CFLAGS) -o test main_dynamic.c -ldl
	./test tests
	make clean

display: 
	./prog create_table 1 merge_files a.txt:b.txt remove_row 0 2 remove_row 0 4
	./prog create_table 2 merge_files f11.txt:f12.txt f21.txt:f22.txt remove_row 0 2 remove_block 1 remove_row 0 3 remove_row 0 4

zad3a: clean
	@echo "\nSTATIC \n" > $(raport_file_a)
	make static
	$(CC) $(CFLAGS) -c main.c -o test.o
	$(CC) $(CFLAGS) -static test.o -L. lib.a -o test
	./test tests >> $(raport_file_a)
	make clean
	
	@echo "\nSHARED \n" >> $(raport_file_a)
	make shared
	$(CC) $(CFLAGS) -o test main.c lib_shared.so -L. -Wl,-rpath=.
	./test tests >> $(raport_file_a)
	make clean

	@echo "\nDYNAMIC \n" >> $(raport_file_a)
	make shared
	$(CC) $(CFLAGS) -o test main_dynamic.c -ldl
	./test tests >> $(raport_file_a)
	make clean


zad3b: clean

	@echo "\n01 \n" > $(raport_file_b)

	@echo "\nSTATIC \n" >> $(raport_file_b)
	make static
	$(CC) -Wall -O1 -std=c11 -c main.c -o test.o
	$(CC) -Wall -O1 -std=c11 -static test.o -L. lib.a -o test
	./test tests >> $(raport_file_b)
	make clean
	
	@echo "\nSHARED \n" >> $(raport_file_b)
	make shared
	$(CC) -Wall -O1 -std=c11 -o test main.c lib_shared.so -L. -Wl,-rpath=.
	./test tests >> $(raport_file_b)
	make clean

	@echo "\nDYNAMIC \n" >> $(raport_file_b)
	make shared
	$(CC) -Wall -O1 -std=c11 -o test main_dynamic.c -ldl
	./test tests >> $(raport_file_b)
	make clean

	@echo "\n02 \n" >> $(raport_file_b)

	@echo "\nSTATIC \n" >> $(raport_file_b)
	make static
	$(CC) -Wall -O2 -std=c11 -c main.c -o test.o
	$(CC) -Wall -O2 -std=c11 -static test.o -L. lib.a -o test
	./test tests >> $(raport_file_b)
	make clean
	
	@echo "\nSHARED \n" >> $(raport_file_b)
	make shared
	$(CC) -Wall -O2 -std=c11 -o test main.c lib_shared.so -L. -Wl,-rpath=.
	./test tests >> $(raport_file_b)
	make clean

	@echo "\nDYNAMIC \n" >> $(raport_file_b)
	make shared
	$(CC) -Wall -O2 -std=c11 -o test main_dynamic.c -ldl
	./test tests >> $(raport_file_b)
	make clean

	@echo "\n0s \n" >> $(raport_file_b)

	@echo "\nSTATIC \n" >> $(raport_file_b)
	make static
	$(CC) -Wall -Os -std=c11 -c main.c -o test.o
	$(CC) -Wall -Os -std=c11 -static test.o -L. lib.a -o test
	./test tests >> $(raport_file_b)
	make clean
	
	@echo "\nSHARED \n" >> $(raport_file_b)
	make shared
	$(CC) -Wall -Os -std=c11 -o test main.c lib_shared.so -L. -Wl,-rpath=.
	./test tests >> $(raport_file_b)
	make clean

	@echo "\nDYNAMIC \n" >> $(raport_file_b)
	make shared
	$(CC) -Wall -Os -std=c11 -o test main_dynamic.c -ldl
	./test tests >> $(raport_file_b)
	make clean



run:
	rm -f fifo_1
	mkfifo fifo_1
	gcc -std=c11 -Wall -ggdb3 producer.c -o producer
	gcc -std=c11 -Wall -ggdb3 consumer.c -o consumer
	./producer fifo_1 4 a.txt 5
	#./consumer fifo_1 b.txt 5

test1:
	rm -f fifo_1
	rm -f b1.txt
	touch b1.txt
	rm -f b2.txt
	touch b2.txt
	rm -f b3.txt
	touch b3.txt
	mkfifo fifo_1
	gcc -std=c11 -Wall -ggdb3 producer.c -o producer
	gcc -std=c11 -Wall -ggdb3 consumer.c -o consumer
	gcc -std=c11 -Wall -ggdb3 main.c -o test
	./test CASE1
	./test CASE2
	./test CASE3

run:
	rm -f fifo_1
	mkfifo fifo_1
	gcc -std=c11 -Wall -ggdb3 producer.c -o producer
	gcc -std=c11 -Wall -ggdb3 consumer.c -o consumer
	./producer fifo_1 4 a.txt 5
	#./consumer fifo_1 b.txt 5

test1:
	rm -f fifo_1
	mkfifo fifo_1
	gcc -std=c11 -Wall -ggdb3 producer.c -o producer
	gcc -std=c11 -Wall -ggdb3 consumer.c -o consumer
	gcc -std=c11 -Wall -ggdb3 main.c -o test
	./test

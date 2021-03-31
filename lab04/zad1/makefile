
run_fork_test:
	gcc testing_fork.c
	./a.out handler
	./a.out ignore
	./a.out mask
	./a.out pending

run_exec_test:
	gcc to_exec.c -o to_exec
	gcc testing_exec.c
	./a.out ignore
	./a.out mask
	./a.out pending
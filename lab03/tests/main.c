#include <stdio.h>
#include <sys/types.h>

int main() {
    pid_t child_pid;
    child_pid = fork();
    //printf("Ten napis zostal wyswietlony w programie 'main'!\n");
    if (child_pid != 0) {
        printf("Ten napis zostal wyswietlony w programie 'main'!\n");
    }
    else {
        execvp("./child", NULL);
    }

    return 0;
}
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

char BASE_PATH[512] = "";
int MAX_DEPTH = 0;
static int PROCESS_CNT = 0;
const int BUFF_SIZE = 1024;

int check_if_exist(char *path, char *pattern) {
    //return 1;

    FILE *f = fopen(path, "r");
    //printf("%s\n", path);
    if(f == NULL) {
        return -1;
    }

    char buff[BUFF_SIZE];
    //printf("%d\n", fread(buff, sizeof (char), BUFF_SIZE, f));
    while (fread(buff, sizeof (char), BUFF_SIZE, f) > 0) {
        //printf("%s", buff);
        if(strstr(buff, pattern) != NULL) return 1;
        fseek(f, -strlen(pattern), SEEK_CUR);
    }

    return 0;
}

void check_dir(char *path, char *pattern, int depth) {

    //printf("PATH %s\t PID %d    PPID %d\n", path, getpid(), getppid());

    DIR* cdir = opendir(path);

    if(cdir == NULL) {
        //show errno error
        exit(0);
    }

    struct dirent* d = readdir(cdir);
    while(d != NULL) {

        if(depth < MAX_DEPTH && DT_DIR == d->d_type && strcmp(".", d->d_name) && strcmp("..", d->d_name)) {
            //printf("Directory: %s\n", d->d_name);
            if(fork() == 0) {
                PROCESS_CNT++;
                //printf("Im child: %d, my parent is: %d AND now what? \n ", getpid(), getppid());
                char path2[512] = "";
                strcat(path2, path);
                strcat(path2, "/");
                strcat(path2, d->d_name);
                //printf("%s\n", path);
                closedir(cdir);
                check_dir(path2, pattern, depth + 1);

                exit(0);
            }
            //jeszcze musi byc kropka!
        } else if(DT_REG == d->d_type && strlen(d->d_name) >= 3 && strcmp("txt", d->d_name + strlen(d->d_name) - 3) == 0) {
            //printf("File (to search): %s \n", d->d_name);

            char file_path[512] = "";
            strcat(file_path, path);
            strcat(file_path, "/");
            strcat(file_path, d->d_name);

            if(check_if_exist(file_path, pattern) > 0) {
                printf("%s\tPID %d\t PPID %d\n", file_path + strlen(BASE_PATH) + 1, getpid(), getppid());
            }
            //printf("Im process: %d, my parent is: %d\n", getpid(), getppid());
        }
        d = readdir(cdir);
    }

    closedir(cdir);
}


int main(int argc, char **argv) {
    if(argc != 4) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 0;
    }
    strcat(BASE_PATH, argv[1]);
    MAX_DEPTH = strtol(argv[3], NULL, 10);

    check_dir(BASE_PATH, argv[2], 0);


    for(int i = 0; i < PROCESS_CNT; i++) {
        wait(NULL);
    }

    return 0;
}




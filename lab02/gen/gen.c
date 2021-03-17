#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void generate_random_line(int a, int b) {
    const int SIZE = (rand() % b) + a;
    char *base = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz0123456789";

    for (int i = 0; i < SIZE; i++) {
        putchar(base[rand() % strlen(base)]);
    }
    puts("");



}


int main() {
    srand(time(NULL));

    for(int i = 0; i < 5000; i++) generate_random_line(200, 255);

    return 0;
}
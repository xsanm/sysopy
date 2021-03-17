#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main() {
    for(int i = 0; i < 300; i++) putchar('a');
    puts("");
    for(int i = 0; i < 256; i++) putchar('b');
    puts("");
    for(int i = 0; i < 255; i++) putchar('c');
    puts("");

    return 0;
}
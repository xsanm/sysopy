#include "shared.h"

int local_socket;
int network_socket;


void local_socket_init(char *path) {
    if ((local_socket = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0) {
        perror("socket failed");
        exit(1);
    }

    struct sockaddr_un sa;
    sa.sun_family = AF_UNIX;
    strcpy(sa.sun_path, path);
    unlink(path);
    if (bind(local_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("bind failed");
        exit(1);
    }
    if (listen(local_socket, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(1);
    }
}

void network_socket_init(char *path) {
    int port = atoi(path);
    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        perror("socket failed");
        exit(1);
    }

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind(network_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("bind failed");
        exit(1);
    }
    if (listen(network_socket, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(1);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    network_socket_init(argv[1]);
    local_socket_init(argv[2]);

    printf("SERVER START\n");

    sleep(100);

    return 0;
}
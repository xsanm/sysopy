#include "shared.h"

char *my_name;
int server_socket;

void connect_to_server(char *type, char *address) {
    if(strcmp(type, "local") == 0) {
        //connect local socket

        if ((server_socket = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0) {
            perror("socket failed");
            exit(1);
        }
        struct sockaddr_un sa;
        sa.sun_family = AF_UNIX;
        strcpy(sa.sun_path, address);
        if (connect(server_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            perror("connect local failed");
            exit(1);
        }

    } else if(strcmp(type, "network") == 0) {
        //connect to network socket

        int port = atoi(address);
        if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
            perror("socket failed");
            exit(1);
        }
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        //sa.sin_addr.s_addr = INADDR_ANY;
        if (connect(server_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            perror("connect network failed");
            exit(1);
        }

    } else {
        exit(1);
    }
}

void register_in_server() {
    char buff[MSG_LEN];
    sprintf(buff, "%s", my_name);
    send(server_socket, buff, MSG_LEN, 0);
}

void server_listen() {
    char buff[MSG_LEN];
    recv(server_socket, buff, MSG_LEN, 0);
    printf("%s\n", buff);
//    while(1 == 1) {
//        recv(server_socket, buff, MSG_LEN, 0);
//        printf("%s\n", buff);
//    }
}

int main(int argc, char **argv) {
    if(argc != 4) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    my_name = argv[1];

    printf("CLIENT START\n");

    connect_to_server(argv[2], argv[3]);

    printf("CLIENT CONNECTED\n");

    register_in_server();

    server_listen();

    return 0;
}
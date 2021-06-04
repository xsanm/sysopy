#include "shared.h"

struct Client {
    char *name;
    int socket;
};


struct Client clients[MAX_CLIENTS];
int clients_no = 0;
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

int monitoring_clients() {
    struct pollfd *sockets = malloc((clients_no + 2) * sizeof(struct pollfd));
    sockets[0].fd = network_socket;
    sockets[1].fd = local_socket;
    sockets[0].events = POLLIN;
    sockets[1].events = POLLIN;
    for(int i = 2; i < clients_no + 2; i++) {
        sockets[i].fd = clients[i - 2].socket;
        sockets[i].events = POLLIN;
    }

    poll(sockets, 2 + clients_no, -1); //wait inf for clients

    for (int i = 0; i < clients_no + 2; i++) {
        if (sockets[i].revents == POLLIN) {
            int socket = sockets[i].fd;
            if (socket == local_socket || socket == network_socket) {
                return accept(socket, NULL, NULL);
            }
            return socket;
        }
    }

    free(sockets);
    return -1;
}

void clients_listen() {
    while (1 == 1) {
        int socket = monitoring_clients();
        char msg[MSG_LEN];
        recv(socket, msg, MSG_LEN, 0);
        printf("%s\n", msg);

        int is_new = 1;
        for (int i = 0; i < clients_no; i++) {
            if(clients[i].socket == socket) {
                is_new = 0;
            }
        }

        if(is_new == 1) {
            clients[clients_no].socket = socket;
            clients[clients_no].name = msg;
            clients_no++;
        }

        char buff[MSG_LEN];
        sprintf(buff, "elo kurwa %s\n", msg);
        send(socket, buff, MSG_LEN, 0);
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


    clients_listen();

//    int client_fd = accept(local_socket, NULL,NULL);
//    printf("%d\n", client_fd);
//    int client_fd = accept(network_socket, NULL,NULL);
//    printf("%d\n", client_fd);
//
//    char buff[MSG_LEN];
//    recv(client_fd, buff, MSG_LEN, 0);
//    printf("%s\n", buff);

    //sleep(100);

    return 0;
}
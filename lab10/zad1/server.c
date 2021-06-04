#include "shared.h"

struct Client {
    char *name;
    int socket;
    int is_alive;
    int opponentSocket; //-1 -> no opponent
};


struct Client clients[MAX_CLIENTS];
int clients_no = 0;
int local_socket;
int network_socket;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


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

    pthread_mutex_lock(&mutex);
    for(int i = 2; i < clients_no + 2; i++) {
        sockets[i].fd = clients[i - 2].socket;
        sockets[i].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);
    poll(sockets, 2 + clients_no, -1); //wait inf for clients

    for (int i = 0; i < clients_no + 2; i++) {
        if (sockets[i].revents == POLLIN) {
            int socket = sockets[i].fd;
            //printf("%d\n", socket);
            if (socket == local_socket || socket == network_socket) {

                return accept(socket, NULL, NULL);
            }
            //pthread_mutex_unlock(&mutex);
            return socket;
        }
    }

    free(sockets);
   // pthread_mutex_unlock(&mutex);
    return -1;
}



void remove_dead_clients() {
    for(int i = 0; i < clients_no; i++) {
        if(!clients[i].is_alive) {
            if(i < clients_no - 1) {
                clients[i] = clients[clients_no - 1];
            }
            clients_no--;
        }
    }
}


void *ping_clients() {
    while(1 == 1) {
        sleep(5);
        pthread_mutex_lock(&mutex);
        printf("[PING]\n");
        remove_dead_clients();
        for(int i = 0; i < clients_no; i++) {
            if(!clients[i].is_alive) continue;
            send(clients[i].socket, "P", MSG_LEN, 0);
            clients[i].is_alive = 0; //ping back set to 1
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void clients_listen() {
    pthread_t pt;
    pthread_create(&pt, NULL, ping_clients, NULL);

    while (1 == 1) {
        int socket = monitoring_clients();
        char msg[MSG_LEN];
        if(recv(socket, msg, MSG_LEN, 0) <= 0) {
            //perror("recv ");
            sleep(1);
            continue;
        }
        //printf("%s\n", msg);

        pthread_mutex_lock(&mutex);
        if(strcmp(msg, "P") == 0) {
            for(int i = 0; i < clients_no; i++) {

                if(clients[i].socket == socket) {
                    //printf("Client [%d] alive\n", i);
                    clients[i].is_alive = 1;
                }
            }
        } else {
            int is_new = 1;
            for (int i = 0; i < clients_no; i++) {
                if(clients[i].socket == socket) {
                    is_new = 0;
                }
            }

            if(is_new == 1) {
                clients[clients_no].socket = socket;
                clients[clients_no].name = msg;
                clients[clients_no].is_alive = 1;
                clients[clients_no].opponentSocket = -1;
                clients_no++;
            }
        }


        pthread_mutex_unlock(&mutex);
        //sleep(1);
        char buff[MSG_LEN];
        //sprintf(buff, "elo kurwa %s\n", msg);
        //send(socket, buff, MSG_LEN, 0);

        //TODO check if client oponent is alive
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    signal(SIGPIPE, SIG_IGN);
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
#include "data.h"

struct client clients[MAX_CLIENTS];
int qid;

void send_message(struct message *msg, mqd_t send_to){
    if(mq_send(send_to, (char *)msg, MSG_BUFFER_SIZE, 0) == -1){
        perror("sending");
    }
}

void init(struct message *msg) {
    int is = -1;
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].IS_CONNECTED == 0) {
            is = i;
            clients[i].IS_CONNECTED = 1;
            clients[i].id = i;
            ///clients[i].queue_id = msg->qid;
            strcpy(clients[i].queue_name, msg->buff);
            if ((clients[i].queue_id = mq_open(msg->buff, O_WRONLY)) == -1){
                printf("Cant open client queue \n");
            }
            break;
        }
    }
    if(is == -1) {
        printf("NO MORE SLOTS FOR CLIENTS");
        return;
    }

    printf("SERVER: Init client with id: %d\n", is);
    struct message response;
    response.client_id = is;
    response.message_type = INIT;
    response.sender_pid = getpid();


    send_message(&response, clients[is].queue_id);
}


void list(struct message *msg) {
    puts("LIST QUERY");
    struct message response;
    response.buff[0] ='\0';
    //printf("%d\n", strlen(mtext.buff));
    strcat(response.buff, "LIST OF CLIENTS\n");
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].IS_CONNECTED == 1) {
            strcat(response.buff, "Client id: ");
            char str[10];
            sprintf(str, "%d", i);
            strcat(response.buff, str);
            if(clients[i].IS_BUSY) {
                strcat(response.buff, " NOT AVAILABLE\n");
            } else {
                strcat(response.buff, " AVAILABLE\n\0");
            }
        }
    }
    response.message_type = LIST;
    send_message(&response, clients[msg->client_id].queue_id);
}

void connect(struct message *msg) {
    puts("CONNECT QUERY");
    int id = 0;
    //printf("%s", msg->message_text.buff);
    for (int i = 8; i < strlen(msg->buff) - 1; i++) {
        id *= 10;
        id += (msg->buff[i] - '0');
        //printf("%d\n", msg->message_text.buff[i] - '0');
    }
    printf("ID %d\n", id);
    if(id < 0 || id >= MAX_CLIENTS) {
        puts("WRONG ID");
        return;
    }
    if(clients[id].IS_CONNECTED == 0) {
        puts("Client with given ID not connected");
        return;
    }
    if(clients[id].IS_BUSY == 1) {
        puts("Client with given ID not available");
        return;
    }

    struct message msg1;
    msg1.qid = msg->qid;
    msg1.client_id = msg->client_id;
    msg1.message_type = CONNECT;
    strcpy(msg1.buff, clients[msg->client_id].queue_name);
    send_message(&msg1, clients[id].queue_id);

    struct message msg2;
    msg2.qid = clients[id].queue_id;
    msg2.client_id = id;
    msg2.message_type = CONNECT;
    strcpy(msg2.buff, clients[id].queue_name);
    send_message(&msg2, clients[msg->client_id].queue_id);

    clients[id].IS_BUSY = 1;
    clients[msg->client_id].IS_BUSY = 1;

    clients[id].mate_id = msg->client_id;
    clients[msg->client_id].mate_id = id;

}

void disconnect(struct message *msg) {
    puts("DISCONNECT QUERY");
    int id1 = msg->client_id;
    int id2 = clients[id1].mate_id;

    clients[id1].IS_BUSY = 0;
    clients[id2].IS_BUSY = 0;

    struct message msg1;
    msg1.message_type = DISCONNECT;
    send_message(&msg1, clients[id1].queue_id);

    struct message msg2;
    msg2.message_type = DISCONNECT;
    send_message(&msg2, clients[id2].queue_id);

}


void stop(struct message *msg) {
    puts("STOP QUERY");
    clients[msg->client_id].IS_BUSY = 0;
    clients[msg->client_id].IS_CONNECTED = 0;
    if (mq_close (clients[msg->client_id].queue_id) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }
}


void choose_mode(struct message *msg) {
    switch (msg->message_type) {
        case INIT:
            init(msg);
            break;
        case LIST:
            list(msg);
            break;
        case CONNECT:
            connect(msg);
            break;
            case DISCONNECT:
            disconnect(msg);
            break;
        case STOP:
            stop(msg);
            break;
        default:
            puts("WRONG MESSAGE TYPE");
    }
}

void sigint_handler() {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].IS_CONNECTED) {
            struct message msg;
            msg.message_type = STOP;
            send_message(&msg, clients[i].queue_id);
            if (mq_close (clients[i].queue_id) == -1) {
                perror ("Client: mq_close");
                exit (1);
            }
        }
    }
    if (mq_close (qid) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }

    if (mq_unlink (SERVER_QUEUE_NAME) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
}


int main(int argc, char ** argv) {

    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].IS_CONNECTED = 0;
        clients[i].IS_BUSY = 0;
    }
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MSG_BUFFER_SIZE;
    attr.mq_curmsgs = 0;

    if((qid = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror("msget");
        exit(1);
    }


    signal(SIGINT, sigint_handler);
    puts("SERVER....");

    struct message mess;

    while(1) {
        //recieving
        if(mq_receive (qid, (char *) &mess, MSG_BUFFER_SIZE, NULL) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Message recieved %ld\n", mess.message_type);
        choose_mode(&mess);
    }

}


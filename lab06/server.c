#include "data.h"

struct client clients[MAX_CLIENTS];

void send_message(struct message *msg, int send_to){
    if(msgsnd(send_to, msg, sizeof(struct message) - sizeof (long), 0) == -1){
        puts("ERROR");
    }
}

void init(struct message *msg) {
    int is = -1;
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].IS_CONNECTED == 0) {
            is = i;
            clients[i].IS_CONNECTED = 1;
            clients[i].id = i;
            clients[i].queue_id = msg->message_text.qid;
            break;
        }
    }
    if(is == -1) {
        printf("NO MORE SLOTS FOR CLIENTS");
        return;
    }

    printf("SERVER: Init client with id: %d\n", is);
    struct message_text mtext;
    mtext.client_id = is;
    struct message response;
    response.message_type = INIT;
    response.sender_pid = getpid();
    response.message_text = mtext;
    send_message(&response, clients[is].queue_id);



}

void list(struct message *msg) {
    puts("LIST QUERY");
    struct message_text mtext;
    mtext.buff[0] ='\0';
    //printf("%d\n", strlen(mtext.buff));
    strcat(mtext.buff, "LIST OF CLIENTS\n");
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].IS_CONNECTED == 1) {
            strcat(mtext.buff, "Client id: ");
            char str[10];
            sprintf(str, "%d", i);
            strcat(mtext.buff, str);
            if(clients[i].IS_BUSY) {
                strcat(mtext.buff, " NOT AVAILABLE\n");
            } else {
                strcat(mtext.buff, " AVAILABLE\n\0");
            }
        }
    }
    struct message response;
    response.message_text = mtext;
    response.message_type = LIST;
    //TODFO check id
    send_message(&response, msg->message_text.qid);
}

void connect(struct message *msg) {
    puts("CONNECT QUERY");
    int id = 0;
    //printf("%s", msg->message_text.buff);
    for (int i = 8; i < strlen(msg->message_text.buff) - 1; i++) {
        id *= 10;
        id += (msg->message_text.buff[i] - '0');
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

    struct message_text mtext1;
    mtext1.qid = msg->message_text.qid;
    mtext1.client_id = msg->message_text.client_id;
    struct message msg1;
    msg1.message_type = CONNECT;
    msg1.message_text = mtext1;

    send_message(&msg1, clients[id].queue_id);

    struct message_text mtext2;
    mtext2.qid = clients[id].queue_id;
    mtext2.client_id = id;
    struct message msg2;
    msg2.message_type = CONNECT;
    msg2.message_text = mtext2;

    send_message(&msg2, msg->message_text.qid);

    clients[id].IS_BUSY = 1;
    clients[msg->message_text.client_id].IS_BUSY = 1;

    clients[id].mate_id = msg->message_text.client_id;
    clients[msg->message_text.client_id].mate_id = id;

    //zmienic flagi

}

void disconnect(struct message *msg) {
    puts("DISCONNECT QUERY");
    int id1 = msg->message_text.client_id;
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
    clients[msg->message_text.client_id].IS_BUSY = 0;
    clients[msg->message_text.client_id].IS_CONNECTED = 0;
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

int main(int argc, char ** argv) {


    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].IS_CONNECTED = 0;
        clients[i].IS_BUSY = 0;
    }


    key_t msg_queue_key;
    int qid;



    //server queue
    if((msg_queue_key = ftok(HOME, PROJECT_ID)) == -1) {
        perror("ftok");
        exit(1);
    }
    //printf("key_t: %d\n", msg_queue_key);
    if((qid = msgget(msg_queue_key, IPC_CREAT | Q_PERM)) == -1) {
        perror("msget");
        exit(1);
    }

    //printf("qid: %d\n", qid);

    puts("SERVER....");

    struct message mess;

    while(1) {
        //recieving
        if(msgrcv(qid, &mess, sizeof (struct message), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        puts("Message recieved");
        choose_mode(&mess);

    }


    return 0;
}


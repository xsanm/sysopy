#include "data.h"

int MY_ID;

void send_message(struct message *msg, int send_to){
    if(msgsnd(send_to, msg, sizeof(struct message), 0) == -1){
        puts("ERROR");
    }
}

void init(int server_qid, int client_qid) {
    puts("INIT");
    struct message_text mtext;
    mtext.qid = client_qid;

    struct message msg;
    msg.message_type = INIT;
    msg.sender_pid = getpid();
    msg.message_text = mtext;

    send_message(&msg, server_qid);
}

void list(struct message *msg) {
    puts("Reciving list");
    printf("%s", msg->message_text.buff);
}



void choose_mode(struct message *msg) {
    switch (msg->message_type) {
        case INIT:
            MY_ID = msg->message_text.client_id;
            printf("INITED. My id is: %d\n", MY_ID);
            break;
        case LIST:
            list(msg);
            break;
        default:
            puts("WRONG MESSAGE TYPE");
    }

}


int main(int argc, char ** argv) {
    key_t server_key, client_key;
    int server_qid, client_qid;

    struct message my_message, return_message;

    //client queue
    if((client_key = ftok(HOME, getpid())) == -1) {
        perror("ftok");
        exit(1);
    }
    if((client_qid = msgget(client_key,  IPC_CREAT | IPC_EXCL | Q_PERM)) == -1) {
        perror("client_qid");
        exit(1);
    }

    printf("client_qid: %d\n", client_qid);

    //client queue
    if((server_key = ftok(HOME, PROJECT_ID)) == -1) {
        perror("ftok");
        exit(1);
    }
   // printf("key_t: %d\n", server_key);
    if ((server_qid = msgget (server_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }

    printf("server_qid: %d\n", server_qid);

    my_message.message_type = MESSAGE;
    my_message.message_text.qid = client_qid;

    init(server_qid, client_qid);


    //printf ("Please type a message: ");

    char line[MAX_MESSAGE_LENGTH];

    while (1) {

        if (msgrcv (client_qid, &return_message, sizeof (struct message), 0, 0) == -1) {
            perror ("client: msgrcv #####");
            exit (1);
        }
        choose_mode(&return_message);

        if(fgets (line, MAX_MESSAGE_LENGTH - 1, stdin)) {
            if(strcmp(line, "LIST\n") == 0) {
                struct message_text mtxt;
                mtxt.qid = client_qid;
                mtxt.client_id = MY_ID;
                struct message msg;
                msg.message_text = mtxt;
                msg.message_type = LIST;
                send_message(&msg, server_qid);
            }
            int length = strlen(line);
            if (line[length - 1] == '\n') line[length - 1] = '\0';

            printf("%s\n", line);


        }
        //sending
        /*if (msgsnd (server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
            perror ("sending error");
            exit (1);
        }*/

        //recieving

        //printf ("Message received from server: %s\n\n", return_message.message_text.buff);

        printf ("Please type a message: ");
    }

    // remove message queue
    if (msgctl (client_qid, IPC_RMID, NULL) == -1) {
        perror ("client: msgctl");
        exit (1);
    }
    printf ("Client: bye\n");

    return 0;
}


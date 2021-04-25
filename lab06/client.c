#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define PROJECT_ID 'P'
#define Q_PERM 0660
#define MAX_CLIENTS  116
#define MAX_MESSAGE_LENGTH 256
#define HOME getenv("HOME")

#define STOP 1
#define DISCONNECT 2
#define LIST 3
#define CONNECT 4
#define INIT 5
#define MESSAGE 6

struct message_text {
    int qid;
    char buf [256];
};

struct message {
    long message_type;
    struct  message_text mess_t;
};


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
    my_message.mess_t.qid = client_qid;

    printf ("Please type a message: ");

    while (fgets (my_message.mess_t.buf, 128, stdin)) {
        int length = strlen (my_message.mess_t.buf);
        if (my_message.mess_t.buf [length - 1] == '\n')
            my_message.mess_t.buf [length - 1] = '\0';

        //sending
        if (msgsnd (server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
            perror ("sending error");
            exit (1);
        }

        //recieving
        if (msgrcv (client_qid, &return_message, sizeof (struct message_text), 0, 0) == -1) {
            perror ("client: msgrcv");
            exit (1);
        }
        printf ("Message received from server: %s\n\n", return_message.mess_t.buf);

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


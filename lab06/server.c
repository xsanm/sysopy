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
    key_t msg_queue_key;
    int qid;

    struct message mess;

    //server queue
    if((msg_queue_key = ftok(HOME, PROJECT_ID)) == -1) {
        perror("ftok");
        exit(1);
    }
    printf("key_t: %d\n", msg_queue_key);
    if((qid = msgget(msg_queue_key, IPC_CREAT | Q_PERM)) == -1) {
        perror("msget");
        exit(1);
    }

    printf("qid: %d\n", qid);

    puts("SERVER....");

    while(1) {
        //recieving
        if(msgrcv(qid, &mess, sizeof (struct message), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        puts("Message recieved");

        int length = strlen (mess.mess_t.buf);
        char buf [20];
        sprintf (buf, " %d", length);
        strcat (mess.mess_t.buf, buf);

        int client_qid = mess.mess_t.qid;
        mess.mess_t.qid = qid;

        //sending reply
        if (msgsnd (client_qid, &mess, sizeof (struct message_text), 0) == -1) {
            perror ("msgget");
            exit (1);
        }

        printf ("Server: response sent to client.\n");

    }


    return 0;
}


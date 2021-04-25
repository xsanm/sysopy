#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define PROJECT_ID 'P'
#define Q_PERM 0660
#define MAX_CLIENTS  16
#define MAX_MESSAGE_LENGTH 256
#define HOME getenv("HOME")

#define STOP 1
#define DISCONNECT 2
#define LIST 3
#define CONNECT 4
#define INIT 5
#define MESSAGE 6

struct message_text {
    int qid; //id of quid we get mess from
    int client_id;
    char buff[MAX_MESSAGE_LENGTH];
};

struct message {
    long message_type;
    pid_t sender_pid;
    struct  message_text message_text;
};

struct client {
    int id;
    int queue_id;
    int IS_CONNECTED;
    int IS_BUSY; //1 - not available
};
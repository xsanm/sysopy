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
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

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

#define MSG_BUFFER_SIZE sizeof (struct message)
#define SERVER_QUEUE_NAME   "/sp-example-server5"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
//#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10



struct message {
    long message_type;
    pid_t sender_pid;
    int qid; //id of quid we get mess from
    int client_id;
    char buff[MAX_MESSAGE_LENGTH];
};

struct client {
    int id;
    int queue_id;
    int IS_CONNECTED;
    int IS_BUSY; //1 - not available
    int mate_id;
    char queue_name[100];
};
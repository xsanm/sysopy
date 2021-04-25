#include "data.h"

int MY_ID;

int server_qid, client_qid;
int mate_qid;
int MODE = 0;

void send_message(struct message *msg, int send_to){
    if(msgsnd(send_to, msg, sizeof(struct message) - sizeof (long), 0) == -1){
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

void connect(struct message *msg) {
    printf("CONNECTING WITH %d\n", msg->message_text.client_id);
    MODE = 1;
    mate_qid = msg->message_text.qid;
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
        case CONNECT:
            connect(msg);
            break;
        case MESSAGE:
            printf("[%d] %s", msg->message_text.client_id, msg->message_text.buff);
            break;
        default:
            puts("WRONG MESSAGE TYPE");
    }
}

void get_response(union sigval sv) {
    (void)sv;
    struct message return_message;
    while (msgrcv(client_qid, &return_message, sizeof(struct message) - sizeof (long), 0, IPC_NOWAIT) != -1) {
        choose_mode(&return_message);
    }
}


void deamon() {
    timer_t timer;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = get_response;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;
    timer_create(CLOCK_REALTIME, &event, &timer);
    struct timespec ten_ms = {0, 10000000};
    struct itimerspec timer_value = {ten_ms, ten_ms};
    timer_settime(timer, 0, &timer_value, NULL);
}


int main(int argc, char ** argv) {
    key_t server_key, client_key;
    //int server_qid, client_qid;

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


    deamon();


    char line[MAX_MESSAGE_LENGTH];
    while (fgets (line, MAX_MESSAGE_LENGTH - 2, stdin)) {
        if(MODE == 1) {
            struct message_text mtext;
            mtext.qid = client_qid;
            mtext.client_id = MY_ID;
            memcpy(mtext.buff, line, sizeof line);
            struct message msg;
            msg.message_type = MESSAGE;
            msg.message_text = mtext;
            send_message(&msg, mate_qid);
        }
        if (strlen(line) >= 9) {
            char *pch;
            pch = strstr (line,"CONNECT ");
            if (pch != NULL) {
                struct message_text mtext;
                mtext.qid = client_qid;
                mtext.client_id = MY_ID;
                memcpy(mtext.buff, line, sizeof line);
                struct message msg;
                msg.message_type = CONNECT;
                msg.message_text = mtext;

                send_message(&msg, server_qid);
            }
        }

        if (strcmp(line, "LIST\n") == 0) {
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

        //printf("%s\n", line);


    }


        //sending
        /*if (msgsnd (server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
            perror ("sending error");
            exit (1);
        }

        //recieving

        //printf ("Message received from server: %s\n\n", return_message.message_text.buff);

        printf ("Please type a message: ");
    }

    // remove message queue
    if (msgctl (client_qid, IPC_RMID, NULL) == -1) {
        perror ("client: msgctl");
        exit (1);
    }
    printf ("Client: bye\n");*/

    return 0;
}


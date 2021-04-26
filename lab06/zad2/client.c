#include "data.h"

int MY_ID;

int server_qid, client_qid;
int mate_qid;
int MODE = 0;
char client_queue_name [64];

void send_message(struct message *msg, mqd_t send_to){
    if(mq_send(send_to, (char *)msg, MSG_BUFFER_SIZE, 0) == -1){
        perror("sending");
    }
}

void init(int server_qid, int client_qid) {
    puts("INIT");

    struct message msg;
    msg.message_type = INIT;
    msg.sender_pid = getpid();
    msg.qid = client_qid;
    strcpy(msg.buff, client_queue_name);
    //sprintf (client_queue_name, "/sp-example-client-%d", getpid ());

    send_message(&msg, server_qid);
}

void list(struct message *msg) {
    puts("Reciving list");
    printf("%s", msg->buff);
}


void connect(struct message *msg) {
    printf("CONNECTING WITH %d\n", msg->client_id);
    MODE = 1;
    mate_qid = mq_open(msg->buff, O_WRONLY);
}


void disconnect(struct message *msg) {
    printf("DISCONNECTED\n");
    MODE = 0;
    if (mq_close (mate_qid) == -1) {
        perror ("Client: mq_close");
    }
}


void sigint_handler() {
    struct message msg;
    msg.qid = client_qid;
    msg.client_id = MY_ID;
    msg.message_type = STOP;
    send_message(&msg, server_qid);
    if (mq_close (server_qid) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }

    if (mq_close (client_qid) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }

    if (mq_unlink (client_queue_name) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
    exit(0);
}



void choose_mode(struct message *msg) {
    switch (msg->message_type) {
        case INIT:
            MY_ID = msg->client_id;
            printf("INITED. My id is: %d\n", MY_ID);
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
            if (mq_close (server_qid) == -1) {
                perror ("Client: mq_close");
                exit (1);
            }

            if (mq_close (client_qid) == -1) {
                perror ("Client: mq_close");
                exit (1);
            }

            if (mq_unlink (client_queue_name) == -1) {
                perror ("Client: mq_unlink");
                exit (1);
            }
            exit(0);
            exit(0);
            break;
        case MESSAGE:
            printf("[MSG FROM %d] %s", msg->client_id, msg->buff);
            break;
        default:
            ;
            //puts("WRONG MESSAGE TYPE");
    }
}

void get_response(union sigval sv) {
    (void)sv;
    struct message return_message;
    while (mq_receive (client_qid, (char *) &return_message, MSG_BUFFER_SIZE, NULL) != -1) {
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


    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MSG_BUFFER_SIZE;
    attr.mq_curmsgs = 0;

    struct message my_message, return_message;

    sprintf (client_queue_name, "/sp-example-client-%d", getpid ());

    if((client_qid = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror("client_qid");
        exit(1);
    }


    if ((server_qid =  mq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }

    printf("server to %d\n",server_qid);

    signal(SIGINT, sigint_handler);


    init(server_qid, client_qid);

    deamon();

    char line[MAX_MESSAGE_LENGTH];
    while (fgets (line, MAX_MESSAGE_LENGTH - 2, stdin)) {
        if(MODE == 1) {
            if(strcmp(line, "DISCONNECT\n")) {
                struct message msg;
                msg.qid = client_qid;
                msg.client_id = MY_ID;
                memcpy(msg.buff, line, sizeof line);

                msg.message_type = MESSAGE;
                send_message(&msg, mate_qid);
            } else {
                struct message msg;
                msg.client_id = MY_ID;

                msg.message_type = DISCONNECT;

                send_message(&msg, server_qid);
            }
        }
        if (strlen(line) >= 9) {
            char *pch;
            pch = strstr (line,"CONNECT ");
            if (pch != NULL) {
                struct message msg;
                msg.qid = client_qid;
                msg.client_id = MY_ID;
                memcpy(msg.buff, line, sizeof line);

                msg.message_type = CONNECT;


                send_message(&msg, server_qid);
            }
        }

        if (strcmp(line, "LIST\n") == 0) {
            struct message msg;
            msg.qid = client_qid;
            msg.client_id = MY_ID;

            msg.message_type = LIST;
            send_message(&msg, server_qid);
        } else if (strcmp(line, "STOP\n") == 0) {

            struct message msg;
            msg.qid = client_qid;
            msg.client_id = MY_ID;


            msg.message_type = STOP;
            send_message(&msg, server_qid);

            if (mq_close (server_qid) == -1) {
                perror ("Client: mq_close");
                exit (1);
            }

            if (mq_close (client_qid) == -1) {
                perror ("Client: mq_close");
                exit (1);
            }

            if (mq_unlink (client_queue_name) == -1) {
                perror ("Client: mq_unlink");
                exit (1);
            }
            exit(0);
        }

        int length = strlen(line);
        if (line[length - 1] == '\n') line[length - 1] = '\0';

    }

    return 0;
}


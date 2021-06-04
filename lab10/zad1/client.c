#include "shared.h"

char *my_name;
int server_socket;
char m_figure;
char o_figure;
int m_moves[4];
int o_moves[4];
int m_moves_no = 0;
int o_moves_no = 0;

void connect_to_server(char *type, char *address) {
    if (strcmp(type, "local") == 0) {
        //connect local socket

        if ((server_socket = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0) {
            perror("socket failed");
            exit(1);
        }
        struct sockaddr_un sa;
        sa.sun_family = AF_UNIX;
        strcpy(sa.sun_path, address);
        if (connect(server_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            perror("connect local failed");
            exit(1);
        }

    } else if (strcmp(type, "network") == 0) {
        //connect to network socket

        int port = atoi(address);
        if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
            perror("socket failed");
            exit(1);
        }
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        sa.sin_addr.s_addr = INADDR_ANY;
        if (connect(server_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            perror("connect network failed");
            exit(1);
        }

    } else {
        exit(1);
    }
}

void register_in_server() {
    char buff[MSG_LEN];
    sprintf(buff, "%s", my_name);
    send(server_socket, buff, MSG_LEN, 0);
}

void display() {
    char board[3][3];
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) board[i][j] = ' ';
    for (int i = 0; i < m_moves_no; i++) {
        int m = m_moves[i] - 1;
        board[m / 3][m % 3] = m_figure;
    }
    for (int i = 0; i < o_moves_no; i++) {
        int m = o_moves[i] - 1;
        board[m / 3][m % 3] = o_figure;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("|%c", board[i][j]);
        }
        puts("|");
    }
}

void check_game() {
    char winning = ' ';
    char board[3][3];
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) board[i][j] = ' ';
    for (int i = 0; i < m_moves_no; i++) {
        int m = m_moves[i] - 1;
        board[m / 3][m % 3] = m_figure;
    }
    for (int i = 0; i < o_moves_no; i++) {
        int m = o_moves[i] - 1;
        board[m / 3][m % 3] = o_figure;
    }
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][0] == board[i][2]) {
            winning = board[i][0];
        }
        if (board[0][i] == board[1][i] && board[0][i] == board[2][i]) {
            winning = board[0][i];
        }
    }
    //diagonals
    if (board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        winning = board[0][0];
    }
    if (board[0][2] == board[1][1] && board[0][2] == board[2][0]) {
        winning = board[0][2];
    }
    if (winning != ' ') {
        puts("I WON");
        send(server_socket, "L", MSG_LEN, 0);
        exit(0);
    }
    if (m_moves_no + o_moves_no == 9) {
        puts("DRAW");
        send(server_socket, "D", MSG_LEN, 0);
        exit(0);
    }
}

void make_move() {
    printf("Your move [square number]: ");
    int m;
    scanf("%d", &m);

    for (int i = 0; i < m_moves_no; i++) {
        if (m_moves[i] == m) {
            send(server_socket, "W", MSG_LEN, 0);
            puts("WRONG MOVE YOU MUPPET, YOU LOSE");
            exit(0);
        }
    }
    for (int i = 0; i < o_moves_no; i++) {
        if (o_moves[i] == m) {
            send(server_socket, "W", MSG_LEN, 0);
            puts("WRONG MOVE YOU MUPPET, YOU LOSE");
            exit(0);
        }
    }
    m_moves[m_moves_no++] = m;
    display();
    check_game();
    char buff[MSG_LEN];
    sprintf(buff, "%d", m);
    send(server_socket, buff, MSG_LEN, 0);
    puts("###################");
}


void server_listen() {
    char msg[MSG_LEN];

    struct pollfd *sockets = malloc(sizeof(struct pollfd));
    sockets->fd = server_socket;
    sockets->events = POLLIN;

    m_moves_no = 0;
    o_moves_no = 0;

    while (1 == 1) {
        poll(sockets, 1, -1);

        recv(server_socket, msg, MSG_LEN, 0);
        if (strcmp(msg, "P") == 0) {
            //puts("PINGed");
            send(server_socket, "P", MSG_LEN, 0);
            continue;
        } else if (strcmp(msg, "NT") == 0) {
            puts("Name taken");
            exit(0);
        } else if (strcmp(msg, "NO") == 0) {
            m_moves_no = 0;
            o_moves_no = 0;
            puts("NO OPPONENT, waiting ...");
        } else if (strcmp(msg, "X") == 0) {
            m_figure = 'X';
            o_figure = 'O';
            printf("My figure %c\n", m_figure);
        } else if (strcmp(msg, "O") == 0) {
            m_figure = 'O';
            o_figure = 'X';
            printf("My figure %c\n", m_figure);
            display();
            make_move();
        } else if (strlen(msg) == 1 && msg[0] >= '1' && msg[0] <= '9') {
            int move = msg[0] - '0';
            o_moves[o_moves_no++] = move;
            display();
            make_move();
        } else if (strlen(msg) == 1 && msg[0] == 'W') {
            puts("I WON");
            exit(0);
        } else if (strlen(msg) == 1 && msg[0] == 'L') {
            puts("I LOST");
            exit(0);
        } else if (strlen(msg) == 1 && msg[0] == 'D') {
            puts("DRAW");
            exit(0);
        } else {
            printf("%s\n", msg);
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    my_name = argv[1];

    printf("CLIENT START\n");

    connect_to_server(argv[2], argv[3]);

    printf("CLIENT CONNECTED\n");

    register_in_server();

    server_listen();

    return 0;
}
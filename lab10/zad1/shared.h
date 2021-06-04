#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8082
#define MAX_CLIENTS 5
#define MSG_LEN 1024

//message types
//NT - name taken
//NO - no oponnent
//1,2,3,4,5,6,7,8,9 - moves
//O , X - your sign (O always begin)
//Q - quit
//P - ping


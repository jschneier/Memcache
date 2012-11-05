#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>

#define DBSIZE 1024
#define PORT "3303"
#define BACKLOG 20

typedef struct block {
    char *key;
    char *data;
    unsigned long len;
    struct block *next;
} block;

void *thread(void *vargp);
int add_block(int, char *, char *);
int init_socket(int *);

static block *database[DBSIZE];

int main(int argc, char **argv) {

    int sock_fd, *conn_fd;
    pthread_t tid;
    struct sockaddr_storage addr;
    socklen_t clientsize = sizeof addr;

    if (init_socket(&sock_fd) != 0)
        perror("Error returned in init_socket");

    for(;;) {
        conn_fd = malloc(sizeof(int)); //check return code
        *conn_fd = accept(sock_fd, (struct sockaddr *) &addr, &clientsize);
        pthread_create(&tid, NULL, &thread, conn_fd);
    }

    return 0;
}

int add_block(int index, char *key, char* data) {

    block *new = malloc(sizeof(block));
    if (new == NULL)
        return -1;

    new->key = key;
    new->data = data;
    new->next = NULL;

    //first insert at this location
    if (database[index] == NULL)
        database[index] = new;

    //hash collision -> linked list
    else {
        block *current = database[index];
        //advance to the last place
        while (current->next != NULL)
            current = current->next;
        current->next = new;
        }

    return 0;
}

void *thread(void *vargp) {
    pthread_detach(pthread_self());
    int conn_fd = *((int *)vargp);
    free(vargp); //check the return code
    return NULL;
}

int init_socket(int *sock_fd) {

    struct addrinfo hints, *info;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &info) != 0)
        return -1;

    *sock_fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    if (*sock_fd == -1)
        return -1;
    if (bind(*sock_fd, info->ai_addr, info->ai_addrlen) == -1)
        return -1;
    if (listen(*sock_fd, BACKLOG) == -1)
        return -1;
    
    return 0;
}

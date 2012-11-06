#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "memcache.h"

#define DBSIZE 1024
#define PORT "3303"
#define BACKLOG 20
#define BUFSIZE 1024

typedef struct block {
    char *key;
    char *data;
    char *flags;
    unsigned long len;
    struct block *next;
} block;

void *thread(void *vargp);
int add_block(int, char *, char *);

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
    int status;
    char buf[BUFSIZE] = {0};
    parsed_text *parsed = malloc(sizeof(parsed_text));

    for(;;) {
        status = recv(conn_fd, buf, BUFSIZE, 0);

        if (status == -1) {
            fprintf(stderr, "recv error on sock-fd: %d\n", conn_fd);
            }
        else if (status == 0) {
            close(conn_fd);
            break;
            }

        //if parse();
    }
    return NULL;
}

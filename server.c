#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "memcache.h"
#include "socket.h"

#define BUFSIZE 1024
#define DBSIZE 2048

void *thread(void *vargp);

block *database[DBSIZE];

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
            fprintf(stderr, "recv error: %s\n", strerror(errno));
            close(conn_fd);
            break;
            }
        else if (status == 0) {
            close(conn_fd);
            break;
            }
        memset(buf, 0, BUFSIZE);

        //if parse();
    }
    return NULL;
}

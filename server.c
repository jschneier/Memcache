#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "memcache.h"
#include "socket.h"
#include "parse.h"

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
        conn_fd = malloc(sizeof(int));
        if (conn_fd == NULL) {
            perror("malloc error");
            exit(1);
            }
        *conn_fd = accept(sock_fd, (struct sockaddr *) &addr, &clientsize);
        pthread_create(&tid, NULL, &thread, conn_fd);
    }

    return 0;
}

void *thread(void *vargp) {
    pthread_detach(pthread_self());
    int status, conn_fd;
    conn_fd = *((int *)vargp);
    free(vargp);
    char buf[BUFSIZE] = {0};
    parsed_text *parsed = malloc(sizeof(parsed_text));

    for(;;) {
        status = recv(conn_fd, buf, BUFSIZE, 0);

        if (status == -1) {
            fprintf(stderr, "recv error: %s\n", strerror(errno));
            close(conn_fd);
            break;
            }
        if (status == 0) {
            close(conn_fd);
            break;
            }

        int cmd = parse_cmd(buf);
        char *resp;
        switch(cmd) {
            case STORE:
                resp = parse_store(buf, parsed);
                if (resp != NULL) {
                    send(conn_fd, resp, strlen(resp), 0);
                    zero_buffer(buf, BUFSIZE);
                    break;
                }
                zero_buffer(buf, BUFSIZE);
                status = recv(conn_fd, buf, BUFSIZE, 0);
                if (status == -1)
                    fprintf(stderr, "recv error: %s\n", strerror(errno));

                //+2 for \r\n
                if (status != (parsed->bytes + 2)) {
                    fprintf(stderr, "data block size not equal to header value\n");
                    break;
                    }

                store(parsed);
                break;
            /*case GET:
                parse_get(buf, parsed);
                break;
            case DEL:
                parse_del(buf, parsed);
                break;
            case CHANGE:
                parse_change(buf, parsed);
                break;
            case STATS:
                parse_stats(buf, parsed);
                break;
            case QUIT:
                close(conn_fd);
                pthread_exit(NULL);
                break;
            */
            case ERROR:
                send(conn_fd, "ERROR\r\n", 7, 0);
                break;
        }
        zero_buffer(buf, BUFSIZE);
    }

    return NULL;
}

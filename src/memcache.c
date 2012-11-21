#include "memcache.h"
#include "commands.h"
#include "parse.h"

#define PORT "3303"
#define BACKLOG 20

static void *thread(void *);
static int init_socket(void);

block *database[DBSIZE];
pthread_rwlock_t rw_lock = PTHREAD_RWLOCK_INITIALIZER;

static void *
thread(void *vargp)
{
    pthread_detach(pthread_self());
    int status, conn_fd;
    conn_fd = *((int *)vargp);
    free(vargp);
    char buf[BUFSIZE];
    parsed_text *parsed = malloc(sizeof(parsed));

    for(;;) {
        status = recv(conn_fd, buf, BUFSIZE, 0);
        strip_n_trailing_spaces(buf, 2);

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
        char *resp, *messages[MAX_KEYS][2];
        int i;
        switch(cmd) {
        case STORE:
            resp = parse_store(buf, parsed);
            if (resp != NULL) {
                send(conn_fd, resp, strlen(resp), 0);
                bzero(buf, BUFSIZE);
                break;
            }
            bzero(buf, BUFSIZE);
            status = recv(conn_fd, buf, BUFSIZE, 0);
            strip_n_trailing_spaces(buf, 2);
            if (status == -1)
                //fprintf(stderr, "recv error: %s\n", strerror(errno));
            if ((unsigned) status != parsed->bytes) {
                fprintf(stderr, "data block size not equal to header value\n");
                break;
            }

            pthread_rwlock_wrlock(&rw_lock);
            resp = store(parsed);
            pthread_rwlock_unlock(&rw_lock);
            if (parsed->no_reply == false)
                send(conn_fd, resp, strlen(resp), 0);
            break;
        case GET:
            parse_get(buf, parsed);
            pthread_rwlock_rdlock(&rw_lock);
            get(parsed, messages);
            pthread_rwlock_unlock(&rw_lock);
            for(i=0; i<MAX_KEYS; ++i) {
                if(messages[i] == NULL)
                    break;
                else{
                    send(conn_fd, messages[i][0], strlen(messages[i][0]), 0);
                    send(conn_fd, messages[i][1], strlen(messages[i][1]), 0);
                }
            }
            send(conn_fd, "END\r\n", 5, 0);
            break;
        case DEL:
            parse_del(buf, parsed);
            pthread_rwlock_wrlock(&rw_lock);
            resp = delete(parsed);
            pthread_rwlock_unlock(&rw_lock);

            if (parsed->no_reply == false)
                send(conn_fd, resp, strlen(resp), 0);
            break;
        case CHANGE:
            parse_change(buf, parsed);
            pthread_rwlock_wrlock(&rw_lock);
            resp = change(parsed);
            pthread_rwlock_unlock(&rw_lock);

            if (parsed->no_reply == false)
                send(conn_fd, resp, strlen(resp), 0);
            break;
        /*case STATS:
            parse_stats(buf, parsed);
            break;*/
        case QUIT:
            close(conn_fd);
            pthread_exit(NULL);
            break;
        case ERROR:
            send(conn_fd, "ERROR\r\n", 7, 0);
            break;
        }
        bzero(buf, BUFSIZE);
    }

    free(parsed);

    return NULL;
}

static int
init_socket(void)
{
    int sock_fd;
    struct addrinfo hints, *info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &info) != 0)
        return -1;

    sock_fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    if (sock_fd == -1)
        return -1;
    if (bind(sock_fd, info->ai_addr, info->ai_addrlen) == -1)
        return -1;
    if (listen(sock_fd, BACKLOG) == -1)
        return -1;
    
    return sock_fd;
}

int
main(void)
{
    int sock_fd, *conn_fd;
    pthread_t tid;
    struct sockaddr_storage addr;
    socklen_t clientsize = sizeof(addr);

    sock_fd = init_socket();
    if (sock_fd == -1) {
        perror("Error returned in init_socket");
        exit(1);
        }

    for(;;) {
        conn_fd = malloc(sizeof(int));
        if (conn_fd == NULL) {
            perror("malloc error");
            exit(1);
        }
        *conn_fd = accept(sock_fd, (struct sockaddr *) &addr, &clientsize);
        pthread_create(&tid, NULL, &thread, conn_fd);
    }

    pthread_rwlock_destroy(&rw_lock);

    return 0;
}

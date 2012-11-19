#include "commands.h"
#include "tests.h"

#define HOSTNAME "127.0.0.1"
#define PORT 3303

static int
set_up()
{
    int sock_fd;
    struct hostent *server;
    struct sockaddr_in serv_addr;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(HOSTNAME);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
                (char *)&serv_addr.sin_addr.s_addr,
                server->h_length);
    serv_addr.sin_port = htons(PORT);

    connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    return sock_fd;
}

char *
test_set()
{
    char buf[BUFSIZE];
    int sock_fd = set_up();
    char *msg = "set foo 14 0 7\r\n";
    char *dat = "0123456\r\n";
    send(sock_fd, msg, strlen(msg), 0);
    send(sock_fd, dat, strlen(dat), 0);
    
    recv(sock_fd, buf, 1024, 0);
    mu_assert("set stored correctly", STR_EQ(buf, "STORED\r\n"));

    return 0;
}

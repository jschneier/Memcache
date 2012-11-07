#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024
#define DBSIZE 2048
#define STR_EQ(s1, s2) (strcmp(s1, s2) == 0)

void zero_buffer(char *, int);

typedef struct {
    char *cmd;
    char *key;
    char *flags;
    unsigned long exptime;
    unsigned long bytes;
    unsigned long long cas_unique;
    long change;
    bool no_reply;
    char *data;
} parsed_text;

typedef struct block {
    char *key;
    char *data;
    char *flags;
    unsigned long exptime;
    unsigned long bytes;
    unsigned long long cas_unique;
    struct block *next;
} block;

extern block* database[];

enum CMDS {
    STORE, GET, DEL, CHANGE,
    STATS, QUIT, ERROR
    };

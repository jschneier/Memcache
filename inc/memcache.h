#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024
#define DBSIZE 2048
#define MAX_KEYS 10
#define STR_EQ(s1, s2) (strcmp(s1, s2) == 0)

unsigned hash(char *str);
void strip_n_trailing_spaces(char *, int);

typedef struct {
    char *cmd;
    char *key;
    unsigned long flags;
    unsigned long exptime;
    unsigned long bytes;
    uint64_t cas_unique;
    uint64_t change;
    char *keys[MAX_KEYS];
    bool no_reply;
    char *data;
} parsed_text;

typedef struct block {
    char *key;
    char *data;
    unsigned long flags;
    unsigned long exptime;
    unsigned long bytes;
    uint64_t cas_unique;
    struct block *next;
} block;

extern block* database[];

enum CMDS {
    STORE, GET, DEL, CHANGE,
    STATS, QUIT, ERROR
    };

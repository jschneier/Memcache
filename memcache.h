#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct {
    char *cmd;
    char *key;
    char *flags;
    unsigned long exptime;
    unsigned long bytes;
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
    unsigned long long cas_uniquie;
    struct block *next;
} block;

extern block* database[];

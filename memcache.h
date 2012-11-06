#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char *cmd;
    char *key;
    char *flags;
    unsigned long exptime;
    unsigned int bytes;
    bool no_reply;
    char *data;
} parsed_text;

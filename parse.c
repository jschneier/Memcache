#include "memcache.h"

#define STR_EQ(s1, s2) (strcmp(s1, s2) == 0)

int parse_cmd(char *buf) {
    const char *delim = " ";
    char temp[BUFSIZE];
    strncpy(temp, buf, BUFSIZE);
    char *cmd = strtok(temp, delim);

    if (STR_EQ(cmd, "set") || STR_EQ(cmd, "add") || STR_EQ(cmd, "replace") ||
            STR_EQ(cmd, "append") || STR_EQ(cmd, "prepend") || STR_EQ(cmd, "cas"))
        return STORE;

    else if (STR_EQ(cmd, "get") || STR_EQ(cmd, "gets"))
        return GET;

    else if (STR_EQ(cmd, "delete"))
        return DEL;

    else if (STR_EQ(cmd, "incr") || STR_EQ(cmd, "decr") || STR_EQ(cmd, "touch"))
        return CHANGE;

    else if (STR_EQ(cmd, "stats"))
        return STATS;

    else if (STR_EQ(cmd, "quit"))
        return QUIT;

    else
        return ERROR;
}

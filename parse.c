#include "memcache.h"

#define STR_EQ(s1, s2) (strcmp(s1, s2) == 0)
#define DELIM " "
#define NEXT_TOKEN strtok(NULL, DELIM)

int parse_cmd(char *buf) {
    char temp[BUFSIZE];
    strncpy(temp, buf, 10); //10 because maximum command length (prepend) is 7
    char *cmd = strtok(temp, DELIM);

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

char *parse_store(char *buf, parsed_text *parsed) {
    char *end; //used in strtoul and strtoull for error checking
    char *cur = strtok(buf, DELIM);
    unsigned long dat;
    parsed->cmd = cur; //should never fail if we got this far

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR no key received\r\n";
    parsed->key = cur;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR no flags received\r\n";
    parsed->flags = cur;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR no exptime received\r\n";

    dat = strtoul(cur, &end, 10);    
    if (*end != '\0') //not all characters were converted
        return "CLIENT_ERROR not all of exptime converted\r\n";
    parsed->exptime = dat;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR no bytes (length) received\r\n";
    dat = strtoul(cur, &end, 10);
    if (*end != '\0')
        return "CLIENT_ERROR not all of bytes converted\r\n";
    parsed->bytes = dat;

    if (STR_EQ("cas", parsed->cmd)) {
        cur = NEXT_TOKEN;
        if (cur == NULL)
            return "CLIENT_ERROR didn't received cas unique\r\n";
        unsigned long long cas = strtoull(cur, &end, 10);
        if (*end != '\0')
            return "CLIENT_ERROR not all of cas unique converted\r\n";
        parsed->cas_unique = cas;
    }
    cur = NEXT_TOKEN;
    if (STR_EQ(cur, "\r\n")) {
        parsed->no_reply = false;
        return NULL;
        }
    else if (STR_EQ(cur, "noreply")) {
        parsed->no_reply = true;
        return NULL;
        }
    else
        return "CLIENT_ERROR too many tokens sent\r\n";

}
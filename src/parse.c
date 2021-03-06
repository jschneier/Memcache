#include "memcache.h"

#define DELIM " "
#define NEXT_TOKEN strtok(NULL, DELIM)
#define IS_REPLY()                                       \
    cur = NEXT_TOKEN;                                    \
    if (cur == NULL) {                                   \
        parsed->no_reply = false;                        \
        return NULL;                                     \
    }                                                    \
    else if (STR_EQ(cur, "noreply")) {                   \
        parsed->no_reply = true;                         \
        return NULL;                                     \
    }                                                    \
    else                                                 \
        return "CLIENT_ERROR: too many tokens sent\r\n"; \

int
parse_cmd(char *buf)
{
    char tmp[BUFSIZE];
    strncpy(tmp, buf, 10); //10 because maximum command length (prepend) is 7
    char *cmd = strtok(tmp, DELIM);

    if (cmd == NULL)
        return ERROR;

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

char *
parse_store(char *buf, parsed_text *parsed)
{
    char *end; //used in strtoul and strtoull for error checking
    char *cur = strtok(buf, DELIM);
    unsigned long dat;
    parsed->cmd = cur; //should never fail if we got this far

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR: no key received\r\n";
    parsed->key = cur;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR: no flags received\r\n";

    dat = strtol(cur, &end, 10);
    if(*end != '\0')
        return "CLIENT_ERROR: flags not number\r\n";
    parsed->flags = dat;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR: no exptime received\r\n";

    dat = strtoul(cur, &end, 10);    
    if (*end != '\0') //not all characters were converted
        return "CLIENT_ERROR: exptime not number\r\n";
    parsed->exptime = dat;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR: no bytes (length) received\r\n";
    dat = strtoul(cur, &end, 10);
    if (*end != '\0')
        return "CLIENT_ERROR: bytes not number\r\n";
    parsed->bytes = dat;

    if (STR_EQ("cas", parsed->cmd)) {
        cur = NEXT_TOKEN;
        if (cur == NULL)
            return "CLIENT_ERROR: didn't receive cas unique\r\n";
        uint64_t cas = strtoull(cur, &end, 10);
        if (*end != '\0')
            return "CLIENT_ERROR: cas unique not number\r\n";
        parsed->cas_unique = cas;
    }

    IS_REPLY()
}

char *
parse_change(char *buf, parsed_text *parsed)
{
    char *end; //used in strtoull for error checking
    char *cur = strtok(buf, DELIM);
    uint64_t val;
    parsed->cmd = cur;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR: no key received\r\n";
    parsed->key = cur;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR: didn't receive value\r\n";

    val = strtoull(cur, &end, 10);
    if (*end != '\0')
        return "CLIENT_ERROR: value not number\r\n";
    parsed->change = val;

    IS_REPLY()
}

char *
parse_get(char *buf, parsed_text *parsed)
{
    char *cur = strtok(buf, DELIM);
    parsed->cmd = cur;
    int i = 0;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR: no keys provided in get command\r\n";

    parsed->keys[i] = cur;
    ++i;

    while (i < MAX_KEYS && ((cur = NEXT_TOKEN) != NULL))
        parsed->keys[i++] = cur;

    IS_REPLY()
}

char *
parse_del(char *buf, parsed_text *parsed)
{
    char *cur = strtok(buf, DELIM);
    parsed->cmd = cur;

    cur = NEXT_TOKEN;
    if (cur == NULL)
        return "CLIENT_ERROR: no key provided for delete\r\n";

    parsed->key = cur;

    IS_REPLY()
}

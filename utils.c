#include "memcache.h"

void
zero_buffer(char *buf, int len) {
    memset(buf, 0, len);
}

/*DJB2 string hashing algorithm*/
unsigned
hash(char *str) {

    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

char *
strip_spaces(char *str) {
    int size = strlen(str);
    char *end = str + size - 1;

    while(isspace(*end))
        end--;
    *(end + 1) = '\0';

    return str;
}

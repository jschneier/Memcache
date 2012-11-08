#include <string.h>

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

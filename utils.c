#include "memcache.h"

void zero_buffer(char *buf, int len) {
    memset(buf, 0, len);
}

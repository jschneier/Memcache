#include "memcache.h"

/*DJB2 string hashing algorithm*/
unsigned
hash(char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

void
strip_n_trailing_spaces(char *str, int n)
{
    int size = strlen(str);
    char *end = str + size - 1;
    int i;

    for(i=0; i<n; i++) {
        if (isspace(*end))
            end--;
        else
            break;
    }
    *end = '\0';
}

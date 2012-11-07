/*DJB2 string hashing algorithm*/

unsigned long
hash (char *str) {

    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash;
    }

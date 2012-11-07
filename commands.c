#include "memcache.h"
#include "hash.h"

void store(parsed_text *parsed) {
    unsigned index = (unsigned) hash(parsed->key) % DBSIZE;
    printf("%u\n", index);
}
/*void store(parsed_text *parsed) {
    int index = hash(parsed->key) % DBSIZE;

    if (STR_EQ(parsed->cmd, "set"))
        set(index, parsed);

    else if (STR_EQ(parsed->cmd, "add"))
        add(index, parsed);

    else if (STR_EQ(parsed->cmd, "replace"))
        replace(index, parsed);

    else if (STR_EQ(parsed->cmd, "append"))
        append(index, parsed);

}*/

int set(int index, parsed_text *parsed) {

    block *new = malloc(sizeof(block));
    if (new == NULL)
        return -1;

    new->key = parsed->key;
    new->data = parsed->data;
    new->next = NULL;

    //first insert at this location
    if (database[index] == NULL)
        database[index] = new;

    //hash collision -> linked list
    else {
        block *current = database[index];
        //advance to the last place
        while (current->next != NULL)
            current = current->next;
        current->next = new;
        }

    return 0;
}

int incr(int index, parsed_text *parsed) {
    
    block *current = database[index];

    if (current == NULL)
        return -1;

    while (current->key != parsed->key)
        current = current->next;

    //we iterated through and didn't find a match
    if (current == NULL)
        return -1;

    unsigned long long value;
    int status = sscanf(current->data, "%llu", &value);

    //data couldn't be interpreted as an unsigned integer
    if (status == EOF)
        return -1;

    value += parsed->change;
    //need to use sprintf to convert back for data

    return 0;
    }

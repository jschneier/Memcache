#include "memcache.h"
#include "hash.h"

#define STORED "STORED\r\n"
#define NOT_STORED "NOT_STORE\r\n"
#define EXISTS "EXISTS\r\n"
#define NOT_FOUND "NOT_FOUND\r\n"

char *store(parsed_text *parsed) {
    unsigned index = hash(parsed->key) % DBSIZE;

    if (STR_EQ(parsed->cmd, "set"))
        return set(index, parsed);

    else if (STR_EQ(parsed->cmd, "add"))
        return add(index, parsed);

    else if (STR_EQ(parsed->cmd, "replace"))
        return replace(index, parsed);

    else if (STR_EQ(parsed->cmd, "append"))
        return append(index, parsed);

    else if (STR_EQ(parsed->cmd, "prepend"))
        return prepend(index, parsed);

    else
        return cas(index, parsed);

}

static char*
set(int index, parsed_text *parsed) {

    block *cur = database[index];
    if (cur == NULL)
        cur = init_block(parsed);

    else {
        //advance to the block before matching key or the end of the list
        while(cur->next != NULL && cur->next->key != parsed->key)
            cur = cur->next;

        cur->next = init_block(parsed);
    }
    return STORED;
}

static char*
add(int index, parsed_text *parsed) {

    block *cur = database[index];
    if (cur == NULL)
        cur = init_block(parsed);

    else {
        //advance to the block before matching key or the end of the list
        while(cur->next != NULL && cur->next->key != parsed->key)
            cur = cur->next;
        
        //want to store on a key we don't already have, ignore matching keys
        if (cur->next != NULL)
            return NOT_STORED;

        cur->next = init_block(parsed);
    }
    return STORED;
}

static char*
replace(int index, parsed_text *parsed) {

    block *cur = database[index];
    if (cur == NULL)
        return NOT_STORED; //only want to store if we already match the key

    else {
        //advance to the block before matching key or the end of the list
        while(cur->next != NULL && cur->next->key != parsed->key)
            cur = cur->next;

        //only want to do replace if we already have that key, if NULL we don't
        if (cur->next != NULL) {
            block *temp = cur->next->next //store the next pointer so we don't break up the linked list
            cur->next = init_block(parsed);
            cur->next->next = temp;
            return STORED;
            }
        else
            return NOT_STORED;
    }
}

static char*
append(int index, parsed_text *parsed) {

    block *cur = database[index];
    if (cur == NULL)
        return NOT_STORED; //can't append if key not in hash

    else {
        //advance to the block before matching key or the end of the list
        while(cur->next != NULL && cur->next->key != parsed->key)
            cur = cur->next;

        if (cur->next != NULL) {
            //TODO: realloc return STORED
        }

        else
            return NOT_STORED;
    }
}

static char*
incr(int index, parsed_text *parsed) {
    
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

static block *
init_block(parsed_text *parsed) {

    block *ret = malloc(sizeof(block));

    ret->key = parsed->key;
    ret->flags = parsed->flags;
    ret->exptime = parsed->exptime;
    ret->bytes = parsed->bytes;
    ret->data = parsed->data;
    ret->next = NULL;
    //TODO ret->cas_unique

    return ret;
}

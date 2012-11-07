#include "memcache.h"
#include "hash.h"

}
/*void store(parsed_text *parsed) {
    unsigned index = hash(parsed->key) % DBSIZE;

    if (STR_EQ(parsed->cmd, "set"))
        set(index, parsed);

    else if (STR_EQ(parsed->cmd, "add"))
        add(index, parsed);

    else if (STR_EQ(parsed->cmd, "replace"))
        replace(index, parsed);

    else if (STR_EQ(parsed->cmd, "append"))
        append(index, parsed);

    else if (STR_EQ(parsed->cmd, "prepend"))
        prepend(index, parsed);

    else
        cas(index, parsed);

}*/

static void
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
}

static void
add(int index, parsed_text *parsed) {

    block *cur = database[index];
    if (cur == NULL)
        cur = init_block(parsed);

    else {
        //advance to the block before matching key or the end of the list
        while(cur->next != NULL && cur->next->key != parsed->key)
            cur = cur->next;
        
        //want to store on a key we don't already have, ignore matching keys
        if (cur->next == NULL)
            cur->next = init_block(parsed);
    }
}

static void
replace(int index, parsed_text *parsed) {

    block *cur = database[index];
    if (cur == NULL)
        ; //only want to store if we already match the key, can't match if first

    else {
        //advance to the block before matching key or the end of the list
        while(cur->next != NULL && cur->next->key != parsed->key)
            cur = cur->next;

        //only want to do replace if we already have that key, if NULL we don't
        if (cur->next != NULL) {
            block *temp = cur->next->next //store the next pointer so we don't break up the linked list
            cur->next = init_block(parsed);
            cur->next->next = temp;
            }
    }
}

static void
append(int index, parsed_text *parsed) {

    block *cur = database[index];
    if (cur == NULL)
        ; //can't append if key not in hash

    else {
        //advance to the block before matching key or the end of the list
        while(cur->next != NULL && cur->next->key != parsed->key)
            cur = cur->next;

        if (cur->next != NULL) {
            //TODO
        }
    }
}

static void
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

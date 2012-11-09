#include "memcache.h"

#define STORED "STORED\r\n"
#define NOT_STORED "NOT_STORED\r\n"
#define EXISTS "EXISTS\r\n"
#define NOT_FOUND "NOT_FOUND\r\n"
#define BAD_VALUE "BAD_VALUE\r\n"
#define DELETED "DELETED\r\n"

static char *set(unsigned, parsed_text*);
static char *add(unsigned, parsed_text*);
static char *replace(unsigned, parsed_text*);
static char *pend(unsigned, parsed_text*);
static char *incr_decr(unsigned, parsed_text*);
static char *touch(unsigned, parsed_text*);
static block *init_block(parsed_text*);

char *
store(parsed_text *parsed)
{
    unsigned index = hash(parsed->key) % DBSIZE;

    if (STR_EQ(parsed->cmd, "set"))
        return set(index, parsed);

    else if (STR_EQ(parsed->cmd, "add"))
        return add(index, parsed);

    else if (STR_EQ(parsed->cmd, "replace"))
        return replace(index, parsed);

    else if (STR_EQ(parsed->cmd, "append") || STR_EQ(parsed->cmd, "prepend"))
        return pend(index, parsed);

    /*
    else
        return cas(index, parsed);
    */
    else
        return "FOO"; //TODO
}

static char *
set(unsigned index, parsed_text *parsed)
{

    block *cur = database[index];
    if (cur == NULL)
        cur = init_block(parsed);

    else {
        //advance to the block before matching key or the end of the list
        while(cur->next != NULL && cur->next->key != parsed->key)
            cur = cur->next;

        block *temp = cur->next->next;
        cur->next = init_block(parsed);
        cur->next->next = temp;
    }
    return STORED;
}

static char *
add(unsigned index, parsed_text *parsed)
{

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

        block *temp = cur->next->next; //store the next pointer so we don't break up the linked list
        cur->next = init_block(parsed);
        cur->next->next = temp;
    }
    return STORED;
}

static char *
replace(unsigned index, parsed_text *parsed)
{

    block *cur = database[index];
    if (cur == NULL)
        return NOT_STORED; //only want to store if we already match the key

    else {
        //advance to the block before matching key or the end of the list
        while(cur->next != NULL && cur->next->key != parsed->key)
            cur = cur->next;

        //only want to do replace if we already have that key, if NULL we don't
        if (cur->next != NULL) {
            block *temp = cur->next->next; //store the next pointer so we don't break up the linked list
            cur->next = init_block(parsed);
            cur->next->next = temp;
            return STORED;
        }
        else
            return NOT_STORED;
    }
}

static char *
pend(unsigned index, parsed_text *parsed)
{

    block *cur = database[index];
    char buf[BUFSIZE];

    if (cur == NULL)
        return NOT_STORED; //can't append if key not in hash

    else {
        //advance to the block before matching key or the end of the list
        while(cur != NULL && cur->key != parsed->key)
            cur = cur->next;

        if (cur != NULL) {

            if (STR_EQ(parsed->cmd, "append"))
                snprintf(buf, BUFSIZE, "%s%s", cur->data, parsed->data);
            else
                snprintf(buf, BUFSIZE, "%s%s", parsed->data, cur->data);

            cur->data = buf;
            return STORED;
        }
    }

    return NOT_STORED;
}

char *
change(parsed_text *parsed)
{
    unsigned index = hash(parsed->key) % DBSIZE;

    if (STR_EQ(parsed->cmd, "touch"))
        return touch(index, parsed);

    else
        return incr_decr(index, parsed);
}

static char *
touch(unsigned index, parsed_text *parsed)
{
    block *cur = database[index];
    return "FOO";
}

static char *
incr_decr(unsigned index, parsed_text *parsed)
{

    block *cur = database[index];
    char *ret;
    char buf[BUFSIZE];

    if (cur == NULL)
        return NOT_FOUND;

    while (cur->key != parsed->key)
        cur = cur->next;

    //we iterated through and didn't find a match
    if (cur == NULL)
        return NOT_FOUND;

    unsigned long long value;
    int status = sscanf(cur->data, "%llu", &value);

    //data couldn't be interpreted as an unsigned integer
    if (status == EOF)
        return BAD_VALUE;

    if (STR_EQ(parsed->cmd, "decr")) {
        if (parsed->change > value)
            value = 0;

        else
            value -= parsed->change;
    }
    //incr
    else {
        value += parsed->change;
    }

    sprintf(buf, "%llu", value);
    cur->data = buf;

    sprintf(buf, "%llu\r\n", value);
    ret = buf;
    return ret;
}

char *
delete(parsed_text *parsed)
{

    unsigned index = hash(parsed->key) % DBSIZE;
    block *cur = database[index];

    while(cur->next != NULL && cur->next->key != parsed->key)
        cur = cur->next;

    if (cur == NULL)
        return NOT_FOUND;

    block *temp = cur->next->next;
    free(cur->next);
    cur->next = temp;

    return DELETED;
}

static block *
init_block(parsed_text *parsed)
{

    block *ret = malloc(sizeof(block));

    if (ret == NULL) {
        perror("malloc failed in init_block");
        exit(1);
    }

    ret->key = parsed->key;
    ret->flags = parsed->flags;
    ret->exptime = parsed->exptime;
    ret->bytes = parsed->bytes;
    ret->data = parsed->data;
    //TODO ret->cas_unique

    return ret;
}

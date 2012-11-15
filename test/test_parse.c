#include "tests.h"
#include "parse.h"

static void prime_buf(char *, char *);

char *test_cmd()
{
    int resp;
    resp = parse_cmd("set foo");
    mu_assert("set foo != STORE", resp == STORE);

    resp = parse_cmd("add foo");
    mu_assert("add foo != STORE", resp == STORE);

    resp = parse_cmd("se foo");
    mu_assert("se foo != ERROR", resp == ERROR);

    resp = parse_cmd("setfoo");
    mu_assert("setfoo != ERROR", resp == ERROR);

    resp = parse_cmd("get foo");
    mu_assert("get foo != GET", resp == GET);

    resp = parse_cmd("gets foo");
    mu_assert("gets foo != GET", resp == GET);

    resp = parse_cmd("delete foo");
    mu_assert("delete foo != DELETE", resp == DEL);

    resp = parse_cmd("del foo");
    mu_assert("del foo != ERROR", resp == ERROR);

    resp = parse_cmd("quit");
    mu_assert("quit != QUIT", resp == QUIT);

    resp = parse_cmd("stats");
    mu_assert("stats foo != STATS", resp == STATS);

    resp = parse_cmd("statsfoo");
    mu_assert("statsfoo != ERROR", resp == ERROR);

    resp = parse_cmd("");
    mu_assert("<empty string> != ERROR", resp == ERROR);

    return 0;
}

char *test_parse_store()
{
    char *resp, buf[BUFSIZE];
    parsed_text *parsed = malloc(sizeof(parsed_text));

    prime_buf(buf, "set");
    resp = parse_store(buf, parsed);
    mu_assert("set != key_error", STR_EQ(resp, "CLIENT_ERROR: no key received\r\n"));

    prime_buf(buf, "set ");
    resp = parse_store(buf, parsed);
    mu_assert("set[space] != key_error", STR_EQ(resp, "CLIENT_ERROR: no key received\r\n"));

    prime_buf(buf, "set foo");
    resp = parse_store(buf, parsed);
    mu_assert("set foo != flags error", STR_EQ(resp, "CLIENT_ERROR: no flags received\r\n"));

    prime_buf(buf, "set foo 14hi");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 14hi != flag convert error", STR_EQ(resp, "CLIENT_ERROR: flags not number\r\n"));

    prime_buf(buf, "set foo bar");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 14hi != flag convert error", STR_EQ(resp, "CLIENT_ERROR: flags not number\r\n"));

    prime_buf(buf, "set foo 17");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 != no exptime error", STR_EQ(resp, "CLIENT_ERROR: no exptime received\r\n"));

    prime_buf(buf, "set foo 17 bar");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 bar != exptime convert error", STR_EQ(resp, "CLIENT_ERROR: exptime not number\r\n"));

    prime_buf(buf, "set foo 17 12");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 != bytes error", STR_EQ(resp, "CLIENT_ERROR: no bytes (length) received\r\n"));

    prime_buf(buf, "set foo 17 12 14ar");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 14ar != bytes error", STR_EQ(resp, "CLIENT_ERROR: bytes not number\r\n"));

    prime_buf(buf, "set foo 17 12 14");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 14 != NULL", (resp == NULL) && (!parsed->no_reply));
    
    prime_buf(buf, "set foo 17 12 14 noreply");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 14 noreply != NULL", ((resp == NULL) && parsed->no_reply));

    prime_buf(buf, "set foo 17 12 14 foo");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 14 foo != tokens error", STR_EQ(resp, "CLIENT_ERROR: too many tokens sent\r\n"));

    return 0;
}

char *test_parse_change()
{
    char *resp, buf[BUFSIZE];
    parsed_text *parsed = malloc(sizeof(parsed_text));

    prime_buf(buf, "incr");
    resp = parse_change(buf, parsed);
    mu_assert("incr != key error", STR_EQ(resp, "CLIENT_ERROR: no key received\r\n"));

    prime_buf(buf, "incr foo");
    resp = parse_change(buf, parsed);
    mu_assert("incr foo != receive value error", STR_EQ(resp, "CLIENT_ERROR: didn't receive value\r\n"));

    prime_buf(buf, "incr foo bar");
    resp = parse_change(buf, parsed);
    mu_assert("incr foo bar != convert error", STR_EQ(resp, "CLIENT_ERROR: value not number\r\n"));

    prime_buf(buf, "incr foo 14ar");
    resp = parse_change(buf, parsed);
    mu_assert("incr foo 14ar != convert error", STR_EQ(resp, "CLIENT_ERROR: value not number\r\n"));

    return 0;
}

char *test_parse_get()
{
    char *resp, buf[BUFSIZE];
    parsed_text *parsed = malloc(sizeof(parsed_text));

    prime_buf(buf, "get");
    resp = parse_get(buf, parsed);
    mu_assert("get != keys provided error", STR_EQ(resp, "CLIENT_ERROR: no keys provided in get command\r\n"));

    prime_buf(buf, "get foo");
    resp = parse_get(buf, parsed);
    mu_assert("get foo != NULL", ((resp == NULL) && STR_EQ("foo", parsed->keys[0])));

    prime_buf(buf, "gets foo bar");
    resp = parse_get(buf, parsed);
    mu_assert("get foo bar != NULL", ((resp == NULL) && STR_EQ("foo", parsed->keys[0]) && STR_EQ("bar", parsed->keys[1])));

    return 0;
}

char *test_parse_del()
{
    char *resp, buf[BUFSIZE];
    parsed_text *parsed = malloc(sizeof(parsed_text));

    prime_buf(buf, "delete");
    resp = parse_del(buf, parsed);
    mu_assert("delete != key error", STR_EQ(resp, "CLIENT_ERROR: no key provided for delete\r\n"));

    prime_buf(buf, "delete foo");
    resp = parse_del(buf, parsed);
    mu_assert("delete foo != NULL", ((resp == NULL) && STR_EQ("foo", parsed->key)));

    return 0;
}

static void
prime_buf(char *buf, char *msg)
{
    bzero(buf, BUFSIZE);
    sprintf(buf, "%s", msg);
}

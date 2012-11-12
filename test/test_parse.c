#include "tests.h"
#include "parse.h"

char *test_cmd() {
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

char *test_parse_store() {
    char *resp, buf[BUFSIZE];
    parsed_text *parsed = malloc(sizeof(parsed_text));

    sprintf(buf, "%s", "set");
    resp = parse_store(buf, parsed);
    mu_assert("set != key_error", STR_EQ(resp, "CLIENT_ERROR: no key received\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set ");
    resp = parse_store(buf, parsed);
    mu_assert("set[space] != key_error", STR_EQ(resp, "CLIENT_ERROR: no key received\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo");
    resp = parse_store(buf, parsed);
    mu_assert("set foo != flags error", STR_EQ(resp, "CLIENT_ERROR: no flags received\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo 14hi");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 14hi != flag convert error", STR_EQ(resp, "CLIENT_ERROR: not all of flags converted\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo bar");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 14hi != flag convert error", STR_EQ(resp, "CLIENT_ERROR: not all of flags converted\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo 17");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 != no exptime error", STR_EQ(resp, "CLIENT_ERROR: no exptime received\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo 17 bar");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 bar != exptime convert error", STR_EQ(resp, "CLIENT_ERROR: not all of exptime converted\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo 17 12ar");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12ar != exptime convert error", STR_EQ(resp, "CLIENT_ERROR: not all of exptime converted\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo 17 12");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 != bytes error", STR_EQ(resp, "CLIENT_ERROR: no bytes (length) received\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo 17 12 bar");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 bar != bytes error", STR_EQ(resp, "CLIENT_ERROR: not all byte chars converted\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo 17 12 14ar");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 14ar != bytes error", STR_EQ(resp, "CLIENT_ERROR: not all byte chars converted\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo 17 12 14");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 14 != NULL", (resp == NULL) && (!parsed->no_reply));
    memset(buf, 0, BUFSIZE);
    
    sprintf(buf, "%s", "set foo 17 12 14 noreply");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 14 noreply != NULL", ((resp == NULL) && parsed->no_reply));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "set foo 17 12 14 foo");
    resp = parse_store(buf, parsed);
    mu_assert("set foo 17 12 14 foo != tokens error", STR_EQ(resp, "CLIENT_ERROR: too many tokens sent\r\n"));
    memset(buf, 0, BUFSIZE);

    return 0;
}

char *test_parse_change() {
    char *resp, buf[BUFSIZE];
    parsed_text *parsed = malloc(sizeof(parsed_text));

    sprintf(buf, "%s", "incr");
    resp = parse_change(buf, parsed);
    mu_assert("incr != key error", STR_EQ(resp, "CLIENT_ERROR: no key received\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "incr foo");
    resp = parse_change(buf, parsed);
    mu_assert("incr foo != receive value error", STR_EQ(resp, "CLIENT_ERROR: didn't receive value\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "incr foo bar");
    resp = parse_change(buf, parsed);
    mu_assert("incr foo bar != convert error", STR_EQ(resp, "CLIENT_ERROR: not all of value converted\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "incr foo 14ar");
    resp = parse_change(buf, parsed);
    mu_assert("incr foo 14ar != convert error", STR_EQ(resp, "CLIENT_ERROR: not all of value converted\r\n"));
    memset(buf, 0, BUFSIZE);

    return 0;
}

char *test_parse_get() {
    char *resp, buf[BUFSIZE];
    parsed_text *parsed = malloc(sizeof(parsed_text));

    sprintf(buf, "%s", "get");
    resp = parse_get(buf, parsed);
    mu_assert("get != keys provided error", STR_EQ(resp, "CLIENT_ERROR: no keys provided in get command\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "get foo");
    resp = parse_get(buf, parsed);
    mu_assert("get foo != NULL", ((resp == NULL) && STR_EQ("foo", parsed->keys[0])));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "gets foo bar");
    resp = parse_get(buf, parsed);
    mu_assert("get foo bar != NULL", ((resp == NULL) && STR_EQ("foo", parsed->keys[0]) && STR_EQ("bar", parsed->keys[1])));
    memset(buf, 0, BUFSIZE);

    return 0;
}

char *test_parse_del() {
    char *resp, buf[BUFSIZE];
    parsed_text *parsed = malloc(sizeof(parsed_text));

    sprintf(buf, "%s", "delete");
    resp = parse_del(buf, parsed);
    mu_assert("delete != key error", STR_EQ(resp, "CLIENT_ERROR: no key provided for delete\r\n"));
    memset(buf, 0, BUFSIZE);

    sprintf(buf, "%s", "delete foo");
    resp = parse_del(buf, parsed);
    mu_assert("delete foo != NULL", ((resp == NULL) && STR_EQ("foo", parsed->key)));
    memset(buf, 0, BUFSIZE);

    return 0;
}

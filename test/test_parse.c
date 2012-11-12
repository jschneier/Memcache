#include "tests.h"
#include "parse.h"

char *test_cmd() {
    int resp;
    resp = parse_cmd("set foo");
    mu_assert("\"set foo\" != STORE", resp == STORE);

    resp = parse_cmd("add foo");
    mu_assert("\"add foo\" != STORE", resp == STORE);

    resp = parse_cmd("se foo");
    mu_assert("\"se foo\" != ERROR", resp == ERROR);

    resp = parse_cmd("setfoo");
    mu_assert("\"setfoo\" != ERROR", resp == ERROR);

    resp = parse_cmd("get foo");
    mu_assert("\"get foo\" != GET", resp == GET);

    resp = parse_cmd("gets foo");
    mu_assert("\"gets foo\" != GET", resp == GET);

    resp = parse_cmd("delete foo");
    mu_assert("\"delete foo\" != DELETE", resp == DEL);

    resp = parse_cmd("del foo");
    mu_assert("\"del foo\" != ERROR", resp == ERROR);

    resp = parse_cmd("quit");
    mu_assert("\"quit\" != QUIT", resp == QUIT);

    resp = parse_cmd("stats");
    mu_assert("\"stats foo\" != STATS", resp == STATS);

    resp = parse_cmd("statsfoo");
    mu_assert("\"statsfoo\" != ERROR", resp == ERROR);

    resp = parse_cmd("");
    mu_assert("\"\" != ERROR", resp == ERROR);
    return 0;
}

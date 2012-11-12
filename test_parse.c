#include <stdio.h>

#include "minunit.h"
#include "memcache.h"
#include "parse.h"

int tests_run = 0;

static char *test_cmd() {
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

static char *all_tests() {
    mu_run_test(test_cmd);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0)
        printf("%s\n", result);
    else
        printf("ALL TESTS PASS\n");
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}

#include <stdio.h>
#include "tests.h"

int tests_run = 0;

static char *all_tests() {
    mu_run_test(test_cmd);
    return 0;
}

int main(void) {
    char *result = all_tests();
    if (result != 0)
        printf("%s\n", result);
    else
        printf("ALL TESTS PASS\n");
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}

#include <stdio.h>
#include <stdlib.h>

static int* bsearch_1(int *, ssize_t, int);
static int* bsearch_2(int *, ssize_t, int);

int* bsearch_1(int *array, ssize_t n, int target) {
    ssize_t a, b;
    size_t m;
    if (n <= 0) {
        return NULL;
    }
    a = 0;
    b = n - 1;
    while (a <= b) {
        m = (a + b) >> 1;
        if (array[m] == target) {
            return array + m;
        } else if (array[m] < target) {
            a = m + 1;
        } else if (array[m] > target) {
            b = m - 1;
        }
    }
    return NULL;
}

// int* bsearch_2(int *array, ssize_t n, int target) {

// }

int main(int argc, char const *argv[]) {
    int array[] = {1, 3, 5, 7, 9, 11, 13, 15, 17};
    int i, *idx;

    for (i = 0; i < 20; ++i) {
        printf("%d: ", i);
        idx = bsearch_1(array, 9, i);
        if (idx == NULL) {
            printf("NULL\n");
        } else {
            printf("%d\n", *idx);
        }
    }
    return 0;
}

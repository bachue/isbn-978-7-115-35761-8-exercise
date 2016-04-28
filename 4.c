#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

struct max_t {
    int max;
    size_t beg, end;
};

typedef void (*benchmark_func)(int *, size_t, struct max_t *);
static void benchmark(const char *, benchmark_func, int *, size_t);
static void find_max_1(int *, size_t, struct max_t *);
static void find_max_2(int *, size_t, struct max_t *);
static void find_max_3(int *, size_t, struct max_t *);
static void find_max_4_helper(int *, size_t, struct max_t *, size_t, size_t);
static void find_max_4(int *, size_t, struct max_t *);
static void init_test_array(int *, size_t);

void find_max_1(int *array, size_t n, struct max_t *max) {
    size_t i, j, k;
    int sum;
    max->max = 0;

    for (i = 0; i < n; ++i) {
        for (j = i; j < n; ++j) {
            sum = 0;
            for (k = i; k <= j; ++k) {
                sum += array[k];
            }
            if (max->max < sum) {
                max->max = sum;
                max->beg = i;
                max->end = j;
            }
        }
    }
}

void find_max_2(int *array, size_t n, struct max_t *max) {
    size_t i, j;
    int sum;
    max->max = 0;

    for (i = 0; i < n; ++i) {
        sum = 0;
        for (j = i; j < n; ++j) {
            sum += array[j];
            if (max->max < sum) {
                max->max = sum;
                max->beg = i;
                max->end = j;
            }
        }
    }
}

void find_max_3(int *array, size_t n, struct max_t *max) {
    int *cumarr, sum;
    size_t i, j;

    max->max = 0;
    cumarr = (int *) malloc(sizeof(int) * (n + 1));
    cumarr[0] = 0;
    for (i = 0; i < n; ++i) {
        cumarr[i + 1] = cumarr[i] + array[i];
    }
    for (i = 0; i < n; ++i) {
        for (j = i; j < n; ++j) {
            sum = cumarr[j + 1] - cumarr[i];
            if (max->max < sum) {
                max->max = sum;
                max->beg = i;
                max->end = j;
            }
        }
    }
    free(cumarr);
}

void find_max_4_helper(int *array, size_t n, struct max_t *max, size_t l, size_t r) {
    struct max_t max_left, max_right;
    int lmax, rmax, sum;
    size_t i, m;
    if (l > r) {
        max->max = 0;
        max->beg = l;
        max->end = r;
        return;
    } else if (l == r) {
        max->max = array[l] > 0 ? array[l] : 0;
        max->beg = l;
        max->end = r;
        return;
    }
    m = (l + r) / 2;
    lmax = sum = 0;
    for (i = m; i >= l; i--) {
        sum += array[i];
        if (lmax < sum) {
            lmax = sum;
            max->beg = i;
        }
        if (i == 0) {
            break;
        }
    }
    rmax = sum = 0;
    for (i = m + 1; i <= r; i++) {
        sum += array[i];
        if (rmax < sum) {
            rmax = sum;
            max->end = i;
        }
    }
    max->max = lmax + rmax;
    find_max_4_helper(array, n, &max_left, l, m);
    find_max_4_helper(array, n, &max_right, m + 1, r);
    if (max->max < max_left.max) {
        max->max = max_left.max;
        max->beg = max_left.beg;
        max->end = max_left.end;
    }
    if (max->max < max_right.max) {
        max->max = max_right.max;
        max->beg = max_right.beg;
        max->end = max_right.end;
    }
}

void find_max_4(int *array, size_t n, struct max_t *max) {
    find_max_4_helper(array, n, max, 0, n - 1);
}

void benchmark(const char *prompt, benchmark_func func, int *array, size_t n) {
    struct timeval tv1, tv2;
    struct max_t max;
    unsigned long diff_usec, diff_sec;

    gettimeofday(&tv1, NULL);
    func(array, n, &max);
    gettimeofday(&tv2, NULL);

    if (tv1.tv_usec > tv2.tv_usec) {
        diff_usec = 1e6 + tv2.tv_usec - tv1.tv_usec;
        diff_sec = tv2.tv_sec - tv1.tv_sec - 1;
    } else {
        diff_usec = tv2.tv_usec - tv1.tv_usec;
        diff_sec = tv2.tv_sec - tv1.tv_sec;
    }
    printf("Benchmark %s: %ld.%06ld\n", prompt, diff_sec, diff_usec);
    printf("Result: %d from %zu to %zu\n", max.max, max.beg, max.end);
}

void init_test_array(int *array, size_t n) {
    int i;
    for (i = 0; i < n; ++i) {
        array[i] = rand() % 256 - 128;
    }
}

int main(int argc, char const *argv[]) {
    // int array[1024];
    int array[] = {1,2,3,4,5,-10,14};
    // srand(time(NULL));

    // init_test_array(array, 1024);
    benchmark("find_max_1", find_max_1, array, 7);
    benchmark("find_max_2", find_max_2, array, 7);
    benchmark("find_max_3", find_max_3, array, 7);
    benchmark("find_max_4", find_max_4, array, 7);

    return 0;
}

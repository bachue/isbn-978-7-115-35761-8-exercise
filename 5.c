#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

typedef void (*benchmark_func)(int *, size_t);
static void benchmark(const char *, benchmark_func, int *, size_t);
static void sort1(int *, size_t);
static void sort2(int *, size_t);
static void init_test_array(int *, size_t);
static void print_test_array(int *, size_t);
static int validate_sorted_array(int *, size_t);

#define swap(a, b) { \
    int tmp; \
    tmp = a; \
    a = b;   \
    b = tmp; \
}

void sort1(int *array, size_t n) {
    size_t i, j;
    for (i = 1; i < n; ++i) {
        for (j = i; j > 0 && array[j - 1] > array[j]; --j) {
            swap(array[j - 1], array[j]);
        }
    }
}

void sort2(int *array, size_t n) {
    size_t i, j;
    int t;
    for (i = 1; i < n; ++i) {
        t = array[i];
        for (j = i; j > 0 && array[j - 1] > t; --j) {
            array[j] = array[j - 1];
        }
        array[j] = t;
    }
}

size_t partition1(int *array, size_t n) {
    size_t left = -1, right = n, k = rand() % n;
    int key = array[k];
    if (n <= 1) {
        return 0;
    }
    for (;;) {
        do {
            left++;
        } while(left < right && array[left] <= key);
        do {
            right--;
        } while (left < right && array[right] >= key);
        if (left > right) {
            break;
        }
        swap(array[left], array[right]);
    }
    swap(array[k], array[right]);
    return right;
}

void qsort1(int *array, size_t n) {
    size_t partition;
    if (n <= 1) {
        return;
    }
    partition = partition1(array, n);
    if (partition > 0) {
        qsort1(array, partition);
    }
    if (partition < n) {
        qsort1(array + partition + 1, n - partition - 1);
    }
}

void benchmark(const char *prompt, benchmark_func func, int *array, size_t n) {
    struct timeval tv1, tv2;
    unsigned long diff_usec, diff_sec;

    gettimeofday(&tv1, NULL);
    func(array, n);
    gettimeofday(&tv2, NULL);

    if (tv1.tv_usec > tv2.tv_usec) {
        diff_usec = 1e6 + tv2.tv_usec - tv1.tv_usec;
        diff_sec = tv2.tv_sec - tv1.tv_sec - 1;
    } else {
        diff_usec = tv2.tv_usec - tv1.tv_usec;
        diff_sec = tv2.tv_sec - tv1.tv_sec;
    }
    printf("Benchmark %s: %ld.%06ld\n", prompt, diff_sec, diff_usec);
}

void init_test_array(int *array, size_t n) {
    int i;
    for (i = 0; i < n; ++i) {
        array[i] = rand() % 65536 - 32768;
    }
}

void print_test_array(int *array, size_t n) {
    size_t i;
    printf("[");
    for (i = 0; i < n; ++i) {
        printf("%d", array[i]);
        if (i + 1 == n) {
            printf("]\n");
        } else {
            printf(", ");
        }
    }
}

int validate_sorted_array(int *array, size_t n) {
    int i;
    for (i = 1; i < n; ++i) {
        if (array[i] < array[i - 1]) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char const *argv[]) {
    const size_t SIZE = 1000000;
    int array[SIZE], dup[SIZE];
    srand(time(NULL));

    init_test_array(array, SIZE);

    memcpy(dup, array, SIZE * sizeof(int));
    benchmark("sort1", sort1, array, SIZE);
    if (validate_sorted_array(array, SIZE) == 1) {
        printf("Success\n");
    } else {
        printf("Failed\n");
    }

    memcpy(dup, array, SIZE * sizeof(int));
    benchmark("sort2", sort2, array, SIZE);
    if (validate_sorted_array(array, SIZE) == 1) {
        printf("Success\n");
    } else {
        printf("Failed\n");
    }

    memcpy(dup, array, SIZE * sizeof(int));
    benchmark("qsort1", qsort1, array, SIZE);
    if (validate_sorted_array(array, SIZE) == 1) {
        printf("Success\n");
    } else {
        printf("Failed\n");
    }

    return 0;
}

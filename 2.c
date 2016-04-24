#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

struct rotate_t {
    int *array;
    size_t n, i;
};

typedef void (*benchmark_func)(void *);

static size_t gcd(size_t, size_t);
static void reverse(int *, size_t);
static void print_vector(void *);
static void vector_rotate1(void *);
static void vector_rotate2(void *);
static void init_test_array(int *, size_t);

size_t gcd(size_t a, size_t b) {
    if (b != 0) {
        return gcd(b, a % b);
    } else {
        return a;
    }
}

void reverse(int *array, size_t n) {
    size_t a, b;
    int tmp;
    if (n == 0) {
        return;
    }
    for (a = 0, b = n - 1; a < b; a++, b--) {
        tmp = array[a];
        array[a] = array[b];
        array[b] = tmp;
    }
}

void print_vector(void *data) {
    struct rotate_t *rotate = (struct rotate_t *) data;
    size_t idx;
    printf("[");
    for (idx = 0; idx < rotate->n; ++idx) {
        printf("%d", rotate->array[idx]);
        if (idx + 1 < rotate->n) {
            printf(", ");
        }
    }
    printf("]\n");
}

void vector_rotate1(void *data) {
    struct rotate_t *rotate = (struct rotate_t *) data;
    size_t x, b, k, t;
    int first;

    t = gcd(rotate->i, rotate->n);
    for (b = 0; b < t; ++b) {
        first = rotate->array[b];
        for (x = b; ; x = k) {
            k = (x + rotate->i) % rotate->n;
            if (k == b) {
                break;
            }
            rotate->array[x] = rotate->array[k];
        }
        rotate->array[x] = first;
    }
}

void vector_rotate2(void *data) {
    struct rotate_t *rotate = (struct rotate_t *) data;
    reverse(rotate->array, rotate->i);
    reverse(rotate->array + rotate->i, rotate->n - rotate->i);
    reverse(rotate->array, rotate->n);
}

void benchmark(const char *prompt, benchmark_func func, void *data) {
    struct timeval tv1, tv2;
    unsigned long diff_usec, diff_sec;
    gettimeofday(&tv1, NULL);
    func(data);
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
    size_t i;
    for (i = 0; i < n; ++i) {
        array[i] = i;
    }
}

int main(int argc, char const *argv[]) {
    struct rotate_t rotate;
    int array[10000];
    rotate.array = array;
    rotate.n = 10000;

    init_test_array(array, 10000);
    rotate.i = 1;
    benchmark("rotate1", vector_rotate1, &rotate);

    init_test_array(array, 10000);
    rotate.i = 40;
    benchmark("rotate1", vector_rotate1, &rotate);

    init_test_array(array, 10000);
    rotate.i = 80;
    benchmark("rotate1", vector_rotate1, &rotate);

    init_test_array(array, 10000);
    rotate.i = 99;
    benchmark("rotate1", vector_rotate1, &rotate);

    init_test_array(array, 10000);
    rotate.i = 10000;
    benchmark("rotate1", vector_rotate1, &rotate);

    init_test_array(array, 10000);
    rotate.i = 1;
    benchmark("rotate2", vector_rotate2, &rotate);

    init_test_array(array, 10000);
    rotate.i = 40;
    benchmark("rotate2", vector_rotate2, &rotate);

    init_test_array(array, 10000);
    rotate.i = 80;
    benchmark("rotate2", vector_rotate2, &rotate);

    init_test_array(array, 10000);
    rotate.i = 99;
    benchmark("rotate2", vector_rotate2, &rotate);

    init_test_array(array, 10000);
    rotate.i = 10000;
    benchmark("rotate2", vector_rotate2, &rotate);

    return 0;
}

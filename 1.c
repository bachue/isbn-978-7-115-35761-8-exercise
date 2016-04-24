#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define LEN_1_KB (1024)
#define LEN_1_MB (1024 * LEN_1_KB)

struct linebuf_t {
    char *buf;
    size_t len;
};

typedef void (*read_bitmap_file_cb)(long num);

static FILE* open_data_file(const char *);
static long read_num_from_data_file(FILE *, struct linebuf_t *);
static int set_bitmap_file(FILE *, long);
static int read_bitmap_file(FILE *, char *, size_t, read_bitmap_file_cb);
static void output_num(long);

int main(int argc, char const *argv[]) {
    char memory[LEN_1_MB];
    FILE *bitmapfile;
    FILE *datafile;
    struct linebuf_t linebuf;
    long num;

    memset(memory, 0, LEN_1_MB);
    if ((bitmapfile = tmpfile()) == NULL) {
        perror("failed in tmpfile()");
        _exit(EXIT_FAILURE);
    }
    if ((datafile = open_data_file("/tmp/data")) == NULL) {
        perror("failed in open_data_file()");
        _exit(EXIT_FAILURE);
    }

    linebuf.buf = (char *) malloc(LEN_1_KB);
    linebuf.len = LEN_1_KB;

    while ((num = read_num_from_data_file(datafile, &linebuf)) != -1) {
        if (set_bitmap_file(bitmapfile, num) == -1) {
            perror("failed in set_bitmap_file()");
            _exit(EXIT_FAILURE);
        }
    }

    if (read_bitmap_file(bitmapfile, memory, LEN_1_MB, output_num) == -1) {
        perror("failed in read_bitmap_file()");
        _exit(EXIT_FAILURE);
    }

    free(linebuf.buf);
    fclose(bitmapfile);
    fclose(datafile);
    return 0;
}

FILE* open_data_file(const char *path) {
    return fopen(path, "r"); // if `fopen` returns NULL, the func returns NULL here.
}

long read_num_from_data_file(FILE *file, struct linebuf_t *linebuf) {
    if (getline(&(linebuf->buf), &(linebuf->len), file) == -1) {
        return -1;
    }
    return atol(linebuf->buf);
}

int set_bitmap_file(FILE *file, long off) {
    int fd = fileno(file);
    char buf = 0;

    if (pread(fd, &buf, 1, off >> 3) == -1) {
        return -1;
    }
    buf |= 1 << (7 - (off % 8));

    if (pwrite(fd, &buf, 1, off >> 3) == -1) {
        return -1;
    }
    return 0;
}

int read_bitmap_file(FILE *file, char *memory, size_t len, read_bitmap_file_cb callback) {
    int fd = fileno(file);
    long i, c;
    size_t j;
    int k;
    char buf;
    struct stat st;
    ssize_t readn;

    if (fstat(fd, &st) == -1) {
        return -1;
    }
    c = 1 + ((st.st_size - 1) / len);

    for (i = 0; i < c; ++i) {
        memset(memory, 0, len);
        if ((readn = pread(fd, memory, len, i * len)) == -1) {
            return -1;
        }
        if (readn == 0) {
            continue;
        }
        for (j = 0; j < len; ++j) {
            buf = memory[j];
            if (buf == 0) {
                continue;
            }
            for (k = 0; k < 8; ++k) {
                if ((buf >> (7 - k)) & 1) {
                    callback(((i * len) << 3) + (j << 3) + k);
                }
            }
        }
    }
    return 0;
}

void output_num(long num) {
    printf("%ld\n", num);
}

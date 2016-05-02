#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

struct priority_queue_t {
    int* list;
    size_t size, capacity;
};

enum priority_queue_status {
    QUEUE_OK = 0,
    QUEUE_FAILED = -1
};

/* PUBLIC */
enum priority_queue_status priority_queue_create(struct priority_queue_t *, size_t);
void priority_queue_free(struct priority_queue_t *);
enum priority_queue_status priority_queue_enqueue(struct priority_queue_t *, int);
int priority_queue_dequeue(struct priority_queue_t *);

/* PRIVATE */
static inline enum priority_queue_status priority_queue_check_and_expand(struct priority_queue_t *);
static inline int priority_queue_is_full(struct priority_queue_t *);
static inline void priority_queue_percolate_up(struct priority_queue_t *);
static inline void priority_queue_percolate_down(struct priority_queue_t *);
static inline void priority_queue_swap(struct priority_queue_t *, size_t, size_t);

/* IMPLEMENTATIONS */
enum priority_queue_status priority_queue_create(struct priority_queue_t *queue, size_t capacity) {
    if (capacity == 0) {
        capacity = 3;
    }
    queue->list = (int *) malloc(capacity * sizeof(int));
    if (queue->list == NULL) {
        return QUEUE_FAILED;
    }
    queue->size = 0;
    queue->capacity = capacity;
    return QUEUE_OK;
}

void priority_queue_free(struct priority_queue_t *queue) {
    free(queue->list);
    queue->list = NULL;
    queue->size = queue->capacity = 0;
}

enum priority_queue_status priority_queue_enqueue(struct priority_queue_t *queue, int value) {
    if (priority_queue_check_and_expand(queue) == QUEUE_FAILED) {
        return QUEUE_FAILED;
    }
    queue->list[queue->size] = value;
    priority_queue_percolate_up(queue);
    queue->size++;
    return QUEUE_OK;
}

enum priority_queue_status priority_queue_check_and_expand(struct priority_queue_t *queue) {
    int *newlist;
    size_t newcapacity;
    if (priority_queue_is_full(queue)) {
        newcapacity = (queue->capacity << 1) + 1;
        newlist = (int *) realloc(queue->list, newcapacity * sizeof(int));
        if (newlist == NULL) {
            return QUEUE_FAILED;
        }
        queue->list = newlist;
        queue->capacity = newcapacity;
    }
    return QUEUE_OK;
}

void priority_queue_percolate_up(struct priority_queue_t *queue) {
    size_t curidx, prtidx;
    for(curidx = queue->size; curidx > 0; curidx = prtidx) {
        prtidx = (curidx - 1) >> 1;
        if (queue->list[curidx] < queue->list[prtidx]) {
            priority_queue_swap(queue, curidx, prtidx);
        } else {
            break;
        }
    }
}

int priority_queue_dequeue(struct priority_queue_t *queue) {
    int queuetop;
    queuetop = queue->list[0];
    queue->list[0] = queue->list[queue->size - 1];
    queue->size--;
    priority_queue_percolate_down(queue);
    return queuetop;
}

void priority_queue_percolate_down(struct priority_queue_t *queue) {
    size_t curidx, chldidx, chldidx_left, chldidx_right;
    int t;
    for (curidx = 0; curidx < queue->size; curidx = chldidx) {
        chldidx_left = (curidx << 1) + 1;
        chldidx_right = chldidx_left + 1;
        if (chldidx_left >= queue->size) {
            break;
        } else if (chldidx_right >= queue->size) {
            chldidx = chldidx_left;
        } else if (queue->list[chldidx_left] <= queue->list[chldidx_right]) {
            chldidx = chldidx_left;
        } else {
            chldidx = chldidx_right;
        }
        if (queue->list[curidx] > queue->list[chldidx]) {
            priority_queue_swap(queue, curidx, chldidx);
        } else {
            break;
        }
    }
}

void priority_queue_swap(struct priority_queue_t *queue, size_t a, size_t b) {
    int tmpdata;
    tmpdata = queue->list[a];
    queue->list[a] = queue->list[b];
    queue->list[b] = tmpdata;
}

int priority_queue_is_full(struct priority_queue_t *queue) {
    return queue->size == queue->capacity;
}

int main(int argc, char const *argv[]) {
    struct priority_queue_t queue;
    size_t i;
    srand(time(NULL));

    if (priority_queue_create(&queue, 0) == QUEUE_FAILED) {
        perror("CREATE QUEUE FAILED");
        _exit(EXIT_FAILURE);
    }

    for (i = 0; i < 1000; ++i) {
        if (priority_queue_enqueue(&queue, rand() % 256 - 128) == QUEUE_FAILED) {
            perror("CREATE ENQUEUE FAILED");
            _exit(EXIT_FAILURE);
        }
    }
    for (i = 0; i < 1000; ++i) {
        printf("%d ", priority_queue_dequeue(&queue));
    }
    printf("\n");

    priority_queue_free(&queue);
    return 0;
}

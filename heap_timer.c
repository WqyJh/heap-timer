#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>

#include "heap_timer.h"

struct heap_timer_s {
    heap_timer_handler_t handler;
    heap_timer_node_t min;
    int capacity;
    int size;
    heap_timer_node_t *heap[0];
};


int heap_timer_memsize(int capacity) {
    return sizeof(heap_timer_t) + sizeof(void*) * (capacity + 1);
}

void heap_timer_init(heap_timer_t *timer, int capacity, heap_timer_handler_t handler) {
    timer->capacity = capacity;
    timer->size = 0;
    timer->min.key = 0; // uint64
    timer->heap[0] = &timer->min;
    timer->heap[0]->idx = 0;
    timer->handler = handler;
}

heap_timer_t *heap_timer_create(int capacity, heap_timer_handler_t handler) {
    int size = heap_timer_memsize(capacity);
    heap_timer_t *timer = malloc(size);
    if (timer == NULL) return NULL;
    heap_timer_init(timer, capacity, handler);
    return timer;
}

static inline void set_heap(heap_timer_node_t **heap, int idx, heap_timer_node_t *node) {
    heap[idx] = node;
    heap[idx]->idx = idx;
}

int heap_timer_add(heap_timer_t *timer, heap_timer_node_t *node, uint64_t expire_time) {
    if (timer->size == timer->capacity) {
        return -ENOSPC;
    }
    node->key = expire_time;
    heap_timer_node_t **heap = timer->heap;
    int i;
    for (i = ++timer->size; heap[i/2]->key > node->key; i /= 2) {
        set_heap(heap, i, heap[i/2]);
    }
    set_heap(heap, i, node);
    assert(i != 0);
    return 0;
}

int heap_timer_delete(heap_timer_t *timer, heap_timer_node_t *node) {
    if (node->idx <= 0 || node->idx > timer->size) {
        return -EINVAL;
    }
    heap_timer_node_t **heap = timer->heap;
    heap_timer_node_t *last = heap[timer->size--];
    int i;
    for (i = 2 * node->idx; i <= timer->size; i *= 2) {
        if (i+1 <= timer->size && heap[i]->key > heap[i+1]->key) i++;
        if (last->key > heap[i]->key) {
            set_heap(heap, i/2, heap[i]);
        }
        else break;
    }
    set_heap(heap, i/2, last);
    assert(i/2 != 0);
    return 0;
}

heap_timer_node_t *heap_timer_min(heap_timer_t *timer) {
    if (timer->size == 0) {
        return NULL;
    }
    return timer->heap[1];
}

void heap_timer_tick_limit(heap_timer_t *timer, uint64_t now, void *ctx, int limit) {
    heap_timer_node_t *node;
    bool infinity = limit == 0;
    for (; infinity || limit--;) {
        node = heap_timer_min(timer);
        if (node == NULL) return;
        if ((heap_timer_key_t)(node->key - now) > 0) {
            return;
        }
        assert(0 == heap_timer_delete(timer, node));
        if (timer->handler != NULL) {
            timer->handler(timer, node, ctx);
        }
    }
}

int heap_timer_count(heap_timer_t *timer) {
    return timer->size;
}

#ifndef _HEAP_TIMER_H_INCLUDED_
#define _HEAP_TIMER_H_INCLUDED_


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define heap_timer_cancel heap_timer_delete
#define heap_timer_tick(timer, now, ctx) heap_timer_tick_limit(timer, now, ctx, 0)

typedef uint64_t heap_timer_key_t;
typedef int64_t heap_timer_key_int_t;
typedef struct heap_timer_s heap_timer_t;
typedef struct heap_timer_node_s heap_timer_node_t;
typedef void (*heap_timer_handler_t)(heap_timer_t *timer, heap_timer_node_t *node, void *ctx);

struct heap_timer_node_s {
    heap_timer_key_t key;
    void *data;

    int idx; // DO NOT TOUCH!!!
};

int heap_timer_memsize(int capacity);
void heap_timer_init(heap_timer_t *timer, int capacity, heap_timer_handler_t handler);
heap_timer_t *heap_timer_create(int capacity, heap_timer_handler_t handler);
int heap_timer_add(heap_timer_t *timer, heap_timer_node_t *node, uint64_t expire_time);
int heap_timer_delete(heap_timer_t *timer, heap_timer_node_t *node);
heap_timer_node_t *heap_timer_min(heap_timer_t *timer);
void heap_timer_tick_limit(heap_timer_t *timer, uint64_t now, void *ctx, int limit);
int heap_timer_count(heap_timer_t *timer);


#ifdef __cplusplus
}
#endif

#endif // !_HEAP_TIMER_H_INCLUDED_

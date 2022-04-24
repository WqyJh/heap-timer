#include <string.h>
#include <benchmark/benchmark.h>

#include "heap_timer.h"

#define EXPECT_TRUE(v1) assert(v1)
#define EXPECT_EQ(v1, v2) assert((v1) == (v2))

// #define ITERATIONS 1000000

static void BM_timer_add_sametime(benchmark::State& state) {
    heap_timer_t *timer;

    const int n = state.range(0);
    timer = heap_timer_create(n, NULL);

    heap_timer_node_t *entries = new heap_timer_node_t[n];
    for (int i = 0; i < n; i++) {
        heap_timer_node_t *te = &entries[i];
        te->key = 0;
        te->data = (void *)(uint64_t)i;
    }
    int items = 0;
    while (state.KeepRunningBatch(n)) {
        for (int i = 0; i < n; i++) {
            heap_timer_add(timer, &entries[i], 100L);
        }
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        EXPECT_EQ(n, heap_timer_count(timer));
        for (int i = 0; i < n; i++) {
            heap_timer_cancel(timer, &entries[i]);
        }
        items += n;
        state.ResumeTiming(); // And resume timers. They are now counting again.
    }
    EXPECT_EQ(0, heap_timer_count(timer));
    state.SetItemsProcessed(items);
    delete[] entries;
}

static void BM_timer_add_difftime(benchmark::State& state) {
    heap_timer_t *timer;

    const int n = state.range(0);
    timer = heap_timer_create(n, NULL);

    heap_timer_node_t *entries = new heap_timer_node_t[n];
    for (int i = 0; i < n; i++) {
        heap_timer_node_t *te = &entries[i];
        te->key = 0;
        te->data = (void *)(uint64_t)i;
    }
    int items = 0;
    while (state.KeepRunningBatch(n)) {
        for (int i = 0; i < n; i++) {
            heap_timer_add(timer, &entries[i], (uint64_t)i);
        }
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        EXPECT_EQ(n, heap_timer_count(timer));
        for (int i = 0; i < n; i++) {
            heap_timer_cancel(timer, &entries[i]);
        }
        items += n;
        state.ResumeTiming(); // And resume timers. They are now counting again.
    }

    EXPECT_EQ(0, heap_timer_count(timer));
    state.SetItemsProcessed(items);
    delete[] entries;
}

static void BM_timer_del(benchmark::State& state) {
    heap_timer_t *timer;

    const int n = state.range(0);
    timer = heap_timer_create(n, NULL);

    heap_timer_node_t *entries = new heap_timer_node_t[n];
    for (int i = 0; i < n; i++) {
        heap_timer_node_t *te = &entries[i];
        te->key = 0;
        te->data = (void *)(uint64_t)i;
    }

    int items = 0;
    while (state.KeepRunningBatch(n)) {
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        for (int i = 0; i < n; i++) {
            heap_timer_add(timer, &entries[i], (uint64_t)i);
        }
        items += n;
        EXPECT_EQ(n, heap_timer_count(timer));
        state.ResumeTiming(); // And resume timers. They are now counting again.

        for (int i = 0; i < n; i++) {
            heap_timer_cancel(timer, &entries[i]);
        }
    }
    EXPECT_EQ(0, heap_timer_count(timer));
    state.SetItemsProcessed(items);
    delete[] entries;
}

void expire_handler(heap_timer_t *timer, heap_timer_node_t *node, void *ctx) {
    EXPECT_EQ(node->data, ctx);
}

static void BM_timer_expire(benchmark::State& state) {
    heap_timer_t *timer;

    const int n = state.range(0);
    timer = heap_timer_create(n, expire_handler);

    heap_timer_node_t *entries = new heap_timer_node_t[n];
    for (int i = 0; i < n; i++) {
        heap_timer_node_t *te = &entries[i];
        te->key = 0;
        te->data = te;
    }

    int items = 0;
    while (state.KeepRunningBatch(n)) {
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        for (int i = 0; i < n; i++) {
            heap_timer_add(timer, &entries[i], (uint64_t)i);
        }
        items += n;
        EXPECT_EQ(n, heap_timer_count(timer));
        state.ResumeTiming(); // And resume timers. They are now counting again.

        for (int i = 0; i < n; i++) {
            heap_timer_tick(timer, i, &entries[i]);
        }
    }
    EXPECT_EQ(0, heap_timer_count(timer));
    state.SetItemsProcessed(items);
    delete[] entries;
}

// BENCHMARK(BM_timer_add_sametime)->Arg(ITERATIONS);
// BENCHMARK(BM_timer_add_difftime)->Arg(ITERATIONS);
// BENCHMARK(BM_timer_del)->Arg(ITERATIONS);

BENCHMARK(BM_timer_add_sametime)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);
BENCHMARK(BM_timer_add_difftime)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);
BENCHMARK(BM_timer_del)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);
BENCHMARK(BM_timer_expire)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);

BENCHMARK_MAIN();

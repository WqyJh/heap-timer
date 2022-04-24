#include <thread>
#include <atomic>

#include <gtest/gtest.h>

#include "heap_timer.h"

#define TIMER_CAPACITY 1024
// data += 1
void test_expire_handler1(heap_timer_t *timer, heap_timer_node_t *node, void *ctx) {
    node->data = (void *)((uint64_t)(node->data) + 1L);
}

TEST(timer, test_expire) {
    heap_timer_t *timer;
    timer = heap_timer_create(TIMER_CAPACITY, test_expire_handler1);
    EXPECT_TRUE(timer != NULL);

    heap_timer_node_t node;
    node.data = (void*)1L;
    heap_timer_add(timer, &node, 100L);

    // Not expire
    heap_timer_tick(timer, 50L, NULL);
    EXPECT_EQ(node.data, (void *)1L);

    // Not expire
    heap_timer_tick(timer, 80L, NULL);
    EXPECT_EQ(node.data, (void *)1L);

    // Expired and excuted
    heap_timer_tick(timer, 100L, NULL);
    EXPECT_EQ(node.data, (void *)2L);

    // Expired and won't be excuted
    heap_timer_tick(timer, 200L, NULL);
    EXPECT_EQ(node.data, (void *)2L);
}

TEST(timer, test_cancel) {
    heap_timer_t *timer;
    timer = heap_timer_create(TIMER_CAPACITY, test_expire_handler1);
    EXPECT_TRUE(timer != NULL);

    heap_timer_node_t node;
    node.data = (void*)1L;
    heap_timer_add(timer, &node, 100L);

    // Not expire
    heap_timer_tick(timer, 50L, NULL);
    EXPECT_EQ(node.data, (void *)1L);

    // Cancel
    heap_timer_cancel(timer, &node);

    // Not expire
    heap_timer_tick(timer, 80L, NULL);
    EXPECT_EQ(node.data, (void *)1L);

    // Expired and won't be excuted
    heap_timer_tick(timer, 100L, NULL);
    EXPECT_EQ(node.data, (void *)1L);

    // Expired and won't be excuted
    heap_timer_tick(timer, 200L, NULL);
    EXPECT_EQ(node.data, (void *)1L);
}

void test_expire_handler2(heap_timer_t *timer, heap_timer_node_t *node, void *ctx) {
    node->data = (void *)((uint64_t)(node->data) + 1L);
    *((uint64_t *)ctx) += 1;
}

TEST(timer, test_ctx) {
    heap_timer_t *timer;
    timer = heap_timer_create(TIMER_CAPACITY, test_expire_handler2);
    EXPECT_TRUE(timer != NULL);

    heap_timer_node_t node;
    node.data = (void*)1L;
    heap_timer_add(timer, &node, 100L);

    uint64_t ctx = 100L;

    // Not expire
    heap_timer_tick(timer, 50L, &ctx);
    EXPECT_EQ(node.data, (void *)1L);
    EXPECT_EQ(ctx, 100L);

    // Expired and excuted
    heap_timer_tick(timer, 100L, &ctx);
    EXPECT_EQ(node.data, (void *)2L);
    EXPECT_EQ(ctx, 101L);
}

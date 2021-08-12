#include "queue.h"

#include "acutest.h"

void test_create(void) {
    Queue queue = queue_create(free);

    TEST_CHECK(queue != NULL);
    TEST_CHECK(queue_size(queue) == 0);
    TEST_CHECK(queue_is_empty(queue) == true);

    DestroyFunc destroy_value = queue_set_destroy_value(queue, NULL);
    TEST_CHECK(destroy_value == free);

    queue_destroy(queue);
}

void test_enqueue(void) {}
void test_dequeue(void) {}

TEST_LIST = {
    {"queue_create", test_create},
    {"queue_enqueue", test_enqueue},
    {"queue_dequeue", test_dequeue},

    {NULL, NULL}  // End of tests.
};
#include "ADTOrderedSet.h"  // oset_create, oset_destroy, oset_size, oset_insert, oset_remove,
                            // oset_set_destroy_key,oset_set_destroy_value, oset_find, oset_get_at,
                            // oset_remove_at, oset_split, oset_merge, oset_concat, oset_find_node,
                            // oset_get_at_node, oset_node_key, oset_node_value, oset_first,
                            // oset_last, oset_next, oset_previous

#include <stdlib.h>  // malloc, free, sizeof, rand, RAND_MAX, size_t

#include "acutest.h"  // TEST_CHECK, TEST_LIST

/// @brief Compares two int*.
///
/// @return < 0, if a < b, or, > 0, if b < a, or, 0, if a and b are equivalent
///
static int compare_ints(const void* a, const void* b) { return *(int*)a - *(int*)b; }

void test_create(void) {
    OrderedSet oset = oset_create(compare_ints, free, free);

    TEST_CHECK(oset != OSET_ERROR);
    TEST_CHECK(oset_size(oset) == 0);

    DestroyFunc destroy_key = oset_set_destroy_key(oset, NULL);
    DestroyFunc destroy_value = oset_set_destroy_value(oset, NULL);

    TEST_CHECK(destroy_key == free);
    TEST_CHECK(destroy_value == free);

    // TODO: Create oset with vector containing values.

    oset_destroy(oset);
}

/// @brief Shuffles the values of an array.
///
void shuffle(int* array[], size_t size) {
    for (int i = 0; i < size; i++) {
        int j = i + rand() / (RAND_MAX / (size - i) + 1);
        int* t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

/// @brief Allocates memory for an integer with given value.
///
/// @return Newly created pointer to integer.
///
static int* create_int(int value) {
    int* pointer = malloc(sizeof(int));
    if (pointer == NULL) {
        return NULL;
    }
    *pointer = value;
    return pointer;
}

/// @brief Creates and returns an array containing (int*)
///
/// @param multiplier Multiplies the values of the array. (i * multiplier)
///
int** create_array(size_t size, int multiplier) {
    int** array = malloc(size * sizeof(*array));
    if (array == NULL) {
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        array[i] = create_int(multiplier * i);
    }

    return array;
}

/// @brief Inserts (key, value) pair to oset and test for correct insertion.
///
void insert_and_test(OrderedSet oset, void* key, void* value) {
    oset_insert(oset, key, value);
    TEST_CHECK(oset_find(oset, key) == value);
}

void test_insert(void) {
    OrderedSet oset = oset_create(compare_ints, free, free);

    int N = 65537;

    // Create key and value arrays.
    int** key_array = create_array(N, 1);
    int** value_array = create_array(N, 1);
    shuffle(key_array, N);  // Shuffle key array for uniform value insertion.

    for (int i = 0; i < N; i++) {
        insert_and_test(oset, key_array[i], value_array[i]);

        TEST_CHECK(oset_size(oset) == (i + 1));
    }

    // Insert key equivalent to the first key, and check if the (key, value) pair was inserted
    // correctly.
    int* duplicate_key = create_int(N / 2);  // N/2, duplicate middle key.
    int* value = create_int(N + N);          // N+N, guarantees big value to check stack-like
                                             // behaviour for duplicates.
    size_t size = oset_size(oset);
    insert_and_test(oset, duplicate_key, value);
    TEST_CHECK(oset_size(oset) == (size + 1));

    // Check if duplicate key was inserted before the original key.
    OrderedSetNode dup = oset_find_node(oset, duplicate_key);
    int* next_key = oset_node_key(oset, oset_next(oset, dup));
    int* next_value = oset_node_value(oset, oset_next(oset, dup));
    TEST_CHECK(*next_key == *duplicate_key);
    TEST_CHECK(next_key != duplicate_key);
    TEST_CHECK(*next_value != *value);
    TEST_CHECK(*next_value < *value);

    // Check if keys are sorted.
    OrderedSetNode node = oset_first(oset);
    for (int i = 0; i < N; i++) {
        int* key = oset_node_key(oset, node);

        if (*key != (N / 2)) {  // key not a duplicate.
            TEST_CHECK(*key == i);
        } else {
            TEST_CHECK(*key == (N / 2));  // N/2, where duplicates are positioned.
            i = N / 2;                    // Resets i to continue traversing after the duplicates.
        }

        node = oset_next(oset, node);
    }

    oset_destroy(oset);

    free(key_array);
    free(value_array);
}

void test_remove(void) {
    OrderedSet oset = oset_create(compare_ints, NULL, NULL);

    int N = 65537;  // To force capacity to double.

    // Create key and value arrays.
    int** key_array = create_array(N, 1);
    int** value_array = create_array(N, 1);
    shuffle(key_array, N);  // Shuffle key array for uniform value insertion.

    // Insert (key, value) pairs.
    for (int i = 0; i < N; i++) {
        oset_insert(oset, key_array[i], value_array[i]);
    }

    size_t size = oset_size(oset);

    // Remove first key.
    int key = 0;
    TEST_CHECK(oset_remove(oset, &key));
    TEST_CHECK(oset_size(oset) == (--size));
    TEST_CHECK(*((int*)oset_node_key(oset, oset_first(oset))) == 1);  // Check first key is 1.

    // Remove last key.
    key = N - 1;
    TEST_CHECK(oset_remove(oset, &key));
    TEST_CHECK(oset_size(oset) == (--size));
    TEST_CHECK(*((int*)oset_node_key(oset, oset_last(oset))) == (N - 2));  // Check last key is N-2.

    // Remove already removed key.
    TEST_CHECK(oset_remove(oset, &key) == false);
    TEST_CHECK(oset_size(oset) == size);

    // Remove not existent key.
    key = N + N;  // N + N, guarantees a large key not in Ordered Set.
    TEST_CHECK(oset_remove(oset, &key) == false);
    TEST_CHECK(oset_size(oset) == size);

    // Insert duplicate key.
    int* dup_key = create_int(N / 2);  // N / 2, guarantees already existent key.
    int* value = create_int(N + N);    // Value can be whatever, doesn't matter.
    oset_insert(oset, dup_key, value);

    // Remove duplicate key and original key.
    key = N / 2;
    TEST_CHECK(oset_remove(oset, dup_key));
    TEST_CHECK(oset_remove(oset, &key));
    TEST_CHECK(oset_find(oset, &key) == NULL);

    // Remove keys.
    size = oset_size(oset);
    for (key = 1; key < N - 1; key++) {
        if (key != (N / 2)) {
            TEST_CHECK(oset_remove(oset, &key));
            TEST_CHECK(oset_size(oset) == (--size));
            TEST_CHECK(oset_find(oset, &key) == NULL);
        }
    }
    TEST_CHECK(oset_size(oset) == 0);

    // Change DestroyFuncs to see if removal of (key, value) pairs frees the allocated memory.
    oset_set_destroy_key(oset, free);
    oset_set_destroy_value(oset, free);

    int removed[N];  // Holds duplicates of values of key_array, in order to avoid calling
                     // oset_find() with a freed key.

    // Re-insert (key, value) pairs.
    for (int i = 0; i < N; i++) {
        oset_insert(oset, key_array[i], value_array[i]);
        removed[i] = *key_array[i];  // Copy key to call oset_find() later.
    }

    // Remove keys.
    size = oset_size(oset);
    for (int i = 0; i < N; i++) {
        TEST_CHECK(oset_remove(oset, key_array[i]));
        TEST_CHECK(oset_size(oset) == (--size));
        TEST_CHECK(oset_find(oset, &(removed[i])) == NULL);
    }

    oset_destroy(oset);

    free(dup_key);
    free(value);
    free(key_array);
    free(value_array);
}

void test_traversal(void) {
    OrderedSet oset = oset_create(compare_ints, free, free);

    int N = 65537;  // To force capacity to double.

    // Create and suffle key array.
    int** key_array = create_array(N, 1);
    int** value_array = create_array(N, 2);

    // Insert (key, value) pairs.
    for (int i = 0; i < N; i++) {
        oset_insert(oset, key_array[i], value_array[i]);
    }

    // Traverse in ascending order.
    int i = 0;
    for (OrderedSetNode node = oset_first(oset); node != OSET_EOF; node = oset_next(oset, node)) {
        int* key = oset_node_key(oset, node);
        int* value = oset_node_value(oset, node);
        TEST_CHECK(*key == i);
        TEST_CHECK(*value == (2 * i));
        i++;
    }

    // Traverse in descending order.
    i = N - 1;
    for (OrderedSetNode node = oset_last(oset); node != OSET_BOF; node = oset_previous(oset, node)) {
        int* key = oset_node_key(oset, node);
        int* value = oset_node_value(oset, node);

        TEST_CHECK(*key == i);
        TEST_CHECK(*value == (2 * i));

        i--;
    }

    oset_destroy(oset);

    free(key_array);
    free(value_array);
}

void test_find(void) {
    OrderedSet oset = oset_create(compare_ints, free, free);

    int N = 1000;

    // Create key and value arrays.
    int** key_array = create_array(N, 1);
    int** value_array = create_array(N, 1);
    shuffle(key_array, N);  // Shuffle key array for uniform value insertion.

    // Insert (key, value) pairs.
    for (int i = 0; i < N; i++) {
        oset_insert(oset, key_array[i], value_array[i]);

        OrderedSetNode found_node = oset_find_node(oset, key_array[i]);
        int* found_key = oset_node_key(oset, found_node);
        int* found_value = oset_node_value(oset, found_node);

        TEST_CHECK(found_node != OSET_EOF);
        TEST_CHECK(found_key == key_array[i]);
        TEST_CHECK(found_value == value_array[i]);
    }

    // Search non-existent key.
    int key = N * 2;  // N*2, guarantees non existent value.
    TEST_CHECK(oset_find_node(oset, &key) == OSET_EOF);
    TEST_CHECK(oset_find(oset, &key) == NULL);

    // Find in the middle:
    key = N / 2;
    TEST_CHECK(*((int*)oset_node_key(oset, oset_find_node(oset, &key))) == (N / 2));

    oset_destroy(oset);

    free(key_array);
    free(value_array);
}

void test_split(void) {
    OrderedSet alpha = oset_create(compare_ints, free, free);

    int N = 65537;  // To force capacity to double.

    // Create key and value arrays.
    int** key_array = create_array(N, 1);
    int** value_array = create_array(N, 1);
    shuffle(key_array, N);  // Shuffle key array for uniform value insertion.

    // Insert (key, value) pairs.
    for (int i = 0; i < N; i++) {
        oset_insert(alpha, key_array[i], value_array[i]);
    }

    // Split in the middle.
    int split_key = N / 2;
    OrderedSet beta = oset_split(alpha, &split_key);

    // Check values of Ordered Sets.
    OrderedSetNode node = oset_first(alpha);

    for (int i = 0; i <= N / 2; i++) {
        int* key = oset_node_key(alpha, node);
        TEST_CHECK(*key == i);
        node = oset_next(alpha, node);
    }
    node = oset_first(beta);
    for (int i = (N / 2) + 1; i < N; i++) {
        int* key = oset_node_key(beta, node);
        TEST_CHECK(*key == i);
        node = oset_next(beta, node);
    }
    oset_destroy(beta);

    size_t size = 0;

    // Split with split_key < 0 (first key)
    split_key = -1;
    size = oset_size(alpha);
    OrderedSet gamma = oset_split(alpha, &split_key);
    TEST_CHECK(oset_size(alpha) == 0);
    TEST_CHECK(oset_size(gamma) == size);
    oset_destroy(alpha);

    // Split with split_key > N (last key)
    split_key = N + 1;
    OrderedSet empty = oset_split(gamma, &split_key);
    TEST_CHECK(oset_size(empty) == 0);
    TEST_CHECK(oset_size(gamma) == size);

    // Split empty Ordered Set.
    OrderedSet error = oset_split(empty, &split_key);
    TEST_CHECK(error == OSET_ERROR);
    oset_destroy(empty);

    oset_destroy(gamma);

    free(key_array);
    free(value_array);
}

void test_merge(void) {
    OrderedSet alpha = oset_create(compare_ints, free, free);
    OrderedSet beta = oset_create(compare_ints, free, free);

    int N = 1000;

    // Create key and value arrays with odd numbers for Ordered Set alpha.
    int** odd_key_array = malloc(N * sizeof(*odd_key_array));
    int** odd_value_array = malloc(N * sizeof(*odd_value_array));
    int number = 1;
    for (int i = 0; i < N; i++) {
        odd_key_array[i] = create_int(number);
        odd_value_array[i] = create_int(number);
        number += 2;
    }
    shuffle(odd_key_array, N);  // Shuffle key array for uniform value insertion.

    // Create key and value arrays with even numbers for Ordered Set beta.
    int** even_key_array = malloc(N * sizeof(*even_key_array));
    int** even_value_array = malloc(N * sizeof(*even_value_array));
    number = 0;
    for (int i = 0; i < N; i++) {
        even_key_array[i] = create_int(number);
        even_value_array[i] = create_int(number);
        number += 2;
    }
    shuffle(even_key_array, N);  // Shuffle key array for uniform value insertion.

    // Insert (key, value) pairs into Ordered Sets.
    for (int i = 0; i < N; i++) {
        oset_insert(alpha, odd_key_array[i], odd_value_array[i]);
        oset_insert(beta, even_key_array[i], even_value_array[i]);
    }

    oset_merge(alpha, beta);

    OrderedSetNode node = oset_first(alpha);
    for (int i = 0; i < (2 * N); i++) {
        int* key = oset_node_key(alpha, node);
        int* value = oset_node_value(alpha, node);

        TEST_CHECK(*key == i);
        TEST_CHECK(*value == i);

        node = oset_next(alpha, node);
    }

    oset_destroy(alpha);

    free(odd_key_array);
    free(odd_value_array);
    free(even_key_array);
    free(even_value_array);
}

void test_concat(void) {
    OrderedSet alpha = oset_create(compare_ints, free, free);

    int N = 65537;  // To force capacity to double.

    // Create key and value arrays.
    int** key_array = create_array(N, 1);
    int** value_array = create_array(N, 1);
    shuffle(key_array, N);  // Shuffle key array for uniform value insertion.

    // Insert (key, value) pairs.
    for (int i = 0; i < N; i++) {
        oset_insert(alpha, key_array[i], value_array[i]);
    }

    // Split in the middle.
    int split_key = N / 2;
    OrderedSet beta = oset_split(alpha, &split_key);

    size_t beta_size = oset_size(beta);
    size_t alpha_size = oset_size(alpha);

    OrderedSetNode last = oset_last(beta);

    oset_concat(alpha, beta);

    TEST_CHECK(oset_last(alpha) == last);
    TEST_CHECK(oset_size(alpha) == (alpha_size + beta_size));

    OrderedSetNode node = oset_first(alpha);
    for (int i = 0; i < N; i++) {
        int* key = oset_node_key(alpha, node);
        TEST_CHECK(*key == i);
        node = oset_next(alpha, node);
    }

    oset_destroy(alpha);

    free(key_array);
    free(value_array);
}

TEST_LIST = {
    {"oset_create", test_create},
    {"oset_insert", test_insert},
    {"oset_remove", test_remove},
    {"oset_traversal", test_traversal},
    {"oset_find", test_find},
    {"oset_split", test_split},
    {"oset_merge", test_merge},
    {"oset_concat", test_concat},

    {NULL, NULL}  // End of tests
};

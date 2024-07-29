#include <stddef.h>
#include <string.h>
#include <unity.h>

#include "text/string.h"

void setUp() {
}

void tearDown() {
}

static const char msg[]  = "This message is for unit testing string library";
static const size_t size = sizeof(msg) / sizeof(*msg);

static void test_splice_return_value(void) {
    String* string = string_new();
    string_copy(string, msg);

    {  // test splicing single time
        String* sp0 = string_splice(string, 0, 4);
        TEST_ASSERT_NOT_NULL(sp0);
        TEST_ASSERT_EQUAL(4, string_length(sp0));
        const char* sp0_iter = string_iter(sp0);
        const char* sp0_ptr  = &msg[0];
        TEST_ASSERT_EQUAL_CHAR_ARRAY(msg, sp0_iter, 4);
        string_free(sp0);
    }

    {  // test mut on original string
        const char* iter = string_iter(string);
        const char* ptr  = &msg[4];
        TEST_ASSERT_EQUAL(strlen(ptr), string_length(string));
        TEST_ASSERT_EQUAL_CHAR_ARRAY(ptr, iter, size - 4);
    }

    {  // Test splicing multiple times
        String* sp1 = string_splice(string, 1, 8);
        TEST_ASSERT_NOT_NULL(sp1);
        TEST_ASSERT_EQUAL(7, string_length(sp1));
        const char* sp1_ptr  = &msg[5];
        const char* sp1_iter = string_iter(sp1);
        TEST_ASSERT_EQUAL_CHAR_ARRAY(sp1_ptr, sp1_iter, 7);
        string_free(sp1);
    }

    {  // check that the original string mutated correctly
        const char* iter = string_iter(string);
        const char* ptr  = "  is for unit testing string library";
        TEST_ASSERT_EQUAL(strlen(ptr), string_length(string));
        TEST_ASSERT_EQUAL_CHAR_ARRAY(ptr, iter, size - 11);
    }

    string_free(string);
}

static void test_string_copy(void) {
    String* string = string_new();

    string_copy(string, msg);
    const char* iter = string_iter(string);
    TEST_ASSERT_EQUAL(strlen(iter), string_length(string));
    for (int i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL_CHAR(msg[i], iter[i]);
    }

    string_copy(string, msg);
    iter = string_iter(string);
    TEST_ASSERT_EQUAL(strlen(iter), string_length(string));
    for (int i = 0; i < (size - 1) * 2; i++) {
        int j = i % (size - 1);
        TEST_ASSERT_EQUAL_CHAR(msg[j], iter[i]);
    }

    string_free(string);
}

static void test_string_insert(void) {
    String* string = string_new();

    string_insert(string, 0, 'a');
    string_insert(string, 0, 'b');
    const char* iter = string_iter(string);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ba", iter, 2);
    TEST_ASSERT_EQUAL(2, string_length(string));

    string_insert(string, 1, 'c');
    TEST_ASSERT_EQUAL_CHAR_ARRAY("bca", iter, 3);
    TEST_ASSERT_EQUAL(3, string_length(string));
    string_clear(string);

    for (int i = 0; i < size - 1; i++) {
        string_insert(string, i, msg[i]);
    }

    iter = string_iter(string);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(msg, iter, size - 1);
    TEST_ASSERT_EQUAL(size - 1, string_length(string));

    string_free(string);
}

static void test_string_pop(void) {
    String* string   = string_from(msg);
    const char* iter = string_iter(string);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(msg, iter, size - 1);

    char c;
    while ((c = string_pop(string))) {
        int index = string_length(string);
        TEST_ASSERT_EQUAL_CHAR(msg[index], c);
    }

    TEST_ASSERT_EQUAL(0, string_length(string));
}

static void test_string_remove(void) {
    String* string   = string_from(msg);
    const char* iter = string_iter(string);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(msg, iter, size - 1);

    char c;
    while ((c = string_remove(string, 0))) {
        int index = (size - 2) - string_length(string);
        TEST_ASSERT_EQUAL_CHAR(msg[index], c);
    }

    TEST_ASSERT_EQUAL(0, string_length(string));
}

static void test_string_push(void) {
    String* string = string_new();

    for (int i = 0; i < size - 1; i++) {
        string_push(string, msg[i]);
    }

    const char* iter = string_iter(string);
    TEST_ASSERT_EQUAL(strlen(iter), string_length(string));
    for (int i = 0; i < size - 1; i++) {
        TEST_ASSERT_EQUAL_CHAR(msg[i], iter[i]);
    }

    string_free(string);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_string_push);
    RUN_TEST(test_string_copy);
    RUN_TEST(test_splice_return_value);
    RUN_TEST(test_string_insert);
    RUN_TEST(test_string_pop);
    RUN_TEST(test_string_remove);
    return UNITY_END();
}

#include <string.h>
#include <unity.h>

#include "text/string.h"
#include "text/string_array.h"

void setUp() {
}
void tearDown() {
}

static void test_string_array_insert(void) {
    {  // test double normal insert
        StringArray* arr = string_array_new();
        String* str0     = string_from("Hello, World!");
        String* str1     = string_from("Goodbye, World!");

        string_array_insert(arr, 0, str0);
        string_array_insert(arr, 1, str1);
        TEST_ASSERT_EQUAL(2, string_array_length(arr));

        for (int i = 0; i < string_array_length(arr); i++) {
            const String* str = string_array_at(arr, i);
            const char* iter  = string_iter(str);

            if (i == 0) {
                const char* s0 = string_iter(str0);
                TEST_ASSERT_EQUAL_CHAR_ARRAY(s0, iter, strlen(s0));
            } else {
                const char* s1 = string_iter(str1);
                TEST_ASSERT_EQUAL_CHAR_ARRAY(s1, iter, strlen(s1));
            }
        }

        string_free(str1);
        string_free(str0);
        string_array_free(arr);
    }

    {  // test overflowing normal capacity
        const int count  = 32;
        StringArray* arr = string_array_new();
        String* str      = string_from("Hello, World!");
        for (int i = 0; i < count; i++) {
            string_array_insert(arr, i, str);
        }

        TEST_ASSERT_EQUAL(count, string_array_length(arr));
        for (int i = 0; i < count; i++) {
            const String* s0 = string_array_at(arr, i);
            const char* iter = string_iter(s0);
            const char* base = string_iter(str);
            TEST_ASSERT_EQUAL_CHAR_ARRAY(base, iter, strlen(base));
        }
    }
}

static void test_string_array_remove(void) {
    StringArray* arr = string_array_new();
    String* str0     = string_from("Hello, World!");
    String* str1     = string_from("Goodbye, World!");

    string_array_insert(arr, 0, str0);
    string_array_insert(arr, 1, str1);

    {  // verify the removed element is the first one
        String* str      = string_array_remove(arr, 0);
        const char* base = string_iter(str);
        const char* iter = string_iter(str0);
        TEST_ASSERT_EQUAL_CHAR_ARRAY(base, iter, strlen(base));
    }

    TEST_ASSERT_EQUAL(1, string_array_length(arr));

    {  // verify the last element got moved
        const String* str = string_array_at(arr, 0);
        const char* base  = string_iter(str1);
        const char* iter  = string_iter(str);
        TEST_ASSERT_EQUAL_CHAR_ARRAY(base, iter, strlen(base));
    }

    string_free(str0);
    string_free(str1);
    string_array_free(arr);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_string_array_insert);
    RUN_TEST(test_string_array_remove);
    return UNITY_END();
}

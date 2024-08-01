#include "text/string_array.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_ARRAY_DEFAULT_CAPACITY 16

typedef struct StringArray {
    String** strings;
    size_t capacity;
    size_t length;
} StringArray;

static void try_string_array_expand(StringArray* arr, size_t length) {
    assert(arr != NULL);

    if (length >= arr->capacity) {
        size_t capacity = arr->capacity * 1.5;
        String** temp   = realloc(arr->strings, capacity * sizeof(void*));
        if (temp == NULL) {
            perror("string array expand failed to realloc");
            return;
        }

        arr->strings  = temp;
        arr->capacity = capacity;
    }
}

static void try_string_array_shrink(StringArray* arr) {
    assert(arr != NULL);

    if (arr->capacity > STRING_ARRAY_DEFAULT_CAPACITY) {
        size_t length   = arr->length * 1.5;
        size_t capacity = arr->capacity / 1.5;
        if (length < capacity) {
            String** temp = realloc(arr->strings, capacity * sizeof(void*));
            if (temp == NULL) {
                perror("string array shrink failed to realloc");
                return;
            }

            arr->strings  = temp;
            arr->capacity = capacity;
        }
    }
}

String* string_array_remove(StringArray* arr, size_t index) {
    assert(arr != NULL);
    assert(index < arr->length);
    assert(index >= 0);

    String* result = arr->strings[index];
    if (index < arr->length - 1) {
        size_t len   = (arr->length - index) * sizeof(void*);
        String** dst = &arr->strings[index];
        String** src = &arr->strings[index + 1];
        memmove(dst, src, len);
    }

    try_string_array_shrink(arr);
    arr->strings[arr->length - 1] = NULL;
    arr->length--;
    return result;
}

void string_array_insert(StringArray* arr, size_t index, String* str) {
    assert(arr != NULL);
    assert(str != NULL);
    assert(index <= arr->length);
    assert(index >= 0);

    try_string_array_expand(arr, arr->length + 1);
    if (index < arr->length - 1) {
        size_t len   = (arr->length - index) * sizeof(void*);
        String** dst = &arr->strings[index + 1];
        String** src = &arr->strings[index];
        memmove(dst, src, len);
    }

    arr->strings[index] = str;
    arr->length++;
}

String* string_array_at(const StringArray* arr, size_t index) {
    assert(arr != NULL);
    assert(index >= 0);
    assert(index < arr->length);
    return arr->strings[index];
}

size_t string_array_length(const StringArray* arr) {
    return arr->length;
}

void string_array_free(StringArray* arr) {
    for (int i = 0; i < arr->length; i++) {
        String* str = string_array_at(arr, i);
        free(str);
    }

    free(arr->strings);
    free(arr);
}

StringArray* string_array_new(void) {
    StringArray* arr = malloc(sizeof(*arr));
    arr->capacity    = STRING_ARRAY_DEFAULT_CAPACITY;
    arr->length      = 0;
    arr->strings     = malloc(arr->capacity * sizeof(void*));
    return arr;
}

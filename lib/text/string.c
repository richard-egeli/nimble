#include "text/string.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct String {
    char* buffer;
    size_t length;
    size_t capacity;
} String;

static void try_string_expand(String* str, size_t length) {
    assert(str != NULL);
    size_t temp;
    size_t capacity = str->capacity;
    while (capacity <= length) {
        capacity *= 1.5;
    }

    if (capacity > str->capacity) {
        char* temp = realloc(str->buffer, capacity);
        if (temp == NULL) {
            perror("failed to expand string");
            return;
        }

        str->buffer   = temp;
        str->capacity = capacity;
    }
}

static void try_string_shrink(String* str) {
    assert(str != NULL);
    if (str->capacity <= STRING_DEFAULT_SIZE) return;

    int temp;
    int capacity     = str->capacity;
    const int length = str->length * 1.5;
    while ((temp = capacity / 1.5) > length) {
        if (temp < STRING_DEFAULT_SIZE) break;

        capacity = temp;
    }

    if (capacity != str->capacity) {
        char* temp = realloc(str->buffer, capacity);
        if (temp == NULL) {
            perror("failed to shrink string");
            return;
        }

        str->capacity = capacity;
        str->buffer   = temp;
    }
}

size_t string_length(const String* str) {
    assert(str != NULL);
    return str->length;
}

void string_copy(String* str, const char* copy) {
    assert(str != NULL);
    size_t len = strlen(copy);
    try_string_expand(str, str->length + len);
    memcpy(&str->buffer[str->length], copy, len);

    str->length += len;
    str->buffer[str->length] = '\0';
}

String* string_splice(String* str, size_t start, size_t end) {
    assert(str != NULL);
    assert(start >= 0 && start <= string_length(str));
    assert(end >= 0 && end <= string_length(str));

    int length = end - start;
    if (length <= 0) return NULL;

    String* spliced = string_new();
    if (spliced == NULL) {
        perror("failed to allocate new spliced string");
        return NULL;
    }

    {  // create a temp buffer to copy the substring
        char* temp = malloc(length + 1);
        if (temp == NULL) {
            perror("failed to allocate temp string");
            return NULL;
        }

        memcpy(temp, &str->buffer[start], length);
        temp[length] = '\0';
        string_copy(spliced, temp);
        free(temp);
    }

    {  // shift end of string over the spliced area
        char* dst  = &str->buffer[start];
        char* src  = &str->buffer[end];
        size_t len = str->length - end;
        memmove(dst, src, len);
        str->length -= length;
        str->buffer[str->length] = '\0';
        try_string_shrink(str);
    }

    return spliced;
}

void string_join(String* s1, String* s2) {
    assert(s1 != NULL);
    assert(s2 != NULL);
    string_copy(s1, s2->buffer);
}

void string_clear(String* str) {
    assert(str != NULL);
    if (str->capacity > STRING_DEFAULT_SIZE) {
        char* temp = realloc(str->buffer, STRING_DEFAULT_SIZE);
        if (temp == NULL) {
            perror("failed to clear string buffer");
            return;
        }

        str->buffer   = temp;
        str->capacity = STRING_DEFAULT_SIZE;
    }

    str->buffer[0] = '\0';
    str->length    = 0;
}

void string_push(String* str, char c) {
    assert(str != NULL);

    if (c) {
        try_string_expand(str, str->length + 1);
        str->buffer[str->length + 1] = '\0';
        str->buffer[str->length]     = c;
        str->length++;
    }
}

char string_pop(String* str) {
    assert(str != NULL);

    char result = 0;
    if (str->length > 0) {
        str->length--;
        result                   = str->buffer[str->length];
        str->buffer[str->length] = '\0';
        try_string_shrink(str);
    }

    return result;
}

char string_at(const String* str, size_t index) {
    assert(str != NULL);
    return str->buffer[index];
}

void string_insert(String* str, size_t index, char c) {
    assert(str != NULL);
    assert(index <= str->length);
    assert(index >= 0);

    if (c) {
        try_string_expand(str, str->length + 1);

        if (index < str->length) {
            int len   = str->length - index;
            char* dst = &str->buffer[index + 1];
            char* src = &str->buffer[index];
            memmove(dst, src, len);
        }

        str->length++;
        str->buffer[index]       = c;
        str->buffer[str->length] = '\0';
    }
}

char string_remove(String* str, size_t index) {
    assert(str != NULL);

    char result = 0;
    if (str->length > 0) {
        result    = str->buffer[index];
        int len   = str->length - index;
        char* dst = &str->buffer[index];
        char* src = &str->buffer[index + 1];
        memmove(dst, src, len);
        str->buffer[str->length - 1] = '\0';
        str->length--;
    }

    return result;
}

const char* string_iter(const String* str) {
    assert(str != NULL);
    return str->buffer;
}

void string_free(String* str) {
    assert(str != NULL);
    free(str->buffer);
    free(str);
}

String* string_from(const char* buf) {
    String* string = string_new();
    string_copy(string, buf);
    return string;
}

String* string_new(void) {
    String* string = malloc(sizeof(*string));
    if (string == NULL) {
        perror("failed to malloc new string");
        return NULL;
    }

    string->capacity = STRING_DEFAULT_SIZE;
    string->buffer   = malloc(string->capacity);
    if (string->buffer == NULL) {
        perror("failed to malloc new string buffer");
        return NULL;
    }

    string->buffer[0] = '\0';
    string->length    = 0;
    return string;
}

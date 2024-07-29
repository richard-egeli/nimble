#ifndef TEXT_STRING_H_
#define TEXT_STRING_H_

#include <stddef.h>

#define STRING_DEFAULT_SIZE 16

typedef struct String String;

size_t string_length(const String* str);

String* string_splice(String* str, size_t start, size_t end);

void string_copy(String* str, const char* copy);

void string_join(String* s1, String* s2);

void string_push(String* str, char c);

char string_pop(String* str);

char string_at(const String* str, size_t index);

char string_remove(String* str, size_t index);

void string_clear(String* str);

void string_insert(String* str, size_t index, char c);

const char* string_iter(const String* str);

void string_free(String* str);

String* string_from(const char* buf);

String* string_new(void);

#endif  // TEXT_STRING_H_

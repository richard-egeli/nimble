#ifndef TEXT_STRING_ARRAY_H_
#define TEXT_STRING_ARRAY_H_

#include <stddef.h>

typedef struct String String;

typedef struct StringArray StringArray;

size_t string_array_length(const StringArray* arr);

String* string_array_at(const StringArray* arr, size_t index);

void string_array_insert(StringArray* arr, size_t index, String* str);

String* string_array_remove(StringArray* arr, size_t index);

void string_array_free(StringArray* arr);

StringArray* string_array_new(void);

#endif  // TEXT_STRING_ARRAY_H_

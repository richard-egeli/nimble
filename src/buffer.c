#include "nimble/buffer.h"

#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nimble/text.h"
#include "text/string.h"

static inline double clamp(double d, double min, double max) {
    return fmax(fmin(d, max), min);
}

void buffer_move_up(Buffer* buffer) {
    assert(buffer != NULL);
    const int offset   = buffer->text->line - 1;
    const int count    = string_array_length(buffer->text->lines) - 1;
    buffer->text->line = clamp(offset, 0, count);
}

void buffer_move_down(Buffer* buffer) {
    assert(buffer != NULL);
    const int offset   = buffer->text->line + 1;
    const int max      = string_array_length(buffer->text->lines) - 1;
    buffer->text->line = clamp(offset, 0, max);
}

void buffer_move_left(Buffer* buffer) {
    assert(buffer != NULL && buffer->text != NULL);
    const String* string = string_array_at(buffer->text->lines,
                                           buffer->text->line);
    assert(string != NULL);

    const int offset     = buffer->text->offset - 1;
    buffer->text->offset = clamp(offset, 0, string_length(string) - 1);
}

void buffer_move_right(Buffer* buffer) {
    assert(buffer != NULL && buffer->text != NULL);
    const String* string = string_array_at(buffer->text->lines,
                                           buffer->text->line);
    assert(string != NULL);

    const int offset     = buffer->text->offset + 1;
    const int max        = string_length(string);
    buffer->text->offset = clamp(offset, 0, max);
}

Buffer* buffer_new(const char* filename, const char* content) {
    Buffer* buffer   = calloc(1, sizeof(*buffer));
    buffer->filepath = filename ? strdup(filename) : NULL;
    buffer->text     = text_new();

    text_append(buffer->text, content);
    return buffer;
}

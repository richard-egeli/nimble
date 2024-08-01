#include "nimble/motions.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "nimble/buffer.h"
#include "nimble/editor.h"
#include "nimble/text.h"
#include "text/string.h"

static const char* TEXT_DELIMITERS = "*!#()<>{}[].,/\";'";

void motion_move_word_next(Editor* editor, void* args) {
    Buffer* buffer       = editor->buffers[editor->buffer_index];
    size_t line          = buffer->text->line;
    size_t offset        = buffer->text->offset;
    const String* string = string_array_at(buffer->text->lines, line);
    const char* base     = string_iter(string);
    const char* ptr      = &base[offset];

    if (offset + 1 >= string_length(string)) {
        line += 1;
        offset = 0;
        string = string_array_at(buffer->text->lines, line);
        base   = string_iter(string);
        ptr    = &base[offset];
    } else if (*ptr && strchr(TEXT_DELIMITERS, *ptr)) {
        ptr++;
    } else {
        while (*ptr && !(isspace(*ptr) || strchr(TEXT_DELIMITERS, *ptr))) {
            ptr++;
        }
    }

    while (*ptr && isspace(*ptr)) ptr++;
    buffer->text->offset = fmin(ptr - base, string_length(string));
    buffer->text->line   = line;
}

void motion_move_word_prev(Editor* editor, void* args) {
    Buffer* buffer       = editor->buffers[editor->buffer_index];
    size_t line          = buffer->text->line;
    size_t offset        = buffer->text->offset;
    const String* string = string_array_at(buffer->text->lines, line);
    const char* base     = string_iter(string);
    const char* ptr      = &base[offset];

    while (ptr > base && isspace(*(ptr - 1))) ptr--;

    if (fmax(ptr - base, 0) == 0) {
        if (line == 0) return;

        string = string_array_at(buffer->text->lines, line - 1);
        base   = string_iter(string);
        offset = fmax(string_length(string) - 1, 0);
        ptr    = &base[offset];
        line -= 1;
    } else if (ptr > base && strchr(TEXT_DELIMITERS, *(ptr - 1))) {
        ptr--;
    } else {
        while (ptr > base && !isspace(*(ptr - 1)) &&
               !strchr(TEXT_DELIMITERS, *(ptr - 1))) {
            ptr--;
        }
    }

    buffer->text->line   = line;
    buffer->text->offset = fmax(ptr - base, 0);
}

void motion_move_word_end(Editor* editor, void* args) {
    Buffer* buffer       = editor->buffers[editor->buffer_index];
    size_t line          = buffer->text->line;
    size_t offset        = buffer->text->offset;
    const String* string = string_array_at(buffer->text->lines, line);
    const char* base     = string_iter(string);
    const char* ptr      = &base[offset + 1];

    if (*ptr && ptr[1] && strchr(TEXT_DELIMITERS, ptr[1])) ptr++;
    if (offset + 1 >= string_length(string)) {
        offset = 0;
        line += 1;

        string = string_array_at(buffer->text->lines, line);
        base   = string_iter(string);
        ptr    = &base[offset];
        if (*ptr && strchr(TEXT_DELIMITERS, *ptr)) {
            buffer->text->offset = fmin(ptr - base, string_length(string));
            buffer->text->line   = line;
            return;
        }
    }

    while (*ptr && isspace(*ptr)) ptr++;
    while (*ptr && ptr[1] && !strchr(TEXT_DELIMITERS, ptr[1])) {
        if (isspace(ptr[1])) {
            break;
        }

        ptr++;
    }

    buffer->text->offset = fmin(ptr - base, string_length(string));
    buffer->text->line   = line;
}

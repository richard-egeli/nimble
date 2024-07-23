#include "nimble/motions.h"

#include <ctype.h>
#include <math.h>
#include <string.h>

#include "nimble/buffer.h"
#include "nimble/text.h"

static const char* TEXT_DELIMITERS = "*!#()<>{}[].,/\";'";

void motion_move_word_next(Buffer* buffer) {
    const struct Text* text = buffer->text;
    const char* ptr         = &text->buffer[buffer->text_index];

    if (*ptr && strchr(TEXT_DELIMITERS, *ptr)) {
        ptr++;
    } else {
        while (*ptr && !(isspace(*ptr) || strchr(TEXT_DELIMITERS, *ptr))) {
            ptr++;
        }
    }

    while (*ptr && isspace(*ptr)) {
        ptr++;
    }

    buffer->text_index = fmin(ptr - text->buffer, text->length);
}

void motion_move_word_prev(Buffer* buffer) {
    const struct Text* text = buffer->text;
    const char* ptr         = &text->buffer[buffer->text_index];

    while (ptr > text->buffer && isspace(*(ptr - 1))) {
        ptr--;
    }

    if (ptr > text->buffer && strchr(TEXT_DELIMITERS, *(ptr - 1))) {
        ptr--;
    } else {
        while (ptr > text->buffer && !isspace(*(ptr - 1)) &&
               !strchr(TEXT_DELIMITERS, *(ptr - 1))) {
            ptr--;
        }
    }

    buffer->text_index = fmax(ptr - text->buffer, 0);
}

void motion_move_word_end(Buffer* buffer) {
    const struct Text* text = buffer->text;
    const char* ptr         = &text->buffer[buffer->text_index + 1];
    if (ptr[1] && strchr(TEXT_DELIMITERS, ptr[1])) ptr++;

    while (isspace(*ptr)) ptr++;
    while (ptr[1] && !strchr(TEXT_DELIMITERS, ptr[1])) {
        if (isspace(ptr[1])) {
            break;
        }

        ptr++;
    }

    buffer->text_index = ptr - text->buffer;
}

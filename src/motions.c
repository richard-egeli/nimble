#include "nimble/motions.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "nimble/buffer.h"
#include "nimble/text.h"

static const char* TEXT_DELIMITERS = "*!#()<>{}[].,/\";'";

void motion_move_word_next(Buffer* buffer) {
    const TextLine* line = text_line_get(buffer->text, buffer->text_pos.line);
    const char* ptr      = &line->buffer[buffer->text_pos.offset];

    if (buffer->text_pos.offset >= line->length - 1) {
        buffer->text_pos = (TextPos){buffer->text_pos.line + 1, 0};
        line             = text_line_get(buffer->text, buffer->text_pos.line);
        ptr              = text_line_offset(buffer->text, buffer->text_pos);
    } else if (*ptr && strchr(TEXT_DELIMITERS, *ptr)) {
        ptr++;
    } else {
        while (*ptr && !(isspace(*ptr) || strchr(TEXT_DELIMITERS, *ptr))) {
            ptr++;
        }
    }

    while (*ptr && isspace(*ptr)) ptr++;

    int line_idx   = buffer->text_pos.line;
    int offset_idx = fmin(ptr - line->buffer, line->length - 1);
    buffer_text_pos_set(buffer, line_idx, offset_idx);
}

void motion_move_word_prev(Buffer* buffer) {
    const TextLine* line = text_line_get(buffer->text, buffer->text_pos.line);
    const char* ptr      = text_line_offset(buffer->text, buffer->text_pos);

    while (ptr > line->buffer && isspace(*(ptr - 1))) ptr--;

    if (fmax(ptr - line->buffer, 0) <= 0) {
        if (buffer->text_pos.line == 0) return;

        line = text_line_get(buffer->text, buffer->text_pos.line - 1);
        ptr  = text_line_offset(buffer->text, buffer->text_pos);
        buffer_text_pos_set(buffer, buffer->text_pos.line - 1, 0x7FFFFFFF);
        return;
    } else if (ptr > line->buffer && strchr(TEXT_DELIMITERS, *(ptr - 1))) {
        ptr--;
    } else {
        while (ptr > line->buffer && !isspace(*(ptr - 1)) &&
               !strchr(TEXT_DELIMITERS, *(ptr - 1))) {
            ptr--;
        }
    }

    int line_idx   = buffer->text_pos.line;
    int offset_idx = fmax(ptr - line->buffer, 0);
    buffer_text_pos_set(buffer, line_idx, offset_idx);
}

void motion_move_word_end(Buffer* buffer) {
    const TextLine* line = text_line_get(buffer->text, buffer->text_pos.line);
    const char* ptr      = &line->buffer[buffer->text_pos.offset + 1];
    if (*ptr && ptr[1] && strchr(TEXT_DELIMITERS, ptr[1])) ptr++;

    if (buffer->text_pos.offset >= line->length - 1) {
        buffer->text_pos = (TextPos){buffer->text_pos.line + 1, 0};
        line             = text_line_get(buffer->text, buffer->text_pos.line);
        if (line == NULL) return;

        ptr = &line->buffer[0];
    }

    while (*ptr && isspace(*ptr)) ptr++;
    while (*ptr && ptr[1] && !strchr(TEXT_DELIMITERS, ptr[1])) {
        if (isspace(ptr[1])) {
            break;
        }

        ptr++;
    }

    int offset       = fmin(ptr - line->buffer, line->length - 1);
    buffer->text_pos = (TextPos){buffer->text_pos.line, offset};
}

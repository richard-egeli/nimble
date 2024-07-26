#include "nimble/buffer.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nimble/settings.h"
#include "nimble/text.h"

static inline double clamp(double d, double min, double max) {
    return fmax(fmin(d, max), min);
}

void buffer_move_up(Buffer* buffer) {
    assert(buffer != NULL);
    const int offset      = buffer->text_pos.line - 1;
    buffer->text_pos.line = clamp(offset, 0, buffer->text->line_count - 1);
}

void buffer_move_down(Buffer* buffer) {
    assert(buffer != NULL);
    const int offset      = buffer->text_pos.line + 1;
    buffer->text_pos.line = clamp(offset, 0, buffer->text->line_count - 1);
}

void buffer_move_left(Buffer* buffer) {
    assert(buffer != NULL && buffer->text != NULL);
    TextLine* line = text_line_get(buffer->text, buffer->text_pos.line);
    assert(line != NULL);

    const int offset        = buffer->text_pos.offset - 1;
    buffer->text_pos.offset = clamp(offset, 0, line->length - 1);
}

void buffer_move_right(Buffer* buffer) {
    assert(buffer != NULL && buffer->text != NULL);
    const TextLine* line = text_line_get(buffer->text, buffer->text_pos.line);
    assert(line != NULL);

    const int offset        = buffer->text_pos.offset + 1;
    buffer->text_pos.offset = clamp(offset, 0, line->length);
}

void buffer_text_pos_set(Buffer* buffer, int line, int offset) {
    int line_max              = buffer->text->line_count - 1;
    int line_idx              = clamp(line, 0, line_max);
    const TextLine* text_line = text_line_get(buffer->text, line_idx);
    int line_offset_max       = text_line->length - 1;
    int line_offset_idx       = clamp(offset, 0, line_offset_max);
    buffer->text_pos          = (TextPos){line_idx, line_offset_idx};
}

Buffer* buffer_new(const char* filename, const char* content) {
    Buffer* buffer           = calloc(1, sizeof(*buffer));
    const Settings* settings = settings_get();
    Font font                = settings->text.font;
    int font_size            = settings->text.font_size;
    int line_spacing         = settings->text.line_spacing;
    buffer->filepath         = filename ? strdup(filename) : NULL;
    buffer->text             = text_new(font, font_size, line_spacing);
    buffer->text_index       = 0;
    buffer->text_scrollY     = 0;

    text_append(buffer->text, content);
    return buffer;
}

#include "nimble/buffer.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nimble/settings.h"
#include "nimble/text.h"

void buffer_pop(Buffer* buffer) {
    text_pop(buffer->text, buffer->text_index);
    buffer->text_index--;
    if (buffer->text_index < 0) {
        buffer->text_index = 0;
    }
}

void buffer_push(Buffer* buffer, char c) {
    text_push(buffer->text, buffer->text_index, c);
    buffer->text_index++;
}

void buffer_move_up(Buffer* buffer) {
    Text* text    = buffer->text;
    int start     = text_line_start(text, buffer->text_index);
    int end       = text_line_end(text, buffer->text_index);
    char* c_start = &text->buffer[start];
    char* c_end   = &text->buffer[end];
    if (c_start != text->buffer) {
        char* p_end   = c_start - 1;
        int index     = text_line_start(text, p_end - text->buffer);
        char* p_start = &text->buffer[index];
        int c_len     = c_end - c_start;
        int p_len     = p_end - p_start;
        int c_off     = c_start - text->buffer;      // current line offset
        int p_off     = p_start - text->buffer;      // previous line offset
        int i_off     = buffer->text_index - c_off;  // char index line offset
        int p_max     = fmax(p_off, p_off + p_len - 1);

        assert(buffer->text_index >= c_off);
        buffer->text_index = fmin(p_off + i_off, p_max);
        assert(buffer->text_index <= text->length);
    }
}

void buffer_move_down(Buffer* buffer) {
    Text* text    = buffer->text;
    int start     = text_line_start(text, buffer->text_index);
    int end       = text_line_end(text, buffer->text_index);
    char* c_start = &text->buffer[start];
    char* c_end   = &text->buffer[end];
    if (end != text->length) {
        char* n_start = c_end + 1;
        int index     = text_line_end(text, n_start - text->buffer);
        char* n_end   = &text->buffer[index];
        int c_len     = c_end - c_start;
        int n_len     = n_end - n_start;
        int c_off     = c_start - text->buffer;      // current line offset
        int n_off     = n_start - text->buffer;      // next line offset
        int i_off     = buffer->text_index - c_off;  // char index line offset
        int n_min     = fmax(n_off, n_off + n_len - 1);

        assert(buffer->text_index >= c_off);
        buffer->text_index = fmin(n_off + i_off, n_min);
        assert(buffer->text_index <= text->length);

        char c = buffer->text->buffer[buffer->text_index];
        while (isspace(c) && c != '\n') {
            buffer->text_index++;
            c = buffer->text->buffer[buffer->text_index];
        }
    }
}

void buffer_move_left(Buffer* buffer) {
    if (buffer->text_index > 0) {
        if (buffer->text->buffer[buffer->text_index - 1] != '\n') {
            buffer->text_index--;
        }
    }
}

void buffer_move_right(Buffer* buffer) {
    if (buffer->text_index < buffer->text->length) {
        if (buffer->text->buffer[buffer->text_index + 1] != '\n') {
            buffer->text_index++;
        }
    }
}

Buffer* buffer_new(const char* filename, const char* content) {
    Buffer* buffer           = malloc(sizeof(*buffer));
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

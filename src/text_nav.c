#include "nimble/text_nav.h"

#include <assert.h>
#include <math.h>

#include "nimble/text.h"

void text_move_up(Text* text) {
    int start     = text_line_start(text, text->cursor_index);
    int end       = text_line_end(text, text->cursor_index);
    char* c_start = &text->buffer[start];
    char* c_end   = &text->buffer[end];
    if (c_start != text->buffer) {
        char* p_end   = c_start - 1;
        int index     = text_line_start(text, p_end - text->buffer);
        char* p_start = &text->buffer[index];
        int c_len     = c_end - c_start;
        int p_len     = p_end - p_start;
        int c_off     = c_start - text->buffer;
        int p_off     = p_start - text->buffer;
        int i_off     = text->cursor_index - c_off;

        assert(text->cursor_index >= c_off);
        text->cursor_index = fmin(p_off + i_off, p_off + p_len);
        assert(text->cursor_index <= text->length);
    }
}

void text_move_down(Text* text) {
    int start     = text_line_start(text, text->cursor_index);
    int end       = text_line_end(text, text->cursor_index);
    char* c_start = &text->buffer[start];
    char* c_end   = &text->buffer[end];
    if (end != text->length) {
        char* n_start = c_end + 1;
        int index     = text_line_end(text, n_start - text->buffer);
        char* n_end   = &text->buffer[index];
        int c_len     = c_end - c_start;
        int n_len     = n_end - n_start;
        int c_off     = c_start - text->buffer;
        int n_off     = n_start - text->buffer;
        int i_off     = text->cursor_index - c_off;

        assert(text->cursor_index >= c_off);
        text->cursor_index = fmin(n_off + i_off, n_off + n_len);
        assert(text->cursor_index <= text->length);
    }
}

void text_move_left(Text* text) {
    if (text->cursor_index > 0) {
        text->cursor_index--;
    }
}

void text_move_right(Text* text) {
    if (text->cursor_index < text->length) {
        text->cursor_index++;
    }
}

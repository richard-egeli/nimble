#ifndef INCLUDE_NIMBLE_TEXT_H_
#define INCLUDE_NIMBLE_TEXT_H_

#include <stdbool.h>

#include "raylib.h"

typedef struct Text {
    char* buffer;
    int length;
    Font font;
    float font_size;
    int line_spacing;
} Text;

bool text_valid(const Text* text);

int text_line_index(const Text* text, int index);

int text_character_index(const Text* text, int index);

int text_next_word(const Text* text, int index);

int text_previous_word(const Text* text, int index);

Vector2 text_cursor_pos(const Text* text, int index);

Vector2 text_cursor_size(const Text* text, int index);

int text_line_start(const Text* text, int index);

int text_line_end(const Text* text, int index);

void text_append(Text* text, const char* value);

void text_push(Text* text, int index, char c);

void text_pop(Text* text, int index);

void text_draw(const Text* text, int scrollX, int scrollY);

Text* text_new(Font font, int font_size, int line_spacing);

#endif  // INCLUDE_NIMBLE_TEXT_H_

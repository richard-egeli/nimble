#ifndef INCLUDE_NIMBLE_TEXT_H_
#define INCLUDE_NIMBLE_TEXT_H_

#include <stdbool.h>

#include "raylib.h"

// End Of Line
#define EOL(line) ((TextPos){line, 0x7FFFFFFF})

typedef struct TextPos {
    int line;
    int offset;
} TextPos;

typedef struct TextLine TextLine;

typedef struct TextLine {
    char* buffer;
    int capacity;
    int length;
} TextLine;

typedef struct Text {
    TextLine* lines;
    int line_count;
    Font font;
    float font_size;
    int line_spacing;
} Text;

bool text_valid(const Text* text);

Rectangle text_cursor_get(const Text* text, TextPos pos);

void text_line_pop(Text* text, TextPos pos);

void text_line_push(Text* text, TextPos pos);

char* text_line_offset(const Text* text, TextPos pos);

int text_line_length(const Text* line, int index);

TextLine* text_line_get(const Text* text, int index);

void text_append(Text* text, const char* value);

TextPos text_push(Text* text, TextPos pos, char c);

TextPos text_pop(Text* text, TextPos pos);

void text_draw(const Text* text, int scrollX, int scrollY);

Text* text_new(Font font, int font_size, int line_spacing);

#endif  // INCLUDE_NIMBLE_TEXT_H_

#ifndef INCLUDE_NIMBLE_TEXT_H_
#define INCLUDE_NIMBLE_TEXT_H_

#include <stdbool.h>

#include "raylib.h"
#include "text/string_array.h"

typedef struct TextLine TextLine;

typedef struct Text {
    StringArray* lines;
    size_t line;
    size_t offset;
    Vector2 scroll;
    RenderTexture target;
    bool is_dirty;
} Text;

bool text_valid(const Text* text);

Rectangle text_cursor_get(const Text* text);

void text_append(Text* text, const char* value);

void text_scroll(Text* text, float x, float y);

void text_push(Text* text, char c);

void text_pop(Text* text);

void text_draw(Text* text, int scrollX, int scrollY);

Text* text_new(void);

#endif  // INCLUDE_NIMBLE_TEXT_H_

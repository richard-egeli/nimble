#include "nimble/text.h"

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

static float code_size(int codepoint, Font font, float scale, float spacing) {
    int index = GetGlyphIndex(font, codepoint);

    if (codepoint == '\t') {
        float width = font.recs[index].width;
        return (width * scale + spacing) * 4;
    } else if (font.glyphs[index].advanceX == 0) {
        float width = font.recs[index].width;
        return width * scale + spacing;
    } else {
        float width = font.glyphs[index].advanceX;
        return width * scale + spacing;
    }
}

bool text_valid(const Text* text) {
    return text != NULL && text->buffer != NULL;
}

int text_line_index(const Text* text) {
    int line  = 0;
    char* ptr = text->buffer;
    while ((ptr = strchr(ptr, '\n'))) {
        if (ptr - text->buffer >= text->cursor_index) break;

        line++;
        ptr++;
    }

    return line;
}

int text_character_index(const Text* text) {
    return text->cursor_index - text_line_start(text, text->cursor_index);
}

int text_line_start(const Text* text, int index) {
    const char* ptr = &text->buffer[index];
    for (int i = index - 1; i >= 0; i--) {
        if (text->buffer[i] == '\n') {
            return i + 1;
        }
    }

    return 0;
}

int text_line_end(const Text* text, int index) {
    const char* ptr = &text->buffer[index];
    for (int i = index; i < text->length; i++) {
        if (text->buffer[i] == '\n') {
            return i;
        }
    }

    return text->length;
}

void text_set_cursor(Text* text, int index) {
    text->cursor_index = index;
}

void text_append(Text* text, const char* value) {
    for (int i = 0; i < strlen(value); i++) {
        text_push(text, value[i]);
    }
}

void text_push(Text* text, char c) {
    text->buffer = realloc(text->buffer, text->length + 2);

    int len      = text->length - text->cursor_index;
    char* cursor = &text->buffer[text->cursor_index];
    memmove(&cursor[1], cursor, len);

    text->buffer[text->cursor_index] = c;
    text->buffer[text->length + 1]   = '\0';
    text->cursor_index++;
    text->length++;
}

void text_pop(Text* text) {
    if (text->length <= 0) return;

    text->length--;
    text->cursor_index--;

    int len      = text->length - text->cursor_index;
    char* cursor = &text->buffer[text->cursor_index];
    memmove(cursor, &cursor[1], len);

    text->buffer               = realloc(text->buffer, text->length + 1);
    text->buffer[text->length] = '\0';
}

void text_draw(const Text* text, int scrollX, int scrollY) {
    assert(text_valid(text));

    Font font         = text->font;
    float font_size   = text->font_size;
    float textOffsetX = 0;
    float textOffsetY = 0;
    float scaleFactor = text->font_size / text->font.baseSize;
    int spacing       = text->font_size / 10;

    for (int i = 0; i < text->length;) {
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&text->buffer[i], &codepointByteCount);

        if (codepoint == '\n') {
            textOffsetY += text->font_size + text->line_spacing;
            textOffsetX = 0;
        } else {
            if (codepoint != ' ' && codepoint != '\t') {
                float size  = text->font_size;
                Vector2 pos = {textOffsetX + scrollX, textOffsetY + scrollY};
                DrawTextCodepoint(font, codepoint, pos, size, WHITE);
            }

            textOffsetX += code_size(codepoint, font, scaleFactor, spacing);
        }

        i += codepointByteCount;
    }
}

Vector2 text_cursor_size(const Text* text) {
    int temp      = 0;
    int spacing   = text->font_size / 10;
    float scale   = text->font_size / text->font.baseSize;
    int codepoint = GetCodepointNext(&text->buffer[text->cursor_index], &temp);
    int index     = GetGlyphIndex(text->font, codepoint);
    float width   = code_size(codepoint, text->font, scale, spacing);
    float height  = text->font_size;
    return (Vector2){width, height};
}

Vector2 text_cursor_pos(const Text* text) {
    int lines     = 0;
    char* p_start = text->buffer;
    char* p_end   = text->buffer;

    assert(text_valid(text));
    while ((p_end = strchr(p_end, '\n'))) {
        int index = p_end - text->buffer;
        if (text->cursor_index <= index) break;

        lines++;
        p_end++;
        p_start = p_end;
    }

    int line_len      = text->cursor_index - (p_start - text->buffer);
    float textOffsetX = 0;
    float textOffsetY = (text->font_size + text->line_spacing) * lines;
    float scaleFactor = text->font_size / text->font.baseSize;
    int spacing       = text->font_size / 10;

    for (int i = 0; i < line_len;) {
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&p_start[i], &codepointByteCount);

        textOffsetX += code_size(codepoint, text->font, scaleFactor, spacing);
        i += codepointByteCount;
    }

    return (Vector2){textOffsetX, textOffsetY};
}

Text* text_new(Font font, int font_size, int line_spacing) {
    Text* text         = calloc(1, sizeof(Text));
    text->buffer       = calloc(1, sizeof(char));
    text->line_spacing = line_spacing;
    text->font_size    = font_size;
    text->font         = font;
    return text;
}
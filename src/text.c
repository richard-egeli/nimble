#include "nimble/text.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

static const char* TEXT_DELIMITERS = "*!#()<>{}[].,/\";'";

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
    return text != NULL && text->lines != NULL;
}

void text_append(Text* text, const char* value) {
    TextPos pos = {0};
    for (int i = 0; i < strlen(value); i++) {
        pos = text_push(text, pos, value[i]);
    }
}

static TextLine text_line_create(const Text* text) {
    const int CAPACITY = 16;
    int width          = text->font_size * CAPACITY;
    int height         = text->font_size;
    TextLine line;

    line.capacity  = 16;
    line.length    = 0;
    line.buffer    = malloc(line.capacity);
    line.buffer[0] = '\0';

    return line;
}

static void text_line_free(const TextLine* line) {
    free(line->buffer);
}

char* text_line_offset(const Text* text, TextPos pos) {
    int line_index = fmax(fmin(pos.line, text->line_count - 1), 0);
    TextLine* line = text_line_get(text, line_index);
    int offset     = fmax(fmin(pos.offset, line->length - 1), 0);
    return &line->buffer[offset];
}

TextLine* text_line_get(const Text* text, int index) {
    if (index < text->line_count && index >= 0) {
        return &text->lines[index];
    }

    return NULL;
}

void text_line_push(Text* text, TextPos pos) {
    text->line_count++;
    text->lines = realloc(text->lines, text->line_count * sizeof(TextLine));
    int index   = pos.line;

    if (index + 2 < text->line_count) {
        TextLine* src = text_line_get(text, index + 1);
        TextLine* dst = text_line_get(text, index + 2);
        int len       = (text->line_count - index - 2) * sizeof(TextLine);
        memmove(dst, src, len);
    }

    TextLine* new_line = text_line_get(text, index + 1);
    TextLine* cur_line = text_line_get(text, index);
    *new_line          = text_line_create(text);

    int offset         = fmin(pos.offset, cur_line->length - 1);
    int length         = cur_line->length - offset;

    if (length > 1) {
        while (length >= new_line->capacity - 1) {
            new_line->capacity *= 2;
            char* temp = realloc(new_line->buffer, new_line->capacity);
            if (temp == NULL) {
                perror("failed to realloc for newline");
                exit(EXIT_FAILURE);
            }

            new_line->buffer = temp;
        }

        strcpy(new_line->buffer, &cur_line->buffer[offset]);
        cur_line->buffer[offset] = '\0';
        cur_line->length -= length;
        new_line->length += length;
    }
}

void text_line_pop(Text* text, TextPos pos) {
    assert(pos.line >= 0 && pos.line < text->line_count);

    TextLine* line = text_line_get(text, pos.line);

    if (pos.line < text->line_count - 1) {
        TextLine* src = text_line_get(text, pos.line + 1);
        int index     = text->line_count - (pos.line + 1);
        int len       = index * sizeof(TextLine);
        memmove(line, src, len);
    }

    text->line_count--;
    TextLine* temp = realloc(text->lines, text->line_count * sizeof(TextLine));
    if (temp == NULL) {
        perror("failed to realloc lines");
        exit(EXIT_FAILURE);
    }

    text->lines = temp;
}

TextPos text_push(Text* text, TextPos pos, char c) {
    if (c == '\n') {
        text_line_push(text, pos);
        return (TextPos){pos.line + 1, 0};
    }

    TextLine* line = text_line_get(text, pos.line);
    if (line->length >= line->capacity - 1) {
        line->capacity *= 2;
        char* temp = realloc(line->buffer, line->capacity);
        if (temp == NULL) {
            perror("failed to realloc line buffer");
            exit(EXIT_FAILURE);
        }

        line->buffer = temp;
    }

    int offset   = fmin(pos.offset, line->length);
    char* cursor = &line->buffer[offset];
    int length   = line->length - offset;
    memmove(&cursor[1], cursor, length);

    line->length += 1;
    line->buffer[offset]       = c;
    line->buffer[line->length] = '\0';

    return (TextPos){pos.line, offset + 1};
}

TextPos text_pop(Text* text, TextPos pos) {
    assert(text != NULL);
    assert(pos.line < text->line_count);

    TextLine* line = text_line_get(text, pos.line);
    int offset     = fmax(fmin(pos.offset, line->length), 0);
    if (offset == 0 && pos.line == 0) return pos;

    if (offset == 0) {
        TextLine* prev = text_line_get(text, pos.line - 1);
        int length     = prev->length + line->length;

        while (length >= prev->capacity - 1) {
            prev->capacity *= 2;

            if (length < prev->capacity - 1) {
                char* temp = realloc(prev->buffer, prev->capacity);
                if (temp == NULL) {
                    perror("failed to realloc prev line buffer");
                    exit(EXIT_FAILURE);
                }

                prev->buffer = temp;
            }
        }

        char* src  = line->buffer;
        char* dst  = &prev->buffer[prev->length];
        int offset = prev->length;
        memcpy(dst, src, line->length);
        prev->length               = length;
        prev->buffer[prev->length] = '\0';
        text_line_pop(text, pos);

        return (TextPos){pos.line - 1, offset};
    } else {
        TextLine* line = text_line_get(text, pos.line);
        char* dst      = &line->buffer[pos.offset - 1];
        char* src      = &line->buffer[pos.offset];
        int len        = fmin(line->length - offset, line->length - 1);
        memmove(dst, src, len);

        line->length--;
        line->buffer[line->length] = '\0';
        return (TextPos){pos.line, pos.offset - 1};
    }
}

void text_draw(const Text* text, int scrollX, int scrollY) {
    assert(text_valid(text));

    Font font        = text->font;
    float font_size  = text->font_size;
    int line_spacing = text->line_spacing;
    float scale      = text->font_size / text->font.baseSize;
    int spacing      = text->font_size / 10;
    int offsetX      = 0;
    int offsetY      = scrollY;

    for (int l_idx = 0; l_idx < text->line_count; l_idx++) {
        TextLine* line  = &text->lines[l_idx];
        const char* ptr = line->buffer;

        while (*ptr) {
            int byte_count = 0;
            int codepoint  = GetCodepointNext(ptr, &byte_count);
            if (codepoint != ' ' && codepoint != '\t') {
                Vector2 pos = {offsetX, offsetY};
                DrawTextCodepoint(font, codepoint, pos, font_size, WHITE);
            }

            offsetX += code_size(codepoint, font, scale, spacing);
            ptr += byte_count;
        }

        offsetY += font_size + line_spacing;
        offsetX = 0;
    }
}

Rectangle text_cursor_get(const Text* text, TextPos pos) {
    assert(text_valid(text));

    const TextLine* line = &text->lines[pos.line];
    int temp;
    int codepoint = GetCodepointNext(&line->buffer[pos.offset], &temp);
    float scale   = text->font_size / text->font.baseSize;
    int spacing   = text->font_size / 10;
    float x       = 0;
    float y       = (text->font_size + text->line_spacing) * pos.line;
    float w       = code_size(codepoint, text->font, scale, spacing);
    float h       = text->font_size;

    for (int i = 0; i < fmin(pos.offset, line->length);) {
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&line->buffer[i], &codepointByteCount);

        x += code_size(codepoint, text->font, scale, spacing);
        i += codepointByteCount;
    }

    return (Rectangle){x, y, w, h};
}

Text* text_new(Font font, int font_size, int line_spacing) {
    Text* text         = calloc(1, sizeof(Text));
    text->line_spacing = line_spacing;
    text->font_size    = font_size;
    text->font         = font;
    text->lines        = calloc(1, sizeof(*text->lines));
    text->lines[0]     = text_line_create(text);
    text->line_count   = 1;

    return text;
}

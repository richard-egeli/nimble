#include "nimble/text.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <rlgl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nimble/settings.h"
#include "raylib.h"
#include "text/string.h"
#include "text/string_array.h"

static float code_size(int codepoint, Font font, float spacing) {
    int index = GetGlyphIndex(font, codepoint);

    if (codepoint == '\t') {
        float width = font.recs[index].width;
        return (width + spacing) * 4;
    } else if (font.glyphs[index].advanceX == 0) {
        float width = font.recs[index].width;
        return width + spacing;
    } else {
        float width = font.glyphs[index].advanceX;
        return width + spacing;
    }
}

bool text_valid(const Text* text) {
    return text != NULL && text->lines != NULL;
}

void text_scroll(Text* text, float x, float y) {
    if (x != 0 || y != 0) {
        const TextConfig* cfg = config_get(CONFIG_TEXT);
        assert(text != NULL);

        int padding    = 40;
        int spacing    = cfg->font.baseSize + cfg->line_spacing;
        int line_count = string_array_length(text->lines);
        int max        = line_count * spacing - GetScreenHeight() + padding;
        text->scroll.y = fmax(fmin(text->scroll.y + y, 0), -max);
        text->scroll.x = fmax(fmin(text->scroll.x + x, 0), 0);
        text->is_dirty = true;
    }
}

void text_append(Text* text, const char* value) {
    size_t line   = text->line;
    size_t offset = text->offset;

    for (int i = 0; i < strlen(value); i++) {
        text_push(text, value[i]);
    }

    text->line   = line;
    text->offset = offset;
}

void text_push(Text* text, char c) {
    if (c == '\n') {
        String* new;
        String* cur = string_array_at(text->lines, text->line);
        size_t len  = string_length(cur);

        if (text->offset < len) {
            String* new = string_splice(cur, text->offset, len);
            string_array_insert(text->lines, text->line + 1, new);
        } else {
            string_array_insert(text->lines, text->line + 1, string_new());
        }

        text->line += 1;
        text->offset   = 0;
        text->is_dirty = true;
    } else {
        String* string = string_array_at(text->lines, text->line);
        string_insert(string, text->offset, c);
        text->offset += 1;
        text->is_dirty = true;
    }
}

void text_pop(Text* text) {
    if (text->offset == 0) {
        if (text->line != 0) {
            String* cur    = string_array_remove(text->lines, text->line);
            String* pre    = string_array_at(text->lines, text->line - 1);
            text->offset   = string_length(pre);
            text->is_dirty = true;
            text->line -= 1;
            string_join(pre, cur);
            string_free(cur);
        }
    } else {
        text->offset -= 1;
        text->is_dirty = true;
        String* line   = string_array_at(text->lines, text->line);
        string_remove(line, text->offset);
    }
}

void text_draw(Text* text, int scrollX, int scrollY) {
    assert(text_valid(text));

    if (text->is_dirty) {
        BeginTextureMode(text->target);
        BeginBlendMode(BLEND_ADD_COLORS);
        ClearBackground(BLANK);
        const TextConfig* cfg = config_get(CONFIG_TEXT);
        Font font             = cfg->font;
        float font_size       = cfg->font.baseSize;
        int line_spacing      = cfg->line_spacing;
        int spacing           = cfg->char_spacing;
        int offsetX           = 0;
        int offsetY           = text->scroll.y;

        for (int l_idx = 0; l_idx < string_array_length(text->lines); l_idx++) {
            String* string   = string_array_at(text->lines, l_idx);
            const char* iter = string_iter(string);

            while (*iter) {
                int byte_count = 0;
                int codepoint  = GetCodepointNext(iter, &byte_count);
                if (codepoint != ' ' && codepoint != '\t') {
                    Vector2 pos = {offsetX, offsetY};
                    DrawTextCodepoint(font, codepoint, pos, font_size, WHITE);
                }

                offsetX += code_size(codepoint, font, spacing);
                iter += byte_count;
            }

            offsetY += font_size + line_spacing;
            offsetX = 0;
        }

        text->is_dirty = false;
        EndBlendMode();
        EndTextureMode();
    }

    int w         = text->target.texture.width;
    int h         = text->target.texture.height;
    Vector2 pos   = {0};
    Rectangle src = {0, 0, w, -h};
    DrawTextureRec(text->target.texture, src, pos, WHITE);
}

Rectangle text_cursor_get(const Text* text) {
    assert(text_valid(text));

    const TextConfig* cfg = config_get(CONFIG_TEXT);
    const String* string  = string_array_at(text->lines, text->line);
    const char* iter      = string_iter(string);
    int temp;
    int codepoint = GetCodepointNext(&iter[text->offset], &temp);
    int spacing   = cfg->char_spacing;
    float x       = 0;
    float y       = (cfg->font.baseSize + cfg->line_spacing) * text->line;
    float w       = code_size(codepoint, cfg->font, spacing);
    float h       = cfg->font.baseSize;

    for (int i = 0; i < fmin(text->offset, string_length(string));) {
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&iter[i], &codepointByteCount);

        x += code_size(codepoint, cfg->font, spacing);
        i += codepointByteCount;
    }

    return (Rectangle){x, y + text->scroll.y, w, h};
}

Text* text_new(void) {
    Text* text     = calloc(1, sizeof(Text));
    text->lines    = string_array_new();
    text->target   = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    text->is_dirty = true;
    string_array_insert(text->lines, 0, string_new());

    return text;
}

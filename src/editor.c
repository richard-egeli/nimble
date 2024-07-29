#include "nimble/editor.h"

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#include "lsp/lsp.h"
#include "nimble/buffer.h"
#include "nimble/insert.h"
#include "nimble/normal.h"
#include "nimble/settings.h"
#include "nimble/text.h"
#include "text/string_array.h"

static const char* const MODE_NAME[] = {
    [MODE_NORMAL] = "NORMAL",
    [MODE_INSERT] = "INSERT",
    [MODE_VISUAL] = "VISUAL",
};

char* editor_file_search(Editor* editor, const char* filter) {
    FILE* fp;
    char cmd[MAXPATHLEN];
    snprintf(cmd, sizeof(cmd), "fd | fzf --filter='%s'", filter);

    fp = popen(cmd, "r");
    assert(fp != NULL);

    size_t read     = 0;
    size_t size     = 0;
    size_t capacity = 1024;
    char* content   = malloc(capacity);

    while ((read = fread(content + size, 1, capacity - size, fp)) > 0) {
        size += read;
        if (size >= capacity) {
            capacity *= 2;
            content = realloc(content, capacity);
            if (!content) {
                free(content);
                pclose(fp);
                return NULL;
            }
        }
    }

    if (size == 0) {
        free(content);
        content = NULL;
        pclose(fp);
        return NULL;
    }

    content[size] = '\0';
    pclose(fp);
    return content;
}

void editor_change_mode(Editor* editor, Mode mode) {
    ModeVTable* old = editor->mode_vtable[editor->mode];
    ModeVTable* new = editor->mode_vtable[mode];
    assert(old != NULL && old->on_exit != NULL);
    assert(new != NULL && old->on_enter != NULL);

    old->on_exit(old);
    editor->mode = mode;
    new->on_enter(new);
}

void editor_refresh(const Editor* editor) {
    const Buffer* buffer   = editor->buffers[editor->buffer_index];
    buffer->text->is_dirty = true;
}

void editor_hover_preview(Editor* editor, const char* text) {
    // FBO / RenderTexture are not working as expected in Raylib so has to wait
}

void editor_update(Editor* editor) {
    assert(editor != NULL);
    assert(editor->buffers != NULL);
    assert(editor->buffers[editor->buffer_index] != NULL);

    ModeVTable* vtable = editor->mode_vtable[editor->mode];
    assert(vtable != NULL);
    assert(vtable->update != NULL);
    vtable->update(vtable, editor->buffers[editor->buffer_index]);
}

void editor_scroll(Editor* editor, int x, int y) {
    assert(editor != NULL);
    assert(editor->buffers != NULL);
    assert(editor->buffer_length > 0);
    Buffer* buffer        = editor->buffers[editor->buffer_index];

    const TextConfig* cfg = config_get(CONFIG_TEXT);
    assert(buffer != NULL);

    int padding      = 40;
    int spacing      = cfg->font.baseSize + cfg->line_spacing;
    int line_count   = string_array_length(buffer->text->lines);
    int max          = line_count * spacing - GetScreenHeight() + padding;
    buffer->scroll.y = fmax(fmin(0, buffer->scroll.y + y), -max);
}

void editor_draw_status_bar(const Editor* editor) {
    int width             = GetScreenWidth();
    int height            = GetScreenHeight();
    const Buffer* buffer  = editor->buffers[editor->buffer_index];
    const TextConfig* cfg = config_get(CONFIG_TEXT);

    Rectangle rect   = {.x = 0, .y = height - 16, .width = width, .height = 16};
    Vector2 position = {.x = 12, .y = height - 20};

    char status[32];
    size_t line   = buffer->text->line;
    size_t offset = buffer->text->offset;
    snprintf(status, sizeof(status), "%zu:%zu", line, offset);

    const char* mode = MODE_NAME[editor->mode];
    DrawRectangle(0, height - 24, width, 24, DARKBROWN);
    DrawTextEx(cfg->font, mode, position, 16, cfg->char_spacing, WHITE);

    Vector2 size = MeasureTextEx(cfg->font, status, 16, cfg->char_spacing);
    position.x   = width - size.x - 16;
    DrawTextEx(cfg->font, status, position, 16, cfg->char_spacing, WHITE);
}

void editor_draw_text_cursor(const Editor* editor) {
    Buffer* buffer = editor->buffers[editor->buffer_index];
    Rectangle c    = text_cursor_get(buffer->text);
    DrawRectangle(c.x, c.y, c.width, c.height, BLUE);
}

void editor_draw_text(const Editor* editor) {
    const Buffer* buffer = editor->buffers[editor->buffer_index];
    text_draw(buffer->text, 0, buffer->scroll.y);
}

int editor_open_file(Editor* editor, const char* relative_path) {
    assert(editor != NULL);
    assert(relative_path != NULL);

    char dir[MAXPATHLEN];
    char path[MAXPATHLEN];

    getcwd(dir, sizeof(dir));
    snprintf(path, sizeof(path), "%s/%s", dir, relative_path);

    FILE* file = fopen(path, "r");
    if (file == NULL) return -ENOENT;

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    rewind(file);

    char* content = malloc(length + 1);
    if (content == NULL) return -ENOMEM;
    fread(content, length, 1, file);
    content[length] = '\0';
    fclose(file);

    Buffer* buffer = buffer_new(relative_path, content);
    if (buffer != NULL) {
        editor->buffer_length++;
        int length      = sizeof(Buffer*) * editor->buffer_length;
        editor->buffers = realloc(editor->buffers, length);
        editor->buffers[editor->buffer_length - 1] = buffer;
    }

    lsp_open(path, content);
    free(content);
    return 0;
}

Editor* editor_create(void) {
    Editor* editor                   = calloc(1, sizeof(Editor));
    editor->mode_vtable[MODE_NORMAL] = (void*)create_normal_mode(editor);
    editor->mode_vtable[MODE_INSERT] = (void*)create_insert_mode(editor);

    return editor;
}

#include "nimble/editor.h"

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#include "lsp/lsp.h"
#include "nimble/buffer.h"
#include "nimble/insert.h"
#include "nimble/normal.h"
#include "nimble/text.h"

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
    Buffer* buffer = editor->buffers[editor->buffer_index];
    assert(buffer != NULL);

    buffer->text_scrollY = fmin(0, buffer->text_scrollY + y);
}

void editor_draw_status_bar(const Editor* editor) {
    int width            = GetScreenWidth();
    int height           = GetScreenHeight();
    const Buffer* buffer = editor->buffers[editor->buffer_index];

    Rectangle rect   = {.x = 0, .y = height - 16, .width = width, .height = 16};
    float spacing    = (float)buffer->text->font.baseSize / 10;
    Vector2 position = {.x = 12, .y = height - 20};

    char status[32];
    int line      = text_line_index(buffer->text, buffer->text_index);
    int character = text_character_index(buffer->text, buffer->text_index);
    snprintf(status, sizeof(status), "%d:%d", line, character);

    const char* mode = MODE_NAME[editor->mode];
    DrawRectangle(0, height - 24, width, 24, DARKBROWN);
    DrawTextEx(buffer->text->font, mode, position, 16, spacing, WHITE);

    Vector2 size = MeasureTextEx(buffer->text->font, status, 16, spacing);

    position.x   = width - size.x - 16;
    DrawTextEx(buffer->text->font, status, position, 16, spacing, WHITE);
}

void editor_draw_text_cursor(const Editor* editor) {
    Buffer* buffer = editor->buffers[editor->buffer_index];
    Vector2 c_pos  = text_cursor_pos(buffer->text, buffer->text_index);
    Vector2 c_size = text_cursor_size(buffer->text, buffer->text_index);
    int scrollY    = buffer->text_scrollY;
    DrawRectangle(c_pos.x, c_pos.y + scrollY, c_size.x, c_size.y, BLUE);
}

void editor_draw_text(const Editor* editor) {
    const Buffer* buffer = editor->buffers[editor->buffer_index];
    text_draw(buffer->text, 0, buffer->text_scrollY);
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

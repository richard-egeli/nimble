#include "nimble/action.h"

#include <math.h>
#include <stdio.h>
#include <sys/param.h>
#include <unistd.h>

#include "hashmap/hashmap.h"
#include "lsp/lsp.h"
#include "nimble/buffer.h"
#include "nimble/config.h"
#include "nimble/editor.h"
#include "nimble/motions.h"
#include "text/string.h"
#include "text/string_array.h"

static HashMap* action_map;

static inline double clamp(double d, double min, double max) {
    return fmax(fmin(d, max), min);
}

static void action_lsp_hover(Editor* editor, void* args) {
    Buffer* buffer = editor->buffers[editor->buffer_index];
    Text* text     = buffer->text;

    char dir[MAXPATHLEN];
    char path[MAXPATHLEN];
    size_t line   = buffer->text->line;
    size_t offset = buffer->text->offset;

    getcwd(dir, MAXPATHLEN);
    snprintf(path, sizeof(path), "%s/%s", dir, buffer->filepath);
    lsp_hover(path, text->line, text->offset);
}

static void action_prev_line_and_insert(Editor* editor, void* args) {
    Buffer* buffer = editor->buffers[editor->buffer_index];
    String* str    = string_array_at(buffer->text->lines, buffer->text->line);
    buffer->text->offset = 0;
    editor_change_mode(editor, MODE_INSERT);
    text_push(buffer->text, '\n');
    buffer->text->line -= 1;
}

static void action_new_line_and_insert(Editor* editor, void* args) {
    Buffer* buffer = editor->buffers[editor->buffer_index];
    String* str    = string_array_at(buffer->text->lines, buffer->text->line);
    buffer->text->offset = string_length(str);
    editor_change_mode(editor, MODE_INSERT);
    text_push(buffer->text, '\n');
}

static void action_jump_down(Editor* editor, void* args) {
    Buffer* buffer = editor->buffers[editor->buffer_index];
    for (int i = 0; i < 10; i++) buffer_move_down(buffer);

    const TextConfig* cfg = config_get(CONFIG_TEXT);
    int spacing           = cfg->font.baseSize + cfg->line_spacing;
    text_scroll(buffer->text, 0, -(spacing * 10));
}

static void action_jump_up(Editor* editor, void* args) {
    Buffer* buffer = editor->buffers[editor->buffer_index];
    for (int i = 0; i < 10; i++) buffer_move_up(buffer);

    const TextConfig* cfg = config_get(CONFIG_TEXT);
    int spacing           = cfg->line_spacing + cfg->font.baseSize;
    text_scroll(buffer->text, 0, spacing * 10);
}

static void action_move_down(Editor* editor, void* args) {
    Buffer* buffer     = editor->buffers[editor->buffer_index];
    const int offset   = buffer->text->line + 1;
    const int max      = string_array_length(buffer->text->lines) - 1;
    buffer->text->line = clamp(offset, 0, max);
}

static void action_move_up(Editor* editor, void* args) {
    Buffer* buffer     = editor->buffers[editor->buffer_index];
    const int offset   = buffer->text->line - 1;
    const int max      = string_array_length(buffer->text->lines) - 1;
    buffer->text->line = clamp(offset, 0, max);
}

static void action_move_left(Editor* editor, void* args) {
    Text* text     = editor->buffers[editor->buffer_index]->text;
    String* string = string_array_at(text->lines, text->line);
    text->offset   = clamp((int)text->offset - 1, 0, string_length(string) - 1);
}

static void action_move_right(Editor* editor, void* args) {
    Text* text     = editor->buffers[editor->buffer_index]->text;
    String* string = string_array_at(text->lines, text->line);
    text->offset   = clamp(text->offset + 1, 0, string_length(string));
}

void action_register(const char* key, Action action) {
    hmap_put(action_map, key, action);
}

void action_call(Editor* editor, const char* key, void* args) {
    Action action = NULL;
    if (hmap_get(action_map, key, (void**)&action)) {
        action(editor, args);
    }
}

void action_init(void) {
    action_map = hmap_new();

    action_register("lsp_hover", action_lsp_hover);
    action_register("buffer_new_line_and_insert", action_new_line_and_insert);
    action_register("buffer_prev_line_and_insert", action_prev_line_and_insert);
    action_register("buffer_move_down", action_move_down);
    action_register("buffer_move_up", action_move_up);
    action_register("buffer_move_left", action_move_left);
    action_register("buffer_move_right", action_move_right);
    action_register("buffer_jump_down", action_jump_down);
    action_register("buffer_jump_up", action_jump_up);
    action_register("buffer_move_word_next", motion_move_word_next);
    action_register("buffer_move_word_prev", motion_move_word_prev);
    action_register("buffer_move_word_end", motion_move_word_end);
}

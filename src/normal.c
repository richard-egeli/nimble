#include "nimble/normal.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#include "lsp/lsp.h"
#include "nimble/buffer.h"
#include "nimble/editor.h"
#include "nimble/motions.h"
#include "nimble/settings.h"
#include "nimble/text.h"
#include "raylib.h"

static void on_enter(void* m) {
}

static void on_exit(void* m) {
}

static bool key_is_pressed(int key) {
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

static bool key_and_modifier_pressed(int key, int modifier) {
    return key_is_pressed(key) && IsKeyDown(modifier);
}

static void update(void* m, Buffer* buffer) {
    Normal* mode = (Normal*)m;

    if (key_is_pressed(KEY_I)) editor_change_mode(mode->parent, MODE_INSERT);
    if (key_is_pressed(KEY_J)) buffer_move_down(buffer);
    if (key_is_pressed(KEY_H)) buffer_move_left(buffer);
    if (key_is_pressed(KEY_L)) buffer_move_right(buffer);
    if (key_is_pressed(KEY_W)) motion_move_word_next(buffer);
    if (key_is_pressed(KEY_B)) motion_move_word_prev(buffer);
    if (key_is_pressed(KEY_E)) motion_move_word_end(buffer);

    if (key_and_modifier_pressed(KEY_D, KEY_LEFT_CONTROL)) {
        for (int i = 0; i < 10; i++) buffer_move_down(buffer);

        const Settings* settings = settings_get();
        int spacing = settings->text.line_spacing + settings->text.font_size;
        editor_scroll(mode->parent, 0, -(spacing * 10));
    }

    if (key_is_pressed(KEY_U) && IsKeyDown(KEY_LEFT_CONTROL)) {
        for (int i = 0; i < 10; i++) buffer_move_up(buffer);

        const Settings* settings = settings_get();
        int spacing = settings->text.line_spacing + settings->text.font_size;
        editor_scroll(mode->parent, 0, spacing * 10);
    }

    if (key_is_pressed(KEY_O)) {
        text_line_push(buffer->text, EOL(buffer->text_pos.line));
        buffer_move_down(buffer);
        editor_change_mode(mode->parent, MODE_INSERT);
    }

    if (key_and_modifier_pressed(KEY_X, KEY_LEFT_SHIFT)) {
        text_pop(buffer->text, buffer->text_pos);
        buffer_move_left(buffer);
    } else if (key_is_pressed(KEY_X)) {
        text_pop(buffer->text,
                 (TextPos){buffer->text_pos.line, buffer->text_pos.offset + 1});
    }

    if (IsKeyPressed(KEY_K) && IsKeyDown(KEY_LEFT_SHIFT)) {
        char dir[MAXPATHLEN];
        char path[MAXPATHLEN];
        TextPos pos = buffer->text_pos;

        getcwd(dir, MAXPATHLEN);
        snprintf(path, sizeof(path), "%s/%s", dir, buffer->filepath);
        lsp_hover(path, pos.line, pos.offset);
    } else if (key_is_pressed(KEY_K)) buffer_move_up(buffer);
}

Normal* create_normal_mode(Editor* parent) {
    Normal* mode = calloc(1, sizeof(Normal));
    assert(mode != NULL);

    mode->vtable.update   = update;
    mode->vtable.on_enter = on_enter;
    mode->vtable.on_exit  = on_exit;
    mode->parent          = parent;
    return mode;
}

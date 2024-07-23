#include "nimble/normal.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lsp/lsp.h"
#include "nimble/editor.h"
#include "nimble/text.h"
#include "nimble/text_nav.h"
#include "raylib.h"

static void on_enter(void* m) {
}

static void on_exit(void* m) {
}

static bool key_is_pressed(int key) {
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

static void update(void* m, Text* text) {
    Normal* mode = (Normal*)m;

    if (key_is_pressed(KEY_I)) editor_change_mode(mode->parent, MODE_INSERT);
    if (key_is_pressed(KEY_J)) text_move_down(text);
    if (key_is_pressed(KEY_H)) text_move_left(text);
    if (key_is_pressed(KEY_L)) text_move_right(text);

    if (IsKeyPressed(KEY_K) && IsKeyDown(KEY_LEFT_SHIFT)) {
        const char* p = "/Users/richardegeli/Documents/Development/cvim/main.c";
        int line      = text_line_index(text);
        int character = text_character_index(text);
        lsp_hover(p, line, character);
    } else if (key_is_pressed(KEY_K)) text_move_up(text);
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

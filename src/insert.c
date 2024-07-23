#include "nimble/insert.h"

#include <assert.h>
#include <stdlib.h>

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
    Insert* mode = (Insert*)m;

    int key      = 0;
    while ((key = GetCharPressed()) != 0) {
        text_push(text, key);
    }

    if (key_is_pressed(KEY_ESCAPE)) {
        editor_change_mode(mode->parent, MODE_NORMAL);
    }

    if (key_is_pressed(KEY_TAB)) text_push(text, '\t');
    if (key_is_pressed(KEY_ENTER)) text_push(text, '\n');
    if (key_is_pressed(KEY_BACKSPACE)) text_pop(text);
    if (key_is_pressed(KEY_LEFT)) text_move_left(text);
    if (key_is_pressed(KEY_RIGHT)) text_move_right(text);
    if (key_is_pressed(KEY_DOWN)) text_move_down(text);
    if (key_is_pressed(KEY_UP)) text_move_up(text);
}

Insert* create_insert_mode(Editor* parent) {
    Insert* mode = calloc(1, sizeof(Insert));
    assert(mode != NULL);

    mode->vtable.update   = update;
    mode->vtable.on_enter = on_enter;
    mode->vtable.on_exit  = on_exit;
    mode->parent          = parent;
    return mode;
}

#include "nimble/insert.h"

#include <assert.h>
#include <stdlib.h>

#include "nimble/buffer.h"
#include "nimble/editor.h"
#include "raylib.h"

static void on_enter(void* m) {
}

static void on_exit(void* m) {
}

static bool key_is_pressed(int key) {
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

static void update(void* m, Buffer* buffer) {
    Insert* mode = (Insert*)m;

    int key      = 0;
    while ((key = GetCharPressed()) != 0) {
        buffer_push(buffer, key);
    }

    if (key_is_pressed(KEY_ESCAPE)) {
        editor_change_mode(mode->parent, MODE_NORMAL);
    }

    if (key_is_pressed(KEY_TAB)) buffer_push(buffer, '\t');
    if (key_is_pressed(KEY_ENTER)) buffer_push(buffer, '\n');
    if (key_is_pressed(KEY_BACKSPACE)) buffer_pop(buffer);
    if (key_is_pressed(KEY_LEFT)) buffer_move_left(buffer);
    if (key_is_pressed(KEY_RIGHT)) buffer_move_right(buffer);
    if (key_is_pressed(KEY_DOWN)) buffer_move_down(buffer);
    if (key_is_pressed(KEY_UP)) buffer_move_up(buffer);
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

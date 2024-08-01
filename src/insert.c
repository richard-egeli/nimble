#include "nimble/insert.h"

#include <_ctype.h>
#include <assert.h>
#include <stdlib.h>

#include "nimble/buffer.h"
#include "nimble/config.h"
#include "nimble/editor.h"
#include "nimble/text.h"
#include "raylib.h"
#include "text/string.h"

static void on_enter(void* m) {
}

static void on_exit(void* m) {
}

static bool key_is_pressed(int key) {
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

static void update(Editor* editor) {
    // isascii()
    // Insert* mode = (Insert*)m;

    // int key      = 0;
    // while ((key = GetCharPressed()) != 0) {
    //     text_push(buffer->text, key);
    // }

    // if (key_is_pressed(KEY_ESCAPE)) {
    //     editor_change_mode(mode->parent, MODE_NORMAL);
    // }

    // if (key_is_pressed(KEY_TAB)) {
    //     const TextConfig* cfg = config_get(CONFIG_TEXT);
    //     int len               = cfg->tab_space;
    //     int tot               = len - (buffer->text->offset % len);
    //     for (int i = 0; i < tot; i++) {
    //         text_push(buffer->text, ' ');
    //     }
    // }

    // if (key_is_pressed(KEY_ENTER)) {
    //     const String* string = string_array_at(buffer->text->lines,
    //                                            buffer->text->line);
    //     const char* ptr      = string_iter(string);
    //     int indent           = 0;
    //     while (*ptr && *ptr == ' ') {
    //         indent++;
    //         ptr++;
    //     }

    //     text_push(buffer->text, '\n');

    //     for (int i = 0; i < indent; i++) {
    //         text_push(buffer->text, ' ');
    //     }
    // }

    // if (key_is_pressed(KEY_BACKSPACE)) {
    //     const TextConfig* cfg = config_get(CONFIG_TEXT);
    //     int line              = buffer->text->line;
    //     int offset            = buffer->text->offset;
    //     int tab_space         = cfg->tab_space;
    //     int count             = 1;
    //     const String* string  = string_array_at(buffer->text->lines,
    //                                            buffer->text->line);

    //     if (offset != 0 && (offset % tab_space) == 0) {
    //         bool space_only = true;
    //         for (int i = 0; i < tab_space; i++) {
    //             int index = offset - i;
    //             if (string_at(string, index) != ' ') {
    //                 space_only = false;
    //                 break;
    //             }
    //         }

    //         if (space_only) {
    //             count = tab_space;
    //         }
    //     }

    //     for (int i = 0; i < count; i++) {
    //         text_pop(buffer->text);
    //     }
    // }

    // if (key_is_pressed(KEY_LEFT)) buffer_move_left(buffer);
    // if (key_is_pressed(KEY_RIGHT)) buffer_move_right(buffer);
    // if (key_is_pressed(KEY_DOWN)) buffer_move_down(buffer);
    // if (key_is_pressed(KEY_UP)) buffer_move_up(buffer);
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

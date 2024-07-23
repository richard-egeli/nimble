#include <assert.h>
#include <pthread.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "lsp/lsp.h"
#include "nimble/editor.h"
#include "nimble/settings.h"

int main(int argc, char** argv) {
    InitWindow(800, 600, "Nimble");
    SetTargetFPS(60);
    SetExitKey(0);

    lsp_start();
    settings_init();

    Editor* editor = editor_create();
    editor_open_file(editor, "main.c");

    int scrollY       = 0;
    bool should_close = false;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        editor_update(editor);

        int scroll = GetMouseWheelMove() * 16;

        editor_scroll(editor, 0, scroll);
        editor_draw_text_cursor(editor);
        editor_draw_text(editor);
        editor_draw_status_bar(editor);

        if (IsKeyPressed(KEY_S) && IsKeyDown(KEY_LEFT_SUPER)) {
            printf("Save File\n");
            // save_text(buffer->text);
        }

        EndDrawing();
    }

    return 0;
}

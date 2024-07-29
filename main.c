#include <assert.h>
#include <pthread.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lsp/lsp.h"
#include "nimble/editor.h"
#include "nimble/settings.h"

int main(int argc, char** argv) {
    InitWindow(1280, 768, "Nimble");
    SetTargetFPS(60);
    SetExitKey(0);

    config_init();
    lsp_start();

    Editor* editor = editor_create();
    editor_open_file(editor, "main.c");

    while (!WindowShouldClose()) {
        editor_update(editor);

        BeginDrawing();
        ClearBackground(BLACK);

        int wheel = GetMouseWheelMove();
        if (wheel != 0) {
            config_font_change(wheel);
            editor_refresh(editor);
        }

        LSP_Event* event;
        while ((event = lsp_poll()) != NULL) {
            printf("Got event on main %d\n", event->type);
            if (event->type == 2) {
                printf("Kind: %s\nValue:\n%s\n",
                       event->data.hover.kind,
                       event->data.hover.value);
            }

            lsp_free(event);
        }

        editor_draw_text_cursor(editor);
        editor_draw_text(editor);
        editor_draw_status_bar(editor);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

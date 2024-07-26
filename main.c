#include <assert.h>
#include <pthread.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lsp/lsp.h"
#include "nimble/editor.h"
#include "nimble/settings.h"
#include "rlgl.h"

#define MAX_BUFFER 1024

static void DrawTxt(const char* text, Vector2 position) {
    const Settings* settings = settings_get();
    Font font                = settings->text.font;
    float spacing            = (float)font.baseSize / 10;
    DrawTextEx(font, text, position, font.baseSize, spacing, WHITE);
}

static void DrawRTex(RenderTexture rt, Vector2 position) {
    int w = rt.texture.width;
    int h = rt.texture.height;
    DrawTextureRec(rt.texture, (Rectangle){0, 0, w, -h}, position, WHITE);
}

int main(int argc, char** argv) {
    InitWindow(1280, 768, "Nimble");
    SetTargetFPS(60);
    SetExitKey(0);

    settings_init();
    lsp_start();

    Editor* editor           = editor_create();
    const Settings* settings = settings_get();
    editor_open_file(editor, "main.c");

    while (!WindowShouldClose()) {
        editor_update(editor);

        BeginDrawing();
        ClearBackground(BLACK);

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

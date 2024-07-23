#include <assert.h>
#include <pthread.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lsp/lsp.h"
#include "nimble/editor.h"
#include "nimble/text.h"

static void save_text(const Text* text) {
    FILE* fd = fopen("sample.txt", "w+");
    fwrite(text->buffer, text->length, 1, fd);
    fclose(fd);
}

static void draw_statusbar(Editor* editor, Text* text) {
    int width      = GetScreenWidth();
    int height     = GetScreenHeight();
    Rectangle rect = {
        .x      = 0,
        .y      = height - 16,
        .width  = width,
        .height = 16,
    };

    static char* mode_name[] = {
        [MODE_NORMAL] = "NORMAL",
        [MODE_INSERT] = "INSERT",
        [MODE_VISUAL] = "VISUAL",
    };

    float spacing    = (float)text->font.baseSize / 10;
    Vector2 position = {
        .x = 12,
        .y = height - 20,
    };

    char buffer[32];
    int line      = text_line_index(text);
    int character = text_character_index(text);
    snprintf(buffer, sizeof(buffer), "%d:%d", line, character);

    DrawRectangle(0, height - 24, width, 24, DARKBROWN);
    DrawTextEx(text->font,
               mode_name[editor->mode],
               position,
               16,
               spacing,
               WHITE);

    Vector2 size = MeasureTextEx(text->font, buffer, 16, spacing);

    position.x   = width - size.x - 16;
    DrawTextEx(text->font, buffer, position, 16, spacing, WHITE);
}

static const char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    char* content;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    content = malloc(size + 1);
    fread(content, size, 1, file);
    content[size] = '\0';
    return content;
}

int main(int argc, char** argv) {
    lsp_start();
    const char* name = "/Users/richardegeli/Documents/Development/cvim/main.c";
    const char* content = read_file(name);

    lsp_open(name, content);

    InitWindow(800, 600, "CVim");
    SetTargetFPS(60);
    SetExitKey(0);

    Font font = LoadFontEx(
        "/opt/homebrew/Caskroom/font-mononoki-nerd-font/3.2.1/"
        "MononokiNerdFont-Regular.ttf",
        16,
        NULL,
        0);

    SetTextureFilter(font.texture, TEXTURE_FILTER_ANISOTROPIC_16X);
    Text* text     = text_new(font, 16, 2);
    Editor* editor = editor_create();

    text_append(text, content);
    text->cursor_index = 0;

    int scrollY        = 0;
    bool should_close  = false;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        editor_update(editor, text);

        scrollY += GetMouseWheelMove() * 16;
        if (scrollY > 0) scrollY = 0;

        Vector2 c_pos  = text_cursor_pos(text);
        Vector2 c_size = text_cursor_size(text);
        DrawRectangle(c_pos.x, c_pos.y + scrollY, c_size.x, c_size.y, BLUE);

        text_draw(text, 0, scrollY);
        draw_statusbar(editor, text);

        if (IsKeyPressed(KEY_S) && IsKeyDown(KEY_LEFT_SUPER)) {
            printf("Save File\n");
            save_text(text);
        }

        EndDrawing();
    }
    return 0;
}

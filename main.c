#include <GLFW/glfw3.h>
#include <assert.h>
#include <ctype.h>
#include <pthread.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lsp/lsp.h"
#include "nimble/action.h"
#include "nimble/config.h"
#include "nimble/editor.h"
#include "nimble/ini.h"

typedef struct Key {
    uint8_t key;
    uint8_t action;
    uint8_t modifier;
} Key;

static Key input_buffer[16];
static size_t input_buffer_length;

static void custom_key(GLFWwindow* window,
                       int key,
                       int scancode,
                       int action,
                       int mods) {
    if (action != GLFW_REPEAT && action != GLFW_PRESS) return;

    const size_t input_max = sizeof(input_buffer) / sizeof(*input_buffer);
    if (input_buffer_length < input_max && key < 255) {
        input_buffer[input_buffer_length] = (Key){
            .action   = action,
            .modifier = mods,
            .key      = key,
        };

        input_buffer_length++;
    }
}

static Key* poll_input(void) {
    if (input_buffer_length > 0) {
        input_buffer_length--;
        return &input_buffer[input_buffer_length];
    }

    return NULL;
}

static const char* get_modifier(const Key* key) {
    switch (key->modifier) {
        case GLFW_MOD_CONTROL | GLFW_MOD_SHIFT:
            return "<ctrl-shift>";
        case GLFW_MOD_CONTROL:
            return "<ctrl>";
        case GLFW_MOD_SHIFT:
            return "<shift>";
        default:
            return "";
    }
}

int main(int argc, char** argv) {
    Settings* settings = ini_load("settings.ini");
    Section* window    = ini_section_get(settings, "window");
    int height         = ini_property_get_int(window, "height");
    int width          = ini_property_get_int(window, "width");
    int refresh_rate   = ini_property_get_int(window, "refresh_rate");

    InitWindow(width, height, "Nimble");
    SetTargetFPS(refresh_rate);
    SetExitKey(0);

    GLFWwindow* glfw_window = glfwGetCurrentContext();
    glfwSetKeyCallback(glfw_window, custom_key);

    config_init();
    action_init();
    lsp_start();

    Editor* editor = editor_create();
    editor_open_file(editor, "main.c");
    editor->settings = settings;

    while (!WindowShouldClose()) {
        editor_update(editor);

        BeginDrawing();
        ClearBackground(BLACK);

        int wheel = GetMouseWheelMove();
        if (wheel != 0) {
            config_font_change(wheel);
            editor_refresh(editor);
        }

        Key* key;
        Section* section = ini_section_get(settings, "keymap.normal");
        while ((key = poll_input()) != NULL) {
            char buffer[64];
            const char* shift = (key->modifier & GLFW_MOD_SHIFT) ? "shift" : "";
            const char* ctrl = (key->modifier & GLFW_MOD_CONTROL) ? "ctrl" : "";
            const char* mod  = get_modifier(key);
            snprintf(buffer, sizeof(buffer), "%s%c", mod, tolower(key->key));
            const char* action = ini_property_get_str(section, buffer);
            if (action != NULL) {
                action_call(editor, action, NULL);
            }
        }

        LSP_Event* event;
        while ((event = lsp_poll()) != NULL) {
            switch (event->type) {
                case LSP_EVENT_TYPE_HOVER:
                    editor_hover_preview(editor, event->data.hover.value);
                    break;
                default:
                    break;
            }

            lsp_free(event);
        }

        editor_draw(editor);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

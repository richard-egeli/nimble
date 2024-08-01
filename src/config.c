#include "nimble/config.h"

#include <math.h>
#include <stdio.h>

#include "raylib.h"

#define FONT_CONFIG_COUNT 12
#define FONT_CONFIG_STEP  4
#define FONT_CONFIG_MIN   12

static struct {
    TextConfig text[FONT_CONFIG_COUNT];
    int text_index;
} config;

static const char* DEFAULT_FONT =
    "/opt/homebrew/Caskroom/font-mononoki-nerd-font/3.2.1/"
    "MononokiNerdFontMono-Regular.ttf";

const void* config_get(ConfigType type) {
    switch (type) {
        case CONFIG_TEXT:
            return &config.text[config.text_index];
        default:
            return NULL;
    }
}

void config_font_change(int step) {
    int i   = fmax(fmin(step, 1), -1);
    int new = fmax(fmin(config.text_index + i, FONT_CONFIG_COUNT - 1), 0);
    config.text_index = new;
}

void config_init(void) {
    for (int i = 0; i < FONT_CONFIG_COUNT; i++) {
        int size                    = FONT_CONFIG_MIN + (i * FONT_CONFIG_STEP);
        config.text[i].font         = LoadFontEx(DEFAULT_FONT, size, NULL, 0);
        config.text[i].font_filter  = TEXTURE_FILTER_POINT;
        config.text[i].tab_space    = 4;
        config.text[i].line_spacing = 2;
        config.text[i].char_spacing = 1;
        SetTextureFilter(config.text[i].font.texture,
                         config.text[i].font_filter);
    }

    config.text_index = 1;
}

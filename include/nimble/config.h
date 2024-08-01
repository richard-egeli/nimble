#ifndef INCLUDE_NIMBLE_CONFIG_H_
#define INCLUDE_NIMBLE_CONFIG_H_

#include <stddef.h>

#include "raylib.h"

typedef struct TextConfig {
    Font font;
    int font_filter;
    size_t line_spacing;
    size_t char_spacing;
    size_t tab_space;
} TextConfig;

typedef struct Config {
    union {
        TextConfig* text;
    };
} Config;

typedef enum ConfigType {
    CONFIG_TEXT,
} ConfigType;

void config_font_change(int step);

const void* config_get(ConfigType type);

void config_init(void);

#endif  // INCLUDE_NIMBLE_CONFIG_H_

#ifndef INCLUDE_NIMBLE_SETTINGS_H_
#define INCLUDE_NIMBLE_SETTINGS_H_

#include "raylib.h"

typedef struct Settings {
    struct {
        Font font;
        int font_size;
        int line_spacing;
        int filter;
    } text;
} Settings;

const Settings* const settings_get(void);

void settings_init(void);

#endif  // INCLUDE_NIMBLE_SETTINGS_H_

#include "nimble/settings.h"

#include <stdio.h>

#include "raylib.h"

static Settings settings;

static const char* DEFAULT_FONT =
    "/opt/homebrew/Caskroom/font-mononoki-nerd-font/3.2.1/"
    "MononokiNerdFont-Regular.ttf";

const Settings* const settings_get(void) {
    return &settings;
}

void settings_init(void) {
    settings.text.font         = LoadFontEx(DEFAULT_FONT, 16, NULL, 0);
    settings.text.filter       = TEXTURE_FILTER_ANISOTROPIC_16X;
    settings.text.font_size    = 16;
    settings.text.line_spacing = 2;
    SetTextureFilter(settings.text.font.texture, settings.text.filter);
}

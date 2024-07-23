#ifndef INCLUDE_CVIM_EDITOR_H_
#define INCLUDE_CVIM_EDITOR_H_

#include "nimble/text.h"

typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_VISUAL,
    MODE_MAX,
} Mode;

typedef struct {
    void (*on_enter)(void* mode);
    void (*on_exit)(void* mode);
    void (*update)(void* vtable, Text* text);
} ModeVTable;

typedef struct {
    Mode mode;
    ModeVTable* mode_vtable[MODE_MAX];
} Editor;

int editor_open_file(Editor* editor, const char* relative_path);

void editor_change_mode(Editor* editor, Mode mode);

void editor_update(Editor* editor, Text* text);

Editor* editor_create(void);

#endif  // INCLUDE_CVIM_EDITOR_H_

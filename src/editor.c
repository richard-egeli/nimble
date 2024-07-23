#include "nimble/editor.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#include "nimble/insert.h"
#include "nimble/normal.h"

void editor_change_mode(Editor* editor, Mode mode) {
    ModeVTable* old = editor->mode_vtable[editor->mode];
    ModeVTable* new = editor->mode_vtable[mode];
    assert(old != NULL && old->on_exit != NULL);
    assert(new != NULL && old->on_enter != NULL);

    old->on_exit(old);
    editor->mode = mode;
    new->on_enter(new);
}

void editor_update(Editor* editor, Text* text) {
    ModeVTable* vtable = editor->mode_vtable[editor->mode];
    assert(vtable != NULL);
    vtable->update(vtable, text);
}

int editor_open_file(Editor* editor, const char* relative_path) {
    char dir[MAXPATHLEN];
    char path[MAXPATHLEN];

    getcwd(dir, MAXPATHLEN);
    snprintf(path, sizeof(path), "%s/%s", dir, relative_path);

    // TODO: better error handling
    FILE* file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    rewind(file);

    char* content = malloc(length + 1);
    fread(content, length, 1, file);

    fclose(file);

    return 0;
}

Editor* editor_create(void) {
    Editor* editor                   = calloc(1, sizeof(Editor));
    editor->mode_vtable[MODE_NORMAL] = (void*)create_normal_mode(editor);
    editor->mode_vtable[MODE_INSERT] = (void*)create_insert_mode(editor);

    return editor;
}

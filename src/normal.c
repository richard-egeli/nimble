#include "nimble/normal.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#include "nimble/editor.h"
#include "nimble/motions.h"

static void on_enter(void* m) {
}

static void on_exit(void* m) {
}

static void update(Editor* editor) {
}

Normal* create_normal_mode(Editor* parent) {
    Normal* mode = calloc(1, sizeof(Normal));
    assert(mode != NULL);

    mode->vtable.update   = update;
    mode->vtable.on_enter = on_enter;
    mode->vtable.on_exit  = on_exit;
    mode->parent          = parent;
    return mode;
}

#ifndef INCLUDE_NIMBLE_NORMAL_H_
#define INCLUDE_NIMBLE_NORMAL_H_

#include "nimble/editor.h"

typedef struct {
    ModeVTable vtable;
    Editor* parent;
} Normal;

Normal* create_normal_mode(Editor* parent);

#endif  // INCLUDE_NIMBLE_NORMAL_H_

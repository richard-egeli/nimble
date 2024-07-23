#ifndef INCLUDE_CVIM_INSERT_H_
#define INCLUDE_CVIM_INSERT_H_

#include "nimble/editor.h"

typedef struct {
    ModeVTable vtable;
    Editor* parent;
} Insert;

Insert* create_insert_mode(Editor* parent);

#endif  // INCLUDE_CVIM_INSERT_H_

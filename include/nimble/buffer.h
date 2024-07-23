#ifndef INCLUDE_CVIM_BUFFER_H_
#define INCLUDE_CVIM_BUFFER_H_

#include "nimble/text.h"

typedef struct {
    Text* text;
    const char* filename;
} Buffer;

#endif  // INCLUDE_CVIM_BUFFER_H_

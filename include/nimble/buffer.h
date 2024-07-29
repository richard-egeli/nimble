#ifndef INCLUDE_NIMBLE_BUFFER_H_
#define INCLUDE_NIMBLE_BUFFER_H_

#include "nimble/text.h"

typedef struct Buffer {
    const char* filepath;
    Text* text;
    Vector2 scroll;
} Buffer;

void buffer_move_up(Buffer* buffer);

void buffer_move_down(Buffer* buffer);

void buffer_move_left(Buffer* buffer);

void buffer_move_right(Buffer* buffer);

Buffer* buffer_new(const char* filename, const char* content);

#endif  // INCLUDE_NIMBLE_BUFFER_H_

#ifndef INCLUDE_NIMBLE_BUFFER_H_
#define INCLUDE_NIMBLE_BUFFER_H_

typedef struct Text Text;

typedef struct Buffer {
    const char* filepath;
    Text* text;
    int text_index;
    int text_scrollY;
} Buffer;

void buffer_push(Buffer* buffer, char c);

void buffer_pop(Buffer* buffer);

void buffer_move_up(Buffer* buffer);

void buffer_move_down(Buffer* buffer);

void buffer_move_left(Buffer* buffer);

void buffer_move_right(Buffer* buffer);

Buffer* buffer_new(const char* filename, const char* content);

#endif  // INCLUDE_NIMBLE_BUFFER_H_

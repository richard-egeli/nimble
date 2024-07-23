#ifndef INCLUDE_NIMBLE_MOTIONS_H_
#define INCLUDE_NIMBLE_MOTIONS_H_

typedef struct Buffer Buffer;

void motion_move_word_next(Buffer* buffer);

void motion_move_word_prev(Buffer* buffer);

void motion_move_word_end(Buffer* buffer);

#endif  // INCLUDE_NIMBLE_MOTIONS_H_

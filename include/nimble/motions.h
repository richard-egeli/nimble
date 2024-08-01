#ifndef INCLUDE_NIMBLE_MOTIONS_H_
#define INCLUDE_NIMBLE_MOTIONS_H_

typedef struct Editor Editor;

void motion_move_word_next(Editor* editor, void* args);

void motion_move_word_prev(Editor* editor, void* args);

void motion_move_word_end(Editor* editor, void* args);

#endif  // INCLUDE_NIMBLE_MOTIONS_H_

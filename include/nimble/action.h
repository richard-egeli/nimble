#ifndef INCLUDE_NIMBLE_ACTION_H_
#define INCLUDE_NIMBLE_ACTION_H_

typedef struct Editor Editor;

typedef void (*Action)(Editor* editor, void* args);

void action_register(const char* key, Action action);

void action_call(Editor* editor, const char* key, void* args);

void action_init(void);

#endif  // INCLUDE_NIMBLE_ACTION_H_

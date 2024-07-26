#ifndef INCLUDE_CVIM_LSP_H_
#define INCLUDE_CVIM_LSP_H_

#include <stdbool.h>
typedef enum {
    LSP_EVENT_TYPE_INIT,
    LSP_EVENT_TYPE_OPEN,
    LSP_EVENT_TYPE_HOVER,
} LSP_EventType;

typedef struct {
    char* kind;
    char* value;
} LSP_EventHover;

typedef struct {
    LSP_EventType type;
    union {
        LSP_EventHover hover;
    } data;
} LSP_Event;

typedef struct {
    LSP_Event** queue;
    int length;
} LSP_EventQueue;

void lsp_hover(const char* path, int line, int character);

void lsp_open(const char* path, const char* content);

LSP_Event* lsp_poll(void);

void lsp_free(LSP_Event* event);

void lsp_start(void);

#endif  // INCLUDE_CVIM_LSP_H_

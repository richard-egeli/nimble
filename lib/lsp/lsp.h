#ifndef INCLUDE_CVIM_LSP_H_
#define INCLUDE_CVIM_LSP_H_

typedef enum {
    LSP_EVENT_TYPE_HOVER,
} LSP_EventType;

typedef struct {
    int id;
    const char* text;
} LSP_EventHover;

typedef struct {
    LSP_EventType type;
    union {
        LSP_EventHover hover;
    } payload;
} LSP_Event;

typedef struct {
    LSP_Event** queue;
    int length;
} LSP_EventQueue;

void lsp_hover(const char* path, int line, int character);

void lsp_open(const char* path, const char* content);

void lsp_start(void);

#endif  // INCLUDE_CVIM_LSP_H_

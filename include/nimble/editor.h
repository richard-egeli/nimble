#ifndef INCLUDE_NIMBLE_EDITOR_H_
#define INCLUDE_NIMBLE_EDITOR_H_

typedef struct Buffer Buffer;

typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_VISUAL,
    MODE_MAX,
} Mode;

typedef struct {
    void (*on_enter)(void* mode);
    void (*on_exit)(void* mode);
    void (*update)(void* vtable, Buffer* buffer);
} ModeVTable;

typedef struct {
    Mode mode;
    ModeVTable* mode_vtable[MODE_MAX];
    Buffer** buffers;
    int buffer_length;
    int buffer_index;
} Editor;

char* editor_file_search(Editor* editor, const char* filter);

int editor_open_file(Editor* editor, const char* relative_path);

void editor_change_mode(Editor* editor, Mode mode);

void editor_hover_preview(Editor* editor, const char* text);

void editor_update(Editor* editor);

void editor_refresh(const Editor* editor);

void editor_scroll(Editor* editor, int x, int y);

void editor_draw_text_cursor(const Editor* editor);

void editor_draw_text(const Editor* editor);

void editor_draw(const Editor* editor);

void editor_draw_status_bar(const Editor* editor);

Editor* editor_create(void);

#endif  // INCLUDE_NIMBLE_EDITOR_H_

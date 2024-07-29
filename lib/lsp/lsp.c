#include "lsp.h"

#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "message_queue.h"
#include "yyjson.h"

static FILE* input_pipe;
static FILE* output_pipe;
static pthread_t listener_thread;
static MessageQueue message_queue;

static atomic_int event_type[64];

static int lsp_get_id(void) {
    static int id = 0;
    const int max = sizeof(event_type) / sizeof(*event_type);
    return (id = (id + 1) % max), id;
}

static void lsp_deserialise_hover(yyjson_doc* doc) {
    yyjson_val* root     = yyjson_doc_get_root(doc);
    yyjson_val* result   = yyjson_obj_get(root, "result");
    yyjson_val* contents = yyjson_obj_get(result, "contents");
    yyjson_val* kind     = yyjson_obj_get(contents, "kind");
    yyjson_val* value    = yyjson_obj_get(contents, "value");

    if (yyjson_is_str(value) && yyjson_is_str(kind)) {
        const char* k    = yyjson_get_str(kind);
        const char* v    = yyjson_get_str(value);

        LSP_Event* event = malloc(sizeof(*event));
        if (event == NULL) {
            perror("failed to allocate hover event response");
            return;
        }

        printf("DESERIALISE: %s \n%s\n", k, v);
        event->type             = LSP_EVENT_TYPE_HOVER;
        event->data.hover.kind  = strdup(k);
        event->data.hover.value = strdup(v);
        assert(event->data.hover.kind != NULL);
        assert(event->data.hover.value != NULL);
        msgq_enqueue(&message_queue, event);
    }
}

static void lsp_deserialise(LSP_EventType type, yyjson_doc* doc) {
    switch (type) {
        case LSP_EVENT_TYPE_INIT:
            break;
        case LSP_EVENT_TYPE_OPEN:
            break;
        case LSP_EVENT_TYPE_HOVER:
            lsp_deserialise_hover(doc);
            break;
    }
}

// Function to send a message to the LSP server
static void lsp_send(LSP_EventType type, yyjson_mut_doc* doc) {
    yyjson_mut_val* root   = yyjson_mut_doc_get_root(doc);
    yyjson_mut_val* id_obj = yyjson_mut_obj_get(root, "id");
    if (yyjson_mut_is_int(id_obj)) {
        int id = yyjson_mut_get_int(id_obj);
        atomic_store(&event_type[id], type);
    }

    size_t len = 0;
    char* json = yyjson_mut_write(doc, 0, &len);
    if (json) {
        fprintf(input_pipe, "Content-Length: %zu\r\n\r\n%s", len, json);
        fflush(input_pipe);
        free(json);
    }
}

// Function to receive a message from the LSP server
static char* lsp_recv(void) {
    char header[128];

    while (fgets(header, sizeof(header), output_pipe)) {
        int length = 0;
        if (sscanf(header, "Content-Length: %d", &length) == 1) {
            char buffer[length + 1];

            // skip \r\n
            fgetc(output_pipe);
            fgetc(output_pipe);
            fread(buffer, length, 1, output_pipe);
            buffer[length]   = '\0';

            yyjson_doc* doc  = yyjson_read(buffer, length, 0);
            yyjson_val* root = yyjson_doc_get_root(doc);
            yyjson_val* id   = yyjson_obj_get(root, "id");
            if (yyjson_is_int(id)) {
                int i    = yyjson_get_int(id);
                int type = atomic_load(&event_type[i]);
                lsp_deserialise(type, doc);
            }

            yyjson_doc_free(doc);
        }

        fpurge(output_pipe);
    }

    return NULL;
}

void lsp_open(const char* path, const char* content) {
    assert(input_pipe != NULL);
    assert(path != NULL);
    assert(content != NULL);

    yyjson_mut_doc* doc  = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);
    yyjson_mut_obj_add_str(doc, root, "jsonrpc", "2.0");

    yyjson_mut_obj_add_str(doc, root, "method", "textDocument/didOpen");
    yyjson_mut_val* params = yyjson_mut_obj_add_obj(doc, root, "params");
    yyjson_mut_val* text = yyjson_mut_obj_add_obj(doc, params, "textDocument");

    char uri[strlen(path) + 8];
    snprintf(uri, sizeof(uri), "%s%s", "file://", path);
    yyjson_mut_obj_add_str(doc, text, "uri", uri);
    yyjson_mut_obj_add_str(doc, text, "languageId", "c");
    yyjson_mut_obj_add_int(doc, text, "version", 1);
    yyjson_mut_obj_add_str(doc, text, "text", content);

    lsp_send(LSP_EVENT_TYPE_OPEN, doc);
    yyjson_mut_doc_free(doc);
}

void lsp_hover(const char* path, int line, int character) {
    yyjson_mut_doc* doc  = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);
    yyjson_mut_obj_add_str(doc, root, "jsonrpc", "2.0");
    yyjson_mut_obj_add_str(doc, root, "method", "textDocument/hover");
    yyjson_mut_obj_add_int(doc, root, "id", lsp_get_id());

    char uri[strlen(path) + 8];
    snprintf(uri, sizeof(uri), "%s%s", "file://", path);
    yyjson_mut_val* params = yyjson_mut_obj_add_obj(doc, root, "params");
    yyjson_mut_val* text = yyjson_mut_obj_add_obj(doc, params, "textDocument");
    yyjson_mut_obj_add_str(doc, text, "uri", uri);

    yyjson_mut_val* position = yyjson_mut_obj_add_obj(doc, params, "position");
    yyjson_mut_obj_add_int(doc, position, "line", line);
    yyjson_mut_obj_add_int(doc, position, "character", character);

    lsp_send(LSP_EVENT_TYPE_HOVER, doc);
    yyjson_mut_doc_free(doc);
}

static void lsp_shutdown_server(FILE* pipe) {
}

static void lsp_init_server(FILE* pipe) {
    yyjson_mut_doc* doc  = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);
    yyjson_mut_obj_add_str(doc, root, "jsonrpc", "2.0");
    yyjson_mut_obj_add_str(doc, root, "method", "initialize");
    yyjson_mut_obj_add_int(doc, root, "id", lsp_get_id());

    yyjson_mut_val* params = yyjson_mut_obj_add_obj(doc, root, "params");
    yyjson_mut_obj_add_null(doc, params, "processId");
    yyjson_mut_obj_add_null(doc, params, "rootUri");
    yyjson_mut_obj_add_obj(doc, params, "capabilities");

    lsp_send(LSP_EVENT_TYPE_INIT, doc);

    yyjson_mut_doc_free(doc);
}

void* lsp_listener(void* arg) {
    FILE* pipe = (FILE*)arg;

    while (1) {
        char* response = lsp_recv();
    }
}

LSP_Event* lsp_poll() {
    return msgq_dequeue(&message_queue);
}

void lsp_free(LSP_Event* event) {
    switch (event->type) {
        case LSP_EVENT_TYPE_HOVER:
            free(event->data.hover.kind);
            free(event->data.hover.value);
            free(event);
            break;
        default:
            free(event);
            break;
    }
}

void lsp_start(void) {
    int stdin_pipe[2];
    int stdout_pipe[2];

    // Create pipes for communication
    if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
        perror("Failed to create pipes");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Failed to fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: Start Clangd
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        execlp("clangd", "clangd", NULL);
        perror("Failed to exec clangd");
        exit(EXIT_FAILURE);
    }

    // Parent process: Set up pipes
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);

    input_pipe  = fdopen(stdin_pipe[1], "w");
    output_pipe = fdopen(stdout_pipe[0], "r");

    pthread_create(&listener_thread, NULL, lsp_listener, output_pipe);
    lsp_init_server(input_pipe);
}

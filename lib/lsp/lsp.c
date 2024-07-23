#include "lsp.h"

#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "yyjson.h"

static int lsp_counter;
static FILE* input_pipe;
static FILE* output_pipe;
static pthread_t listener_thread;

// Function to send a message to the LSP server
static void lsp_send(FILE* pipe, const char* message, int length) {
    fprintf(pipe, "Content-Length: %d\r\n\r\n%s", length, message);
    fflush(pipe);
}

// Function to receive a message from the LSP server
static char* lsp_recv(FILE* pipe) {
    char header[128];

    while (fgets(header, sizeof(header), pipe)) {
        int length = 0;
        if (sscanf(header, "Content-Length: %d", &length) == 1) {
            char buffer[length + 1];

            // skip \r\n
            fgetc(pipe);
            fgetc(pipe);
            fread(buffer, length, 1, pipe);
            buffer[length] = '\0';
            printf("%s\n", buffer);
        }

        fpurge(pipe);
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

    size_t length    = 0;
    const char* json = yyjson_mut_write(doc, 0, &length);
    if (json) {
        lsp_send(input_pipe, json, length);
        printf("%s\n", json);
        free((void*)json);
    }

    yyjson_mut_doc_free(doc);
}

void lsp_hover(const char* path, int line, int character) {
    yyjson_mut_doc* doc  = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);
    yyjson_mut_obj_add_str(doc, root, "jsonrpc", "2.0");
    yyjson_mut_obj_add_int(doc, root, "id", lsp_counter++);
    yyjson_mut_obj_add_str(doc, root, "method", "textDocument/hover");

    char uri[strlen(path) + 8];
    snprintf(uri, sizeof(uri), "%s%s", "file://", path);
    yyjson_mut_val* params = yyjson_mut_obj_add_obj(doc, root, "params");
    yyjson_mut_val* text = yyjson_mut_obj_add_obj(doc, params, "textDocument");
    yyjson_mut_obj_add_str(doc, text, "uri", uri);

    yyjson_mut_val* position = yyjson_mut_obj_add_obj(doc, params, "position");
    yyjson_mut_obj_add_int(doc, position, "line", line);
    yyjson_mut_obj_add_int(doc, position, "character", character);

    size_t length    = 0;
    const char* json = yyjson_mut_write(doc, 0, &length);
    if (json) {
        lsp_send(input_pipe, json, length);
        free((void*)json);
    }

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
    yyjson_mut_obj_add_int(doc, root, "id", lsp_counter++);

    yyjson_mut_val* params = yyjson_mut_obj_add_obj(doc, root, "params");
    yyjson_mut_obj_add_null(doc, params, "processId");
    yyjson_mut_obj_add_null(doc, params, "rootUri");
    yyjson_mut_obj_add_obj(doc, params, "capabilities");

    size_t length    = 0;
    const char* json = yyjson_mut_write(doc, 0, &length);

    if (json) {
        // Send initialize request
        lsp_send(pipe, json, length);
        free((void*)json);
    }

    yyjson_mut_doc_free(doc);
}

void* lsp_listener(void* arg) {
    FILE* pipe = (FILE*)arg;

    while (1) {
        char* response = lsp_recv(pipe);
        return NULL;
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

#include "runtime_adapter.h"
#include <stdio.h>
#include <string.h>

typedef int (*forth_word_fn)(const irc_event *, char *, size_t);

typedef struct {
    const char *name;
    forth_word_fn fn;
} forth_word;

static int initialized;

static int word_ping(const irc_event *event, char *reply, size_t reply_size) {
    (void)event;
    snprintf(reply, reply_size, "PONG");
    return 1;
}

static int word_hello(const irc_event *event, char *reply, size_t reply_size) {
    (void)event;
    snprintf(reply, reply_size, "Hello from ForC");
    return 1;
}

static const forth_word dictionary[] = {
    {"PING", word_ping},
    {"HELLO", word_hello},
};

int forc_runtime_init(void) {
    initialized = 1;
    return 0;
}

void forc_runtime_shutdown(void) {
    initialized = 0;
}

int forc_runtime_word(const char *word, const irc_event *event, char *reply, size_t reply_size) {
    size_t count = sizeof(dictionary) / sizeof(dictionary[0]);
    if (!initialized || !word || !reply || reply_size == 0) return 0;
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(dictionary[i].name, word) == 0) {
            return dictionary[i].fn(event, reply, reply_size);
        }
    }
    return 0;
}

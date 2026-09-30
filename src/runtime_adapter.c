#include "runtime_adapter.h"
#include "forth_vm.h"
#include <stdio.h>
#include <string.h>

static int initialized;
static forth_vm vm;

static int word_ping(forth_vm *runtime) {
    return forth_vm_emit(runtime, "PONG");
}

static int word_hello(forth_vm *runtime) {
    return forth_vm_emit(runtime, "Hello from ForC");
}

int forc_runtime_init(void) {
    forth_vm_init(&vm);
    if (!forth_vm_define_native(&vm, "PING", word_ping)) return -1;
    if (!forth_vm_define_native(&vm, "HELLO", word_hello)) return -1;
    initialized = 1;
    return 0;
}

void forc_runtime_shutdown(void) {
    memset(&vm, 0, sizeof(vm));
    initialized = 0;
}

int forc_runtime_word(const char *word, const irc_event *event, char *reply, size_t reply_size) {
    (void)event;
    if (!initialized || !word || !reply || reply_size == 0) return 0;

    forth_vm_clear_output(&vm);
    if (!forth_vm_eval(&vm, word)) return 0;
    if (forth_vm_output(&vm)[0] == '\0') return 0;

    snprintf(reply, reply_size, "%s", forth_vm_output(&vm));
    return 1;
}

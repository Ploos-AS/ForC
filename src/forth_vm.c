#include "forth_vm.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int append_output(forth_vm *vm, const char *text) {
    size_t used = strlen(vm->output);
    size_t left = sizeof(vm->output) - used;
    if (left <= 1) return 0;
    snprintf(vm->output + used, left, "%s", text);
    return 1;
}

static forth_entry *find_entry(forth_vm *vm, const char *name) {
    for (size_t i = 0; i < vm->dictionary_count; ++i) {
        if (strcmp(vm->dictionary[i].name, name) == 0) return &vm->dictionary[i];
    }
    return NULL;
}

static int word_add(forth_vm *vm) {
    long a, b;
    if (!forth_vm_pop(vm, &b) || !forth_vm_pop(vm, &a)) return 0;
    return forth_vm_push(vm, a + b);
}

static int word_sub(forth_vm *vm) {
    long a, b;
    if (!forth_vm_pop(vm, &b) || !forth_vm_pop(vm, &a)) return 0;
    return forth_vm_push(vm, a - b);
}

static int word_dup(forth_vm *vm) {
    if (vm->sp == 0) return 0;
    return forth_vm_push(vm, vm->stack[vm->sp - 1]);
}

static int word_drop(forth_vm *vm) {
    long v;
    return forth_vm_pop(vm, &v);
}

static int word_dot(forth_vm *vm) {
    long v;
    char buf[64];
    if (!forth_vm_pop(vm, &v)) return 0;
    snprintf(buf, sizeof(buf), "%ld ", v);
    return append_output(vm, buf);
}

void forth_vm_init(forth_vm *vm) {
    memset(vm, 0, sizeof(*vm));
    forth_vm_define_native(vm, "+", word_add);
    forth_vm_define_native(vm, "-", word_sub);
    forth_vm_define_native(vm, "DUP", word_dup);
    forth_vm_define_native(vm, "DROP", word_drop);
    forth_vm_define_native(vm, ".", word_dot);
}

int forth_vm_push(forth_vm *vm, long value) {
    if (!vm || vm->sp >= FORTH_STACK_MAX) return 0;
    vm->stack[vm->sp++] = value;
    return 1;
}

int forth_vm_pop(forth_vm *vm, long *value) {
    if (!vm || !value || vm->sp == 0) return 0;
    *value = vm->stack[--vm->sp];
    return 1;
}

int forth_vm_define_native(forth_vm *vm, const char *name, forth_native_fn fn) {
    forth_entry *e;
    if (!vm || !name || !fn || vm->dictionary_count >= FORTH_DICT_MAX) return 0;
    e = &vm->dictionary[vm->dictionary_count++];
    memset(e, 0, sizeof(*e));
    snprintf(e->name, sizeof(e->name), "%s", name);
    e->native = fn;
    return 1;
}

static int define_colon(forth_vm *vm, const char *name, const char *body) {
    forth_entry *e;
    if (!vm || !name || !body || vm->dictionary_count >= FORTH_DICT_MAX) return 0;
    e = &vm->dictionary[vm->dictionary_count++];
    memset(e, 0, sizeof(*e));
    snprintf(e->name, sizeof(e->name), "%s", name);
    snprintf(e->body, sizeof(e->body), "%s", body);
    e->is_colon = 1;
    return 1;
}

static int eval_token(forth_vm *vm, const char *token) {
    char *end;
    long value;
    forth_entry *e;

    value = strtol(token, &end, 10);
    if (*token && *end == '\0') return forth_vm_push(vm, value);

    e = find_entry(vm, token);
    if (!e) return 0;
    if (e->is_colon) return forth_vm_eval(vm, e->body);
    return e->native(vm);
}

int forth_vm_eval(forth_vm *vm, const char *source) {
    char buf[FORTH_BODY_MAX * 2];
    char *tok;
    char *save = NULL;

    if (!vm || !source) return 0;
    snprintf(buf, sizeof(buf), "%s", source);
    tok = strtok_r(buf, " \t\r\n", &save);

    while (tok) {
        if (strcmp(tok, ":") == 0) {
            char *name = strtok_r(NULL, " \t\r\n", &save);
            char body[FORTH_BODY_MAX] = {0};
            size_t used = 0;
            char *part;
            if (!name) return 0;
            while ((part = strtok_r(NULL, " \t\r\n", &save)) != NULL) {
                if (strcmp(part, ";") == 0) break;
                if (used && used + 1 < sizeof(body)) body[used++] = ' ';
                if (used + strlen(part) >= sizeof(body)) return 0;
                strcpy(body + used, part);
                used += strlen(part);
            }
            if (!part || strcmp(part, ";") != 0) return 0;
            if (!define_colon(vm, name, body)) return 0;
        } else if (!eval_token(vm, tok)) {
            return 0;
        }
        tok = strtok_r(NULL, " \t\r\n", &save);
    }
    return 1;
}

const char *forth_vm_output(const forth_vm *vm) {
    return vm ? vm->output : "";
}

void forth_vm_clear_output(forth_vm *vm) {
    if (vm) vm->output[0] = '\0';
}

#ifndef FORC_FORTH_VM_H
#define FORC_FORTH_VM_H

#include <stddef.h>

#define FORTH_STACK_MAX 64
#define FORTH_DICT_MAX 64
#define FORTH_BODY_MAX 256

typedef struct forth_vm forth_vm;

typedef int (*forth_native_fn)(forth_vm *vm);

typedef struct {
    char name[32];
    forth_native_fn native;
    char body[FORTH_BODY_MAX];
    int is_colon;
} forth_entry;

struct forth_vm {
    long stack[FORTH_STACK_MAX];
    size_t sp;
    forth_entry dictionary[FORTH_DICT_MAX];
    size_t dictionary_count;
    char output[512];
};

void forth_vm_init(forth_vm *vm);
int forth_vm_push(forth_vm *vm, long value);
int forth_vm_pop(forth_vm *vm, long *value);
int forth_vm_define_native(forth_vm *vm, const char *name, forth_native_fn fn);
int forth_vm_eval(forth_vm *vm, const char *source);
int forth_vm_emit(forth_vm *vm, const char *text);
const char *forth_vm_output(const forth_vm *vm);
void forth_vm_clear_output(forth_vm *vm);

#endif

#include "../src/forth_vm.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    forth_vm vm;
    long value = 0;

    forth_vm_init(&vm);

    if (!forth_vm_eval(&vm, "2 3 +")) return 1;
    if (!forth_vm_pop(&vm, &value) || value != 5) return 2;

    if (!forth_vm_eval(&vm, ": DOUBLE DUP + ; 21 DOUBLE")) return 3;
    if (!forth_vm_pop(&vm, &value) || value != 42) return 4;

    forth_vm_clear_output(&vm);
    if (!forth_vm_eval(&vm, "7 .")) return 5;
    if (strcmp(forth_vm_output(&vm), "7 ") != 0) return 6;

    puts("forth_vm: PASS");
    return 0;
}

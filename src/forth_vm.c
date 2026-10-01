#include "forth_vm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static forth_entry *find(forth_vm *vm,const char *name){for(size_t i=0;i<vm->dictionary_count;i++)if(strcmp(vm->dictionary[i].name,name)==0)return &vm->dictionary[i];return NULL;}
void forth_vm_init(forth_vm *vm){if(vm)memset(vm,0,sizeof(*vm));}
int forth_vm_push(forth_vm *vm,long v){if(!vm||vm->sp>=FORTH_STACK_MAX)return 0;vm->stack[vm->sp++]=v;return 1;}
int forth_vm_pop(forth_vm *vm,long *v){if(!vm||!v||!vm->sp)return 0;*v=vm->stack[--vm->sp];return 1;}
int forth_vm_define_native(forth_vm *vm,const char *name,forth_native_fn fn){if(!vm||!name||!fn||vm->dictionary_count>=FORTH_DICT_MAX)return 0;strncpy(vm->dictionary[vm->dictionary_count].name,name,31);vm->dictionary[vm->dictionary_count].name[31]='\0';vm->dictionary[vm->dictionary_count].native=fn;vm->dictionary[vm->dictionary_count].is_colon=0;vm->dictionary_count++;return 1;}
int forth_vm_emit(forth_vm *vm,const char *text){if(!vm||!text)return 0;snprintf(vm->output,sizeof(vm->output),"%s",text);return 1;}
const char *forth_vm_output(const forth_vm *vm){return vm?vm->output:"";}
void forth_vm_clear_output(forth_vm *vm){if(vm)vm->output[0]='\0';}
int forth_vm_eval(forth_vm *vm,const char *source){char word[64];const char *p;if(!vm||!source)return 0;p=source;while(*p){while(*p==' '||*p=='\t'||*p=='\n'||*p=='\r')p++;if(!*p)break;size_t n=0;while(p[n]&&p[n]!=' '&&p[n]!='\t'&&p[n]!='\n'&&p[n]!='\r'&&n<63)n++;memcpy(word,p,n);word[n]='\0';forth_entry *e=find(vm,word);if(!e||!e->native)return 0;if(!e->native(vm))return 0;p+=n;}return 1;}

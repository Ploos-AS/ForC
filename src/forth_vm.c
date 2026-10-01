#include "forth_vm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static int eval_tokens(forth_vm *vm,const char *source);
static forth_entry *find(forth_vm *vm,const char *name){for(size_t i=0;i<vm->dictionary_count;i++)if(strcmp(vm->dictionary[i].name,name)==0)return &vm->dictionary[i];return NULL;}
void forth_vm_init(forth_vm *vm){if(vm)memset(vm,0,sizeof(*vm));}
int forth_vm_depth(const forth_vm *vm){return vm?(int)vm->sp:0;}
int forth_vm_peek(forth_vm *vm,long *v){if(!vm||!v||!vm->sp||vm->stack[vm->sp-1].type!=FORTH_VALUE_INT)return 0;*v=vm->stack[vm->sp-1].integer;return 1;}
int forth_vm_push(forth_vm *vm,long v){if(!vm||vm->sp>=FORTH_STACK_MAX)return 0;vm->stack[vm->sp]=(forth_value){FORTH_VALUE_INT,v,{0}};vm->sp++;return 1;}
int forth_vm_push_string(forth_vm *vm,const char *v){if(!vm||!v||vm->sp>=FORTH_STACK_MAX||strlen(v)>=FORTH_STRING_MAX)return 0;vm->stack[vm->sp].type=FORTH_VALUE_STRING;vm->stack[vm->sp].integer=0;snprintf(vm->stack[vm->sp].string,FORTH_STRING_MAX,"%s",v);vm->sp++;return 1;}
int forth_vm_pop(forth_vm *vm,long *v){if(!vm||!v||!vm->sp||vm->stack[vm->sp-1].type!=FORTH_VALUE_INT)return 0;*v=vm->stack[--vm->sp].integer;return 1;}
int forth_vm_pop_string(forth_vm *vm,char *v,size_t n){if(!vm||!v||!n||!vm->sp||vm->stack[vm->sp-1].type!=FORTH_VALUE_STRING)return 0;snprintf(v,n,"%s",vm->stack[--vm->sp].string);return 1;}
int forth_vm_define_native(forth_vm *vm,const char *name,forth_native_fn fn){if(!vm||!name||!fn||vm->dictionary_count>=FORTH_DICT_MAX)return 0;snprintf(vm->dictionary[vm->dictionary_count].name,sizeof(vm->dictionary[0].name),"%s",name);vm->dictionary[vm->dictionary_count].native=fn;vm->dictionary[vm->dictionary_count].is_colon=0;vm->dictionary_count++;return 1;}
int forth_vm_emit(forth_vm *vm,const char *text){if(!vm||!text)return 0;snprintf(vm->output,sizeof(vm->output),"%s",text);return 1;}
const char *forth_vm_output(const forth_vm *vm){return vm?vm->output:"";}
void forth_vm_clear_output(forth_vm *vm){if(vm)vm->output[0]='\0';}
static int token(forth_vm *vm,const char *t){forth_entry *e=find(vm,t);if(e&&e->native)return e->native(vm);if(t[0]=='"'){size_t n=strlen(t);if(n<2||t[n-1]!='"'||n-2>=FORTH_STRING_MAX)return 0;char s[FORTH_STRING_MAX];memcpy(s,t+1,n-2);s[n-2]='\0';return forth_vm_push_string(vm,s);}char *end;long v=strtol(t,&end,10);if(*t&&*end=='\0')return forth_vm_push(vm,v);return 0;}
int forth_vm_define_colon(forth_vm *vm,const char *name,const char *body){forth_entry *e;if(!vm||!name||!body||vm->dictionary_count>=FORTH_DICT_MAX||strlen(name)>=32||strlen(body)>=FORTH_BODY_MAX)return 0;e=&vm->dictionary[vm->dictionary_count++];memset(e,0,sizeof(*e));snprintf(e->name,sizeof(e->name),"%s",name);snprintf(e->body,sizeof(e->body),"%s",body);e->is_colon=1;return 1;}
int forth_vm_eval(forth_vm *vm,const char *source){char word[FORTH_STRING_MAX];const char *p;if(!vm||!source)return 0;p=source;while(*p){while(*p==' '||*p=='\t'||*p=='\n'||*p=='\r')p++;if(!*p)break;size_t n=0;if(*p=='"'){word[n++]=*p++;while(*p&&n<FORTH_STRING_MAX-1){word[n++]=*p;if(*p=='"'){p++;break;}p++;}word[n]='\0';}else{while(p[n]&&p[n]!=' '&&p[n]!='\t'&&p[n]!='\n'&&p[n]!='\r'&&n<FORTH_STRING_MAX-1)n++;memcpy(word,p,n);word[n]='\0';p+=n;}if(!token(vm,word))return 0;}return 1;}

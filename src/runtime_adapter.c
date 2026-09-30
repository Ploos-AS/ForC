#include "runtime_adapter.h"
#include "forth_vm.h"
#include <stdio.h>
#include <string.h>
static int initialized; static forth_vm vm;
static int word_ping(forth_vm *r){return forth_vm_emit(r,"PONG");}
static int word_hello(forth_vm *r){return forth_vm_emit(r,"Hello from ForC");}
static int word_join(forth_vm *r){return forth_vm_emit(r,"Welcome from FORC");}
int forc_runtime_init(void){forth_vm_init(&vm);if(!forth_vm_define_native(&vm,"PING",word_ping))return -1;if(!forth_vm_define_native(&vm,"HELLO",word_hello))return -1;if(!forth_vm_define_native(&vm,"JOIN",word_join))return -1;initialized=1;return 0;}
void forc_runtime_shutdown(void){memset(&vm,0,sizeof(vm));initialized=0;}
int forc_runtime_word(const char *word,const irc_event *event,char *reply,size_t rs){(void)event;if(!initialized||!word||!reply||!rs)return 0;forth_vm_clear_output(&vm);if(!forth_vm_eval(&vm,word))return 0;if(forth_vm_output(&vm)[0]=='\0')return 0;snprintf(reply,rs,"%s",forth_vm_output(&vm));return 1;}
int forc_runtime_event(const char *name,const irc_event *event,char *reply,size_t rs){const char *word;if(!initialized||!name||!event||!reply||!rs)return 0;word=name;if(strcmp(name,"join")==0)word="JOIN";else if(strcmp(name,"part")==0)word="PART";else if(strcmp(name,"nick")==0)word="NICK";else if(strcmp(name,"quit")==0)word="QUIT";else if(strcmp(name,"notice")==0)word="NOTICE";else return 0;return forc_runtime_word(word,event,reply,rs);}
int forc_runtime_bind_events(event_registry *registry){if(!registry)return 0;return event_registry_register(registry,IRC_EVENT_JOIN,NULL,NULL)?1:0;}

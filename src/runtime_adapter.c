#include "runtime_adapter.h"
#include "forth_vm.h"
#include "timer_handlers.h"
#include <stdio.h>
#include <string.h>
static bot_named_timer_context forc_named_timers[BOT_MAX_TIMERS]; static size_t forc_named_count;\nstatic int initialized; static forth_vm vm; static irc_output_sink forc_sink; static bot_timer_registry forc_timers;
void forc_runtime_timer_init(void){bot_timer_registry_init(&forc_timers);memset(forc_named_timers,0,sizeof(forc_named_timers));forc_named_count=0;}
bot_timer_id forc_runtime_timer_after(uint64_t d,bot_timer_fn fn,void *u){return bot_timer_add(&forc_timers,d,0,0,fn,u);}
bot_timer_id forc_runtime_timer_every(uint64_t i,bot_timer_fn fn,void *u){return bot_timer_add(&forc_timers,i,i,1,fn,u);}
int forc_runtime_timer_cancel(bot_timer_id id){return bot_timer_cancel(&forc_timers,id);}
size_t forc_runtime_timer_poll(uint64_t now){return bot_timer_poll(&forc_timers,now);}
static int forc_timer_dispatch(const char *handler,bot_timer_id id,void *user){forth_vm *v=(forth_vm *)user;(void)id;if(!v||!handler)return -1;forth_vm_clear_output(v);return forth_vm_eval(v,handler)?0:-1;}
static bot_timer_id forc_named_timer(uint64_t delay,uint64_t interval,int repeat,const char *name){bot_named_timer_context *ctx;bot_timer_id id;if(!name||!name[0]||forc_named_count>=BOT_MAX_TIMERS)return 0;ctx=&forc_named_timers[forc_named_count++];memset(ctx,0,sizeof(*ctx));snprintf(ctx->name,sizeof(ctx->name),"%s",name);ctx->dispatcher.dispatch=forc_timer_dispatch;ctx->dispatcher.user=&vm;id=bot_timer_add(&forc_timers,delay,interval,repeat,bot_named_timer_callback,ctx);if(!id){forc_named_count--;return 0;}return id;}
static int word_after(forth_vm*r){long d;char name[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,name,sizeof(name))||!forth_vm_pop(r,&d)||d<0)return 0;return forc_named_timer((uint64_t)d,0,0,name)!=0;}
static int word_every(forth_vm*r){long d;char name[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,name,sizeof(name))||!forth_vm_pop(r,&d)||d<=0)return 0;return forc_named_timer((uint64_t)d,(uint64_t)d,1,name)!=0;}
static int word_ping(forth_vm*r){return forth_vm_emit(r,"PONG");}
static int word_hello(forth_vm*r){return forth_vm_emit(r,"Hello from ForC");}
static int word_say(forth_vm*r){char x[FORTH_STRING_MAX],t[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,x,sizeof(x))||!forth_vm_pop_string(r,t,sizeof(t)))return 0;return irc_send_privmsg(&forc_sink,t,x);}
static int word_notice(forth_vm*r){char x[FORTH_STRING_MAX],t[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,x,sizeof(x))||!forth_vm_pop_string(r,t,sizeof(t)))return 0;return irc_send_notice(&forc_sink,t,x);}
static int word_join(forth_vm*r){char c[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,c,sizeof(c)))return 0;return irc_send_join(&forc_sink,c);}
static int word_part(forth_vm*r){char x[FORTH_STRING_MAX],c[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,x,sizeof(x))||!forth_vm_pop_string(r,c,sizeof(c)))return 0;return irc_send_part(&forc_sink,c,x);}
int forc_runtime_init(void){forc_runtime_timer_init();forth_vm_init(&vm);if(!forth_vm_define_native(&vm,"PING",word_ping)||!forth_vm_define_native(&vm,"HELLO",word_hello)||!forth_vm_define_native(&vm,"JOIN",word_join)||!forth_vm_define_native(&vm,"SAY",word_say)||!forth_vm_define_native(&vm,"NOTICE",word_notice)||!forth_vm_define_native(&vm,"PART",word_part)||!forth_vm_define_native(&vm,"AFTER",word_after)||!forth_vm_define_native(&vm,"EVERY",word_every))return -1;initialized=1;return 0;}
void forc_runtime_shutdown(void){memset(&vm,0,sizeof(vm));initialized=0;}
int forc_runtime_word(const char*word,const irc_event*event,char*reply,size_t rs){(void)event;if(!initialized||!word||!reply||!rs)return 0;forth_vm_clear_output(&vm);if(!forth_vm_eval(&vm,word))return 0;if(forth_vm_output(&vm)[0]=='\0')return 0;snprintf(reply,rs,"%s",forth_vm_output(&vm));return 1;}
int forc_runtime_event(const char*n,const irc_event*e,char*r,size_t rs){if(!initialized||!n||!e||!r||!rs)return 0;return forc_runtime_word(n,e,r,rs);}
void forc_runtime_set_output_sink(const irc_output_sink*s){if(s)forc_sink=*s;else memset(&forc_sink,0,sizeof(forc_sink));}
int forc_runtime_say(const char*t,const char*x){return irc_send_privmsg(&forc_sink,t,x);}
int forc_runtime_notice(const char*t,const char*x){return irc_send_notice(&forc_sink,t,x);}
int forc_runtime_join(const char*c){return irc_send_join(&forc_sink,c);}
int forc_runtime_part(const char*c,const char*r){return irc_send_part(&forc_sink,c,r);}

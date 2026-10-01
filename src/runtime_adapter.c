#include "runtime_adapter.h"
#include "bot_runtime.h"
#include "forth_vm.h"
#include "timer_handlers.h"
#include <stdio.h>
#include <string.h>
typedef struct { char event[32]; char handler[32]; int active; } forc_event_binding; static forc_event_binding forc_events[8];
static bot_named_timer_context forc_named_timers[BOT_MAX_TIMERS]; static size_t forc_named_count;\nstatic int initialized;
static bot_runtime *bot_rt; static forth_vm vm; static irc_output_sink forc_sink; static bot_timer_registry forc_timers;
void forc_runtime_timer_init(void){bot_timer_registry_init(&forc_timers);memset(forc_named_timers,0,sizeof(forc_named_timers));forc_named_count=0;}
bot_timer_id forc_runtime_timer_after(uint64_t d,bot_timer_fn fn,void *u){return bot_timer_add(&forc_timers,d,0,0,fn,u);}
bot_timer_id forc_runtime_timer_every(uint64_t i,bot_timer_fn fn,void *u){return bot_timer_add(&forc_timers,i,i,1,fn,u);}
int forc_runtime_timer_cancel(bot_timer_id id){return bot_timer_cancel(&forc_timers,id);}
size_t forc_runtime_timer_poll(uint64_t now){return bot_timer_poll(&forc_timers,now);}
static int forc_timer_dispatch(const char *handler,bot_timer_id id,void *user){forth_vm *v=(forth_vm *)user;(void)id;if(!v||!handler)return -1;forth_vm_clear_output(v);return forth_vm_eval(v,handler)?0:-1;}
static bot_timer_id forc_named_timer(uint64_t delay,uint64_t interval,int repeat,const char *name){bot_named_timer_context *ctx;bot_timer_id id;if(!name||!name[0]||forc_named_count>=BOT_MAX_TIMERS)return 0;ctx=&forc_named_timers[forc_named_count++];memset(ctx,0,sizeof(*ctx));snprintf(ctx->name,sizeof(ctx->name),"%s",name);ctx->dispatcher.dispatch=forc_timer_dispatch;ctx->dispatcher.user=&vm;id=bot_timer_add(&forc_timers,delay,interval,repeat,bot_named_timer_callback,ctx);if(!id){forc_named_count--;return 0;}return id;}
static int word_after(forth_vm*r){long d;char name[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,name,sizeof(name))||!forth_vm_pop(r,&d)||d<0)return 0;bot_timer_id id=forc_named_timer((uint64_t)d,0,0,name);return id?forth_vm_push(r,(long)id):0;}
static int word_every(forth_vm*r){long d;char name[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,name,sizeof(name))||!forth_vm_pop(r,&d)||d<=0)return 0;bot_timer_id id=forc_named_timer((uint64_t)d,(uint64_t)d,1,name);return id?forth_vm_push(r,(long)id):0;}
static int word_cancel(forth_vm*r){long id;if(!forth_vm_pop(r,&id)||id<=0)return 0;return forc_runtime_timer_cancel((bot_timer_id)id);}
static irc_event forc_active_event; static int forc_active_event_valid;
static int word_nick(forth_vm*r){return forth_vm_emit(r,forc_active_event_valid?forc_active_event.nick:"");}
static int word_target(forth_vm*r){return forth_vm_emit(r,forc_active_event_valid?forc_active_event.target:"");}
static int word_text(forth_vm*r){return forth_vm_emit(r,forc_active_event_valid?forc_active_event.text:"");}
static int word_args(forth_vm*r){return forth_vm_emit(r,forc_active_event_valid?forc_active_event.args:"");}
static int word_command(forth_vm*r){return forth_vm_emit(r,forc_active_event_valid?forc_active_event.token:"");}
static int word_state_event_user(forth_vm*r){char b[256];if(!forc_active_event_valid||!bot_state_scope_event_user(&forc_active_event,b,sizeof(b)))return 0;return forth_vm_emit(r,b);}
static int word_state_event_target(forth_vm*r){char b[256];if(!forc_active_event_valid||!bot_state_scope_event_target(&forc_active_event,b,sizeof(b)))return 0;return forth_vm_emit(r,b);}
static int word_state_user_scope(forth_vm*r){char n[256],b[256];if(!forth_vm_pop_string(r,n,sizeof(n))||!bot_state_scope_user(n,b,sizeof(b)))return 0;return forth_vm_emit(r,b);}
static int word_state_channel_scope(forth_vm*r){char n[256],b[256];if(!forth_vm_pop_string(r,n,sizeof(n))||!bot_state_scope_channel(n,b,sizeof(b)))return 0;return forth_vm_emit(r,b);}
static int word_state_set(forth_vm*r){char v[512],k[256],s[256];if(!forth_vm_pop_string(r,v,sizeof(v))||!forth_vm_pop_string(r,k,sizeof(k))||!forth_vm_pop_string(r,s,sizeof(s)))return 0;return bot_runtime_has(bot_rt,"state.write")&&bot_state_set(bot_rt->state,s,k,v);}
static int word_state_get(forth_vm*r){char k[256],s[256];const char*v;if(!forth_vm_pop_string(r,k,sizeof(k))||!forth_vm_pop_string(r,s,sizeof(s)))return 0;v=bot_runtime_has(bot_rt,"state.read")?bot_state_get(bot_rt->state,s,k):NULL;return v?forth_vm_emit(r,v):0;}
static int word_state_delete(forth_vm*r){char k[256],s[256];if(!forth_vm_pop_string(r,k,sizeof(k))||!forth_vm_pop_string(r,s,sizeof(s)))return 0;return bot_runtime_has(bot_rt,"state.delete")&&bot_state_delete(bot_rt->state,s,k);}
static int word_ping(forth_vm*r){return forth_vm_emit(r,"PONG");}
static int word_hello(forth_vm*r){return forth_vm_emit(r,"Hello from ForC");}
static int word_say(forth_vm*r){char x[FORTH_STRING_MAX],t[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,x,sizeof(x))||!forth_vm_pop_string(r,t,sizeof(t)))return 0;return irc_send_privmsg(&forc_sink,t,x);}
static int word_notice(forth_vm*r){char x[FORTH_STRING_MAX],t[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,x,sizeof(x))||!forth_vm_pop_string(r,t,sizeof(t)))return 0;return irc_send_notice(&forc_sink,t,x);}
static int word_join(forth_vm*r){char c[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,c,sizeof(c)))return 0;return irc_send_join(&forc_sink,c);}
static int word_part(forth_vm*r){char x[FORTH_STRING_MAX],c[FORTH_STRING_MAX];if(!forth_vm_pop_string(r,x,sizeof(x))||!forth_vm_pop_string(r,c,sizeof(c)))return 0;return irc_send_part(&forc_sink,c,x);}
static int forc_event_type(const char *name){if(!name)return 0;if(strcmp(name,"JOIN")==0)return IRC_EVENT_JOIN;if(strcmp(name,"PART")==0)return IRC_EVENT_PART;if(strcmp(name,"NICK")==0)return IRC_EVENT_NICK;if(strcmp(name,"QUIT")==0)return IRC_EVENT_QUIT;if(strcmp(name,"NOTICE")==0)return IRC_EVENT_NOTICE;if(strcmp(name,"PRIVMSG")==0)return IRC_EVENT_PRIVMSG;return 0;}
static int word_on(forth_vm*r){char handler[FORTH_STRING_MAX],event[FORTH_STRING_MAX];size_t i;if(!forth_vm_pop_string(r,handler,sizeof(handler))||!forth_vm_pop_string(r,event,sizeof(event)))return 0;if(!forc_event_type(event)||strlen(handler)>=sizeof(forc_events[0].handler))return 0;for(i=0;i<8;i++)if(!forc_events[i].active||strcmp(forc_events[i].event,event)==0){snprintf(forc_events[i].event,sizeof(forc_events[i].event),"%s",event);snprintf(forc_events[i].handler,sizeof(forc_events[i].handler),"%s",handler);forc_events[i].active=1;return 1;}return 0;}
int forc_runtime_init(void){memset(forc_events,0,sizeof(forc_events));bot_rt=bot_runtime_create();if(!bot_rt)return -1;forc_runtime_timer_init();forth_vm_init(&vm);if(!forth_vm_define_native(&vm,"STATE-EVENT-USER",word_state_event_user)||!forth_vm_define_native(&vm,"STATE-EVENT-TARGET",word_state_event_target)||!forth_vm_define_native(&vm,"STATE-USER-SCOPE",word_state_user_scope)||!forth_vm_define_native(&vm,"STATE-CHANNEL-SCOPE",word_state_channel_scope)||!forth_vm_define_native(&vm,"STATE-SET",word_state_set)||!forth_vm_define_native(&vm,"STATE-GET",word_state_get)||!forth_vm_define_native(&vm,"STATE-DELETE",word_state_delete)||!forth_vm_define_native(&vm,"NICK",word_nick)||!forth_vm_define_native(&vm,"TARGET",word_target)||!forth_vm_define_native(&vm,"TEXT",word_text)||!forth_vm_define_native(&vm,"ARGS",word_args)||!forth_vm_define_native(&vm,"COMMAND",word_command)||!forth_vm_define_native(&vm,"PING",word_ping)||!forth_vm_define_native(&vm,"HELLO",word_hello)||!forth_vm_define_native(&vm,"JOIN",word_join)||!forth_vm_define_native(&vm,"SAY",word_say)||!forth_vm_define_native(&vm,"NOTICE",word_notice)||!forth_vm_define_native(&vm,"PART",word_part)||!forth_vm_define_native(&vm,"AFTER",word_after)||!forth_vm_define_native(&vm,"EVERY",word_every)||!forth_vm_define_native(&vm,"CANCEL",word_cancel)||!forth_vm_define_native(&vm,"ON",word_on))return -1;initialized=1;return 0;}
void forc_runtime_shutdown(void){memset(&vm,0,sizeof(vm));initialized=0;}
int forc_runtime_stack_depth(void){return forth_vm_depth(&vm);}
int forc_runtime_stack_peek(long *value){return forth_vm_peek(&vm,value);}
int forc_runtime_word(const char*word,const irc_event*event,char*reply,size_t rs){int ok;if(!initialized||!word||!reply||!rs)return 0;forth_vm_clear_output(&vm);if(event){forc_active_event=*event;forc_active_event_valid=1;}ok=forth_vm_eval(&vm,word);if(event)forc_active_event_valid=0;if(!ok)return 0;if(forth_vm_output(&vm)[0]=='\0')return 0;snprintf(reply,rs,"%s",forth_vm_output(&vm));return 1;}
int forc_runtime_event(const char*n,const irc_event*e,char*r,size_t rs){size_t i;if(!initialized||!n||!e||!r||!rs)return 0;for(i=0;i<8;i++)if(forc_events[i].active&&strcasecmp(forc_events[i].event,n)==0)return forc_runtime_word(forc_events[i].handler,e,r,rs);return forc_runtime_word(n,e,r,rs);}
int forc_runtime_command(const char*n,const irc_event*e,char*r,size_t rs){size_t i;if(!initialized||!n||!e||!r||!rs)return 0;for(i=0;i<32;i++)if(forc_commands[i].active&&strcmp(forc_commands[i].event,n)==0)return forc_runtime_word(forc_commands[i].handler,e,r,rs);return forc_runtime_word(n,e,r,rs);}
void forc_runtime_set_output_sink(const irc_output_sink*s){if(s)forc_sink=*s;else memset(&forc_sink,0,sizeof(forc_sink));}
int forc_runtime_say(const char*t,const char*x){return irc_send_privmsg(&forc_sink,t,x);}
int forc_runtime_notice(const char*t,const char*x){return irc_send_notice(&forc_sink,t,x);}
int forc_runtime_join(const char*c){return irc_send_join(&forc_sink,c);}
int forc_runtime_part(const char*c,const char*r){return irc_send_part(&forc_sink,c,r);}

int forc_runtime_grant_capability(const char *cap){return bot_rt&&cap&&bot_runtime_grant(bot_rt,cap);}

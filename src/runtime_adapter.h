#ifndef FORC_RUNTIME_ADAPTER_H
#define FORC_RUNTIME_ADAPTER_H
#include "irc_core.h"
#include "events.h"
#include "irc_output_sink.h"
#include "timers.h"
#include <stddef.h>
int forc_runtime_init(void);
void forc_runtime_shutdown(void);
int forc_runtime_command(const char *name,const irc_event *event,char *reply,size_t reply_size);
int forc_runtime_event(const char *name,const irc_event *event,char *reply,size_t reply_size);
int forc_runtime_bind_events(event_registry *registry);
void forc_runtime_set_output_sink(const irc_output_sink *sink);
int forc_runtime_say(const char *target,const char *text);
int forc_runtime_notice(const char *target,const char *text);
int forc_runtime_join(const char *channel);
int forc_runtime_part(const char *channel,const char *reason);
void forc_runtime_timer_init(void);
bot_timer_id forc_runtime_timer_after(uint64_t delay_ms,bot_timer_fn handler,void *user);
bot_timer_id forc_runtime_timer_every(uint64_t interval_ms,bot_timer_fn handler,void *user);
int forc_runtime_timer_cancel(bot_timer_id id);
size_t forc_runtime_timer_poll(uint64_t now_ms);
int forc_runtime_timer_cancel(bot_timer_id id);
#endif

int forc_runtime_grant_capability(const char *cap);

#ifndef FORC_RUNTIME_ADAPTER_H
#define FORC_RUNTIME_ADAPTER_H
#include "irc_core.h"
#include <stddef.h>
int forc_runtime_init(void);
void forc_runtime_shutdown(void);
int forc_runtime_word(const char *word, const irc_event *event, char *reply, size_t reply_size);
int forc_runtime_event(const char *name, const irc_event *event, char *reply, size_t reply_size);
#endif

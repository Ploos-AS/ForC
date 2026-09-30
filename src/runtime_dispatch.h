#ifndef FORC_RUNTIME_DISPATCH_H
#define FORC_RUNTIME_DISPATCH_H
#include "irc_core.h"
#include <stddef.h>
int forc_dispatch_runtime(const irc_event *event, char *reply, size_t reply_size);
#endif

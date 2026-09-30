#include "runtime_dispatch.h"
#include "runtime_adapter.h"
#include <string.h>

int forc_dispatch_runtime(const irc_event *event, char *reply, size_t reply_size) {
    const char *command;
    if (!event || event->type != IRC_EVENT_PRIVMSG || !reply || reply_size == 0) return 0;
    command = event->text;
    if (*command == '!') command++;
    if (!*command) return 0;
    return forc_runtime_word("HELLO", event, reply, reply_size);
}

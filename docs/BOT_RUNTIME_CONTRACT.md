# Bot Runtime Contract

ForC implements the common standalone bot-runtime contract using Forth-native words.

## M1 qualification

| Capability | Contract | ForC |
|---|---|---|
| IRC commands | C dispatcher -> language callback | PASS |
| IRC events | C event dispatcher -> language callback | PASS |
| One-shot timer | AFTER | PASS |
| Repeating timer | EVERY | PASS |
| Timer cancellation | CANCEL | PASS |
| Named callback | Forth word | PASS |
| Deterministic polling | explicit runtime clock | PASS |
| CI qualification | make test | PASS |

## Forth timer API

    : HEARTBEAT
        "#ploos" "I'm alive" SAY
    ;
    60000 "HEARTBEAT" EVERY
    DUP CANCEL

Timer IDs are returned on the Forth data stack. The C timer registry is language-neutral; Forth only supplies the callback binding.

## Contract rule

Future language features should add a deterministic test at the language-binding layer and retain the same runtime semantics.


## Command and event context

The common contract exposes the same IRC context to every language binding:

- command: command name, sender nick, target, and original message text
- event: event name, sender nick, target, and event text/payload

The exact call syntax remains language-native. Bindings must not invent different semantics for these fields. A handler may ignore fields it does not need.

The runtime keeps the parsed `irc_event` as the canonical source of truth; language adapters translate that context without changing its meaning.

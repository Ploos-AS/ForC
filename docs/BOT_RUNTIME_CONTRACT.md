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

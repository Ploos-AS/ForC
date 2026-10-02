# ForC

ForC is a C-based IRC bot with an embedded Forth scripting environment.

## M1 status

The standalone runtime now has a qualified language-neutral contract for IRC commands/events and deterministic timers. See docs/BOT_RUNTIME_CONTRACT.md.

- IRC command/event dispatch: qualified
- Timer registry and named handlers: qualified
- Forth timer bindings: qualified
- AFTER / EVERY / CANCEL semantics: qualified
- CI regression coverage: enabled

## M0 goals

- Small, portable C IRC core
- Small embeddable Forth VM in C
- IRC-oriented Forth words and event hooks
- PBMP integration boundary defined from the start
- Hooks for BotWeb and BotAI
- Standalone-first operation
- Standalone qualification with PBMP, BotWeb, BotAI and BotLogic disabled
- Deterministic tests for parser, dispatch, dictionary and VM behavior

## Initial architecture

```text
IRC network
    |
    v
C IRC core
    |
    +-- parser/state
    +-- command/event dispatcher
    +-- PBMP adapter
    +-- BotWeb/BotAI hooks
    |
    `-- Forth VM
         +-- dictionary
         +-- IRC words
         +-- events
         +-- timers
         `-- optional operator REPL
```

## M0 Forth surface

The first dictionary should include IRC-oriented words for:

- `SAY`
- `NOTICE`
- `JOIN`
- `PART`
- `NICK`
- command registration
- event registration
- timers
- safe bot/channel/user state access

A privileged IRC/private-message REPL is a later milestone, not an M0 requirement.

## Design principles

1. C first: both bot and VM should remain understandable and portable.
2. The Forth VM should be small enough to study and potentially reuse elsewhere.
3. IRC concepts should feel native to the Forth dictionary.
4. Script/VM errors must be isolated from the IRC core.
5. PBMP integration is optional at runtime, but first-class in the architecture.
6. BotWeb, BotAI and BotLogic remain optional components.
7. No malware or offensive payloads are stored in the repository.


Standalone qualification requires the complete bot, including its embedded Forth VM, to build and pass its test suite with PBMP, BotWeb, BotAI and BotLogic disabled. The language/runtime is a required part of ForC, not an optional integration. PBMP, BotWeb, BotAI and BotLogic must remain optional build/runtime dependencies.

## License

Software: MIT.

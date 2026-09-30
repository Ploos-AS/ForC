# ForC

ForC is a C-based IRC bot with an embedded Forth scripting environment.

## M0 goals

- Small, portable C IRC core
- Small embeddable Forth VM in C
- IRC-oriented Forth words and event hooks
- PBMP integration boundary defined from the start
- Hooks for BotWeb and BotAI
- Standalone-first operation
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
6. BotWeb and BotAI remain optional components.
7. No malware or offensive payloads are stored in the repository.

## License

Software: MIT.

# M270 live actor mailbox and isolation runtime implementation expectations

Contract ID: `objc3c-part7-live-actor-mailbox-and-isolation-runtime/m270-d002-v1`

This issue is complete when:

- the private actor helper cluster now includes live mailbox binding/enqueue/drain helpers and `objc3_runtime_actor_runtime_state_snapshot` proves their runnable state publication
- live actor mailbox helper traffic compiles, links, and executes through the packaged runtime archive path without widening the public runtime header
- `tests/tooling/runtime/m270_d002_live_actor_mailbox_runtime_probe.cpp` proves deterministic actor mailbox binding, enqueue, and drain snapshot publication
- the next issue remains `M270-D003`

# M270 cross-module isolation metadata hardening expectations

Contract ID: `objc3c-cross-module-actor-isolation-metadata-hardening/m270-d003-v1`

This issue is complete when:

- `module.runtime-import-surface.json` preserves the actor mailbox/isolation import packet for actor-bearing modules
- `module.cross-module-runtime-link-plan.json` preserves imported actor mailbox/isolation replay facts and fails closed on drift
- a provider/consumer compile pair proves the happy path and a tampered imported runtime surface fails closed
- the next issue remains `M270-E001`

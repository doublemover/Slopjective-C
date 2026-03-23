# Packet: `M270-D003`

- Issue: `#7317`
- Milestone: `M270`
- Lane: `D`
- Title: `Cross-module isolation metadata hardening - Edge-case and compatibility completion`
- Contract ID: `objc3c-cross-module-actor-isolation-metadata-hardening/m270-d003-v1`

## Scope

Harden the cross-module actor runtime boundary by preserving actor mailbox/isolation replay facts through:

- `module.runtime-import-surface.json`
- `module.cross-module-runtime-link-plan.json`
- mixed-module runtime archive/link-plan validation

## Proof

- provider fixture: `tests/tooling/fixtures/native/m270_d003_cross_module_actor_isolation_provider.objc3`
- consumer fixture: `tests/tooling/fixtures/native/m270_d003_cross_module_actor_isolation_consumer.objc3`
- checker: `scripts/check_m270_d003_cross_module_isolation_metadata_hardening_edge_case_and_compatibility_completion.py`

## Continuity

- depends on `M270-D002`
- depends on `M270-C003`
- next issue remains `M270-E001`

# M314-B001 Planning Packet

## Summary

Freeze the semantic model for the command surface so `M314-B002` through `M314-D001` can simplify it without ambiguity.

## Public-versus-internal model

- Public operator layer:
  - package scripts only
  - small documented set
  - frozen public families: `build`, `compile`, `lint`, `test`, `package`, `tool`, `proof`
- Internal layer:
  - Python parameterized runners under `scripts/`
- Implementation layer:
  - PowerShell wrappers and helper scripts
- CI-only layer:
  - `.github/workflows/*.yml`

## Compatibility model

- Legacy milestone-local package aliases remain temporary compatibility surfaces only.
- README direct Python and PowerShell commands remain transitional documentation leaks only.
- `M314-B004` owns compatibility-window policy.
- `M314-B005` owns the dead prototype compiler path retirement.

## Prohibited growth

- No new public `plan:*`, `refresh:*`, `run:*`, or `dev:*` commands.
- No new public operator docs that bypass package scripts where a public package entrypoint exists.
- No new CI workflow invocations documented as operator commands.
- No new public compiler path outside `native/objc3c/`.

## Evidence

- `spec/planning/compiler/m314/m314_b001_public_versus_internal_command_model_contract_and_architecture_freeze_model.json`

Next issue: `M314-B002`.

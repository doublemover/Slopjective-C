# M315-B004 Planning Packet

Issue: `#7799`  
Title: `Synthetic-versus-genuine IR fixture compatibility semantics`

## Objective

Implement one truthful compatibility model for tracked `.ll` fixtures so the repo
can distinguish explicit synthetic parity stubs from replay candidates without
claiming that every committed IR file is already a fully provenance-backed genuine
compiler output.

## Implemented semantics

- `synthetic_fixture`
  - allowed only under `tests/tooling/fixtures/native/library_cli_parity/**`
  - must carry the explicit `fixture parity IR` header
- `generated_replay_candidate`
  - allowed only under `tests/tooling/fixtures/objc3c/**/replay_run_*/module.ll`
  - may carry the `objc3c native frontend IR` header
  - missing-header cases remain allowed only as a tracked legacy debt for later
    provenance regeneration work

## Frozen baseline

- tracked `.ll` files: `78`
- explicit synthetic `.ll`: `2`
- replay candidate `.ll`: `76`
- replay `.ll` with frontend header: `30`
- replay `.ll` without frontend header: `46`

## Validation posture

Static verification is justified because this issue installs the classification and
compatibility guard for the currently tracked `.ll` surface.

Next issue: `M315-B005`.

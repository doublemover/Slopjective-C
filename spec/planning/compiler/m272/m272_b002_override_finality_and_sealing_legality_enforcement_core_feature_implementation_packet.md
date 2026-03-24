# M272-B002 Packet: Override, Finality, And Sealing Legality Enforcement - Core Feature Implementation

Issue: `#7337`

## Goal

Implement live Part 9 lane-B legality enforcement for superclass finality,
superclass sealing, and direct/final override restrictions.

## Scope

This issue is sema-only.
It must:

- publish one dedicated Part 9 legality packet,
- consume the existing `M272-B001` semantic-model packet,
- fail closed on:
  - `objc_final` superclass inheritance,
  - `objc_sealed` superclass inheritance,
  - `objc_final` method override,
  - override chains that use `objc_direct` dispatch.

This issue does not claim:

- direct-call lowering,
- metadata realization,
- runnable dispatch-boundary behavior.

## Required packet

The emitted frontend manifest must publish:

- `frontend.pipeline.semantic_surface.objc_part9_override_finality_and_sealing_legality`

## Required diagnostics

- `O3S307`
- `O3S308`
- `O3S309`
- `O3S310`

## Positive fixture proof

The positive fixture must publish:

- `subclass_sites = 1`
- `override_sites = 1`
- `illegal_final_superclass_sites = 0`
- `illegal_sealed_superclass_sites = 0`
- `illegal_final_override_sites = 0`
- `illegal_direct_override_sites = 0`

## Required code anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/DECISIONS_LOG.md`

## Validation

- static checker verifies code/docs/spec/package anchors
- dynamic checker proves the happy path through `objc3c-frontend-c-api-runner.exe`
  with `--no-emit-ir --no-emit-object`
- dynamic checker proves the four negative fixtures fail closed with
  `O3S307`, `O3S308`, `O3S309`, and `O3S310`

## Next issue

- `M272-B003`

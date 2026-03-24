# M272-B003 Packet: Compatibility Diagnostics For Dynamism Controls - Edge-Case And Compatibility Completion

Issue: `#7338`

## Goal

Close the remaining Part 9 compatibility and unsupported-topology diagnostics
for dispatch-intent and dynamism-control surfaces.

## Scope

This issue is sema-only.
It must:

- publish one dedicated Part 9 compatibility packet,
- consume the existing `M272-B002` legality packet,
- fail closed on:
  - `objc_direct` + `objc_dynamic` callable conflicts,
  - `objc_final` + `objc_dynamic` callable conflicts,
  - dispatch-intent callable attributes on free functions,
  - dispatch-intent callable attributes on protocol methods,
  - dispatch-intent callable attributes on category methods,
  - `objc_direct_members` / `objc_final` / `objc_sealed` on categories.

This issue does not claim:

- direct-call lowering,
- metadata realization,
- runnable dispatch-boundary behavior.

## Required packet

The emitted frontend manifest must publish:

- `frontend.pipeline.semantic_surface.objc_part9_dynamism_control_compatibility_diagnostics`

## Required diagnostics

- `O3S311`
- `O3S312`
- `O3S313`
- `O3S314`
- `O3S315`
- `O3S316`

## Positive fixture proof

The positive fixture must publish:

- `callable_dispatch_intent_sites = 4`
- `container_dispatch_intent_sites = 1`
- `illegal_direct_dynamic_conflict_sites = 0`
- `illegal_final_dynamic_conflict_sites = 0`
- `illegal_non_method_callable_sites = 0`
- `illegal_protocol_method_sites = 0`
- `illegal_category_method_sites = 0`
- `illegal_category_container_sites = 0`

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
- dynamic checker proves the six negative fixtures fail closed with
  `O3S311`, `O3S312`, `O3S313`, `O3S314`, `O3S315`, and `O3S316`

## Next issue

- `M272-C001`

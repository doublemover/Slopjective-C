# M272-B001 Packet: Dynamism And Dispatch-Control Semantic Model - Contract And Architecture Freeze

Issue: `#7336`

## Goal

Freeze one truthful lane-B semantic-model packet for the current Part 9
dynamism/dispatch-control surface before later `M272` issues widen legality,
lowering, metadata, and runtime behavior.

## Scope

This issue is sema/accounting only.
It must:

- publish one dedicated Part 9 semantic-model packet,
- consume the existing `M272-A002` Part 9 source-completion packet,
- reuse the existing override lookup/conflict accounting surface,
- preserve deterministic counts for:
  - prefixed container-attribute sites,
  - `objc_direct_members` / `objc_final` / `objc_sealed` container sites,
  - effective direct-member sites,
  - direct-members defaulted method sites,
  - `objc_dynamic` opt-out sites,
  - override lookup hits, misses, conflicts, and unresolved base-interface
    counts.

This issue does not claim:

- direct-call lowering,
- final/sealed legality enforcement,
- metadata realization,
- runnable dispatch-boundary behavior.

## Required packet

The emitted frontend manifest must publish:

- `frontend.pipeline.semantic_surface.objc_part9_dynamism_and_dispatch_control_semantic_model`

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
- dynamic checker verifies the emitted manifest publishes deterministic counts at
  `frontend.pipeline.semantic_surface.objc_part9_dynamism_and_dispatch_control_semantic_model`

## Positive fixture proof

The positive fixture must publish:

- `prefixed_container_attribute_sites = 1`
- `direct_members_container_sites = 1`
- `final_container_sites = 1`
- `sealed_container_sites = 1`
- `effective_direct_member_sites = 4`
- `direct_members_defaulted_method_sites = 2`
- `direct_members_dynamic_opt_out_sites = 2`
- `override_lookup_sites = 3`
- `override_lookup_hits = 1`
- `override_lookup_misses = 2`
- `override_conflicts = 0`
- `unresolved_base_interfaces = 0`

## Next issue

- `M272-B002`

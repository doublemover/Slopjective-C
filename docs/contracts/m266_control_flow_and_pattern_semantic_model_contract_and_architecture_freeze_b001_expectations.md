# M266-B001 Expectations

Issue: `M266-B001`
Contract ID: `objc3c-part5-control-flow-semantic-model/m266-b001-v1`
Surface path: `frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model`

## Goal

Freeze one truthful semantic-model packet for the currently implemented Part 5 control-flow surface.

## Required truths

- `guard` binding refinement is live in sema.
- `guard` condition clauses are validated as bool-compatible sema inputs.
- `guard ... else` must exit the current scope.
- statement-form `match` creates deterministic case-local binding scopes.
- result-case patterns create deterministic case-local binding scopes.
- match exhaustiveness is not implemented yet and must remain marked as deferred.
- `defer` cleanup ordering is not implemented yet and must remain marked as deferred.
- `break` legality is live.
- `continue` legality is live.
- the semantic model must remain deterministic and replayable.

## Code anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Spec/doc anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/CROSS_CUTTING_RULE_INDEX.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Required validation

- issue-local checker proves the contract/doc/code anchors exist
- positive fixture proves the semantic-surface payload is emitted with the expected live/deferred counts
- negative fixtures prove:
  - `guard ... else` non-exit fails closed
  - `break` outside loop/switch fails closed
  - `continue` outside loop fails closed
- issue-local pytest passes
- lane-B readiness runner passes

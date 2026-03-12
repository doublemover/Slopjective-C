# M266-B003 Expectations

Issue: `M266-B003`
Surface: `frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model`
Contract ID: `objc3c-part5-control-flow-semantic-model/m266-b001-v1`

## Goal

Turn `defer` from a contract-only placeholder into a truthful source-only semantic capability with deterministic cleanup-order accounting and deterministic non-local-exit diagnostics.

## Required truths

- statement-form `defer { ... }` is admitted in the source-only Part 5 frontend/sema path
- `defer` contributes live sema accounting to the Part 5 semantic packet
- `defer` cleanup ordering is modeled as deterministic LIFO scope-exit cleanup ordering in sema
- `return` inside a `defer` body fails closed with deterministic diagnostics
- `break` and `continue` may not escape a `defer` body
- loop-local `break` and `continue` inside a loop nested within a `defer` body remain legal
- native emit paths still fail closed for runnable `defer` lowering until later lane-C/lane-D work lands

## Required validation

- positive source-only fixture proves admitted `defer` sites compile and emit the updated semantic packet
- negative fixtures prove `return`, escaping `break`, and escaping `continue` fail closed with deterministic diagnostics
- checker, pytest, and lane-B readiness all pass

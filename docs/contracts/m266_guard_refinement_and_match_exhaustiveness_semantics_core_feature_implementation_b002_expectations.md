# M266-B002 Expectations

Issue: `M266-B002`
Surface: `frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model`
Contract ID: `objc3c-part5-control-flow-semantic-model/m266-b001-v1`

## Goal

Turn the current Part 5 semantic packet from a deferred exhaustiveness placeholder into a truthful live semantic surface.

## Required truths

- guard refinement remains live
- guard else-exit enforcement remains live
- admitted `match` statements must be semantically exhaustive
- currently admitted exhaustive forms are:
  - `default`
  - wildcard `_`
  - binding catch-all `let name` / `var name`
  - `true` plus `false`
  - `.Ok(...)` plus `.Err(...)`
- non-exhaustive admitted `match` statements fail closed with deterministic diagnostics
- `defer` cleanup ordering remains deferred
- result payload typing beyond binding-scope behavior remains deferred

## Required validation

- positive fixture proves exhaustive bool and result-case matches compile and emit the updated packet
- negative fixtures prove non-exhaustive bool and result-case matches fail closed
- checker, pytest, and lane-B readiness all pass

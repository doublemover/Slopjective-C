# M266 Match Lowering Dispatch And Exhaustiveness Runtime Alignment Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-part5-match-lowering-runtime-alignment/m266-c003-v1`
Issue: `#7264`
Scope: truthful lane-C expansion of the already published Part 5 lowering contract.

## Objective

`M266-C003` makes the admitted runnable `match` slice real in native lowering without claiming a runtime `Result` payload ABI that does not exist yet.

This issue covers:

- native lowering for exhaustive `match` over `true` plus `false`
- native lowering for literal/default/wildcard/binding statement-form `match` arms
- case-local binding materialization for `case let name` / `case var name`
- truthful manifest/IR evidence on the existing Part 5 lowering surface

This issue does not cover:

- result-case payload extraction for `.Ok(let value)` / `.Err(let error)`
- `match` expressions
- guarded patterns
- type-test patterns

## Required Invariants

1. The native path continues to publish the lowering packet at `frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract`.
2. The packet contract id remains `objc3c-part5-control-flow-safety-lowering/m266-c001-v1`.
3. For lowerable `match` programs, the packet must now truthfully publish:
   - `live_match_dispatch_sites = match_statement_sites`
   - `fail_closed_match_dispatch_sites = 0`
   - `ready_for_native_match_lowering = true`
4. For programs that still use result-case patterns, the same packet must remain truthful and fail closed:
   - `live_match_dispatch_sites = 0`
   - `fail_closed_match_dispatch_sites = match_statement_sites`
   - `ready_for_native_match_lowering = false`
5. The emitted IR must retain the existing `; part5_control_flow_safety_lowering = ...` replay boundary comment.
6. Bound `case let` / `case var` names must be materialized as case-local storage before the case body executes.
7. Non-exhaustive bool and result-case `match` programs must continue to fail closed with deterministic `O3S206` diagnostics from the semantic layer.

## Dynamic Coverage

The issue-local checker must prove all of the following:

1. A generated exhaustive bool `match` probe compiles, emits `module.manifest.json`, `module.ll`, and `module.obj`, and publishes the live `match` lowering counts on the existing Part 5 lowering packet.
2. A generated literal-plus-binding `match` probe compiles, emits the same artifacts, and proves case-local binding materialization is now executable.
3. Generated non-exhaustive bool and result-case probes still fail closed with `O3S206` diagnostics.
4. A generated result-case observation probe still fails closed deterministically with `O3L300`, and the summary records that as intentional deferred scope rather than success.
5. Evidence lands under `tmp/reports/m266/M266-C003/`.

## Validation

- `python scripts/check_m266_c003_match_lowering_dispatch_and_exhaustiveness_runtime_alignment_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m266_c003_match_lowering_dispatch_and_exhaustiveness_runtime_alignment_core_feature_implementation.py -q`
- `python scripts/run_m266_c003_lane_c_readiness.py`

## Evidence Path

- `tmp/reports/m266/M266-C003/match_lowering_dispatch_and_exhaustiveness_runtime_alignment_summary.json`

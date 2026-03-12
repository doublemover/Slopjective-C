# M266-C003 Match Lowering Dispatch And Exhaustiveness Runtime Alignment Core Feature Implementation Packet

Packet: `M266-C003`
Issue: `#7264`
Milestone: `M266`
Lane: `C`
Wave: `W59`
Contract ID: `objc3c-part5-match-lowering-runtime-alignment/m266-c003-v1`

## Why This Issue Exists

`M266-B002` already made the admitted statement-form `match` semantic slice live:

- exhaustive bool `match`
- exhaustive result-case `match` as a semantic shape
- case-local binding scopes
- deterministic `O3S206` diagnostics for non-exhaustive admitted forms

`M266-C002` made `guard` and `defer` lowering truthful, but `match` lowering still remained completely fail closed. `M266-C003` exists to remove that mismatch for the subset the compiler can actually execute today.

## Implementation Goal

Make native lowering truthful for:

- exhaustive bool `match`
- literal/default/wildcard/binding statement-form `match`
- case-local binding materialization for binding arms

Keep result-case payload matching explicitly deferred until a runtime `Result` payload ABI exists.

## Required Implementation Shape

1. Reuse the existing Part 5 lowering packet instead of inventing a parallel lane-C packet.
2. Update the lowering counts so the packet distinguishes live lowerable `match` sites from still fail-closed result-case sites.
3. Materialize `case let name` / `case var name` bindings as case-local storage visible to the case body.
4. Preserve deterministic source-order dispatch for literal tests and catch-all arms.
5. End non-terminated `match` arms at the `match` merge label rather than inheriting `switch` fallthrough semantics.
6. Preserve explicit fail-closed behavior for result-case patterns with deterministic `O3L300` diagnostics.

## Truthful Acceptance Criteria

- A generated exhaustive bool `match` probe compiles successfully and emits the live lowering counts on the existing Part 5 lowering packet.
- A generated literal-plus-binding `match` probe compiles successfully and proves executable case-local binding materialization.
- Generated non-exhaustive bool and result-case probes still fail with deterministic `O3S206`.
- A generated result-case observation probe still fails deterministically with `O3L300`.
- The emitted IR still carries the Part 5 lowering replay boundary comment.
- Evidence is written under `tmp/reports/m266/M266-C003/`.

## Validation Commands

- `python scripts/check_m266_c003_match_lowering_dispatch_and_exhaustiveness_runtime_alignment_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m266_c003_match_lowering_dispatch_and_exhaustiveness_runtime_alignment_core_feature_implementation.py -q`
- `python scripts/run_m266_c003_lane_c_readiness.py`

## Evidence Output

- `tmp/reports/m266/M266-C003/match_lowering_dispatch_and_exhaustiveness_runtime_alignment_summary.json`

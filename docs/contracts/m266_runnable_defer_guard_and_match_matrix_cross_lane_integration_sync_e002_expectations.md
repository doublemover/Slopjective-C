# M266 Runnable Defer, Guard, And Match Matrix Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-part5-runnable-control-flow-matrix/m266-e002-v1`

Scope: `M266-E002` closes `M266` by publishing the compact runnable matrix for
the already-supported Part 5 control-flow slice.

## Required outcomes

1. The closeout matrix must line up the already-landed `M266-A002`,
   `M266-B003`, `M266-C003`, `M266-D002`, and `M266-E001` evidence surfaces.
2. The matrix must prove the currently runnable Part 5 paths only:
   - ordinary lexical `defer` cleanup execution
   - guard-mediated early return cleanup execution
   - nested-scope return unwind ordering
   - one integrated native `guard` + supported statement-form `match` + `defer`
     program
3. The matrix must not claim more than the frozen `M266-E001` boundary:
   - no expression-form `match`
   - no guarded patterns using `where`
   - no type-test patterns
   - no public cleanup/unwind runtime ABI widening
4. Docs/spec/package/code anchors must remain explicit and deterministic.
5. Validation evidence must land at
   `tmp/reports/m266/M266-E002/runnable_control_flow_matrix_summary.json`.

## Canonical proof artifacts

- `tmp/reports/m266/M266-A002/frontend_pattern_guard_surface_summary.json`
- `tmp/reports/m266/M266-B003/defer_legality_cleanup_order_summary.json`
- `tmp/reports/m266/M266-C003/match_lowering_dispatch_and_exhaustiveness_runtime_alignment_summary.json`
- `tmp/reports/m266/M266-D002/runtime_cleanup_and_unwind_integration_summary.json`
- `tmp/reports/m266/M266-E001/control_flow_execution_gate_summary.json`
- `scripts/check_m266_e002_runnable_defer_guard_and_match_matrix_cross_lane_integration_sync.py`
- `tests/tooling/test_check_m266_e002_runnable_defer_guard_and_match_matrix_cross_lane_integration_sync.py`
- `scripts/run_m266_e002_lane_e_readiness.py`

## Truthful boundary

- This issue is a closeout matrix and docs sync only.
- The `B003` dependency is consumed as a historical semantic guardrail; the
  runnable execution claims are carried by `D002` and `E001`.
- `M267-A001` is the next issue after `M266` closes.

# M262 Runnable ARC Conformance Matrix, Execution Smoke, And Operator Docs Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-runnable-arc-closeout/m262-e002-v1`

Issue: `#7207`

## Objective

Close the current ARC milestone without widening the supported ARC slice.
`M262-E002` proves the supported ARC closeout surface through:

- the upstream `A002/B003/C004/D003/E001` proof chain
- ARC-positive source execution rows in the canonical execution-smoke corpus
- the private `D003` property/runtime probe row
- the published operator runbook for the closeout commands

## Required implementation

1. Add a canonical expectations document and packet for the closeout issue.
2. Add a runbook at `docs/runbooks/m262_arc_runtime_closeout_runbook.md`.
3. Add:
   - `scripts/check_m262_e002_runnable_arc_conformance_matrix_execution_smoke_and_operator_docs_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m262_e002_runnable_arc_conformance_matrix_execution_smoke_and_operator_docs_cross_lane_integration_sync.py`
   - `scripts/run_m262_e002_lane_e_readiness.py`
4. Add positive execution-smoke fixtures that exercise the supported ARC source
   slice under `-fobjc-arc`:
   - cleanup scope
   - implicit cleanup/void sink
   - block/autorelease return
5. Keep the closeout fail closed over:
   - `tmp/reports/m262/M262-A002/arc_mode_handling_summary.json`
   - `tmp/reports/m262/M262-B003/arc_interaction_semantics_summary.json`
   - `tmp/reports/m262/M262-C004/arc_block_autorelease_return_lowering_summary.json`
   - `tmp/reports/m262/M262-D003/arc_debug_instrumentation_summary.json`
   - `tmp/reports/m262/M262-E001/arc_runtime_gate_summary.json`
   - `tmp/artifacts/objc3c-native/execution-smoke/m262_e002_arc_closeout/summary.json`
6. Code/spec anchors must remain explicit and deterministic.
7. `package.json` must wire:
   - `check:objc3c:m262-e002-runnable-arc-closeout`
   - `test:tooling:m262-e002-runnable-arc-closeout`
   - `check:objc3c:m262-e002-lane-e-readiness`
8. The closeout must explicitly hand off to `M263-A001`.

## Canonical models

- Matrix model:
  `closeout-matrix-consumes-a002-b003-c004-d003-and-e001-evidence-without-widening-the-supported-runnable-arc-slice`
- Runnable smoke model:
  `integrated-arc-fixtures-and-private-property-runtime-probes-prove-supported-cleanup-block-and-property-behavior-through-native-toolchain-and-runtime`
- Failure model:
  `fail-closed-on-runnable-arc-closeout-drift-or-runbook-mismatch`

## Non-goals

- No new ARC source-surface expansion.
- No new ARC semantic or lowering behavior.
- No public ARC runtime ABI widening.
- No claim that source-level property allocation/construction is now a public language feature.
- No module/import ARC closeout; that belongs to later milestones.

## Evidence

- `tmp/reports/m262/M262-E002/runnable_arc_closeout_summary.json`

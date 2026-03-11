# M260 Runnable Ownership Smoke Matrix And Docs Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-runnable-ownership-smoke-matrix/m260-e002-v1`

Scope: `M260-E002` closes the M260 ownership baseline by publishing the compact
matrix that proves the existing runtime-backed ownership slice is meaningfully
usable for object programs.

## Required outcomes

1. The closeout matrix must line up the already-landed `M260-A002`,
   `M260-B003`, `M260-C002`, `M260-D002`, and `M260-E001` evidence surfaces.
2. The matrix must prove one truthful integrated runtime path:
   - strong runtime-backed property ownership
   - weak zeroing on final release
   - `@autoreleasepool` push/pop drain behavior
3. The matrix must not claim more than the frozen `M260-E001` boundary:
   - no ARC automation
   - no block ownership runtime
   - no public ownership/autoreleasepool ABI widening
4. Docs/spec/package/code anchors must remain explicit and deterministic.
5. Validation evidence must land at
   `tmp/reports/m260/M260-E002/runnable_ownership_smoke_matrix_summary.json`.

## Canonical proof artifacts

- `tmp/reports/m260/M260-A002/runtime_backed_object_ownership_attribute_surface_summary.json`
- `tmp/reports/m260/M260-B003/pytest_autoreleasepool_destruction_order_semantics_summary.json`
- `tmp/reports/m260/M260-C002/ownership_runtime_hook_emission_summary.json`
- `tmp/reports/m260/M260-D002/reference_counting_weak_autoreleasepool_summary.json`
- `tmp/reports/m260/M260-E001/ownership_runtime_gate_contract_summary.json`
- `scripts/check_m260_e002_runnable_ownership_smoke_matrix_and_docs_cross_lane_integration_sync.py`
- `tests/tooling/test_check_m260_e002_runnable_ownership_smoke_matrix_and_docs_cross_lane_integration_sync.py`
- `scripts/run_m260_e002_lane_e_readiness.py`

## Truthful boundary

- This issue is a closeout matrix and documentation sync only.
- The `B003` dependency is consumed as historical semantic-guardrail proof; the
  live runtime path is now carried by `D002` and `E001`.
- `M261-A001` is the next issue after M260 closes.

# Lowering/Runtime Stability and Invariant Proofs Conformance Matrix Expectations (M250-C009)

Contract ID: `objc3c-lowering-runtime-stability-conformance-matrix/m250-c009-v1`
Status: Accepted
Scope: lane-C lowering/runtime conformance-matrix guardrails.

## Objective

Expand C008 recovery/determinism closure with explicit conformance-matrix consistency and readiness gates so lowering/runtime stability fails closed on matrix drift.

## Deterministic Invariants

1. `Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface` carries conformance-matrix fields:
   - `conformance_matrix_consistent`
   - `conformance_matrix_ready`
   - `conformance_matrix_key`
2. `BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(...)` computes conformance matrix deterministically from:
   - C008 recovery/determinism closure
   - lane-A conformance-matrix readiness (`long_tail_grammar_conformance_matrix_*`)
   - parse conformance matrix surfaces
   - non-empty conformance-matrix keys
3. `expansion_ready` remains fail-closed and now requires conformance-matrix readiness.
4. `expansion_key` includes conformance-matrix evidence so runtime replay identity remains deterministic.
5. Failure reasons remain explicit for lowering/runtime conformance-matrix drift.
6. C008 remains a mandatory prerequisite.

## Validation

- `python scripts/check_m250_c009_lowering_runtime_stability_conformance_matrix_contract.py`
- `python -m pytest tests/tooling/test_check_m250_c009_lowering_runtime_stability_conformance_matrix_contract.py -q`
- `npm run check:objc3c:m250-c009-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m250/M250-C009/lowering_runtime_stability_conformance_matrix_contract_summary.json`

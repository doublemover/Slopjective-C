# Lowering/Runtime Stability and Invariant Proofs Edge-Case and Compatibility Completion Expectations (M250-C005)

Contract ID: `objc3c-lowering-runtime-stability-edge-case-compatibility-completion/m250-c005-v1`
Status: Accepted
Scope: lane-C lowering/runtime edge-case and compatibility completion built on C004 expansion guardrails.

## Objective

Complete lane-C edge-case compatibility closure by threading parse/lowering compatibility and edge-robustness readiness into lowering/runtime core-feature expansion gates.

## Deterministic Invariants

1. `Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface` carries explicit edge-case compatibility fields:
   - `compatibility_handoff_consistent`
   - `language_version_pragma_coordinate_order_consistent`
   - `parse_edge_case_robustness_consistent`
   - `edge_case_compatibility_ready`
   - `edge_case_compatibility_key`
2. `BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(...)` computes `edge_case_compatibility_ready` from parse readiness compatibility + edge robustness + replay determinism.
3. `expansion_ready` remains fail-closed and now requires `edge_case_compatibility_ready`.
4. Failure reasons include explicit edge-case compatibility drift.
5. C004 remains a mandatory prerequisite.

## Validation

- `python scripts/check_m250_c005_lowering_runtime_stability_edge_case_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m250_c005_lowering_runtime_stability_edge_case_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m250-c005-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m250/M250-C005/lowering_runtime_stability_edge_case_compatibility_completion_contract_summary.json`

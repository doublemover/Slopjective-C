# Lowering/Runtime Stability and Invariant Proofs Core Feature Expansion Expectations (M250-C004)

Contract ID: `objc3c-lowering-runtime-stability-core-feature-expansion/m250-c004-v1`
Status: Accepted
Scope: lane-C core-feature expansion accounting and replay-key guardrails for lowering/runtime stability readiness.

## Objective

Expand C003 lowering/runtime core-feature closure with explicit expansion-accounting and replay-key guardrails so readiness fails closed on accounting drift rather than implicit aggregate predicates.

## Deterministic Invariants

1. `Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface` carries explicit expansion guardrail fields:
   - `typed_expansion_accounting_consistent`
   - `parse_conformance_accounting_consistent`
   - `replay_keys_ready`
   - `expansion_ready`
   - `expansion_key`
2. `BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(...)` computes expansion readiness deterministically from typed/parse accounting and replay-key availability.
3. `core_feature_impl_ready` remains fail-closed and requires `expansion_ready`.
4. Failure reasons remain explicit for typed expansion accounting, parse conformance accounting, replay keys, and expansion readiness drift.
5. C003 remains a mandatory prerequisite.

## Validation

- `python scripts/check_m250_c004_lowering_runtime_stability_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m250_c004_lowering_runtime_stability_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m250-c004-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m250/M250-C004/lowering_runtime_stability_core_feature_expansion_contract_summary.json`

# M250-C004 Lowering/Runtime Stability and Invariant Proofs Core Feature Expansion Packet

Packet: `M250-C004`
Milestone: `M250`
Lane: `C`
Dependencies: `M250-C003`

## Scope

Expand lane-C lowering/runtime core-feature implementation with explicit expansion-accounting and replay-key guardrails.

## Anchors

- Contract: `docs/contracts/m250_lowering_runtime_stability_core_feature_expansion_c004_expectations.md`
- Checker: `scripts/check_m250_c004_lowering_runtime_stability_core_feature_expansion_contract.py`
- Tooling tests: `tests/tooling/test_check_m250_c004_lowering_runtime_stability_core_feature_expansion_contract.py`
- C004 surface: `native/objc3c/src/pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
- Pipeline surface types: `native/objc3c/src/pipeline/objc3_frontend_types.h`

## Required Evidence

- `tmp/reports/m250/M250-C004/lowering_runtime_stability_core_feature_expansion_contract_summary.json`

## Determinism Criteria

- Expansion-accounting and replay-key guardrails are first-class lane-C readiness fields.
- C004 remains fail-closed and deterministic when accounting or replay-key drift occurs.

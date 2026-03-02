# M227 Typed Sema-to-Lowering Core Feature Expectations (C003)

Contract ID: `objc3c-typed-sema-to-lowering-core-feature/m227-c003-v1`
Status: Accepted
Scope: typed sema-to-lowering core feature accounting and parse/lowering readiness integration.

## Objective

Implement core feature accounting for typed sema-to-lowering handoff so readiness depends on explicit case-level consistency and deterministic typed handoff keys.

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries core feature accounting fields:
   - `semantic_handoff_consistent`
   - `semantic_handoff_deterministic`
   - `typed_handoff_key_deterministic`
   - `typed_core_feature_consistent`
   - `typed_core_feature_case_count`, `typed_core_feature_passed_case_count`, `typed_core_feature_failed_case_count`
   - `typed_core_feature_key`
2. Typed contract builder computes deterministic case accounting and core feature key, and binds `ready_for_lowering` to core feature consistency.
3. Parse/lowering readiness surface carries typed core feature accounting fields and requires them for readiness.
4. Parse/lowering readiness uses precomputed typed contract surface when present to avoid duplicate drift.

## Validation

- `python scripts/check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c003_typed_sema_to_lowering_core_feature_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-C003/typed_sema_to_lowering_core_feature_contract_summary.json`

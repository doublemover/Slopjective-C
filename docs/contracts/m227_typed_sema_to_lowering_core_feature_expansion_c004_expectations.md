# M227 Typed Sema-to-Lowering Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-typed-sema-to-lowering-core-feature-expansion/m227-c004-v1`
Status: Accepted
Scope: typed sema-to-lowering core-feature expansion determinism and parse/lowering readiness carry-through.

## Objective

Expand the typed sema-to-lowering contract so protocol/category, class-protocol-category linking, selector normalization, and property attribute deterministic handoffs are explicitly accounted and gated as core-feature expansion.

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries core-feature expansion fields:
   - `protocol_category_handoff_deterministic`
   - `class_protocol_category_linking_handoff_deterministic`
   - `selector_normalization_handoff_deterministic`
   - `property_attribute_handoff_deterministic`
   - `typed_core_feature_expansion_consistent`
   - `typed_core_feature_expansion_case_count`, `typed_core_feature_expansion_passed_case_count`, `typed_core_feature_expansion_failed_case_count`
   - `typed_core_feature_expansion_key`
2. Typed contract builder computes expansion accounting with fixed case budget and requires expansion key determinism for final typed-core consistency.
3. Parse/lowering readiness surface carries expansion consistency, expansion accounting, and expansion key readiness.
4. Parse/lowering typed-surface detection (`HasObjc3TypedSemaToLoweringCoreFeatureSurface`) treats expansion fields as first-class readiness signals.

## Validation

- `python scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-C004/typed_sema_to_lowering_core_feature_expansion_contract_summary.json`

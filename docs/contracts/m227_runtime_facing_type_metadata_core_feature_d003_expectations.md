# Runtime-Facing Type Metadata Core Feature Implementation Expectations (M227-D003)

Contract ID: `objc3c-runtime-facing-type-metadata-core-feature/m227-d003-v1`
Status: Accepted
Scope: Runtime-facing type metadata core feature implementation across typed sema-to-lowering and parse/lowering readiness surfaces.

## Objective

Implement deterministic runtime-facing type metadata core feature accounting so protocol/category linkage, selector normalization, and property attribute handoff semantics are explicitly case-accounted, replay-keyed, and fail-closed in lowering readiness gates.

## Required Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries runtime-facing core feature expansion fields:
   - `protocol_category_handoff_deterministic`
   - `class_protocol_category_linking_handoff_deterministic`
   - `selector_normalization_handoff_deterministic`
   - `property_attribute_handoff_deterministic`
   - `typed_core_feature_expansion_consistent`
   - `typed_core_feature_expansion_case_count`, `typed_core_feature_expansion_passed_case_count`, `typed_core_feature_expansion_failed_case_count`
   - `typed_core_feature_expansion_key`
2. Typed sema-to-lowering contract construction computes deterministic expansion case accounting using `kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount = 4u`, derives `BuildObjc3TypedSemaToLoweringCoreFeatureExpansionKey(...)`, and requires expansion consistency before final core-feature readiness.
3. `Objc3ParseLoweringReadinessSurface` imports typed expansion fields, validates expansion case accounting/key presence, and requires expansion readiness in `typed_core_feature_ready`.
4. Parse/lowering semantic handoff determinism includes runtime-facing deterministic signals for protocol/category handoff, class/protocol/category linking, selector normalization, and property attributes.
5. D003 contract tooling remains fail-closed and emits deterministic JSON summary artifacts under `tmp/reports/m227/`.

## Validation

- `python scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m227/M227-D003/runtime_facing_type_metadata_core_feature_contract_summary.json`

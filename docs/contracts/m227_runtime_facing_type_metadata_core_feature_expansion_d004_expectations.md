# Runtime-Facing Type Metadata Core Feature Expansion Expectations (M227-D004)

Contract ID: `objc3c-runtime-facing-type-metadata-core-feature-expansion/m227-d004-v1`
Status: Accepted
Dependencies: `M227-D003`, `M227-A004`, `M227-B004`, `M227-C004`
Scope: Lane-D core feature expansion dependency continuity for runtime-facing type metadata contract governance.

## Objective

Fail closed unless lane-D core feature expansion anchors remain explicit,
deterministic, and dependency-traceable across prior lane-D baseline outputs and
upstream A/B/C core feature expansion outputs.

## Prerequisite Dependency Matrix

Issue `#5150` governs lane-D core feature expansion scope and dependency-token/reference continuity.

| Lane Task | Frozen Dependency Token | Dependency Reference |
| --- | --- | --- |
| `M227-D003` | `M227-D003` | Contract `docs/contracts/m227_runtime_facing_type_metadata_core_feature_d003_expectations.md`; checker `scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py`; tooling test `tests/tooling/test_check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py`; packet `spec/planning/compiler/m227/m227_d003_runtime_facing_type_metadata_core_feature_packet.md`. |
| `M227-A004` | `M227-A004` | Contract `docs/contracts/m227_semantic_pass_core_feature_expansion_expectations.md`; checker `scripts/check_m227_a004_semantic_pass_core_feature_expansion_contract.py`; tooling test `tests/tooling/test_check_m227_a004_semantic_pass_core_feature_expansion_contract.py`; packet `spec/planning/compiler/m227/m227_a004_semantic_pass_core_feature_expansion_packet.md`. |
| `M227-B004` | `M227-B004` | Contract `docs/contracts/m227_type_system_objc3_forms_core_feature_expansion_b004_expectations.md`; checker `scripts/check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py`; tooling test `tests/tooling/test_check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py`; packet `spec/planning/compiler/m227/m227_b004_type_system_objc3_forms_core_feature_expansion_packet.md`. |
| `M227-C004` | `M227-C004` | Contract `docs/contracts/m227_typed_sema_to_lowering_core_feature_expansion_c004_expectations.md`; checker `scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`; tooling test `tests/tooling/test_check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`; packet `spec/planning/compiler/m227/m227_c004_typed_sema_to_lowering_core_feature_expansion_packet.md`. |

## Deterministic Fail-Closed Assertions

1. Dependency matrix ordering is fixed to `M227-D003`, `M227-A004`, `M227-B004`, `M227-C004`.
2. Frozen dependency token must match the lane-task token exactly for every row.
3. Dependency references remain pinned to the expected checker anchors for each prerequisite lane task.
4. Expectation and packet dependency matrices must remain text-consistent for every row.
5. Checker output is deterministic JSON and fails closed on any snippet, matrix, or asset drift.

## Validation

- `python scripts/check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-D004/runtime_facing_type_metadata_core_feature_expansion_contract_summary.json`

# M227-D004 Runtime-Facing Type Metadata Core Feature Expansion Packet

Packet: `M227-D004`
Milestone: `M227`
Lane: `D`
Issue: `#5150`
Scaffold date: `2026-03-03`
Dependencies: `M227-D003`, `M227-A004`, `M227-B004`, `M227-C004`

## Purpose

Execute lane-D runtime-facing type metadata core feature expansion governance by
freezing prerequisite dependency tokens/references and failing closed on
dependency drift before downstream lane-D edge-compatibility workpacks.

## Scope Anchors

- Contract:
  `docs/contracts/m227_runtime_facing_type_metadata_core_feature_expansion_d004_expectations.md`
- Checker:
  `scripts/check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py`
- Evidence summary:
  `tmp/reports/m227/M227-D004/runtime_facing_type_metadata_core_feature_expansion_contract_summary.json`

## Frozen Dependency Tokens and References

| Lane Task | Frozen Dependency Token | Dependency Reference |
| --- | --- | --- |
| `M227-D003` | `M227-D003` | Contract `docs/contracts/m227_runtime_facing_type_metadata_core_feature_d003_expectations.md`; checker `scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py`; tooling test `tests/tooling/test_check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py`; packet `spec/planning/compiler/m227/m227_d003_runtime_facing_type_metadata_core_feature_packet.md`. |
| `M227-A004` | `M227-A004` | Contract `docs/contracts/m227_semantic_pass_core_feature_expansion_expectations.md`; checker `scripts/check_m227_a004_semantic_pass_core_feature_expansion_contract.py`; tooling test `tests/tooling/test_check_m227_a004_semantic_pass_core_feature_expansion_contract.py`; packet `spec/planning/compiler/m227/m227_a004_semantic_pass_core_feature_expansion_packet.md`. |
| `M227-B004` | `M227-B004` | Contract `docs/contracts/m227_type_system_objc3_forms_core_feature_expansion_b004_expectations.md`; checker `scripts/check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py`; tooling test `tests/tooling/test_check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py`; packet `spec/planning/compiler/m227/m227_b004_type_system_objc3_forms_core_feature_expansion_packet.md`. |
| `M227-C004` | `M227-C004` | Contract `docs/contracts/m227_typed_sema_to_lowering_core_feature_expansion_c004_expectations.md`; checker `scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`; tooling test `tests/tooling/test_check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`; packet `spec/planning/compiler/m227/m227_c004_typed_sema_to_lowering_core_feature_expansion_packet.md`. |

Dependency matrices in the expectations contract and this packet must remain
identical and deterministic; checker semantics fail closed on token drift,
reference drift, row-order drift, or row-count drift.

## Gate Commands

- `python scripts/check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py -q`

## Evidence Output

- `tmp/reports/m227/M227-D004/runtime_facing_type_metadata_core_feature_expansion_contract_summary.json`

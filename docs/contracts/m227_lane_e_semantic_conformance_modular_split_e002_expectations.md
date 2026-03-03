# M227 Lane E Semantic Conformance Modular Split and Scaffolding Expectations (E002)

Contract ID: `objc3c-lane-e-semantic-conformance-modular-split-contract/m227-e002-v1`
Status: Accepted
Scope: M227 lane-E modular split/scaffolding continuity for semantic conformance dependency governance and readiness wiring.

## Objective

Fail closed unless lane-E semantic conformance modular split/scaffolding anchors remain explicit, deterministic, and traceable across upstream dependency assets, lane-E planning artifacts, shared architecture/spec anchors, and package readiness wiring.

## Dependency Scope

| Lane Task | Required Contract Assets |
| --- | --- |
| `M227-E001` | `docs/contracts/m227_lane_e_semantic_conformance_quality_gate_expectations.md`, `scripts/check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py`, `tests/tooling/test_check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py`, `spec/planning/compiler/m227/m227_e001_semantic_conformance_lane_e_quality_gate_contract_freeze.md` |
| `M227-A002` | `docs/contracts/m227_semantic_pass_modular_split_expectations.md`, `scripts/check_m227_a002_semantic_pass_modular_split_contract.py`, `tests/tooling/test_check_m227_a002_semantic_pass_modular_split_contract.py`, `spec/planning/compiler/m227/m227_a002_semantic_pass_modular_split_packet.md` |
| `M227-B004` | `docs/contracts/m227_type_system_objc3_forms_core_feature_expansion_b004_expectations.md`, `scripts/check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py`, `tests/tooling/test_check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py`, `spec/planning/compiler/m227/m227_b004_type_system_objc3_forms_core_feature_expansion_packet.md` |
| `M227-C003` | `docs/contracts/m227_typed_sema_to_lowering_core_feature_c003_expectations.md`, `scripts/check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`, `tests/tooling/test_check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`, `spec/planning/compiler/m227/m227_c003_typed_sema_to_lowering_core_feature_packet.md` |
| `M227-D002` | `docs/contracts/m227_runtime_facing_type_metadata_modular_split_d002_expectations.md`, `scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`, `tests/tooling/test_check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`, `spec/planning/compiler/m227/m227_d002_runtime_facing_type_metadata_modular_split_packet.md` |

- Issue `#5160` defines canonical lane-E modular split/scaffolding scope.
- Dependencies: `M227-E001`, `M227-A002`, `M227-B004`, `M227-C003`, `M227-D002`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m227/m227_e002_semantic_conformance_lane_e_modular_split_packet.md`
  - `scripts/check_m227_e002_semantic_conformance_lane_e_modular_split_contract.py`
  - `tests/tooling/test_check_m227_e002_semantic_conformance_lane_e_modular_split_contract.py`

## Modular Split and Scaffolding Integration

- Packet anchor: `spec/planning/compiler/m227/m227_e002_semantic_conformance_lane_e_modular_split_packet.md`.
- Checker persists fail-closed evidence summary at `tmp/reports/m227/M227-E002/semantic_conformance_lane_e_modular_split_contract_summary.json`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` contains explicit M227 lane-E E002 modular split/scaffolding dependency anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` contains fail-closed lane-E modular split/scaffolding governance wording for dependency-token and readiness continuity.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` contains deterministic lane-E modular split/scaffolding dependency-anchor wording for semantic conformance metadata governance.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m227-e002-semantic-conformance-lane-e-modular-split-contract`.
- `package.json` includes `test:tooling:m227-e002-semantic-conformance-lane-e-modular-split-contract`.
- `package.json` includes `check:objc3c:m227-e002-lane-e-modular-split-readiness`.

## Milestone Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m227_e002_semantic_conformance_lane_e_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m227_e002_semantic_conformance_lane_e_modular_split_contract.py -q`
- `npm run check:objc3c:m227-e002-lane-e-modular-split-readiness`

## Evidence Path

- `tmp/reports/m227/M227-E002/semantic_conformance_lane_e_modular_split_contract_summary.json`

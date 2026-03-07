# M240 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-metadata-declaration-source-modeling/m240-a001-v1`
Status: Accepted
Scope: M240 lane-A qualifier/generic grammar normalization contract and architecture freeze for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-A qualifier/generic grammar normalization anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5030` defines canonical lane-A contract-freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m240/m240_a001_metadata_declaration_source_modeling_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m240_a001_metadata_declaration_source_modeling_contract.py`
  - `tests/tooling/test_check_m240_a001_metadata_declaration_source_modeling_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M240 lane-A A001 qualifier/generic grammar normalization fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A qualifier/generic grammar normalization fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A qualifier/generic grammar normalization metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m240-a001-metadata-declaration-source-modeling-contract`.
- `package.json` includes `test:tooling:m240-a001-metadata-declaration-source-modeling-contract`.
- `package.json` includes `check:objc3c:m240-a001-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m240_a001_metadata_declaration_source_modeling_contract.py`
- `python -m pytest tests/tooling/test_check_m240_a001_metadata_declaration_source_modeling_contract.py -q`
- `npm run check:objc3c:m240-a001-lane-a-readiness`

## Evidence Path

- `tmp/reports/m240/M240-A001/metadata_declaration_source_modeling_contract_summary.json`


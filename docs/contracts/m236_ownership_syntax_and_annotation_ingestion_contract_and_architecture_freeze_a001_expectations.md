# M236 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-ownership-syntax-and-annotation-ingestion/m236-a001-v1`
Status: Accepted
Scope: M236 lane-A qualifier/generic grammar normalization contract and architecture freeze for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-A qualifier/generic grammar normalization anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5856` defines canonical lane-A contract-freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m236/m236_a001_ownership_syntax_and_annotation_ingestion_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m236_a001_ownership_syntax_and_annotation_ingestion_contract.py`
  - `tests/tooling/test_check_m236_a001_ownership_syntax_and_annotation_ingestion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M236 lane-A A001 qualifier/generic grammar normalization fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A qualifier/generic grammar normalization fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A qualifier/generic grammar normalization metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m236-a001-ownership-syntax-and-annotation-ingestion-contract`.
- `package.json` includes `test:tooling:m236-a001-ownership-syntax-and-annotation-ingestion-contract`.
- `package.json` includes `check:objc3c:m236-a001-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m236_a001_ownership_syntax_and_annotation_ingestion_contract.py`
- `python -m pytest tests/tooling/test_check_m236_a001_ownership_syntax_and_annotation_ingestion_contract.py -q`
- `npm run check:objc3c:m236-a001-lane-a-readiness`

## Evidence Path

- `tmp/reports/m236/M236-A001/ownership_syntax_and_annotation_ingestion_contract_summary.json`


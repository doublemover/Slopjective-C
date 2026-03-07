# M239 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Expectations (A005)

Contract ID: `objc3c-frontend-normalization-hints-for-cfg-quality/m239-a005-v1`
Status: Accepted
Scope: M239 lane-A qualifier/generic grammar normalization edge-case and compatibility completion for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-A qualifier/generic grammar normalization anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6134` defines canonical lane-A contract-freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m239/m239_a005_frontend_normalization_hints_for_cfg_quality_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m239_a005_frontend_normalization_hints_for_cfg_quality_contract.py`
  - `tests/tooling/test_check_m239_a005_frontend_normalization_hints_for_cfg_quality_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M239 lane-A A005 qualifier/generic grammar normalization fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A qualifier/generic grammar normalization fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A qualifier/generic grammar normalization metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m239-a005-frontend-normalization-hints-for-cfg-quality-contract`.
- `package.json` includes `test:tooling:m239-a005-frontend-normalization-hints-for-cfg-quality-contract`.
- `package.json` includes `check:objc3c:m239-a005-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m239_a005_frontend_normalization_hints_for_cfg_quality_contract.py`
- `python -m pytest tests/tooling/test_check_m239_a005_frontend_normalization_hints_for_cfg_quality_contract.py -q`
- `npm run check:objc3c:m239-a005-lane-a-readiness`

## Evidence Path

- `tmp/reports/m239/M239-A005/frontend_normalization_hints_for_cfg_quality_contract_summary.json`






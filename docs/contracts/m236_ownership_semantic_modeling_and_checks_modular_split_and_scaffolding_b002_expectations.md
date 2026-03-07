# M236 Qualifier/Generic Semantic Inference Contract and Architecture Freeze Expectations (B002)

Contract ID: `objc3c-ownership-semantic-modeling-and-checks/m236-b002-v1`
Status: Accepted
Scope: M236 lane-B qualifier/generic semantic inference modular split and scaffolding for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-B qualifier/generic semantic inference anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5872` defines canonical lane-B contract-freeze scope.
- Dependencies: `M236-B001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m236/m236_b002_ownership_semantic_modeling_and_checks_modular_split_and_scaffolding_packet.md`
  - `scripts/check_m236_b002_ownership_semantic_modeling_and_checks_contract.py`
  - `tests/tooling/test_check_m236_b002_ownership_semantic_modeling_and_checks_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M236 lane-B B002 qualifier/generic semantic inference fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B qualifier/generic semantic inference fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B qualifier/generic semantic inference metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m236-b002-ownership-semantic-modeling-and-checks-modular-split-and-scaffolding-contract`.
- `package.json` includes `test:tooling:m236-b002-ownership-semantic-modeling-and-checks-modular-split-and-scaffolding-contract`.
- `package.json` includes `check:objc3c:m236-b002-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m236_b002_ownership_semantic_modeling_and_checks_contract.py`
- `python -m pytest tests/tooling/test_check_m236_b002_ownership_semantic_modeling_and_checks_contract.py -q`
- `npm run check:objc3c:m236-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m236/M236-B002/ownership_semantic_modeling_and_checks_modular_split_and_scaffolding_summary.json`





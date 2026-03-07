# M237 Qualifier/Generic Semantic Inference Contract and Architecture Freeze Expectations (B009)

Contract ID: `objc3c-block-semantic-capture-and-lifetime-rules/m237-b009-v1`
Status: Accepted
Scope: M237 lane-B qualifier/generic semantic inference conformance matrix implementation for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-B qualifier/generic semantic inference anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5980` defines canonical lane-B contract-freeze scope.
- Dependencies: `M237-B008`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m237/m237_b009_block_semantic_capture_and_lifetime_rules_conformance_matrix_implementation_packet.md`
  - `scripts/check_m237_b009_block_semantic_capture_and_lifetime_rules_contract.py`
  - `tests/tooling/test_check_m237_b009_block_semantic_capture_and_lifetime_rules_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M237 lane-B B009 qualifier/generic semantic inference fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B qualifier/generic semantic inference fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B qualifier/generic semantic inference metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m237-b009-block-semantic-capture-and-lifetime-rules-conformance-matrix-implementation-contract`.
- `package.json` includes `test:tooling:m237-b009-block-semantic-capture-and-lifetime-rules-conformance-matrix-implementation-contract`.
- `package.json` includes `check:objc3c:m237-b009-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m237_b009_block_semantic_capture_and_lifetime_rules_contract.py`
- `python -m pytest tests/tooling/test_check_m237_b009_block_semantic_capture_and_lifetime_rules_contract.py -q`
- `npm run check:objc3c:m237-b009-lane-b-readiness`

## Evidence Path

- `tmp/reports/m237/M237-B009/block_semantic_capture_and_lifetime_rules_conformance_matrix_implementation_summary.json`



















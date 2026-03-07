# M237 Qualifier/Generic Semantic Inference Contract and Architecture Freeze Expectations (B010)

Contract ID: `objc3c-block-semantic-capture-and-lifetime-rules/m237-b010-v1`
Status: Accepted
Scope: M237 lane-B qualifier/generic semantic inference conformance corpus expansion for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-B qualifier/generic semantic inference anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5981` defines canonical lane-B contract-freeze scope.
- Dependencies: `M237-B009`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m237/m237_b010_block_semantic_capture_and_lifetime_rules_conformance_corpus_expansion_packet.md`
  - `scripts/check_m237_b010_block_semantic_capture_and_lifetime_rules_contract.py`
  - `tests/tooling/test_check_m237_b010_block_semantic_capture_and_lifetime_rules_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M237 lane-B B010 qualifier/generic semantic inference fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B qualifier/generic semantic inference fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B qualifier/generic semantic inference metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m237-b010-block-semantic-capture-and-lifetime-rules-conformance-corpus-expansion-contract`.
- `package.json` includes `test:tooling:m237-b010-block-semantic-capture-and-lifetime-rules-conformance-corpus-expansion-contract`.
- `package.json` includes `check:objc3c:m237-b010-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m237_b010_block_semantic_capture_and_lifetime_rules_contract.py`
- `python -m pytest tests/tooling/test_check_m237_b010_block_semantic_capture_and_lifetime_rules_contract.py -q`
- `npm run check:objc3c:m237-b010-lane-b-readiness`

## Evidence Path

- `tmp/reports/m237/M237-B010/block_semantic_capture_and_lifetime_rules_conformance_corpus_expansion_summary.json`





















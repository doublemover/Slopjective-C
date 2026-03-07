# M238 Qualifier/Generic Semantic Inference Contract and Architecture Freeze Expectations (B016)

Contract ID: `objc3c-exception-semantic-legality-and-typing/m238-b016-v1`
Status: Accepted
Scope: M238 lane-B qualifier/generic semantic inference integration closeout and gate sign-off for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-B qualifier/generic semantic inference anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6075` defines canonical lane-B contract-freeze scope.
- Dependencies: `M238-B015`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m238/m238_b016_exception_semantic_legality_and_typing_integration_closeout_and_gate_sign_off_packet.md`
  - `scripts/check_m238_b016_exception_semantic_legality_and_typing_contract.py`
  - `tests/tooling/test_check_m238_b016_exception_semantic_legality_and_typing_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M238 lane-B B016 qualifier/generic semantic inference fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B qualifier/generic semantic inference fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B qualifier/generic semantic inference metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m238-b016-exception-semantic-legality-and-typing-integration-closeout-and-gate-sign-off-contract`.
- `package.json` includes `test:tooling:m238-b016-exception-semantic-legality-and-typing-integration-closeout-and-gate-sign-off-contract`.
- `package.json` includes `check:objc3c:m238-b016-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m238_b016_exception_semantic_legality_and_typing_contract.py`
- `python -m pytest tests/tooling/test_check_m238_b016_exception_semantic_legality_and_typing_contract.py -q`
- `npm run check:objc3c:m238-b016-lane-b-readiness`

## Evidence Path

- `tmp/reports/m238/M238-B016/exception_semantic_legality_and_typing_integration_closeout_and_gate_sign_off_summary.json`

































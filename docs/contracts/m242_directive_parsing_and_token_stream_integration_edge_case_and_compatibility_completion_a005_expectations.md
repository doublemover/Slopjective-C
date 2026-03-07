# M242 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Expectations (A005)

Contract ID: `objc3c-directive-parsing-and-token-stream-integration/m242-a005-v1`
Status: Accepted
Scope: M242 lane-A qualifier/generic grammar normalization edge-case and compatibility completion for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-A qualifier/generic grammar normalization anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6334` defines canonical lane-A contract-freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m242/m242_a005_directive_parsing_and_token_stream_integration_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m242_a005_directive_parsing_and_token_stream_integration_contract.py`
  - `tests/tooling/test_check_m242_a005_directive_parsing_and_token_stream_integration_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M242 lane-A A005 qualifier/generic grammar normalization fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A qualifier/generic grammar normalization fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A qualifier/generic grammar normalization metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m242-a005-directive-parsing-and-token-stream-integration-contract`.
- `package.json` includes `test:tooling:m242-a005-directive-parsing-and-token-stream-integration-contract`.
- `package.json` includes `check:objc3c:m242-a005-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m242_a005_directive_parsing_and_token_stream_integration_contract.py`
- `python -m pytest tests/tooling/test_check_m242_a005_directive_parsing_and_token_stream_integration_contract.py -q`
- `npm run check:objc3c:m242-a005-lane-a-readiness`

## Evidence Path

- `tmp/reports/m242/M242-A005/directive_parsing_and_token_stream_integration_contract_summary.json`






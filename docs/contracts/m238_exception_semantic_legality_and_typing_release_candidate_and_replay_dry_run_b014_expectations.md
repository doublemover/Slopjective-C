# M238 Qualifier/Generic Semantic Inference Contract and Architecture Freeze Expectations (B014)

Contract ID: `objc3c-exception-semantic-legality-and-typing/m238-b014-v1`
Status: Accepted
Scope: M238 lane-B qualifier/generic semantic inference release-candidate and replay dry-run for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-B qualifier/generic semantic inference anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6073` defines canonical lane-B contract-freeze scope.
- Dependencies: `M238-B013`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m238/m238_b014_exception_semantic_legality_and_typing_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m238_b014_exception_semantic_legality_and_typing_contract.py`
  - `tests/tooling/test_check_m238_b014_exception_semantic_legality_and_typing_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M238 lane-B B014 qualifier/generic semantic inference fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B qualifier/generic semantic inference fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B qualifier/generic semantic inference metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m238-b014-exception-semantic-legality-and-typing-release-candidate-and-replay-dry-run-contract`.
- `package.json` includes `test:tooling:m238-b014-exception-semantic-legality-and-typing-release-candidate-and-replay-dry-run-contract`.
- `package.json` includes `check:objc3c:m238-b014-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m238_b014_exception_semantic_legality_and_typing_contract.py`
- `python -m pytest tests/tooling/test_check_m238_b014_exception_semantic_legality_and_typing_contract.py -q`
- `npm run check:objc3c:m238-b014-lane-b-readiness`

## Evidence Path

- `tmp/reports/m238/M238-B014/exception_semantic_legality_and_typing_release_candidate_and_replay_dry_run_summary.json`





























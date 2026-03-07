# M238 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Expectations (A012)

Contract ID: `objc3c-exception-syntax-and-parse-recovery/m238-a012-v1`
Status: Accepted
Scope: M238 lane-A qualifier/generic grammar normalization integration closeout and gate sign-off for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-A qualifier/generic grammar normalization anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6059` defines canonical lane-A contract-freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m238/m238_a012_exception_syntax_and_parse_recovery_integration_closeout_and_gate_sign_off_packet.md`
  - `scripts/check_m238_a012_exception_syntax_and_parse_recovery_contract.py`
  - `tests/tooling/test_check_m238_a012_exception_syntax_and_parse_recovery_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M238 lane-A A012 qualifier/generic grammar normalization fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A qualifier/generic grammar normalization fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A qualifier/generic grammar normalization metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m238-a012-exception-syntax-and-parse-recovery-contract`.
- `package.json` includes `test:tooling:m238-a012-exception-syntax-and-parse-recovery-contract`.
- `package.json` includes `check:objc3c:m238-a012-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m238_a012_exception_syntax_and_parse_recovery_contract.py`
- `python -m pytest tests/tooling/test_check_m238_a012_exception_syntax_and_parse_recovery_contract.py -q`
- `npm run check:objc3c:m238-a012-lane-a-readiness`

## Evidence Path

- `tmp/reports/m238/M238-A012/exception_syntax_and_parse_recovery_contract_summary.json`













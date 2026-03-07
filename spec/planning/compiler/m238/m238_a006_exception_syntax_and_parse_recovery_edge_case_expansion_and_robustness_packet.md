# M238-A006 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Packet

Packet: `M238-A006`
Milestone: `M238`
Lane: `A`
Issue: `#5764`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-A qualifier/generic grammar normalization contract prerequisites for M238 so nullability, generics, and qualifier completeness governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m238_exception_syntax_and_parse_recovery_edge_case_expansion_and_robustness_a006_expectations.md`
- Checker:
  `scripts/check_m238_a006_exception_syntax_and_parse_recovery_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m238_a006_exception_syntax_and_parse_recovery_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m238-a006-exception-syntax-and-parse-recovery-contract`
  - `test:tooling:m238-a006-exception-syntax-and-parse-recovery-contract`
  - `check:objc3c:m238-a006-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m238_a006_exception_syntax_and_parse_recovery_contract.py`
- `python -m pytest tests/tooling/test_check_m238_a006_exception_syntax_and_parse_recovery_contract.py -q`
- `npm run check:objc3c:m238-a006-lane-a-readiness`

## Evidence Output

- `tmp/reports/m238/M238-A006/exception_syntax_and_parse_recovery_contract_summary.json`







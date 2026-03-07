# M238-B008 Qualifier/Generic Semantic Inference Contract and Architecture Freeze Packet

Packet: `M238-B008`
Milestone: `M238`
Lane: `B`
Issue: `#5781`
Freeze date: `2026-03-05`
Dependencies: `M238-B007`

## Purpose

Freeze lane-B qualifier/generic semantic inference contract prerequisites for M238 so nullability, generics, and qualifier completeness governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m238_exception_semantic_legality_and_typing_recovery_and_determinism_hardening_b008_expectations.md`
- Checker:
  `scripts/check_m238_b008_exception_semantic_legality_and_typing_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m238_b008_exception_semantic_legality_and_typing_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m238-b008-exception-semantic-legality-and-typing-recovery-and-determinism-hardening-contract`
  - `test:tooling:m238-b008-exception-semantic-legality-and-typing-recovery-and-determinism-hardening-contract`
  - `check:objc3c:m238-b008-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m238_b008_exception_semantic_legality_and_typing_contract.py`
- `python -m pytest tests/tooling/test_check_m238_b008_exception_semantic_legality_and_typing_contract.py -q`
- `npm run check:objc3c:m238-b008-lane-b-readiness`

## Evidence Output

- `tmp/reports/m238/M238-B008/exception_semantic_legality_and_typing_recovery_and_determinism_hardening_summary.json`

















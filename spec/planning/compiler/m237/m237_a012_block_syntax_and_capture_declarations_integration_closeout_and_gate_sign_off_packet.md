# M237-A012 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Packet

Packet: `M237-A012`
Milestone: `M237`
Lane: `A`
Issue: `#5764`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-A qualifier/generic grammar normalization contract prerequisites for M237 so nullability, generics, and qualifier completeness governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m237_block_syntax_and_capture_declarations_integration_closeout_and_gate_sign_off_a012_expectations.md`
- Checker:
  `scripts/check_m237_a012_block_syntax_and_capture_declarations_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m237_a012_block_syntax_and_capture_declarations_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m237-a012-block-syntax-and-capture-declarations-contract`
  - `test:tooling:m237-a012-block-syntax-and-capture-declarations-contract`
  - `check:objc3c:m237-a012-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m237_a012_block_syntax_and_capture_declarations_contract.py`
- `python -m pytest tests/tooling/test_check_m237_a012_block_syntax_and_capture_declarations_contract.py -q`
- `npm run check:objc3c:m237-a012-lane-a-readiness`

## Evidence Output

- `tmp/reports/m237/M237-A012/block_syntax_and_capture_declarations_contract_summary.json`













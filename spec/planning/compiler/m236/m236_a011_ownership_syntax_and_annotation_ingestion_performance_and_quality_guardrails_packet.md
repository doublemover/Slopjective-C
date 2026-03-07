# M236-A011 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Packet

Packet: `M236-A011`
Milestone: `M236`
Lane: `A`
Issue: `#5764`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-A qualifier/generic grammar normalization contract prerequisites for M236 so nullability, generics, and qualifier completeness governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m236_ownership_syntax_and_annotation_ingestion_performance_and_quality_guardrails_a011_expectations.md`
- Checker:
  `scripts/check_m236_a011_ownership_syntax_and_annotation_ingestion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m236_a011_ownership_syntax_and_annotation_ingestion_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m236-a011-ownership-syntax-and-annotation-ingestion-contract`
  - `test:tooling:m236-a011-ownership-syntax-and-annotation-ingestion-contract`
  - `check:objc3c:m236-a011-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m236_a011_ownership_syntax_and_annotation_ingestion_contract.py`
- `python -m pytest tests/tooling/test_check_m236_a011_ownership_syntax_and_annotation_ingestion_contract.py -q`
- `npm run check:objc3c:m236-a011-lane-a-readiness`

## Evidence Output

- `tmp/reports/m236/M236-A011/ownership_syntax_and_annotation_ingestion_contract_summary.json`












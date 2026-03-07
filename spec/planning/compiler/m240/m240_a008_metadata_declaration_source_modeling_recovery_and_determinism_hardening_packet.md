# M240-A008 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Packet

Packet: `M240-A008`
Milestone: `M240`
Lane: `A`
Issue: `#5764`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-A qualifier/generic grammar normalization contract prerequisites for M240 so nullability, generics, and qualifier completeness governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m240_metadata_declaration_source_modeling_recovery_and_determinism_hardening_a008_expectations.md`
- Checker:
  `scripts/check_m240_a008_metadata_declaration_source_modeling_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m240_a008_metadata_declaration_source_modeling_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m240-a008-metadata-declaration-source-modeling-contract`
  - `test:tooling:m240-a008-metadata-declaration-source-modeling-contract`
  - `check:objc3c:m240-a008-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m240_a008_metadata_declaration_source_modeling_contract.py`
- `python -m pytest tests/tooling/test_check_m240_a008_metadata_declaration_source_modeling_contract.py -q`
- `npm run check:objc3c:m240-a008-lane-a-readiness`

## Evidence Output

- `tmp/reports/m240/M240-A008/metadata_declaration_source_modeling_contract_summary.json`









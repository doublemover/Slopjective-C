# M239-A010 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Packet

Packet: `M239-A010`
Milestone: `M239`
Lane: `A`
Issue: `#5764`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-A qualifier/generic grammar normalization contract prerequisites for M239 so nullability, generics, and qualifier completeness governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m239_frontend_normalization_hints_for_cfg_quality_conformance_corpus_expansion_a010_expectations.md`
- Checker:
  `scripts/check_m239_a010_frontend_normalization_hints_for_cfg_quality_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m239_a010_frontend_normalization_hints_for_cfg_quality_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m239-a010-frontend-normalization-hints-for-cfg-quality-contract`
  - `test:tooling:m239-a010-frontend-normalization-hints-for-cfg-quality-contract`
  - `check:objc3c:m239-a010-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m239_a010_frontend_normalization_hints_for_cfg_quality_contract.py`
- `python -m pytest tests/tooling/test_check_m239_a010_frontend_normalization_hints_for_cfg_quality_contract.py -q`
- `npm run check:objc3c:m239-a010-lane-a-readiness`

## Evidence Output

- `tmp/reports/m239/M239-A010/frontend_normalization_hints_for_cfg_quality_contract_summary.json`











# M240-C010 Qualified Type Lowering and ABI Representation Conformance Corpus Expansion Packet

Packet: `M240-C010`
Milestone: `M240`
Lane: `C`
Issue: `#5811`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-C qualified type lowering and ABI representation contract
prerequisites for M240 so nullability, generics, and qualifier completeness
lowering/ABI boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m240_metadata_lowering_and_section_emission_conformance_corpus_expansion_c010_expectations.md`
- Checker:
  `scripts/check_m240_c010_metadata_lowering_and_section_emission_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m240_c010_metadata_lowering_and_section_emission_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m240-c010-metadata-lowering-and-section-emission-contract`
  - `test:tooling:m240-c010-metadata-lowering-and-section-emission-contract`
  - `check:objc3c:m240-c010-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m240_c010_metadata_lowering_and_section_emission_contract.py`
- `python -m pytest tests/tooling/test_check_m240_c010_metadata_lowering_and_section_emission_contract.py -q`
- `npm run check:objc3c:m240-c010-lane-c-readiness`

## Evidence Output

- `tmp/reports/m240/M240-C010/metadata_lowering_and_section_emission_contract_summary.json`












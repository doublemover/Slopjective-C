# M230-A008 Conformance Corpus Governance and Sharding Recovery and Determinism Hardening Packet

Packet: `M230-A008`
Milestone: `M230`
Lane: `A`
Issue: `#5402`
Freeze date: `2026-03-06`
Dependencies: `M230-A007`

## Purpose

Execute recovery and determinism hardening governance for lane-A conformance corpus governance and sharding while preserving deterministic dependency continuity from `M230-A007` and fail-closed readiness behavior.
This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m230_conformance_corpus_governance_and_sharding_recovery_and_determinism_hardening_a008_expectations.md`
- Checker:
  `scripts/check_m230_a008_conformance_corpus_governance_and_sharding_recovery_and_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m230_a008_conformance_corpus_governance_and_sharding_recovery_and_determinism_hardening_contract.py`
- Prior dependency packet:
  `spec/planning/compiler/m230/m230_a007_conformance_corpus_governance_and_sharding_diagnostics_hardening_packet.md`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m230-a008-conformance-corpus-governance-and-sharding-recovery-and-determinism-hardening-contract`
  - `test:tooling:m230-a008-conformance-corpus-governance-and-sharding-recovery-and-determinism-hardening-contract`
  - `check:objc3c:m230-a008-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m230_a008_conformance_corpus_governance_and_sharding_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m230_a008_conformance_corpus_governance_and_sharding_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m230-a008-lane-a-readiness`

## Evidence Output

- `tmp/reports/m230/M230-A008/conformance_corpus_governance_and_sharding_recovery_and_determinism_hardening_summary.json`







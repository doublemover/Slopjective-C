# M243-C010 Lowering/Runtime Diagnostics Surfacing Conformance Corpus Expansion Packet

Packet: `M243-C010`
Milestone: `M243`
Lane: `C`
Freeze date: `2026-03-03`
Dependencies: `M243-C009`

## Purpose

Freeze lane-C conformance corpus expansion continuity for lowering/runtime
diagnostics surfacing so C009 conformance matrix closure remains deterministic
and fail-closed across conformance-corpus consistency/readiness and
conformance-corpus-key continuity.

## Scope Anchors

- Contract:
  `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_c010_expectations.md`
- Checker:
  `scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py`
- Dependency anchors from `M243-C009`:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_c009_expectations.md`
  - `spec/planning/compiler/m243/m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_packet.md`
  - `scripts/check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract`
  - `test:tooling:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract`
  - `check:objc3c:m243-c010-lane-c-readiness`
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

- `python scripts/check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py`
- `python scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py`
- `python scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m243-c010-lane-c-readiness`

## Evidence Output

- `tmp/reports/m243/M243-C010/lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract_summary.json`


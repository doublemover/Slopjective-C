# M243 Lowering/Runtime Diagnostics Surfacing Conformance Corpus Expansion Expectations (C010)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion/m243-c010-v1`
Status: Accepted
Scope: lane-C lowering/runtime diagnostics surfacing conformance corpus expansion closure built on C009 conformance matrix implementation foundations.

## Objective

Expand lane-C diagnostics surfacing closure with deterministic conformance-corpus
consistency/readiness and conformance-corpus-key continuity so conformance
evidence cannot drift fail-open after C009 conformance matrix closure.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-C009`
- M243-C009 conformance matrix implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_c009_expectations.md`
  - `spec/planning/compiler/m243/m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_packet.md`
  - `scripts/check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py`
- Packet/checker/test assets for C010 remain mandatory:
  - `spec/planning/compiler/m243/m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_packet.md`
  - `scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py`

## Deterministic Invariants

1. C010 remains the canonical lane-C conformance corpus expansion governance
   layer for lowering/runtime diagnostics surfacing and stays explicitly
   dependency-bound to C009 conformance matrix closure.
2. Conformance corpus expansion closure remains fail-closed and requires:
   - conformance-corpus consistency
   - conformance-corpus readiness
   - conformance-corpus-key continuity
3. Architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Build/readiness wiring remains fail-closed and chains from C009:
   - `check:objc3c:m243-c009-lane-c-readiness`
   - `check:objc3c:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract`
   - `test:tooling:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract`
   - `check:objc3c:m243-c010-lane-c-readiness`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract`.
- `package.json` includes
  `test:tooling:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract`.
- `package.json` includes `check:objc3c:m243-c010-lane-c-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py`
- `python scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py`
- `python scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m243-c010-lane-c-readiness`

## Evidence Path

- `tmp/reports/m243/M243-C010/lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract_summary.json`


# M245-C010 Lowering/IR Portability Contracts Conformance Corpus Expansion Packet

Packet: `M245-C010`
Milestone: `M245`
Lane: `C`
Issue: `#6645`
Freeze date: `2026-03-04`
Dependencies: `M245-C009`

## Purpose

Freeze lane-C lowering/IR portability contracts conformance corpus expansion
continuity for M245 so predecessor continuity remains explicit, deterministic,
and fail-closed.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m245_lowering_ir_portability_contracts_conformance_corpus_expansion_c010_expectations.md`
- Checker:
  `scripts/check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py`
- Dependency anchors (`M245-C009`):
  - `docs/contracts/m245_lowering_ir_portability_contracts_conformance_matrix_implementation_c009_expectations.md`
  - `spec/planning/compiler/m245/m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_packet.md`
  - `scripts/check_m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_contract.py`
- Shared wiring handoff:
  - `native/objc3c/src/ARCHITECTURE.md` (shared-owner follow-up)
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` (shared-owner follow-up)
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md` (shared-owner follow-up)
  - `package.json` lane-C readiness chain (shared-owner follow-up)

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py`
- `python scripts/check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py -q`

## Evidence Output

- `tmp/reports/m245/M245-C010/lowering_ir_portability_contracts_conformance_corpus_expansion_summary.json`

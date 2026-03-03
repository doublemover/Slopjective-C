# M244-C010 Interop Lowering and ABI Conformance Conformance Corpus Expansion Packet

Packet: `M244-C010`
Milestone: `M244`
Lane: `C`
Issue: `#6559`
Dependencies: `M244-C009`

## Purpose

Execute lane-C interop lowering and ABI conformance conformance corpus expansion
governance on top of C009 conformance matrix implementation assets so downstream
expansion and cross-lane conformance integration remain deterministic and
fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_lowering_and_abi_conformance_conformance_corpus_expansion_c010_expectations.md`
- Checker:
  `scripts/check_m244_c010_interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_c010_interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-c010-interop-lowering-abi-conformance-conformance-corpus-expansion-contract`
  - `test:tooling:m244-c010-interop-lowering-abi-conformance-conformance-corpus-expansion-contract`
  - `check:objc3c:m244-c010-lane-c-readiness`

## Dependency Anchors (M244-C009)

- `docs/contracts/m244_interop_lowering_and_abi_conformance_conformance_matrix_implementation_c009_expectations.md`
- `spec/planning/compiler/m244/m244_c009_interop_lowering_and_abi_conformance_conformance_matrix_implementation_packet.md`
- `scripts/check_m244_c009_interop_lowering_and_abi_conformance_conformance_matrix_implementation_contract.py`
- `tests/tooling/test_check_m244_c009_interop_lowering_and_abi_conformance_conformance_matrix_implementation_contract.py`

## Gate Commands

- `python scripts/check_m244_c010_interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract.py`
- `python scripts/check_m244_c010_interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c010_interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m244-c010-lane-c-readiness`

## Evidence Output

- `tmp/reports/m244/M244-C010/interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract_summary.json`


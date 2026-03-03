# M227-C010 Typed Sema-to-Lowering Conformance Corpus Expansion Packet

Packet: `M227-C010`
Milestone: `M227`
Lane: `C`
Issue: `#5130`
Dependencies: `M227-C009`

## Scope

Implement lane-C typed sema-to-lowering conformance-corpus consistency and
readiness by wiring conformance corpus invariants through typed contract and
parse/lowering readiness surfaces with deterministic fail-closed alignment.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m227_typed_sema_to_lowering_conformance_corpus_expansion_c010_expectations.md`
- Checker:
  `scripts/check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py`
- Dependency anchors (`M227-C009`):
  - `docs/contracts/m227_typed_sema_to_lowering_conformance_matrix_implementation_c009_expectations.md`
  - `scripts/check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
  - `spec/planning/compiler/m227/m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_packet.md`
- Typed/pipeline anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-c010-typed-sema-to-lowering-conformance-corpus-expansion-contract`
  - `test:tooling:m227-c010-typed-sema-to-lowering-conformance-corpus-expansion-contract`
  - `check:objc3c:m227-c010-lane-c-readiness`

## Required Evidence

- `tmp/reports/m227/M227-C010/typed_sema_to_lowering_conformance_corpus_expansion_contract_summary.json`

## Gate Commands

- `python scripts/check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
- `python scripts/check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m227-c010-lane-c-readiness`

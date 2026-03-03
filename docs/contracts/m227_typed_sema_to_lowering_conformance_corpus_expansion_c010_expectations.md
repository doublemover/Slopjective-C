# M227 Typed Sema-to-Lowering Conformance Corpus Expansion Expectations (C010)

Contract ID: `objc3c-typed-sema-to-lowering-conformance-corpus-expansion/m227-c010-v1`
Status: Accepted
Scope: typed sema-to-lowering conformance corpus expansion on top of C009 conformance matrix implementation.

## Objective

Execute issue `#5130` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with conformance-corpus consistency and readiness
invariants, with deterministic fail-closed alignment checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C009`
- `M227-C009` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_conformance_matrix_implementation_c009_expectations.md`
  - `scripts/check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
  - `spec/planning/compiler/m227/m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries conformance corpus fields:
   - `typed_conformance_corpus_consistent`
   - `typed_conformance_corpus_ready`
   - `typed_conformance_corpus_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed conformance-corpus fields:
   - `typed_sema_conformance_corpus_consistent`
   - `typed_sema_conformance_corpus_ready`
   - `typed_sema_conformance_corpus_key`
3. Parse/lowering readiness fails closed when typed conformance-corpus alignment
   drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c010-typed-sema-to-lowering-conformance-corpus-expansion-contract`
  - `test:tooling:m227-c010-typed-sema-to-lowering-conformance-corpus-expansion-contract`
  - `check:objc3c:m227-c010-lane-c-readiness`

## Validation

- `python scripts/check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
- `python scripts/check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m227-c010-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C010/typed_sema_to_lowering_conformance_corpus_expansion_contract_summary.json`

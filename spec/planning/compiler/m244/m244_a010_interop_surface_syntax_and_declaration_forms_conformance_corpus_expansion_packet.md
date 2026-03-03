# M244-A010 Interop Surface Syntax and Declaration Forms Conformance Corpus Expansion Packet

Packet: `M244-A010`
Milestone: `M244`
Lane: `A`
Issue: `#6527`
Dependencies: `M244-A009`

## Purpose

Execute lane-A interop surface syntax/declaration-form conformance corpus expansion
governance on top of A009 conformance matrix implementation assets so
downstream expansion and cross-lane interop integration remain deterministic
and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_conformance_corpus_expansion_a010_expectations.md`
- Checker:
  `scripts/check_m244_a010_interop_surface_syntax_and_declaration_forms_conformance_corpus_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_a010_interop_surface_syntax_and_declaration_forms_conformance_corpus_expansion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-a010-interop-surface-syntax-declaration-forms-conformance-corpus-expansion-contract`
  - `test:tooling:m244-a010-interop-surface-syntax-declaration-forms-conformance-corpus-expansion-contract`
  - `check:objc3c:m244-a010-lane-a-readiness`

## Dependency Anchors (M244-A009)

- `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_conformance_matrix_implementation_a009_expectations.md`
- `spec/planning/compiler/m244/m244_a009_interop_surface_syntax_and_declaration_forms_conformance_matrix_implementation_packet.md`
- `scripts/check_m244_a009_interop_surface_syntax_and_declaration_forms_conformance_matrix_implementation_contract.py`
- `tests/tooling/test_check_m244_a009_interop_surface_syntax_and_declaration_forms_conformance_matrix_implementation_contract.py`

## Gate Commands

- `python scripts/check_m244_a010_interop_surface_syntax_and_declaration_forms_conformance_corpus_expansion_contract.py`
- `python scripts/check_m244_a010_interop_surface_syntax_and_declaration_forms_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_a010_interop_surface_syntax_and_declaration_forms_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m244-a010-lane-a-readiness`

## Evidence Output

- `tmp/reports/m244/M244-A010/interop_surface_syntax_and_declaration_forms_conformance_corpus_expansion_contract_summary.json`

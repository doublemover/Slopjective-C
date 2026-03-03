# M244-A002 Interop Surface Syntax and Declaration Forms Modular Split and Scaffolding Packet

Packet: `M244-A002`
Milestone: `M244`
Lane: `A`
Issue: `#6519`
Dependencies: `M244-A001`

## Purpose

Execute lane-A interop surface syntax/declaration-form modular split and
scaffolding governance on top of A001 freeze assets so downstream feature
implementation and bridge integration remain deterministic and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_a002_expectations.md`
- Checker:
  `scripts/check_m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-a002-interop-surface-syntax-declaration-forms-modular-split-scaffolding-contract`
  - `test:tooling:m244-a002-interop-surface-syntax-declaration-forms-modular-split-scaffolding-contract`
  - `check:objc3c:m244-a002-lane-a-readiness`

## Dependency Anchors (M244-A001)

- `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_contract_and_architecture_freeze_a001_expectations.md`
- `spec/planning/compiler/m244/m244_a001_interop_surface_syntax_and_declaration_forms_contract_and_architecture_freeze_packet.md`
- `scripts/check_m244_a001_interop_surface_syntax_and_declaration_forms_contract.py`
- `tests/tooling/test_check_m244_a001_interop_surface_syntax_and_declaration_forms_contract.py`

## Gate Commands

- `python scripts/check_m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract.py`
- `python scripts/check_m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m244-a002-lane-a-readiness`

## Evidence Output

- `tmp/reports/m244/M244-A002/interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract_summary.json`

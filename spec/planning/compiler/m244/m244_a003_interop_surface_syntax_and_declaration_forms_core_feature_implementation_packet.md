# M244-A003 Interop Surface Syntax and Declaration Forms Core Feature Implementation Packet

Packet: `M244-A003`
Milestone: `M244`
Lane: `A`
Issue: `#6520`
Dependencies: `M244-A002`

## Purpose

Execute lane-A interop surface syntax/declaration-form core-feature
implementation governance on top of A002 modular split/scaffolding assets so
downstream expansion and cross-lane interop integration remain deterministic
and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_core_feature_implementation_a003_expectations.md`
- Checker:
  `scripts/check_m244_a003_interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_a003_interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-a003-interop-surface-syntax-declaration-forms-core-feature-implementation-contract`
  - `test:tooling:m244-a003-interop-surface-syntax-declaration-forms-core-feature-implementation-contract`
  - `check:objc3c:m244-a003-lane-a-readiness`

## Dependency Anchors (M244-A002)

- `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_a002_expectations.md`
- `spec/planning/compiler/m244/m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_packet.md`
- `scripts/check_m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract.py`
- `tests/tooling/test_check_m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract.py`

## Gate Commands

- `python scripts/check_m244_a003_interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract.py`
- `python scripts/check_m244_a003_interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_a003_interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m244-a003-lane-a-readiness`

## Evidence Output

- `tmp/reports/m244/M244-A003/interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract_summary.json`

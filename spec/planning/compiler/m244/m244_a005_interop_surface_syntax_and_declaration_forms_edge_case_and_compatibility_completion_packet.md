# M244-A005 Interop Surface Syntax and Declaration Forms Edge-Case and Compatibility Completion Packet

Packet: `M244-A005`
Milestone: `M244`
Lane: `A`
Issue: `#6522`
Dependencies: `M244-A004`

## Purpose

Execute lane-A interop surface syntax/declaration-form edge-case and
compatibility completion governance on top of A004 core-feature expansion assets so
downstream expansion and cross-lane interop integration remain deterministic
and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_a005_expectations.md`
- Checker:
  `scripts/check_m244_a005_interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_a005_interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-a005-interop-surface-syntax-declaration-forms-edge-case-and-compatibility-completion-contract`
  - `test:tooling:m244-a005-interop-surface-syntax-declaration-forms-edge-case-and-compatibility-completion-contract`
  - `check:objc3c:m244-a005-lane-a-readiness`

## Dependency Anchors (M244-A004)

- `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_core_feature_expansion_a004_expectations.md`
- `spec/planning/compiler/m244/m244_a004_interop_surface_syntax_and_declaration_forms_core_feature_expansion_packet.md`
- `scripts/check_m244_a004_interop_surface_syntax_and_declaration_forms_core_feature_expansion_contract.py`
- `tests/tooling/test_check_m244_a004_interop_surface_syntax_and_declaration_forms_core_feature_expansion_contract.py`

## Gate Commands

- `python scripts/check_m244_a005_interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract.py`
- `python scripts/check_m244_a005_interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_a005_interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m244-a005-lane-a-readiness`

## Evidence Output

- `tmp/reports/m244/M244-A005/interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract_summary.json`

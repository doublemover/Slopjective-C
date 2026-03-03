# M244 Interop Surface Syntax and Declaration Forms Edge-Case and Compatibility Completion Expectations (A005)

Contract ID: `objc3c-interop-surface-syntax-and-declaration-forms-edge-case-and-compatibility-completion/m244-a005-v1`
Status: Accepted
Dependencies: `M244-A004`
Scope: lane-A interop surface syntax/declaration edge-case and compatibility completion governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-A edge-case and compatibility completion governance for interop surface syntax
and declaration forms on top of A004 core-feature expansion assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6522` defines canonical lane-A edge-case and compatibility completion scope.
- `M244-A004` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_core_feature_expansion_a004_expectations.md`
  - `spec/planning/compiler/m244/m244_a004_interop_surface_syntax_and_declaration_forms_core_feature_expansion_packet.md`
  - `scripts/check_m244_a004_interop_surface_syntax_and_declaration_forms_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m244_a004_interop_surface_syntax_and_declaration_forms_core_feature_expansion_contract.py`

## Deterministic Invariants

1. lane-A edge-case and compatibility completion dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-A004` before `M244-A005`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-a005-interop-surface-syntax-declaration-forms-edge-case-and-compatibility-completion-contract`.
- `package.json` includes
  `test:tooling:m244-a005-interop-surface-syntax-declaration-forms-edge-case-and-compatibility-completion-contract`.
- `package.json` includes `check:objc3c:m244-a005-lane-a-readiness`.
- lane-A readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-a004-lane-a-readiness`
  - `check:objc3c:m244-a005-lane-a-readiness`

## Validation

- `python scripts/check_m244_a005_interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract.py`
- `python scripts/check_m244_a005_interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_a005_interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m244-a005-lane-a-readiness`

## Evidence Path

- `tmp/reports/m244/M244-A005/interop_surface_syntax_and_declaration_forms_edge_case_and_compatibility_completion_contract_summary.json`

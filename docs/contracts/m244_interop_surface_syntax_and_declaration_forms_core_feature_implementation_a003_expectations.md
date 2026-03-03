# M244 Interop Surface Syntax and Declaration Forms Core Feature Implementation Expectations (A003)

Contract ID: `objc3c-interop-surface-syntax-and-declaration-forms-core-feature-implementation/m244-a003-v1`
Status: Accepted
Dependencies: `M244-A002`
Scope: lane-A interop surface syntax/declaration core-feature implementation governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-A core-feature implementation governance for interop surface syntax
and declaration forms on top of A002 modular split/scaffolding assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6520` defines canonical lane-A core-feature implementation scope.
- `M244-A002` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_a002_expectations.md`
  - `spec/planning/compiler/m244/m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_packet.md`
  - `scripts/check_m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m244_a002_interop_surface_syntax_and_declaration_forms_modular_split_scaffolding_contract.py`

## Deterministic Invariants

1. lane-A core-feature implementation dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-A002` before `M244-A003`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-a003-interop-surface-syntax-declaration-forms-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m244-a003-interop-surface-syntax-declaration-forms-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m244-a003-lane-a-readiness`.
- lane-A readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-a002-lane-a-readiness`
  - `check:objc3c:m244-a003-lane-a-readiness`

## Validation

- `python scripts/check_m244_a003_interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract.py`
- `python scripts/check_m244_a003_interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_a003_interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m244-a003-lane-a-readiness`

## Evidence Path

- `tmp/reports/m244/M244-A003/interop_surface_syntax_and_declaration_forms_core_feature_implementation_contract_summary.json`

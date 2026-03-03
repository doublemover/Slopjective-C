# M244 Interop Surface Syntax and Declaration Forms Conformance Matrix Implementation Expectations (A009)

Contract ID: `objc3c-interop-surface-syntax-and-declaration-forms-conformance-matrix-implementation/m244-a009-v1`
Status: Accepted
Dependencies: `M244-A008`
Scope: lane-A interop surface syntax/declaration conformance matrix implementation governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-A conformance matrix implementation governance for interop surface syntax
and declaration forms on top of A008 recovery and determinism hardening assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6526` defines canonical lane-A conformance matrix implementation scope.
- `M244-A008` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_surface_syntax_and_declaration_forms_recovery_and_determinism_hardening_a008_expectations.md`
  - `spec/planning/compiler/m244/m244_a008_interop_surface_syntax_and_declaration_forms_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m244_a008_interop_surface_syntax_and_declaration_forms_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m244_a008_interop_surface_syntax_and_declaration_forms_recovery_and_determinism_hardening_contract.py`

## Deterministic Invariants

1. lane-A conformance matrix implementation dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-A008` before `M244-A009`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-a009-interop-surface-syntax-declaration-forms-conformance-matrix-implementation-contract`.
- `package.json` includes
  `test:tooling:m244-a009-interop-surface-syntax-declaration-forms-conformance-matrix-implementation-contract`.
- `package.json` includes `check:objc3c:m244-a009-lane-a-readiness`.
- lane-A readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-a008-lane-a-readiness`
  - `check:objc3c:m244-a009-lane-a-readiness`

## Validation

- `python scripts/check_m244_a009_interop_surface_syntax_and_declaration_forms_conformance_matrix_implementation_contract.py`
- `python scripts/check_m244_a009_interop_surface_syntax_and_declaration_forms_conformance_matrix_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_a009_interop_surface_syntax_and_declaration_forms_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m244-a009-lane-a-readiness`

## Evidence Path

- `tmp/reports/m244/M244-A009/interop_surface_syntax_and_declaration_forms_conformance_matrix_implementation_contract_summary.json`

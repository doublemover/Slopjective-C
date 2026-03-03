# M244 Interop Surface Syntax and Declaration Forms Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-interop-surface-syntax-and-declaration-forms-freeze/m244-a001-v1`
Status: Accepted
Dependencies: none
Scope: lane-A interop surface syntax and declaration forms contract/architecture freeze for deterministic anchor and dependency-token continuity.

## Objective

Freeze lane-A interop surface syntax and declaration forms boundaries before
downstream interop lowering, runtime projection, and metadata expansion.
Deterministic anchors, dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Tokens

- Root dependency token: `none`
- Contract token: `M244-A001` is required across contract, packet, checker, and
  readiness command surfaces.

## Required Anchors

1. Contract/checker/test assets remain mandatory:
   - `spec/planning/compiler/m244/m244_a001_interop_surface_syntax_and_declaration_forms_contract_and_architecture_freeze_packet.md`
   - `scripts/check_m244_a001_interop_surface_syntax_and_declaration_forms_contract.py`
   - `tests/tooling/test_check_m244_a001_interop_surface_syntax_and_declaration_forms_contract.py`
2. Architecture and spec anchors remain explicit:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
3. Build/readiness wiring remains explicit in `package.json`:
   - `check:objc3c:m244-a001-interop-surface-syntax-declaration-forms-contract`
   - `test:tooling:m244-a001-interop-surface-syntax-declaration-forms-contract`
   - `check:objc3c:m244-a001-lane-a-readiness`

## Interop Spec Surface References

- `spec/FORMAL_GRAMMAR_AND_PRECEDENCE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Validation

- `python scripts/check_m244_a001_interop_surface_syntax_and_declaration_forms_contract.py`
- `python scripts/check_m244_a001_interop_surface_syntax_and_declaration_forms_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_a001_interop_surface_syntax_and_declaration_forms_contract.py -q`
- `npm run check:objc3c:m244-a001-lane-a-readiness`

## Evidence Path

- `tmp/reports/m244/M244-A001/interop_surface_syntax_and_declaration_forms_contract_summary.json`

# M234 Property and Ivar Syntax Surface Completion Conformance Matrix Implementation Expectations (A009)

Contract ID: `objc3c-property-and-ivar-syntax-surface-completion-conformance-matrix-implementation/m234-a009-v1`
Status: Accepted
Scope: M234 lane-A conformance matrix implementation continuity for property and ivar syntax surface completion dependency wiring.

## Objective

Fail closed unless lane-A conformance matrix implementation dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue: `#5679`
- Dependencies: `M234-A008`
- M234-A008 recovery and determinism hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m234_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_a008_expectations.md`
  - `spec/planning/compiler/m234/m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_contract.py`
- Packet/checker/test assets for A009 remain mandatory:
  - `spec/planning/compiler/m234/m234_a009_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_packet.md`
  - `scripts/check_m234_a009_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m234_a009_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M234 lane-A A009 property and ivar syntax surface completion conformance matrix implementation anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A property and ivar syntax surface completion conformance matrix implementation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A property and ivar syntax surface completion conformance matrix implementation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m234-a009-property-and-ivar-syntax-surface-completion-conformance-matrix-implementation-contract`.
- `package.json` includes
  `test:tooling:m234-a009-property-and-ivar-syntax-surface-completion-conformance-matrix-implementation-contract`.
- `package.json` includes `check:objc3c:m234-a009-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m234_a009_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m234_a009_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m234-a009-lane-a-readiness`

## Evidence Path

- `tmp/reports/m234/M234-A009/property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_summary.json`


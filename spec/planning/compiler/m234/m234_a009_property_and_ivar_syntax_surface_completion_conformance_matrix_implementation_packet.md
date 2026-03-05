# M234-A009 Property and Ivar Syntax Surface Completion Conformance Matrix Implementation Packet

Packet: `M234-A009`
Milestone: `M234`
Lane: `A`
Issue: `#5679`
Freeze date: `2026-03-04`
Dependencies: `M234-A008`

## Purpose

Freeze lane-A conformance matrix implementation prerequisites for M234 property and ivar syntax surface completion continuity so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m234_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_a009_expectations.md`
- Checker:
  `scripts/check_m234_a009_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m234_a009_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_contract.py`
- Dependency anchors from `M234-A008`:
  - `docs/contracts/m234_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_a008_expectations.md`
  - `spec/planning/compiler/m234/m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m234_a009_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m234_a009_property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m234-a009-lane-a-readiness`

## Evidence Output

- `tmp/reports/m234/M234-A009/property_and_ivar_syntax_surface_completion_conformance_matrix_implementation_summary.json`


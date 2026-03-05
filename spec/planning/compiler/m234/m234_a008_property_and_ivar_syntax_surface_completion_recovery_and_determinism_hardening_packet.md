# M234-A008 Property and Ivar Syntax Surface Completion Recovery and Determinism Hardening Packet

Packet: `M234-A008`
Milestone: `M234`
Lane: `A`
Issue: `#6619`
Freeze date: `2026-03-04`
Dependencies: `M234-A007`

## Purpose

Freeze lane-A recovery and determinism hardening prerequisites for M234 property and ivar syntax surface completion continuity so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m234_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_a008_expectations.md`
- Checker:
  `scripts/check_m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_contract.py`
- Dependency anchors from `M234-A007`:
  - `docs/contracts/m234_property_and_ivar_syntax_surface_completion_diagnostics_hardening_a007_expectations.md`
  - `spec/planning/compiler/m234/m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_packet.md`
  - `scripts/check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m234_a008_property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m234-a008-lane-a-readiness`

## Evidence Output

- `tmp/reports/m234/M234-A008/property_and_ivar_syntax_surface_completion_recovery_and_determinism_hardening_summary.json`


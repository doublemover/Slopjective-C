# M234-A006 Property and Ivar Syntax Surface Completion Edge-Case Expansion and Robustness Packet

Packet: `M234-A006`
Milestone: `M234`
Lane: `A`
Issue: `#6617`
Freeze date: `2026-03-04`
Dependencies: `M234-A005`

## Purpose

Freeze lane-A edge-case expansion and robustness prerequisites for M234 property and ivar syntax surface completion continuity so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m234_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_a006_expectations.md`
- Checker:
  `scripts/check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py`
- Dependency anchors from `M234-A005`:
  - `docs/contracts/m234_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_a005_expectations.md`
  - `spec/planning/compiler/m234/m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m234_a005_property_and_ivar_syntax_surface_completion_edge_case_and_compatibility_completion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m234-a006-lane-a-readiness`

## Evidence Output

- `tmp/reports/m234/M234-A006/property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_summary.json`


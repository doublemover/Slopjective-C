# M249-A005 Feature Packaging Surface and Compatibility Contracts Edge-Case and Compatibility Completion Packet

Packet: `M249-A005`
Milestone: `M249`
Lane: `A`
Freeze date: `2026-03-03`
Dependencies: `M249-A004`

## Purpose

Freeze lane-A edge-case and compatibility completion prerequisites for M249 feature packaging surface and compatibility contracts continuity so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_a005_expectations.md`
- Checker:
  `scripts/check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py`
- Readiness runner:
  `scripts/run_m249_a005_lane_a_readiness.py`
  - Chains through `check:objc3c:m249-a004-lane-a-readiness` before A005 checks.
- Dependency anchors from `M249-A004`:
  - `docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_a004_expectations.md`
  - `spec/planning/compiler/m249/m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_packet.md`
  - `scripts/check_m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py -q`
- `python scripts/run_m249_a005_lane_a_readiness.py`
- `npm run check:objc3c:m249-a005-lane-a-readiness`

## Evidence Output

- `tmp/reports/m249/M249-A005/feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_summary.json`

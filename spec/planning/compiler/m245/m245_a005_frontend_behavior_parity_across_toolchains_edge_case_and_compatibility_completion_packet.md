# M245-A005 Frontend Behavior Parity Across Toolchains Edge-Case and Compatibility Completion Packet

Packet: `M245-A005`
Milestone: `M245`
Lane: `A`
Issue: `#6616`
Freeze date: `2026-03-04`
Dependencies: `M245-A004`

## Purpose

Freeze lane-A edge-case and compatibility completion prerequisites for M245 frontend behavior parity across toolchains continuity so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m245_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_a005_expectations.md`
- Checker:
  `scripts/check_m245_a005_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m245_a005_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_contract.py`
- Dependency anchors from `M245-A004`:
  - `docs/contracts/m245_frontend_behavior_parity_across_toolchains_core_feature_expansion_a004_expectations.md`
  - `spec/planning/compiler/m245/m245_a004_frontend_behavior_parity_across_toolchains_core_feature_expansion_packet.md`
  - `scripts/check_m245_a004_frontend_behavior_parity_across_toolchains_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m245_a004_frontend_behavior_parity_across_toolchains_core_feature_expansion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m245_a005_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m245_a005_frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m245-a005-lane-a-readiness`

## Evidence Output

- `tmp/reports/m245/M245-A005/frontend_behavior_parity_across_toolchains_edge_case_and_compatibility_completion_summary.json`

# M246-B005 Semantic Invariants for Optimization Legality Edge-Case and Compatibility Completion Packet

Packet: `M246-B005`
Milestone: `M246`
Lane: `B`
Issue: `#5064`
Freeze date: `2026-03-04`
Dependencies: `M246-B004`

## Purpose

Freeze lane-B semantic invariants for optimization legality edge-case and compatibility completion prerequisites for M246 so optimizer pipeline integration and invariants governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Issue Anchor

- Issue: `#5064`

## Scope Anchors

- Contract:
  `docs/contracts/m246_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_b005_expectations.md`
- Checker:
  `scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py`
  (fail-closed with deterministic sorted failures and `--emit-json` support)
- Tooling tests:
  `tests/tooling/test_check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py`
- Local readiness runner:
  `scripts/run_m246_b005_lane_b_readiness.py`
- Dependency anchors from `M246-B004`:
  - `docs/contracts/m246_semantic_invariants_for_optimization_legality_core_feature_expansion_b004_expectations.md`
  - `spec/planning/compiler/m246/m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_packet.md`
  - `scripts/check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py`
  - `scripts/run_m246_b004_lane_b_readiness.py`

## Gate Commands

- `python scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py --emit-json --summary-out tmp/reports/m246/M246-B005/semantic_invariants_optimization_legality_edge_case_and_compatibility_completion_summary.json`
- `python -m pytest tests/tooling/test_check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py -q`
- `python scripts/run_m246_b005_lane_b_readiness.py`

## Evidence Output

- `tmp/reports/m246/M246-B005/semantic_invariants_optimization_legality_edge_case_and_compatibility_completion_summary.json`

# M246-B006 Semantic Invariants for Optimization Legality Edge-Case Expansion and Robustness Packet

Packet: `M246-B006`
Milestone: `M246`
Lane: `B`
Issue: `#5065`
Freeze date: `2026-03-04`
Dependencies: `M246-B005`
Theme: `edge-case expansion and robustness`

## Purpose

Freeze lane-B semantic invariants for optimization legality edge-case expansion and robustness prerequisites for M246 so optimizer pipeline integration and invariants governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m246_semantic_invariants_for_optimization_legality_edge_case_expansion_and_robustness_b006_expectations.md`
- Checker:
  `scripts/check_m246_b006_semantic_invariants_for_optimization_legality_edge_case_expansion_and_robustness_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m246_b006_semantic_invariants_for_optimization_legality_edge_case_expansion_and_robustness_contract.py`
- Local readiness runner:
  `scripts/run_m246_b006_lane_b_readiness.py`
- Dependency anchors from `M246-B005`:
  - `docs/contracts/m246_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_b005_expectations.md`
  - `spec/planning/compiler/m246/m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py`
  - `scripts/run_m246_b005_lane_b_readiness.py`

## Gate Commands

- `python scripts/check_m246_b006_semantic_invariants_for_optimization_legality_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m246_b006_semantic_invariants_for_optimization_legality_edge_case_expansion_and_robustness_contract.py -q`
- `python scripts/run_m246_b006_lane_b_readiness.py`

## Evidence Output

- `tmp/reports/m246/M246-B006/semantic_invariants_optimization_legality_edge_case_expansion_and_robustness_summary.json`


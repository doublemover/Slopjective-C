# M235-B017 Qualifier/Generic Semantic Inference Advanced Diagnostics Workpack (Shard 1) Packet

Packet: `M235-B017`
Milestone: `M235`
Lane: `B`
Freeze date: `2026-03-05`
Issue: `#5797`
Dependencies: `M235-B016`
Theme: `advanced diagnostics workpack (shard 1)`

## Purpose

Freeze lane-B semantic inference advanced diagnostics workpack (shard 1) prerequisites so `M235-B016` dependency continuity remains explicit, deterministic, and fail-closed before downstream advanced conformance workpack sequencing.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_b017_expectations.md`
- Checker:
  `scripts/check_m235_b017_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_b017_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_contract.py`
- Immediate predecessor issue: `#5796` (`M235-B016`)
- Dependency anchors from `M235-B016`:
  - `docs/contracts/m235_qualifier_and_generic_semantic_inference_advanced_edge_compatibility_workpack_shard_1_b016_expectations.md`
  - `spec/planning/compiler/m235/m235_b016_qualifier_and_generic_semantic_inference_advanced_edge_compatibility_workpack_shard_1_packet.md`
  - `scripts/check_m235_b016_qualifier_and_generic_semantic_inference_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m235_b016_qualifier_and_generic_semantic_inference_advanced_edge_compatibility_workpack_shard_1_contract.py`

## Gate Commands

- `python scripts/check_m235_b017_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_contract.py --summary-out tmp/reports/m235/M235-B017/local_check_summary.json`
- `python -m pytest tests/tooling/test_check_m235_b017_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_contract.py -q`

## Evidence Output

- `tmp/reports/m235/M235-B017/qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_contract_summary.json`

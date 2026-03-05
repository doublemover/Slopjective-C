# M235-B025 Qualifier/Generic Semantic Inference Advanced Integration Workpack (Shard 2) Packet

Packet: `M235-B025`
Milestone: `M235`
Lane: `B`
Freeze date: `2026-03-05`
Issue: `#5805`
Dependencies: `M235-B024`
Theme: `advanced integration workpack shard 2`

## Purpose

Execute lane-B qualifier/generic semantic inference advanced integration workpack (shard 2) governance on top of B024 advanced conformance assets so downstream advanced integration (shard 2) evidence remains deterministic and fail-closed with explicit predecessor continuity (`#5804`).
Code/spec anchor continuity and milestone optimization improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualifier_and_generic_semantic_inference_advanced_integration_workpack_shard_2_b025_expectations.md`
- Checker:
  `scripts/check_m235_b025_qualifier_and_generic_semantic_inference_advanced_integration_workpack_shard_2_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_b025_qualifier_and_generic_semantic_inference_advanced_integration_workpack_shard_2_contract.py`
- Dependency anchors from `M235-B024`:
  - `docs/contracts/m235_qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_2_b024_expectations.md`
  - `spec/planning/compiler/m235/m235_b024_qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_2_packet.md`
  - `scripts/check_m235_b024_qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_2_contract.py`
  - `tests/tooling/test_check_m235_b024_qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_2_contract.py`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `test:objc3c:perf-budget`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m235_b025_qualifier_and_generic_semantic_inference_advanced_integration_workpack_shard_2_contract.py --summary-out tmp/reports/m235/M235-B025/local_check_summary.json`
- `python -m pytest tests/tooling/test_check_m235_b025_qualifier_and_generic_semantic_inference_advanced_integration_workpack_shard_2_contract.py -q`

## Evidence Output

- `tmp/reports/m235/M235-B025/qualifier_and_generic_semantic_inference_advanced_integration_workpack_shard_2_contract_summary.json`







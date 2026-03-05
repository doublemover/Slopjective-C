# M235-B018 Qualifier/Generic Semantic Inference Advanced Conformance Workpack (Shard 1) Packet

Packet: `M235-B018`
Milestone: `M235`
Lane: `B`
Freeze date: `2026-03-05`
Issue: `#5798`
Dependencies: `M235-B017`
Theme: `advanced conformance workpack shard 1`

## Purpose

Execute lane-B qualifier/generic semantic inference advanced conformance workpack (shard 1) governance on top of B017 advanced diagnostics assets so downstream conformance evidence remains deterministic and fail-closed with explicit predecessor continuity (`#5797`).
Code/spec anchor continuity and milestone optimization improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_1_b018_expectations.md`
- Checker:
  `scripts/check_m235_b018_qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_b018_qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_1_contract.py`
- Dependency anchors from `M235-B017`:
  - `docs/contracts/m235_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_b017_expectations.md`
  - `spec/planning/compiler/m235/m235_b017_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_packet.md`
  - `scripts/check_m235_b017_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m235_b017_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_1_contract.py`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `test:objc3c:perf-budget`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m235_b018_qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_1_contract.py --summary-out tmp/reports/m235/M235-B018/local_check_summary.json`
- `python -m pytest tests/tooling/test_check_m235_b018_qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_1_contract.py -q`

## Evidence Output

- `tmp/reports/m235/M235-B018/qualifier_and_generic_semantic_inference_advanced_conformance_workpack_shard_1_contract_summary.json`

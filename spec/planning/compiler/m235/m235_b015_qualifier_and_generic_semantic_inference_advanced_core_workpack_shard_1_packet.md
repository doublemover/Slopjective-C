# M235-B015 Qualifier/Generic Semantic Inference Advanced Core Workpack (Shard 1) Packet

Packet: `M235-B015`
Milestone: `M235`
Lane: `B`
Freeze date: `2026-03-05`
Issue: `#5795`
Dependencies: `M235-B014`
Theme: `advanced core workpack (shard 1)`

## Purpose

Execute lane-B qualifier/generic semantic inference advanced core workpack (shard 1) governance on top of B014 release-candidate and replay dry-run assets so downstream advanced core evidence remains deterministic and fail-closed with explicit dependency continuity.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_b015_expectations.md`
- Checker:
  `scripts/check_m235_b015_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_b015_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_contract.py`
- Dependency anchors from `M235-B014`:
  - `docs/contracts/m235_qualifier_and_generic_semantic_inference_release_candidate_and_replay_dry_run_b014_expectations.md`
  - `spec/planning/compiler/m235/m235_b014_qualifier_and_generic_semantic_inference_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m235_b014_qualifier_and_generic_semantic_inference_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m235_b014_qualifier_and_generic_semantic_inference_release_candidate_and_replay_dry_run_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m235-b015-qualifier-and-generic-semantic-inference-advanced-core-workpack-shard-1-contract`
  - `test:tooling:m235-b015-qualifier-and-generic-semantic-inference-advanced-core-workpack-shard-1-contract`
  - `check:objc3c:m235-b015-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `test:objc3c:perf-budget`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m235_b015_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_contract.py`
- `python -m pytest tests/tooling/test_check_m235_b015_qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_contract.py -q`
- `npm run check:objc3c:m235-b015-lane-b-readiness`

## Evidence Output

- `tmp/reports/m235/M235-B015/qualifier_and_generic_semantic_inference_advanced_core_workpack_shard_1_summary.json`

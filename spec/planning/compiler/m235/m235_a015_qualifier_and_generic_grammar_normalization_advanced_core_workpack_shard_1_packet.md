# M235-A015 Qualifier/Generic Grammar Normalization Advanced Core Workpack (Shard 1) Packet

Packet: `M235-A015`
Milestone: `M235`
Lane: `A`
Freeze date: `2026-03-05`
Issue: `#5778`
Dependencies: `M235-A014`

## Purpose

Freeze lane-A qualifier/generic grammar normalization advanced core
workpack (shard 1) prerequisites so `M235-A014` dependency continuity stays
explicit, deterministic, and fail-closed, including code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_a015_expectations.md`
- Checker:
  `scripts/check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py`
- Readiness runner:
  `scripts/run_m235_a015_lane_a_readiness.py`
- Dependency anchors from `M235-A014`:
  - `docs/contracts/m235_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_a014_expectations.md`
  - `spec/planning/compiler/m235/m235_a014_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m235_a014_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m235_a014_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_contract.py`
  - `scripts/run_m235_a014_lane_a_readiness.py`
- Canonical readiness command names:
  - `check:objc3c:m235-a015-qualifier-and-generic-grammar-normalization-advanced-core-workpack-shard-1-contract`
  - `test:tooling:m235-a015-qualifier-and-generic-grammar-normalization-advanced-core-workpack-shard-1-contract`
  - `check:objc3c:m235-a015-lane-a-readiness`
  - `check:objc3c:m235-a014-lane-a-readiness`
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

- `python scripts/check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py`
- `python -m pytest tests/tooling/test_check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py -q`
- `python scripts/run_m235_a015_lane_a_readiness.py`
- `npm run check:objc3c:m235-a015-lane-a-readiness`

## Evidence Output

- `tmp/reports/m235/M235-A015/qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract_summary.json`

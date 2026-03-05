# M235 Qualifier/Generic Grammar Normalization Advanced Core Workpack (Shard 1) Expectations (A015)

Contract ID: `objc3c-qualifier-and-generic-grammar-normalization-advanced-core-workpack-shard-1/m235-a015-v1`
Status: Accepted
Dependencies: `M235-A014`
Scope: M235 lane-A qualifier/generic grammar normalization advanced core workpack (shard 1) dependency continuity and fail-closed readiness governance.

## Objective

Fail closed unless lane-A qualifier/generic grammar normalization advanced
core workpack (shard 1) dependency anchors remain explicit, deterministic, and
traceable across dependency surfaces, including code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5778` defines canonical lane-A advanced core workpack (shard 1) scope.
- `M235-A014` release-candidate/replay dry-run anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_a014_expectations.md`
  - `spec/planning/compiler/m235/m235_a014_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m235_a014_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m235_a014_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_contract.py`
  - `scripts/run_m235_a014_lane_a_readiness.py`
- Packet/checker/test assets for A015 remain mandatory:
  - `spec/planning/compiler/m235/m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_packet.md`
  - `scripts/check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py`
  - `scripts/run_m235_a015_lane_a_readiness.py`

## Deterministic Invariants

1. Lane-A advanced core workpack (shard 1) dependency references remain explicit
   and fail closed when dependency tokens drift.
2. advanced-core-workpack command sequencing and advanced-core-workpack-shard-1-key
   continuity remain deterministic and fail-closed across lane-A readiness wiring.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- Canonical command names for this contract:
  - `check:objc3c:m235-a015-qualifier-and-generic-grammar-normalization-advanced-core-workpack-shard-1-contract`
  - `test:tooling:m235-a015-qualifier-and-generic-grammar-normalization-advanced-core-workpack-shard-1-contract`
  - `check:objc3c:m235-a015-lane-a-readiness`
- Lane-A readiness chaining expected by this contract remains deterministic and fail-closed:
  - `python scripts/run_m235_a014_lane_a_readiness.py`
  - `python scripts/check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py`
  - `python -m pytest tests/tooling/test_check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py -q`

## Milestone Optimization Inputs

- `compile:objc3c`
- `test:objc3c:perf-budget`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Validation

- `python scripts/check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py`
- `python -m pytest tests/tooling/test_check_m235_a015_qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract.py -q`
- `python scripts/run_m235_a015_lane_a_readiness.py`
- `npm run check:objc3c:m235-a015-lane-a-readiness`

## Evidence Path

- `tmp/reports/m235/M235-A015/qualifier_and_generic_grammar_normalization_advanced_core_workpack_shard_1_contract_summary.json`

# M235-C015 Qualified Type Lowering and ABI Representation Advanced Core Workpack (shard 1) Packet

Packet: `M235-C015`
Milestone: `M235`
Lane: `C`
Issue: `#5825`
Freeze date: `2026-03-05`
Dependencies: `M235-C014`

## Purpose

Freeze lane-C qualified type lowering and ABI representation edge-case and
compatibility completion continuity for M235 so dependency wiring remains deterministic and
fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualified_type_lowering_and_abi_representation_advanced_core_workpack_shard_1_c015_expectations.md`
- Checker:
  `scripts/check_m235_c015_qualified_type_lowering_and_abi_representation_advanced_core_workpack_shard_1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_c015_qualified_type_lowering_and_abi_representation_advanced_core_workpack_shard_1_contract.py`
- Dependency anchors from `M235-C014`:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_release_candidate_and_replay_dry_run_c014_expectations.md`
  - `spec/planning/compiler/m235/m235_c014_qualified_type_lowering_and_abi_representation_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m235_c014_qualified_type_lowering_and_abi_representation_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m235_c014_qualified_type_lowering_and_abi_representation_release_candidate_and_replay_dry_run_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Readiness Chain

- `C014 readiness -> C015 checker -> C015 pytest`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `npm run check:objc3c:m235-c014-lane-c-readiness`
- `python scripts/check_m235_c015_qualified_type_lowering_and_abi_representation_advanced_core_workpack_shard_1_contract.py`
- `python -m pytest tests/tooling/test_check_m235_c015_qualified_type_lowering_and_abi_representation_advanced_core_workpack_shard_1_contract.py -q`
- `npm run check:objc3c:m235-c015-lane-c-readiness`

## Evidence Output

- `tmp/reports/m235/M235-C015/qualified_type_lowering_and_abi_representation_advanced_core_workpack_shard_1_contract_summary.json`












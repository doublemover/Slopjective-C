# M228-B020 Ownership-Aware Lowering Behavior Advanced Performance Workpack (Shard 1) Packet

Packet: `M228-B020`
Milestone: `M228`
Lane: `B`
Freeze date: `2026-03-06`
Issue: `#5214`
Dependencies: `M228-B019`

## Scope

Freeze lane-B ownership-aware lowering advanced performance workpack (shard 1)
governance so dependency, command, and evidence continuity remains deterministic
and fail-closed on top of B019 advanced integration governance.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m228_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_b020_expectations.md`
- Operator runbook:
  `docs/runbooks/m228_wave_execution_runbook.md`
- Checker:
  `scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py`
- Dependency anchors (`M228-B019`):
  - `docs/contracts/m228_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_b019_expectations.md`
  - `scripts/check_m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_contract.py`
  - `spec/planning/compiler/m228/m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_packet.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m228-b019-lane-b-readiness`
  - `check:objc3c:m228-b020-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1-contract`
  - `test:tooling:m228-b020-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1-contract`
  - `check:objc3c:m228-b020-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-smoke`
- `test:objc3c:lowering-replay-proof`

## Required Evidence

- `tmp/reports/m228/M228-B020/ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract_summary.json`

## Gate Commands

- `python scripts/check_m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_contract.py`
- `python scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m228-b020-lane-b-readiness`





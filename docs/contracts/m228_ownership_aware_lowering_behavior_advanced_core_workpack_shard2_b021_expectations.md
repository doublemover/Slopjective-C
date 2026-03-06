# M228 Ownership-Aware Lowering Behavior Advanced Core Workpack (Shard 2) Expectations (B021)

Contract ID: `objc3c-ownership-aware-lowering-behavior-advanced-core-workpack-shard2/m228-b021-v1`
Status: Accepted
Scope: lane-B ownership-aware lowering advanced core workpack (shard 2) closure on top of B020 advanced performance governance.

## Objective

Execute issue `#5215` by locking deterministic lane-B advanced core
workpack (shard 2) governance continuity over ownership-aware lowering
dependency anchors, operator command sequencing, and evidence paths so
readiness remains fail-closed when dependency or sequencing drift appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-B readiness.

## Dependency Scope

- Dependencies: `M228-B020`
- `M228-B020` remains a mandatory prerequisite:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_b020_expectations.md`
  - `scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py`
  - `spec/planning/compiler/m228/m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_packet.md`

## Deterministic Invariants

1. Operator runbook advanced core workpack (shard 2) continuity remains explicit in:
   - `docs/runbooks/m228_wave_execution_runbook.md`
2. Runbook anchor continuity remains deterministic for:
   - `objc3c-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1/m228-b020-v1`
   - `objc3c-ownership-aware-lowering-behavior-advanced-core-workpack-shard2/m228-b021-v1`
3. Lane-B advanced core workpack (shard 2) command sequencing remains fail-closed for:
   - `python scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py`
   - `python scripts/check_m228_b021_ownership_aware_lowering_behavior_advanced_core_workpack_shard2_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b021_ownership_aware_lowering_behavior_advanced_core_workpack_shard2_contract.py -q`
   - `npm run check:objc3c:m228-b021-lane-b-readiness`
4. Dependency continuity remains explicit and deterministic across
   `M228-B020` contract/checker/test/packet assets.
5. Readiness remains fail-closed when lane-B advanced core workpack
   (shard 2) command sequencing or evidence continuity drifts from
   `M228-B020` dependency continuity.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m228-b020-lane-b-readiness`
  - `check:objc3c:m228-b021-ownership-aware-lowering-behavior-advanced-core-workpack-shard2-contract`
  - `test:tooling:m228-b021-ownership-aware-lowering-behavior-advanced-core-workpack-shard2-contract`
  - `check:objc3c:m228-b021-lane-b-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M228 lane-B B021
  advanced core workpack (shard 2) anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B B021 fail-closed
  advanced core workpack (shard 2) governance text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B B021
  advanced core workpack (shard 2) metadata anchors.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-smoke`.
- `package.json` includes `test:objc3c:lowering-replay-proof`.

## Validation

- `python scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py`
- `python scripts/check_m228_b021_ownership_aware_lowering_behavior_advanced_core_workpack_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b021_ownership_aware_lowering_behavior_advanced_core_workpack_shard2_contract.py -q`
- `npm run check:objc3c:m228-b021-lane-b-readiness`

## Evidence Path

- `tmp/reports/m228/M228-B021/ownership_aware_lowering_behavior_advanced_core_workpack_shard2_contract_summary.json`







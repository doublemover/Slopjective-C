# M238 Qualified Type Lowering and ABI Representation Advanced Core Workpack (shard 2) Expectations (C021)

Contract ID: `objc3c-cleanup-lowering-and-unwind-control-flow-contract/m238-c021-v1`
Status: Accepted
Dependencies: none
Scope: M238 lane-C qualified type lowering and ABI representation advanced core workpack (shard 2) for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
anchors remain explicit, deterministic, and traceable across code/spec
anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6096` defines canonical lane-C advanced core workpack (shard 2) scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m238/m238_c021_cleanup_lowering_and_unwind_control_flow_advanced_core_workpack_shard_2_packet.md`
  - `scripts/check_m238_c021_cleanup_lowering_and_unwind_control_flow_contract.py`
  - `tests/tooling/test_check_m238_c021_cleanup_lowering_and_unwind_control_flow_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M238 lane-C C021
  qualified type lowering and ABI representation fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m238-c021-cleanup-lowering-and-unwind-control-flow-contract`.
- `package.json` includes
  `test:tooling:m238-c021-cleanup-lowering-and-unwind-control-flow-contract`.
- `package.json` includes `check:objc3c:m238-c021-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m238_c021_cleanup_lowering_and_unwind_control_flow_contract.py`
- `python -m pytest tests/tooling/test_check_m238_c021_cleanup_lowering_and_unwind_control_flow_contract.py -q`
- `npm run check:objc3c:m238-c021-lane-c-readiness`

## Evidence Path

- `tmp/reports/m238/M238-C021/cleanup_lowering_and_unwind_control_flow_contract_summary.json`























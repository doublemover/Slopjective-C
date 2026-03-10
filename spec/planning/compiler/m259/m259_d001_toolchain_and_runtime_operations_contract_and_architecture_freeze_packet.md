# M259-D001 Toolchain And Runtime Operations Contract And Architecture Freeze Packet

Packet: `M259-D001`
Milestone: `M259`
Lane: `D`
Issue: `#7214`
Dependencies: none

## Purpose

Freeze the supported toolchain/runtime operations boundary for the runnable
Objective-C 3 core so build, compile, smoke, replay, and package-output claims
remain explicit, deterministic, and fail closed before `M259-D002` expands the
workflow and packaging implementation surface.

## Scope Anchors

- Contract:
  `docs/contracts/m259_toolchain_and_runtime_operations_contract_and_architecture_freeze_d001_expectations.md`
- Checker:
  `scripts/check_m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze.py`
- Tooling tests:
  `tests/tooling/test_check_m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze.py`
- Readiness runner:
  `scripts/run_m259_d001_lane_d_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m259-d001-toolchain-and-runtime-operations-contract`
  - `test:tooling:m259-d001-toolchain-and-runtime-operations-contract`
  - `check:objc3c:m259-d001-lane-d-readiness`
- Architecture/spec anchors:
  - `docs/objc3c-native.md`
  - `docs/objc3c-native/src/50-artifacts.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Script anchors:
  - `scripts/check_objc3c_native_execution_smoke.ps1`
  - `scripts/check_objc3c_execution_replay_proof.ps1`

## Supported Host Baseline

- Windows x64
- `pwsh`
- `python`
- `node`/`npm`
- MSVC + CMake + Ninja
- LLVM `llc` and `llvm-readobj`

## Frozen Operations Boundary

- `npm run build:objc3c-native`
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 <input.objc3> --out-dir <out_dir> --emit-prefix module`
- `npm run test:objc3c:execution-smoke`
- `npm run test:objc3c:execution-replay-proof`

## Frozen Package And Evidence Outputs

- `artifacts/bin/objc3c-native.exe`
- `artifacts/lib/objc3_runtime.lib`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`

## Truthful Non-Goals

- no installer or system-wide deployment claim
- no cross-platform packaging claim
- no toolchain auto-provisioning claim

## Gate Commands

- `python scripts/check_m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze.py -q`
- `python scripts/run_m259_d001_lane_d_readiness.py`

## Evidence Output

- `tmp/reports/m259/M259-D001/toolchain_and_runtime_operations_contract_summary.json`

## Next Issue

- `M259-D002`

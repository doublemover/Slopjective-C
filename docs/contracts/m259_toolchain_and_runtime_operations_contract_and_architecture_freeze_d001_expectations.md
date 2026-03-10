# M259 Toolchain And Runtime Operations Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-runnable-toolchain-runtime-operations-freeze/m259-d001-v1`

Issue: `#7214`

## Objective

Freeze the supported build, compile, run, and packaging operations for the
runnable Objective-C 3 core on this machine and the primary supported Windows
host setup.

## Required implementation

1. Add the issue-local contract/checker/test/readiness assets:
   - `docs/contracts/m259_toolchain_and_runtime_operations_contract_and_architecture_freeze_d001_expectations.md`
   - `spec/planning/compiler/m259/m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze_packet.md`
   - `scripts/check_m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze.py`
   - `scripts/run_m259_d001_lane_d_readiness.py`
2. Publish explicit docs/spec/package/script anchors in:
   - `docs/objc3c-native.md`
   - `docs/objc3c-native/src/50-artifacts.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - `scripts/check_objc3c_execution_replay_proof.ps1`
   - `package.json`
3. Freeze the canonical runnable-core operations:
   - `npm run build:objc3c-native`
   - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 <input.objc3> --out-dir <out_dir> --emit-prefix module`
   - `npm run test:objc3c:execution-smoke`
   - `npm run test:objc3c:execution-replay-proof`
4. Freeze the canonical packaged outputs and evidence anchors:
   - `artifacts/bin/objc3c-native.exe`
   - `artifacts/lib/objc3_runtime.lib`
   - `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
   - `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
5. Add deterministic checker coverage over the supported host baseline:
   - Windows x64
   - `pwsh`
   - `python`
   - `node`/`npm`
   - MSVC + CMake + Ninja
   - LLVM `llc` and `llvm-readobj`
6. Keep explicit truthful boundaries:
   - no installer or system-wide deployment claim lands here
   - no cross-platform packaging claim lands here
   - no toolchain auto-provisioning claim lands here
   - D001 freezes the operations boundary only; `M259-D002` implements the workflow and packaging surface
7. The contract must explicitly hand off to `M259-D002`.

## Canonical models

- Operations model:
  `runnable-core-build-compile-smoke-replay-operations-boundary`
- Evidence model:
  `operations-freeze-docs-package-and-script-anchors-for-runnable-core`
- Failure model:
  `fail-closed-on-unsupported-packaging-or-runtime-operations-claim-drift`

## Evidence

- `tmp/reports/m259/M259-D001/toolchain_and_runtime_operations_contract_summary.json`

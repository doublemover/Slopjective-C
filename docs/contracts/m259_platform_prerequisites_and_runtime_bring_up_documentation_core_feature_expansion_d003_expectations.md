# M259 Platform Prerequisites And Runtime Bring-Up Documentation Core Feature Expansion Expectations (D003)

Contract ID: `objc3c-runnable-platform-prerequisites-runtime-bringup/m259-d003-v1`

Issue: `#7216`

## Objective

Publish exact, truthful Windows bring-up documentation for the runnable Objective-C 3 slice so developers know which tools must exist, which environment overrides are supported, and which commands can be run from the repo root versus the staged package root.

## Required implementation

1. Add the issue-local contract/checker/test/readiness assets:
   - `docs/contracts/m259_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion_d003_expectations.md`
   - `spec/planning/compiler/m259/m259_d003_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion_packet.md`
   - `scripts/check_m259_d003_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion.py`
   - `tests/tooling/test_check_m259_d003_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion.py`
   - `scripts/run_m259_d003_lane_d_readiness.py`
2. Publish explicit docs/spec/package/script anchors in:
   - `docs/objc3c-native.md`
   - `docs/objc3c-native/src/50-artifacts.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - `scripts/check_objc3c_execution_replay_proof.ps1`
   - `package.json`
3. Document the supported Windows host prerequisite surface truthfully:
   - Windows x64
   - `pwsh`
   - `python`
   - `node`/`npm`
   - LLVM `clang`, `clang++`, `llc`, `llvm-readobj`, `llvm-lib`
   - MSVC/Windows SDK linker tools reachable from `clang`
4. Document the supported environment override surface truthfully:
   - `OBJC3C_NATIVE_EXECUTABLE`
   - `OBJC3C_NATIVE_EXECUTION_CLANG_PATH`
   - `OBJC3C_NATIVE_EXECUTION_LLC_PATH`
   - `OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH`
   - `OBJC3C_NATIVE_EXECUTION_RUN_ID`
5. Document the repo-root/package-root command surface truthfully:
   - `npm run build:objc3c-native`
   - `npm run package:objc3c-native:runnable-toolchain`
   - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 <input.objc3> --out-dir <out_dir> --emit-prefix module`
   - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1`
   - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1`
6. Keep explicit truthful boundaries:
   - package-root execution still assumes the repo-relative staged layout preserved by `M259-D002`
   - no system install claim lands here
   - no toolchain auto-provisioning claim lands here
   - release-gate closure remains deferred to `M259-E001`
7. Extend evidence coverage so documentation drift fails closed without requiring another full packaged runtime proof.
8. The contract must explicitly hand off to `M259-E001`.

## Canonical models

- Bring-up model:
  `supported-windows-host-prereqs-and-package-root-runtime-bringup`
- Evidence model:
  `docs-and-script-anchors-for-prereq-and-runtime-bringup-truthfulness`
- Failure model:
  `fail-closed-on-prerequisite-or-runtime-bringup-claim-drift`

## Evidence

- `tmp/reports/m259/M259-D003/platform_prerequisites_and_runtime_bring_up_documentation_summary.json`

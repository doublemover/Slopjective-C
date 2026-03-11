# M259 Build/install/run workflow and binary packaging Expectations (D002)

Contract ID: `objc3c-runnable-build-install-run-package/m259-d002-v1`
Issue: `#7215`
Dependencies: `M259-D001`

## Required outcome

`M259-D002` must implement a real staged runnable toolchain package rooted under
`tmp/` so the current supported Windows x64 Objective-C 3 runnable slice can be
built, packaged, and exercised without ad hoc local path surgery.

The implementation must:

- keep the staged package layout repo-relative so the copied `scripts/`,
  `artifacts/`, `tests/tooling/fixtures/native/execution/`, and
  `tests/tooling/runtime/objc3_msgsend_i32_shim.c` surfaces work unchanged from
  the package root;
- publish a package manifest at
  `artifacts/package/objc3c-runnable-toolchain-package.json` describing the
  staged bundle, command surfaces, canonical fixture, runtime library,
  `artifacts/bin/objc3c-frontend-c-api-runner.exe`, and copied frontend
  readiness artifacts;
- expose the supported package step through
  `npm run package:objc3c-native:runnable-toolchain`;
- prove the packaged happy path with the copied commands:
  - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3 --out-dir tmp/package-run --emit-prefix module`
  - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1`
  - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1`
- keep the truthful boundary explicit:
  - staged local package root only
  - no system install claim
  - no cross-platform packaging claim
  - no toolchain auto-provisioning claim

## Validation

The issue must ship:

- `scripts/package_objc3c_runnable_toolchain.ps1`
- `scripts/check_m259_d002_build_install_run_workflow_and_binary_packaging_core_feature_implementation.py`
- `tests/tooling/test_check_m259_d002_build_install_run_workflow_and_binary_packaging_core_feature_implementation.py`
- `scripts/run_m259_d002_lane_d_readiness.py`
- `tmp/reports/m259/M259-D002/build_install_run_workflow_and_binary_packaging_summary.json`

The contract must explicitly hand off to `M259-D003`.

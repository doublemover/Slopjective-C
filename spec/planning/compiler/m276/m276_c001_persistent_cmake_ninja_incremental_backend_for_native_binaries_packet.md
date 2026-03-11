# M276-C001 Persistent CMake/Ninja Incremental Backend For Native Binaries Packet

Packet: `M276-C001`

Issue: `#7387`

## Objective

Implement the persistent native binary backend under `tmp/` while preserving the
current wrapper entrypoint and canonical published artifact paths.

## Dependencies

- `M276-A001`
- `M276-A002`

## Contract

- contract id
  `objc3c-persistent-cmake-ninja-native-build-backend/m276-c001-v1`
- backend model
  `wrapper-owned-entrypoint-with-persistent-cmake-ninja-native-binary-backend`
- publication model
  `cmake-build-tree-under-tmp-with-stable-final-artifacts-under-artifacts`
- incremental proof model
  `warm-reuse-plus-single-source-timestamp-bump-rebuild-scope-proof`

## Required anchors

- `docs/contracts/m276_persistent_cmake_ninja_incremental_backend_for_native_binaries_c001_expectations.md`
- `scripts/check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py`
- `tests/tooling/test_check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py`
- `scripts/run_m276_c001_lane_c_readiness.py`
- `check:objc3c:m276-c001-persistent-cmake-ninja-incremental-backend`
- `check:objc3c:m276-c001-lane-c-readiness`

## Implemented truths

- `scripts/build_objc3c_native.ps1` remains the public entrypoint
- native binary compilation now routes through CMake/Ninja under
  `tmp/build-objc3c-native`
- wrapper-owned LLVM/include/libclang discovery now threads into the CMake
  configure step
- canonical published artifacts remain:
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
  - `artifacts/lib/objc3_runtime.lib`
- `compile_commands.json` now lives at
  `tmp/build-objc3c-native/compile_commands.json`
- frontend packet generation still remains on the wrapper path after the native
  binaries are built

## Required proof model

- cold configure/build proof in a scratch build tree under `tmp/`
- warm wrapper proof showing persistent build-tree reuse
- one-file timestamp-bump incremental rebuild proof with reduced compile scope
- canonical publication proof for the three native artifacts
- compile database proof at `tmp/build-objc3c-native/compile_commands.json`
- fingerprint proof at `tmp/build-objc3c-native/native_build_backend_fingerprint.json`

## Handoff

`M276-C002` is the explicit next handoff after this implementation closes.

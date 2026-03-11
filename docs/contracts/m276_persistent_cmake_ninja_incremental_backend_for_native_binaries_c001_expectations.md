# M276 Persistent CMake/Ninja Incremental Backend For Native Binaries Expectations (C001)

Contract ID: `objc3c-persistent-cmake-ninja-native-build-backend/m276-c001-v1`

## Objective

Replace the current translation-unit-at-a-time native binary rebuild path with a
persistent CMake/Ninja backend for the native binaries while preserving the
existing wrapper entrypoint and canonical published artifact paths.

## Required implementation

1. Keep `scripts/build_objc3c_native.ps1` as the public entrypoint for
   `npm run build:objc3c-native`.
2. Change native binary compilation so the wrapper configures and reuses a
   persistent CMake/Ninja build tree under `tmp/build-objc3c-native`.
3. Route native binary builds for all of the following through CMake/Ninja:
   - `objc3c-native`
   - `objc3c-frontend-c-api-runner`
   - `objc3_runtime.lib`
4. Preserve canonical final published artifact locations:
   - `artifacts/bin/objc3c-native.exe`
   - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
   - `artifacts/lib/objc3_runtime.lib`
5. Preserve wrapper-owned toolchain discovery and thread the resolved LLVM input
   paths into CMake configuration:
   - `LLVM_ROOT`
   - LLVM include directory
   - `libclang` import library
6. Emit `compile_commands.json` at the frozen path:
   - `tmp/build-objc3c-native/compile_commands.json`
7. Update `native/objc3c/CMakeLists.txt` so the target graph now owns the
   canonical include/warning/define/publication behavior for the native binary
   backend.
8. Keep the current frontend packet generation on the wrapper path after the
   native binaries are built.
9. Add this packet, a deterministic checker, tooling tests, and a direct lane-C
   readiness runner:
   - `scripts/check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py`
   - `tests/tooling/test_check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py`
   - `scripts/run_m276_c001_lane_c_readiness.py`
10. Add `M276-C001` anchor text to:
   - `docs/objc3c-native/src/50-artifacts.md`
   - `docs/objc3c-native.md`
   - `README.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `native/objc3c/CMakeLists.txt`
   - `scripts/build_objc3c_native.ps1`
   - `package.json`
11. `package.json` must wire:
   - `check:objc3c:m276-c001-persistent-cmake-ninja-incremental-backend`
   - `test:tooling:m276-c001-persistent-cmake-ninja-incremental-backend`
   - `check:objc3c:m276-c001-lane-c-readiness`
12. The contract must explicitly hand off to `M276-C002`.

## Required proof model

- Cold configure/build proof for the new backend in a scratch build tree under
  `tmp/`
- Warm wrapper invocation proof showing persistent build-tree reuse
- Single-source timestamp-bump incremental rebuild proof showing narrowed
  compilation scope instead of a full translation-unit-at-a-time rebuild
- Canonical publication proof for `artifacts/bin` and `artifacts/lib`
- `compile_commands.json` publication proof at
  `tmp/build-objc3c-native/compile_commands.json`
- fingerprint publication proof at
  `tmp/build-objc3c-native/native_build_backend_fingerprint.json`
- dynamic evidence must record the cold-proof build tree, cold-proof artifacts,
  warm wrapper logs, and the incremental verbose rebuild log under `tmp/`

## Canonical models

- Backend model:
  `wrapper-owned-entrypoint-with-persistent-cmake-ninja-native-binary-backend`
- Publication model:
  `cmake-build-tree-under-tmp-with-stable-final-artifacts-under-artifacts`
- Incremental proof model:
  `warm-reuse-plus-single-source-timestamp-bump-rebuild-scope-proof`

## Non-goals

- No packet-family decomposition yet.
- No readiness-runner migration yet.
- No command-surface split into binary/contracts/full modes yet.

## Evidence

- `tmp/reports/m276/M276-C001/persistent_cmake_ninja_incremental_backend_summary.json`
- `tmp/reports/m276/M276-C001/incremental_rebuild_verbose.log`
- `tmp/reports/m276/M276-C001/cold_configure.log`
- `tmp/reports/m276/M276-C001/cold_build.log`
- `tmp/reports/m276/M276-C001/wrapper_first.log`
- `tmp/reports/m276/M276-C001/wrapper_second.log`

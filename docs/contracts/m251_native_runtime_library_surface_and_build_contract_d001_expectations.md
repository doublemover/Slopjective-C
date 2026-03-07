# M251 Native Runtime Library Surface and Build Contract Expectations (D001)

Contract ID: `objc3c-runtime-support-library-surface-build-contract/m251-d001-v1`
Status: Accepted
Scope: M251 lane-D native runtime-library surface and build-integration contract freeze for the in-tree Objective-C 3 runtime foundation.

## Objective

Fail closed unless the canonical in-tree runtime-library target, exported
entrypoints, ownership boundaries, and deferred driver-link mode remain
explicit, deterministic, and published through manifest/IR surfaces before the
library skeleton lands.

## Dependency Scope

- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m251/m251_d001_native_runtime_library_surface_and_build_contract_packet.md`
  - `scripts/check_m251_d001_native_runtime_library_surface_and_build_contract.py`
  - `tests/tooling/test_check_m251_d001_native_runtime_library_surface_and_build_contract.py`

## Required Invariants

1. `Objc3RuntimeSupportLibraryContractSummary` remains the canonical lane-D
   freeze packet for the native runtime-library surface.
2. Manifest JSON and emitted LLVM IR publish the same contract through
   `runtime_support_library_contract_id` and `!objc3.objc_runtime_support_library`.
3. The in-tree runtime-library surface remains frozen as:
   - target `objc3_runtime`
   - source root `native/objc3c/src/runtime`
   - public header `native/objc3c/src/runtime/objc3_runtime.h`
   - library kind `static`
   - archive basename `objc3_runtime`
4. The exported entrypoint family remains frozen as:
   - `objc3_runtime_register_image`
   - `objc3_runtime_lookup_selector`
   - `objc3_runtime_dispatch_i32`
   - `objc3_runtime_reset_for_testing`
5. Ownership remains frozen as compiler-owned source/IR/object metadata
   emission and runtime-owned registration/lookup/dispatch state.
6. Driver link mode remains frozen as `not-linked-until-m251-d003`.
7. `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains explicit test-only
   evidence and not the canonical runtime library.

## Non-Goals and Fail-Closed Rules

- `M251-D001` does not land the actual `objc3_runtime` library skeleton.
- `M251-D001` does not wire the native driver or smoke paths to link that
  library yet.
- `M251-D001` does not replace the deterministic `objc3_msgsend_i32` shim with
  live runtime lookup/dispatch.
- The freeze packet must therefore remain fail-closed until `M251-D002` and
  `M251-D003` land the real runtime library and link wiring.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m251-d001-native-runtime-library-surface-and-build-contract`.
- `package.json` includes `test:tooling:m251-d001-native-runtime-library-surface-and-build-contract`.
- `package.json` includes `check:objc3c:m251-d001-lane-d-readiness`.

## Validation

- `python scripts/check_m251_d001_native_runtime_library_surface_and_build_contract.py`
- `python -m pytest tests/tooling/test_check_m251_d001_native_runtime_library_surface_and_build_contract.py -q`
- `npm run check:objc3c:m251-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m251/M251-D001/runtime_support_library_contract_summary.json`

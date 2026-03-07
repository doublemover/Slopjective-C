# M251-D001 Native Runtime Library Surface and Build Contract Packet

Packet: `M251-D001`
Milestone: `M251`
Lane: `D`
Issue: `#7065`

## Objective

Freeze the initial in-tree `objc3_runtime` support-library surface so the next
runtime-library implementation issues must preserve one canonical target,
entrypoint family, ownership boundary, and deferred driver-link contract.

## Dependencies

- none

## Required implementation

1. Add a canonical `Objc3RuntimeSupportLibraryContractSummary`.
2. Project the runtime-library contract into manifest JSON and
   `Objc3IRFrontendMetadata`.
3. Publish the same contract into emitted LLVM IR as
   `!objc3.objc_runtime_support_library`.
4. Reserve the canonical runtime-library surface in the native CMake graph as:
   - target `objc3_runtime`
   - source root `native/objc3c/src/runtime`
   - public header `native/objc3c/src/runtime/objc3_runtime.h`
   - library kind `static`
   - archive basename `objc3_runtime`
5. Freeze the exported entrypoint family as:
   - `objc3_runtime_register_image`
   - `objc3_runtime_lookup_selector`
   - `objc3_runtime_dispatch_i32`
   - `objc3_runtime_reset_for_testing`
6. Freeze the ownership boundary:
   - compiler owns emitted metadata records and payload publication
   - runtime owns registration, selector lookup, and dispatch state
7. Freeze the driver-link mode as `not-linked-until-m251-d003`.
8. Keep `tests/tooling/runtime/objc3_msgsend_i32_shim.c` explicitly test-only.
9. Add deterministic checker and tooling tests that prove the manifest/IR
   contract and CMake reservations stay in sync.

## Determinism and fail-closed rules

- `M251-D001` freezes the runtime-library surface only; it does not land the
  actual library skeleton or link wiring.
- The packet therefore treats `native_runtime_library_present=false` and
  `driver_link_wiring_pending=true` as intentional contract values.
- The runtime shim remains evidence only and must not be reframed as the native
  runtime library.

## Validation plan

The checker runs one deterministic dynamic probe:

1. Native compile probe on `tests/tooling/fixtures/native/hello.objc3` proving
   the emitted manifest and LLVM IR publish the same runtime-library surface
   contract.

## Evidence

- `tmp/reports/m251/M251-D001/runtime_support_library_contract_summary.json`

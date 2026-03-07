# M251-D003 Driver, CMake, and Link Wiring for Runtime Library Consumption Packet

Packet: `M251-D003`
Milestone: `M251`
Lane: `D`
Issue: `#7067`
Contract ID: `objc3c-runtime-support-library-link-wiring/m251-d003-v1`

## Objective

Wire emitted-object execution consumers to the real native runtime archive in a
deterministic, fail-closed way across local smoke and checker paths.

## Required Outcomes

- publish `runtime_support_library_link_wiring_contract_id`
- emit `!objc3.objc_runtime_support_library_link_wiring`
- keep canonical link mode explicit as
  `emitted-object-links-against-objc3_runtime-lib`
- link runtime-requiring emitted objects against `artifacts/lib/objc3_runtime.lib`
- export `objc3_msgsend_i32` as a compatibility bridge to
  `objc3_runtime_dispatch_i32`
- keep negative unresolved-symbol fixtures fail closed by omitting runtime
  linkage where appropriate

## Code Anchors

- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/CMakeLists.txt`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `scripts/check_objc3c_native_execution_smoke.ps1`
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c`

## Evidence

- `tmp/reports/m251/M251-D003/runtime_support_library_link_wiring_summary.json`
- smoke summary under `tmp/artifacts/objc3c-native/execution-smoke/<run-id>/summary.json`

## Acceptance Notes

- D001 and D002 remain frozen historical packets; D003 adds a new packet rather
  than rewriting them.
- The shim remains in-repo as explicit test-only evidence, but the canonical
  positive execution path consumes the real runtime archive.

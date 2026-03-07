# M251-D002 Native Runtime Library Skeleton and Exported Entrypoints Packet

Packet: `M251-D002`
Milestone: `M251`
Lane: `D`
Issue: `#7066`

## Objective

Instantiate the frozen `M251-D001` runtime-library surface as a real in-tree
native library artifact with exported C ABI entrypoints and deterministic probe
coverage.

## Scope

- land `native/objc3c/src/runtime/objc3_runtime.h`
- land `native/objc3c/src/runtime/objc3_runtime.cpp`
- build `artifacts/lib/objc3_runtime.lib`
- publish a canonical D002 frontend/IR contract packet
- compile and run `tests/tooling/runtime/m251_d002_runtime_library_probe.cpp`

## Constraints

- `M251-D001` remains frozen and continues to publish the surface reservation
  contract rather than mutating into the implementation contract.
- Driver link mode stays `not-linked-until-m251-d003`.
- The deterministic `objc3_msgsend_i32` shim remains test-only evidence.
- `objc3_runtime_dispatch_i32` must preserve the shim arithmetic formula during
  D002 so later driver-link migration does not introduce semantic drift.

## Checker requirements

The checker must prove:

1. D002 docs/spec/code/package anchors are present and deterministic.
2. `npm run build:objc3c-native` emits `artifacts/lib/objc3_runtime.lib`.
3. `hello.objc3` manifest/IR artifacts publish the D002 contract through
   `runtime_support_library_core_feature_contract_id` and
   `!objc3.objc_runtime_support_library_core_feature`.
4. The runtime probe links against the archive and successfully executes
   registration, selector lookup, dispatch, and reset-for-testing.

## Evidence path

- `tmp/reports/m251/M251-D002/runtime_support_library_core_feature_summary.json`

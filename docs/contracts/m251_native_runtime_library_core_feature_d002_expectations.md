# M251 Native Runtime Library Core Feature Expectations (D002)

Contract ID: `objc3c-runtime-support-library-core-feature/m251-d002-v1`

`M251-D002` layers a real in-tree runtime library implementation on top of the
frozen `M251-D001` surface contract.

## Expectations

1. `Objc3RuntimeSupportLibraryCoreFeatureSummary` is the canonical lane-D D002
   implementation packet for the native runtime library skeleton.
2. Manifest JSON and emitted LLVM IR publish the same D002 contract through
   `runtime_support_library_core_feature_contract_id` and
   `!objc3.objc_runtime_support_library_core_feature`.
3. `npm run build:objc3c-native` produces the real native archive
   `artifacts/lib/objc3_runtime.lib`.
4. The in-tree implementation lives at
   `native/objc3c/src/runtime/objc3_runtime.cpp` and exports:
   - `objc3_runtime_register_image`
   - `objc3_runtime_lookup_selector`
   - `objc3_runtime_dispatch_i32`
   - `objc3_runtime_reset_for_testing`
5. `tests/tooling/runtime/m251_d002_runtime_library_probe.cpp` links against the
   archive and proves the happy path works.
6. `objc3_runtime_dispatch_i32` intentionally preserves the deterministic
   `objc3_msgsend_i32` arithmetic formula so D003 can replace shim-backed
   execution without semantic drift.
7. The deterministic shim remains test-only evidence, and driver link mode
   remains `not-linked-until-m251-d003`.

## Validation

- `npm run build:objc3c-native`
- `npm run check:objc3c:m251-d002-native-runtime-library-skeleton-and-exported-entrypoints`
- `npm run test:tooling:m251-d002-native-runtime-library-skeleton-and-exported-entrypoints`
- `npm run check:objc3c:m251-d002-lane-d-readiness`

## Evidence

- `tmp/reports/m251/M251-D002/runtime_support_library_core_feature_summary.json`

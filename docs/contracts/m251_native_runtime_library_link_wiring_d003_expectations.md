# M251 Native Runtime Library Link Wiring Expectations (D003)

Contract ID: `objc3c-runtime-support-library-link-wiring/m251-d003-v1`

`M251-D003` wires emitted-object execution consumers to the real in-tree native
runtime archive without mutating the frozen `M251-D001` surface or the
`M251-D002` implementation packet.

## Expectations

1. `Objc3RuntimeSupportLibraryLinkWiringSummary` is the canonical lane-D D003
   packet for emitted-object runtime-library consumption.
2. Manifest JSON and emitted LLVM IR publish the same D003 contract through
   `runtime_support_library_link_wiring_contract_id` and
   `!objc3.objc_runtime_support_library_link_wiring`.
3. Emitted-object runtime link mode is
   `emitted-object-links-against-objc3_runtime-lib`.
4. `scripts/check_objc3c_native_execution_smoke.ps1` resolves the runtime
   archive from emitted manifest data and links runtime-requiring fixtures
   against `artifacts/lib/objc3_runtime.lib`.
5. `native/objc3c/src/runtime/objc3_runtime.cpp` exports the compatibility
   bridge symbol `objc3_msgsend_i32`, and that bridge forwards to
   `objc3_runtime_dispatch_i32`.
6. The canonical runtime API remains `objc3_runtime_dispatch_i32`; the bridge is
   for emitted-object compatibility and not a replacement public surface.
7. `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains explicit test-only
   evidence for unresolved-symbol negative coverage and formula parity.
8. Validation evidence lands under
   `tmp/reports/m251/M251-D003/runtime_support_library_link_wiring_summary.json`.

## Non-Goals and Fail-Closed Rules

- `M251-D003` does not claim metadata registration/startup is complete.
- `M251-D003` does not claim classes, protocols, categories, or properties are
  executable runtime features yet.
- `M251-D003` does not remove the shim file; it removes the shim from the
  canonical emitted-object execution path.
- Link-failure fixtures must continue to fail closed by omitting runtime library
  linkage where unresolved-symbol behavior is the expected outcome.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

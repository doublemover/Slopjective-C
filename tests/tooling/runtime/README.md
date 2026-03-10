# Objective-C 3 Runtime Shim

`objc3_msgsend_i32` is a deterministic test shim:

- `M = 2147483629`
- `selector_score = sum((byte_i * i), i starting at 1) mod M`, using selector bytes as unsigned 8-bit values
- If `selector == NULL`, `selector_score = 0`
- Return value:

`(41 + 97*receiver + 7*a0 + 11*a1 + 13*a2 + 17*a3 + 19*selector_score) mod M`

The implementation normalizes negative modulo results into `[0, M-1]`.

`M251-D001` reserves the real native runtime-library surface separately:

- `objc3_runtime`
- `native/objc3c/src/runtime`
- `native/objc3c/src/runtime/objc3_runtime.h`
- `objc3_runtime_register_image`
- `objc3_runtime_lookup_selector`
- `objc3_runtime_dispatch_i32`
- `objc3_runtime_reset_for_testing`

This shim is not that runtime library and remains test-only evidence.

`M251-D002` now instantiates the in-tree native runtime library skeleton:

- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `artifacts/lib/objc3_runtime.lib`
- `tests/tooling/runtime/m251_d002_runtime_library_probe.cpp`

The current native library intentionally preserves the deterministic
`objc3_msgsend_i32` arithmetic formula for `objc3_runtime_dispatch_i32` so the
driver can migrate from shim-backed execution to native-library linkage in
`M251-D003` without semantic drift.

`M251-D003` now makes that linkage real for emitted-object execution paths:

- `scripts/check_objc3c_native_execution_smoke.ps1` resolves the runtime
  archive from emitted manifest data and links against
  `artifacts/lib/objc3_runtime.lib`
- `native/objc3c/src/runtime/objc3_runtime.cpp` exports
  `objc3_msgsend_i32` as a compatibility bridge to
  `objc3_runtime_dispatch_i32`
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains test-only evidence
  for explicit unresolved-symbol and formula-parity coverage

`M251-E003` publishes the canonical operator runbook for this runtime-foundation
slice at `docs/runbooks/m251_runtime_foundation_developer_runbook.md`.

`M254-A001` freezes the next startup-registration handoff without emitting it
yet:

- translation-unit preregistration inventory is the emitted
  `module.runtime-metadata.bin` payload plus
  `module.runtime-metadata-linker-options.rsp` and
  `module.runtime-metadata-discovery.json`
- the reserved constructor-root ownership model is
  `compiler-emits-constructor-root-runtime-owns-registration-state`
- the runtime-owned entrypoint remains `objc3_runtime_register_image`
- no startup constructor/root emission or bootstrap execution lands in
  `M254-A001`

`M254-A002` now emits one real `module.runtime-registration-manifest.json`
artifact on the native object-emission path:

- the manifest is authoritative for constructor-root shape under
  `registration-manifest-authoritative-for-constructor-root-shape`
- init-stub ownership remains
  `lowering-emits-init-stub-from-registration-manifest`
- the emitted manifest carries forward the same runtime-owned payload
  inventory, runtime archive path `artifacts/lib/objc3_runtime.lib`, and
  translation-unit identity key from linker-retention discovery
- init-stub emission and runtime bootstrap execution are still deferred to
  later `M254` issues

`M254-B001` freezes the startup/bootstrap semantic contract over that emitted
manifest:

- duplicate registration is defined against the translation-unit identity key
  and must fail closed rather than silently merging
- realization order is defined as constructor-root handoff followed by
  registration-manifest order
- bootstrap failure mode is
  `abort-before-user-main-no-partial-registration-commit`
- image-local initialization remains runtime-owned and image-local rather than
  driver-owned global mutable state

`M254-B002` turns that freeze into a live runtime contract:

- `objc3_runtime_register_image` now rejects duplicate translation-unit
  identity keys with status `-2`
- non-monotonic registration ordinals are rejected with status `-3`
- invalid descriptors are rejected with status `-1`
- `objc3_runtime_copy_registration_state_for_testing` exposes the committed
  runtime state so probes can verify failed registrations do not partially
  commit
- emitted `module.runtime-registration-manifest.json` payloads now carry the
  same duplicate/order/failure/status-code surface consumed by the runtime
  probe

`M254-C001` freezes the lowering-owned startup materialization boundary without
emitting it yet:

- the canonical lowering contract is
  `objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1`
- the preserved constructor root remains `__objc3_runtime_register_image_ctor`
- the preserved init-stub prefix remains
  `__objc3_runtime_register_image_init_stub_`
- the reserved registration-table prefix is
  `__objc3_runtime_registration_table_`
- the future ctor list model is
  `llvm.global_ctors-single-root-priority-65535`
- emitted IR must stay explicit that no ctor root, no init stub, and no
  registration table are materialized yet

`M254-C002` then turns that frozen boundary into a live emitted startup path:

- emitted IR now contains `@llvm.global_ctors`
- the canonical ctor root remains `__objc3_runtime_register_image_ctor`
- the canonical init-stub prefix remains
  `__objc3_runtime_register_image_init_stub_`
- the canonical registration-table prefix remains
  `__objc3_runtime_registration_table_`
- startup probes link a metadata-only `.objc3` object and prove registration
  happened before `main`

`M254-C003` expands the same emitted startup path with one self-describing
registration table and one image-local init-state cell per emitted image:

- emitted registration tables now carry ABI/version integers and section-root
  pointers for class/protocol/category/property/ivar aggregates
- selector/string pool roots are carried directly in the table when present
- the init stub now guards startup with the image-local init-state cell before
  calling `objc3_runtime_register_image`

`M254-D001` freezes the runtime-owned bootstrap API surface that the startup
path now targets:

- semantic surface
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_contract`
- public header `native/objc3c/src/runtime/objc3_runtime.h`
- archive `artifacts/lib/objc3_runtime.lib`
- preserved runtime entrypoints:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_copy_registration_state_for_testing`
  - `objc3_runtime_reset_for_testing`
- runtime-owned locking model `process-global-mutex-serialized-runtime-state`
- image walk remains deferred to `M254-D002`
- deterministic reset expansion remains deferred to `M254-D003`

`M254-D002` proves the emitted startup path now stages and walks the
registration table before the frozen D001 register-image call:

- runtime probe
  `tests/tooling/runtime/m254_d002_runtime_registrar_image_walk_probe.cpp`
- startup evidence verifies the private stage hook
  `objc3_runtime_stage_registration_table_for_bootstrap`
- startup evidence verifies the private image-walk snapshot hook
  `objc3_runtime_copy_image_walk_state_for_testing`
- the probe validates that emitted selector pools preintern known selectors like
  `tokenValue`

`M254-D003` extends the private bootstrap probe surface with deterministic
same-process reset/replay coverage:

- the runtime zeroes retained emitted image-local init-state cells on
  `objc3_runtime_reset_for_testing`
- the runtime preserves a retained bootstrap catalog across reset
- the private replay hook
  `objc3_runtime_replay_registered_images_for_testing` re-registers retained
  startup images in original registration order
- the private reset/replay snapshot hook
  `objc3_runtime_copy_reset_replay_state_for_testing` proves reset/replay
  generation counts and cleared-init-state counts

`M254-D004` then makes the operator launch surfaces consume the same emitted
runtime launch contract:

- compile wrapper `scripts/objc3c_native_compile.ps1`
- compile proof `scripts/run_objc3c_native_compile_proof.ps1`
- execution smoke `scripts/check_objc3c_native_execution_smoke.ps1`
- authoritative manifest `module.runtime-registration-manifest.json`
- contract id `objc3c-runtime-launch-integration/m254-d004-v1`
- smoke/proof link commands consume emitted `driver_linker_flags` from that
  manifest rather than heuristic runtime-library fallback logic

`M254-E001` then freezes one lane-E startup-registration gate over the
implemented bootstrap proof chain:

- `M254-A002` registration manifest ownership
- `M254-B002` live bootstrap semantics
- `M254-C003` registration-table/image-local-init realization
- `M254-D003` deterministic reset/replay proof
- `M254-D004` operator launch contract
- canonical gate evidence
  `tmp/reports/m254/M254-E001/startup_registration_gate_summary.json`
- no new runtime/bootstrap behavior lands in `M254-E001`; it is a fail-closed
  validation boundary ahead of `M254-E002`

`M254-E002` closes the milestone on top of the same replay/bootstrap evidence
with one published operator runbook:

- `docs/runbooks/m254_bootstrap_replay_operator_runbook.md`
- `scripts/check_objc3c_native_execution_smoke.ps1`
- canonical closeout evidence
  `tmp/reports/m254/M254-E002/replay_bootstrap_runbook_closeout_summary.json`

`M263-E002` closes the bootstrap-hardening milestone with one published operator runbook
and one runnable bootstrap matrix:

- `docs/runbooks/m263_bootstrap_matrix_operator_runbook.md`
- `scripts/check_objc3c_bootstrap_matrix.ps1`
- canonical matrix summary
  `tmp/artifacts/objc3c-native/bootstrap-matrix/m263_e002_bootstrap_matrix_closeout/summary.json`
- canonical closeout evidence
  `tmp/reports/m263/M263-E002/bootstrap_matrix_closeout_summary.json`

`M255-D001` freezes the next runtime-owned boundary after the live dispatch
cutover:

- contract id `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
- canonical selector lookup symbol `objc3_runtime_lookup_selector`
- canonical dispatch symbol `objc3_runtime_dispatch_i32`
- selector handles remain stable per canonical selector spelling within the
  process-global runtime state
- metadata-backed selector lookup tables remain deferred to `M255-D002`
- method-cache / slow-path resolution remain deferred to `M255-D003`
- protocol/category-aware resolution remains deferred to `M255-D004`
- `objc3_msgsend_i32` remains compatibility/test evidence only

`M255-D002` adds the selector-table proof surface:

- runtime snapshots:
  - `objc3_runtime_copy_selector_lookup_table_state_for_testing`
  - `objc3_runtime_copy_selector_lookup_entry_for_testing`
- emitted selector pools are consumed during startup registration and replay
- duplicate selector spellings inside one selector pool fail closed
- duplicate selector spellings across images merge onto one canonical stable ID
- unknown selectors remain dynamic until `M255-D003`

`M255-D003` adds the live method-cache / slow-path proof surface:

- runtime snapshots:
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
- known class receivers and class self receivers normalize onto one metaclass
  cache identity
- emitted class/metaclass method tables resolve live callable bodies during the
  first slow-path lookup
- positive cache entries are reused for repeat live dispatch
- negative cache entries are reused for unresolved selectors while preserving
  the compatibility fallback result

`M255-D004` adds the protocol/category-aware proof surface:

- category-backed instance dispatch resolves live category implementation bodies
- category-backed class dispatch resolves live category class methods
- protocol-declared but body-less selectors stay negative while reporting
  category/protocol probe counts
- cache snapshots preserve category/protocol probe counts across repeat
  dispatches

`M256-D001` freezes the runtime-owned class realization boundary above that
lookup/dispatch surface:

- contract id `objc3c-runtime-class-realization-freeze/m256-d001-v1`
- class/metaclass realization remains driven by emitted `M256-C003`
  realization records rather than source rediscovery
- superclass walking remains deterministic per resolved class bundle chain
- category attachment remains runtime-owned through preferred implementation
  records per category name
- protocol records remain declaration-aware negative runtime checks only
- proof surface remains the private runtime snapshots in
  `runtime/objc3_runtime_bootstrap_internal.h`

`M256-D002` proves the runtime-owned realized class graph and root-class
baseline above that same lookup/dispatch surface:

- contract id `objc3c-runtime-metaclass-graph-root-class-baseline/m256-d002-v1`
- probe:
  `tests/tooling/runtime/m256_d002_metaclass_graph_root_class_probe.cpp`
- fixture:
  `tests/tooling/fixtures/native/m256_d002_metaclass_graph_root_class_library.objc3`
- additional proof surface:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`

`M256-D003` proves attached-category dispatch and runtime protocol conformance
queries above that same realized-graph surface:

- contract id `objc3c-runtime-category-attachment-protocol-conformance/m256-d003-v1`
- probe:
  `tests/tooling/runtime/m256_d003_category_attachment_protocol_runtime_probe.cpp`
- fixture:
  `tests/tooling/fixtures/native/m256_d003_category_attachment_protocol_runtime_library.objc3`
- additional proof surface:
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`

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

## M268 continuation runtime-helper probe

`tests/tooling/runtime/m268_d001_continuation_runtime_helper_probe.cpp`
proves the first private Part 7 continuation helper ABI executes and publishes
deterministic state:

- allocation through `objc3_runtime_allocate_async_continuation_i32`
- executor handoff through
  `objc3_runtime_handoff_async_continuation_to_executor_i32`
- resume through `objc3_runtime_resume_async_continuation_i32`
- state inspection through
  `objc3_runtime_copy_async_continuation_state_for_testing`

This probe is intentionally narrower than later live async runtime work:

- it does not claim compiled async functions suspend
- it does not claim state-machine execution
- it does not claim executor scheduling beyond deterministic helper traffic

## M268 live continuation runtime integration probe

`tests/tooling/runtime/m268_d002_live_continuation_runtime_integration_probe.cpp`
links a compiled Part 7 object with the packaged runtime archive and proves the
supported `await` happy path now executes through the private continuation
helper cluster.

The probe validates:

- compiled async function and async method entrypoints return the expected
  direct-call value
- runtime snapshot traffic records deterministic allocation, handoff, and
  resume counts from those compiled entrypoints
- the current executable slice is still non-suspending and retires all logical
  continuation handles before returning
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

`M257-D001` freezes the current property/layout runtime boundary above the same
lookup/dispatch surface:

- contract id `objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1`
- repeated `alloc`/`new` still produce one canonical realized instance identity
  per class
- synthesized property accessors still execute through the lane-C storage
  globals rather than true per-instance slots
- proof surface remains:
  - `objc3_runtime_copy_registration_state_for_testing`
  - `objc3_runtime_copy_selector_lookup_table_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
  - `tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp`

`M257-D002` upgrades the same lookup/dispatch surface to true per-instance
allocation:

- contract id `objc3c-runtime-instance-allocation-layout-support/m257-d002-v1`
- repeated `alloc` / `new` now produce distinct realized instance identities
- synthesized property accessors now execute through realized per-instance slot
  storage instead of the lane-C storage globals
- proof surface now additionally relies on:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `tests/tooling/runtime/m257_d002_instance_allocation_runtime_probe.cpp`

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

`M256-D004` adds the canonical runnable object sample split above the same
runtime-owned realized graph:

- contract id `objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1`
- executable sample:
  `tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_sample.objc3`
- metadata-rich library:
  `tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_runtime_library.objc3`
- probe:
  `tests/tooling/runtime/m256_d004_canonical_runnable_object_probe.cpp`
- additional proof surface:
  - `objc3_runtime_copy_method_cache_entry_for_testing`

`M257-D003` adds private property metadata reflection helpers over that same
runtime-owned graph:

- contract id `objc3c-runtime-property-metadata-reflection/m257-d003-v1`
- probe:
  `tests/tooling/runtime/m257_d003_property_metadata_reflection_probe.cpp`
- fixture:
  `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
- additional proof surface:
  - `objc3_runtime_copy_property_registry_state_for_testing`
  - `objc3_runtime_copy_property_entry_for_testing`

## Incremental native build commands

Use the public build surface this way:

- `npm run build:objc3c-native`
  - ordinary local native binary refresh
- `npm run build:objc3c-native:contracts`
  - contract packet refresh
- `npm run build:objc3c-native:full`
  - closeout / broad validation path
- `npm run build:objc3c-native:reconfigure`
  - fingerprint or toolchain drift recovery without deleting the build tree

Relevant paths:

- persistent build tree
  - `tmp/build-objc3c-native`
- compile database
  - `tmp/build-objc3c-native/compile_commands.json`
- fingerprint
  - `tmp/build-objc3c-native/native_build_backend_fingerprint.json`

## M267 error runtime and bridge-helper probe

`M267-D001` adds the first private runtime helper cluster for the runnable Part
6 error surface:

- contract id `objc3c-part6-error-runtime-and-bridge-helper-api/m267-d001-v1`
- probe:
  `tests/tooling/runtime/m267_d001_error_runtime_bridge_helper_probe.cpp`
- private testing snapshot:
  - `objc3_runtime_copy_error_bridge_state_for_testing`
- helper entrypoints exercised by the probe:
  - `objc3_runtime_store_thrown_error_i32`
  - `objc3_runtime_load_thrown_error_i32`
  - `objc3_runtime_bridge_status_error_i32`
  - `objc3_runtime_bridge_nserror_error_i32`
  - `objc3_runtime_catch_matches_error_i32`

## M267 live error runtime integration probe

`M267-D002` links generated Part 6 object code against the packaged runtime
library and proves the helper surface from `M267-D001` is live:

- contract id `objc3c-part6-live-error-runtime-integration/m267-d002-v1`
- fixture:
  `tests/tooling/fixtures/native/m267_d002_live_error_runtime_integration_positive.objc3`
- probe:
  `tests/tooling/runtime/m267_d002_live_error_runtime_integration_probe.cpp`
- runtime registration manifest remains the runtime-library path source:
  - `module.runtime-registration-manifest.json`
- helper traffic proven by the linked probe:
  - `objc3_runtime_store_thrown_error_i32`
  - `objc3_runtime_load_thrown_error_i32`
  - `objc3_runtime_bridge_status_error_i32`
  - `objc3_runtime_catch_matches_error_i32`

## M267 cross-module error-surface preservation probe

`M267-D003` hardens the cross-module side of the same runnable Part 6 slice:

- a provider emits the canonical runtime import surface plus
  `module.part6-error-replay.json`
- a consumer imports that surface and emits a cross-module link plan that now
  preserves the imported Part 6 replay contract ids, readiness booleans, and
  replay keys
- a tampered imported runtime surface must fail closed during cross-module
  link-plan construction before mixed-module executable claims are made

## M269 task-runtime lowering probe

`M269-C002` adds a helper-backed runtime probe for the supported task-runtime
lowering slice:

- contract id `objc3c-part7-task-runtime-lowering-implementation/m269-c002-v1`
- fixture:
  `tests/tooling/fixtures/native/m269_c002_task_runtime_lowering_positive.objc3`
- probe:
  `tests/tooling/runtime/m269_c002_task_runtime_lowering_probe.cpp`
- helper traffic proven by the linked probe:
  - `objc3_runtime_spawn_task_i32`
  - `objc3_runtime_enter_task_group_scope_i32`
  - `objc3_runtime_add_task_group_task_i32`
  - `objc3_runtime_wait_task_group_next_i32`
  - `objc3_runtime_cancel_task_group_i32`
  - `objc3_runtime_task_is_cancelled_i32`
  - `objc3_runtime_task_on_cancel_i32`
  - `objc3_runtime_executor_hop_i32`

## M269 task-runtime ABI completion probe

`M269-C003` reuses the same private runtime helper boundary but adds an
artifact-level ABI packet and IR metadata marker for it:

- contract id `objc3c-part7-task-runtime-abi-completion/m269-c003-v1`
- probe:
  `tests/tooling/runtime/m269_c003_task_runtime_abi_completion_probe.cpp`
- private runtime snapshot symbol:
  - `objc3_runtime_copy_task_runtime_state_for_testing`

## M269 scheduler and executor runtime contract probe

`M269-D001` reuses the same helper-backed runtime slice but freezes it as the
canonical private scheduler/executor/task-state contract for the supported
Part 7 surface.

- runtime probe:
  `tests/tooling/runtime/m269_d001_scheduler_executor_runtime_contract_probe.cpp`
- the probe validates:
  - `objc3_runtime_spawn_task_i32`
  - `objc3_runtime_enter_task_group_scope_i32`
  - `objc3_runtime_add_task_group_task_i32`
  - `objc3_runtime_wait_task_group_next_i32`
  - `objc3_runtime_cancel_task_group_i32`
  - `objc3_runtime_task_is_cancelled_i32`
  - `objc3_runtime_task_on_cancel_i32`
  - `objc3_runtime_executor_hop_i32`
  - `objc3_runtime_copy_task_runtime_state_for_testing`

## M269 live task runtime and executor implementation probe

`M269-D002` keeps the same private helper surface but proves it as a live
runtime execution boundary rather than only a frozen contract.

- runtime probe:
  `tests/tooling/runtime/m269_d002_live_task_runtime_and_executor_implementation_probe.cpp`
- the probe validates live execution through:
  - `objc3_runtime_spawn_task_i32`
  - `objc3_runtime_enter_task_group_scope_i32`
  - `objc3_runtime_add_task_group_task_i32`
  - `objc3_runtime_wait_task_group_next_i32`
  - `objc3_runtime_cancel_task_group_i32`
  - `objc3_runtime_task_is_cancelled_i32`
  - `objc3_runtime_task_on_cancel_i32`
  - `objc3_runtime_executor_hop_i32`
  - `objc3_runtime_copy_task_runtime_state_for_testing`

## M269 task runtime hardening probe

`M269-D003` adds a reset/autorelease hardening probe above the same live task
runtime boundary.

- runtime probe:
  `tests/tooling/runtime/m269_d003_task_runtime_hardening_probe.cpp`
- the probe validates:
  - `objc3_runtime_reset_for_testing`
  - `objc3_runtime_copy_task_runtime_state_for_testing`
  - `objc3_runtime_copy_memory_management_state_for_testing`
  - `objc3_runtime_copy_arc_debug_state_for_testing`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`

## M270 actor lowering runtime probe

`M270-C002` adds the first live helper-backed actor lowering proof slice.

- contract id `objc3c-part7-actor-thunk-hop-and-isolation-lowering/m270-c002-v1`
- fixture:
  `tests/tooling/fixtures/native/m270_c002_actor_lowering_runtime_positive.objc3`
- probe:
  `tests/tooling/runtime/m270_c002_actor_lowering_runtime_probe.cpp`
- helper traffic proven by the probe:
  - `objc3_runtime_actor_enter_isolation_thunk_i32`
  - `objc3_runtime_actor_enter_nonisolated_i32`
  - `objc3_runtime_actor_hop_to_executor_i32`
  - `objc3_runtime_copy_actor_runtime_state_for_testing`

## M270 actor replay/race artifact probe

`M270-C003` extends the same private actor runtime snapshot with the
strict-concurrency replay-proof and race-guard artifact slice.

- contract id `objc3c-part7-actor-replay-proof-and-race-guard-integration/m270-c003-v1`
- fixture:
  `tests/tooling/fixtures/native/m270_c003_actor_replay_race_guard_positive.objc3`
- probe:
  `tests/tooling/runtime/m270_c003_actor_replay_race_guard_probe.cpp`
- helper traffic proven by the probe:
  - `objc3_runtime_actor_record_replay_proof_i32`
  - `objc3_runtime_actor_record_race_guard_i32`
  - `objc3_runtime_copy_actor_runtime_state_for_testing`

## M269 task and executor conformance gate

`M269-E001` does not add a new runtime probe. It freezes lane-E on top of the
existing `M269-D003` hardened runtime proof and the upstream `M269-A002`
through `M269-D003` summary chain.

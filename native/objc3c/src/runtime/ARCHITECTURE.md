# objc3 Runtime Architecture

## Runtime Subsystem Model

The live runtime execution path is split into five owned subsystems:

1. compile publication
2. installation and registration
3. selector lookup and dispatch
4. property, storage, and ownership execution
5. acceptance and replay reporting

Owned code paths:

- compile publication:
  - `native/objc3c/src/driver/objc3_compilation_driver.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- installation and registration:
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- selector lookup and dispatch:
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- property, storage, and ownership execution:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- acceptance and replay reporting:
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/objc3c_public_workflow_runner.py`

Allowed subsystem dependencies:

- compile publication -> installation and registration, selector lookup and dispatch, property/storage/ownership execution
- installation and registration -> runtime-owned state only
- selector lookup and dispatch -> installation and registration
- property, storage, and ownership execution -> installation and registration, selector lookup and dispatch
- acceptance and replay reporting -> compile publication, installation and registration, selector lookup and dispatch, property/storage/ownership execution

Forbidden subsystem shortcuts:

- acceptance or replay reporting must not claim runtime execution from sidecars alone
- selector or property execution must not bypass installation through alternate loader state
- compile publication must not widen the public runtime ABI outside `native/objc3c/src/runtime/objc3_runtime.h`
- milestone-specific closeout helpers must not become runtime subsystem dependencies

Bootstrap registration source of truth:

- compile publication exposes the coupled bootstrap registration source through
  the emitted compile-manifest key `runtime_bootstrap_registration_source_surface`
- that surface must point at the emitted registration descriptor artifact,
  coupled registration manifest, emitted object/IR, constructor root,
  registration entrypoint, translation-unit identity key, and registration order
  ordinal
- later bootstrap, loader, and replay work must consume that emitted surface
  rather than reconstructing bootstrap source facts from partial sidecars

Bootstrap lowering and registration artifact source of truth:

- compile publication exposes the coupled lowering/artifact boundary through the
  emitted compile-manifest key
  `runtime_bootstrap_lowering_registration_artifact_surface`
- that surface must point at the live lowering contract, registration manifest,
  bootstrap semantics, registration-descriptor frontend closure, emitted
  manifest/object/IR artifacts, ctor/init-stub/registration-table symbol
  family, loader-table ABI/version facts, and the lowered registration
  descriptor fields that name the exact derived init-stub/registration-table/
  image-local-init-state globals
- later loader-table realization, imported-image replay, and packaging work
  must consume that emitted surface rather than reconstructing lowering truth
  from the semantic packet alone
- the emitted registration descriptor and `module.ll` must agree on the exact
  lowered constructor-root, init-stub, registration-table, and image-local
  init-state symbols; drift is a compile-coupled acceptance failure

Multi-image startup ordering source of truth:

- compile publication exposes the coupled multi-image ordering boundary through
  the emitted compile-manifest key
  `runtime_multi_image_startup_ordering_source_surface`
- that surface must point at the live legality semantics, restart/replay
  semantics, bootstrap API/reset/registrar contracts, archive static-link
  replay corpus contract, runtime header paths, and the installation-lifecycle
  proof symbols, duplicate/out-of-order diagnostic status codes, and
  registration-order snapshot fields
- later image-walk, duplicate-install rejection, and reset/replay expansion work
  must consume that emitted surface plus the
  `tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp` proof
  path instead of reconstructing ordering truth from scattered semantic
  fragments or hand-maintained notes

Object-model realization source of truth:

- compile publication exposes the coupled realization boundary through the
  emitted compile-manifest key
  `runtime_object_model_realization_source_surface`
- that surface must point at the emitted realization records contracts, runtime
  realization/metaclass/category-protocol contracts, coupled registration
  artifacts, and the runtime entrypoints plus private realized-class/protocol
  query snapshots that later work must consume
- later realization semantics, dispatch-table lowering, reflection query
  completion, and runnable conformance work must consume that emitted surface
  rather than rederiving class/category/protocol behavior from sidecars or
  stale milestone notes

Bootstrap legality and installation semantics:

- duplicate translation-unit identity keys must fail closed with
  `OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY`
  and must not advance installed-image counts or next-expected registration
  ordinals
- duplicate-install diagnostics must publish `last_rejected_module_name`,
  `last_rejected_translation_unit_identity_key`, and
  `last_rejected_registration_order_ordinal` without partially committing
  runtime installation state
- out-of-order registration ordinals must fail closed with
  `OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION` and must not
  advance installed-image counts or next-expected registration ordinals
- out-of-order diagnostics must publish `last_rejected_module_name`,
  `last_rejected_translation_unit_identity_key`, and
  `last_rejected_registration_order_ordinal` without partially committing
  runtime installation state
- malformed staged registration roots must fail closed with
  `OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS`; the linker
  anchor must point at the discovery root, and the discovery root must close
  over the class/protocol/category/property/ivar descriptor roots before any
  image-walk state is published
- the shared proof path for those guarantees is
  `tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp`,
  exercised through `python scripts/check_objc3c_runtime_acceptance.py`

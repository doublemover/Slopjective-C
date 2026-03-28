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

Block/ARC unified source of truth:

- compile publication exposes the coupled block/ARC source boundary through the
  emitted compile-manifest key `runtime_block_arc_unified_source_surface`
- that surface must point at the live block source/model/storage/semantic
  contracts, the executable block lowering contracts already emitted through
  the real compiler path, the ARC mode/semantic/inference/interaction
  contracts, the authoritative semantic-packet field names, and the runtime
  entrypoints `objc3_runtime_promote_block_i32`,
  `objc3_runtime_invoke_block_i32`, and
  `objc3_runtime_copy_arc_debug_state_for_testing` plus the private ABI/testing
  snapshot `objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing`
- later ownership transfer, capture-family, generalized ARC insertion, block
  helper/runtime implementation, and runnable block-ARC validation work must
  extend that emitted surface rather than inferring runtime truth from sidecar
  probes, ad hoc IR inspection, or milestone-local notes

Block/ARC runtime ABI source of truth:

- compile publication exposes the private block/ARC runtime ABI boundary
  through the emitted compile-manifest key
  `runtime_block_arc_runtime_abi_surface`
- that surface must point at the unchanged public runtime header, the internal
  bootstrap runtime header, the fixed public registration/lookup/dispatch/reset
  boundary, and the private block promotion/invoke, ARC helper, weak property
  helper, and testing snapshot entrypoints that make up the live block/ARC
  runtime ABI
- later escaping block implementation, byref forwarding, ARC hardening, and
  packaged validation work must extend that emitted ABI surface instead of
  widening the public header or rebuilding runtime symbol inventories from
  probe-local assumptions

Ownership-transfer and capture-family source of truth:

- compile publication exposes the coupled ownership-transfer/capture-family
  boundary through the emitted compile-manifest key
  `runtime_ownership_transfer_capture_family_source_surface`
- that surface must point at the Part 8 resource-move/use-after-move summary,
  the Part 8 capture-list/retainable-family legality summary, the block object
  capture ownership contract, the ARC inference and interaction contracts, and
  the authoritative AST fields that publish owned/weak/unowned capture counts
  plus the normalized block capture ownership profile
- later block storage semantics, escaping byref legality, and ARC cleanup or
  ownership-transfer implementation must extend that emitted surface instead of
  inventing a second semantic path or re-deriving capture-family truth during
  lowering

Property/ivar/storage/accessor source of truth:

- compile publication exposes the coupled property/ivar/storage/accessor source
  boundary through the emitted compile-manifest key
  `runtime_property_ivar_storage_accessor_source_surface`
- that surface must point at the authoritative AST/sema/IR/pipeline/runtime
  code paths, the executable property/ivar source closure/model-completion/
  semantics contracts, and the frozen property attribute, accessor selector,
  synthesized-binding, and ivar-layout source models that later work must
  consume
- later storage legality, synthesis, lowering, runtime realization, and
  storage/reflection conformance work must extend that emitted surface rather
  than inventing semantics from milestone-local notes, sidecar-only metadata,
  or lowering-owned re-derivation

Property atomicity/synthesis/reflection source of truth:

- compile publication exposes the coupled property atomicity/synthesis/
  reflection source boundary through the emitted compile-manifest key
  `runtime_property_atomicity_synthesis_reflection_source_surface`
- that surface must point at the property source surface, property metadata
  reflection contract, ownership-backed property attribute surface, the
  authoritative atomic/non-atomic/conflict/property-attribute source fields,
  and the exact sema/pipeline/runtime reflection code paths that later issues
  must use
- later legality, lowering, reflection, and runtime-storage work must extend
  that emitted surface instead of widening the public ABI, inventing an
  ad hoc atomic helper path, or bypassing the existing private property query
  boundary

Dispatch and synthesized-accessor lowering source of truth:

- compile publication exposes the coupled accessor-storage lowering and
  metadata boundary through the emitted compile-manifest key
  `dispatch_and_synthesized_accessor_lowering_surface`
- that surface must point at the property/ivar storage source surface, the
  executable property accessor layout / ivar layout / synthesized accessor
  lowering contracts, the storage/accessor runtime ABI surface, the live
  lowering/IR/frontend-artifacts/runtime code paths, and the coupled
  manifest/runtime-registration-manifest/object/LLVM IR artifacts produced by
  the real compiler path
- later storage reflection, runtime helper ABI, and packaged validation work
  must consume that emitted surface instead of inferring helper selection or
  synthesized accessor truth from sidecar-only metadata, probe-local
  assumptions, or ad hoc IR inspection

Executable accessor-body and ivar-layout lowering source of truth:

- compile publication exposes the emitted executable lowering boundaries
  through the compile-manifest keys
  `executable_property_accessor_layout_lowering_surface`,
  `executable_ivar_layout_emission_surface`, and
  `executable_synthesized_accessor_property_lowering_surface`
- those surfaces must point at the live lowering/IR/frontend-artifacts/runtime
  code paths, the coupled manifest/runtime-registration-manifest/object/LLVM IR
  artifacts, and the emitted descriptor/layout/helper inventories that the live
  runtime probes consume
- later storage/reflection/runtime work must consume those emitted lowering
  surfaces instead of re-deriving accessor bodies or ivar layouts from
  sidecars, probe-local assumptions, or ad hoc IR scans

Realization lowering and reflection artifact source of truth:

- compile publication exposes the coupled realization-lowering and reflection-
  artifact boundary through the emitted compile-manifest key
  `runtime_realization_lowering_reflection_artifact_surface`
- that surface must point at the realization source surface, dispatch and
  synthesized-accessor lowering surface, executable realization records
  contract, property metadata reflection contract, ownership-backed property
  metadata contract, and the coupled manifest/descriptor/object/LLVM IR/
  provenance artifacts produced by the real compiler path
- later dispatch-table lowering, reflection artifact preservation, and replay
  hardening work must extend that emitted surface instead of reconstructing
  lowering truth from source-only manifests, ad hoc IR inspection, or stale
  milestone notes

Dispatch-table and reflection-record lowering source of truth:

- compile publication exposes the coupled dispatch-table and reflection-record
  lowering boundary through the emitted compile-manifest key
  `runtime_dispatch_table_reflection_record_lowering_surface`
- that surface must point at the realization source surface, runtime state
  publication surface, dispatch/accessor lowering surface, method
  dispatch/selector thunk lowering contract, executable realization records
  contract, and the selector-pool plus metadata-aggregate roots emitted by the
  real compiler path
- later cross-module replay preservation and runtime ABI work must consume that
  emitted surface instead of rediscovering selector-table or reflection-record
  roots from ad hoc IR inspection or milestone notes

Cross-module realized-metadata replay preservation source of truth:

- emitted packaging proof exposes the cross-module realized-metadata replay
  boundary through `module.cross-module-runtime-link-plan.json`
- that artifact must point at the realization source surface,
  realization-lowering reflection-artifact surface, dispatch-table/
  reflection-record lowering surface, and the preserved local/imported
  descriptor-count, identity, registration-ordinal, and replay-readiness facts
  derived from emitted runtime registration manifests
- later runtime ABI, lookup, and end-to-end conformance work must consume that
  emitted link-plan boundary instead of reconstructing cross-image replay truth
  from ad hoc probe payloads or milestone-local notes

Object-model runtime ABI and query surface source of truth:

- compile publication exposes the coupled object-model ABI/query boundary
  through the emitted compile-manifest key
  `runtime_object_model_abi_query_surface`
- that surface must point at the realization source surface, realization/
  reflection lowering surfaces, cross-module replay preservation surface, the
  realization/lookup/category/coherence surfaces, the stable public runtime
  header path, and the private object-model query symbols the current
  executable probes consume
- later realization lookup implementation and runnable conformance work must
  extend that surface rather than widening the public runtime ABI or
  reconstructing object-model query truth from probe-specific assumptions

Realization lookup and reflection implementation source of truth:

- compile publication exposes the live implementation boundary through the
  emitted compile-manifest key
  `runtime_realization_lookup_reflection_implementation_surface`
- that surface must point at the object-model ABI/query surface, the
  reflection/lookup/category/coherence surfaces, the private runtime internal
  header, and the aggregate object-model query snapshot symbol consumed by the
  live executable probe
- later conformance work must validate the runtime-owned aggregate query state
  through that same emitted surface instead of reconstructing lookup/reflection
  truth from separate probe-local reads

Reflection query source of truth:

- compile publication exposes the coupled reflection boundary through the
  emitted compile-manifest key `runtime_reflection_query_surface`
- that surface must point at the realization source surface, dispatch/accessor
  ABI surface, property metadata reflection contract, ownership-backed property
  metadata contract, and the private realized-class/property/protocol query
  symbols that current executable probes consume
- later reflection-query completion and conformance work must extend that
  private testing surface rather than widening a public reflection ABI or
  reconstructing query answers from source text

Realization and lookup semantics source of truth:

- compile publication exposes the coupled realization/lookup semantics boundary
  through the emitted compile-manifest key
  `runtime_realization_lookup_semantics_surface`
- that surface must point at the realization source surface, reflection query
  surface, dispatch/accessor ABI surface, runtime selector/materialization
  entrypoints, and the private selector-table, method-cache, realized-class,
  and protocol-conformance query symbols that the live executable probes
  consume
- later class/metaclass realization, category attachment, reflection
  visibility, and runnable object-model conformance work must consume that
  emitted surface instead of inferring lookup order from ad hoc probe logic or
  stale milestone notes

Class/metaclass/protocol realization source of truth:

- compile publication exposes the coupled class/metaclass/protocol realization
  boundary through the emitted compile-manifest key
  `runtime_class_metaclass_protocol_realization_surface`
- that surface must point at the realization source surface, reflection query
  surface, realization/lookup semantics surface, coupled registration
  artifacts, and the private realized-class graph, realized-class entry, and
  protocol-conformance query symbols that the live executable probes consume
- later category attachment, merged dispatch resolution, and runnable
  conformance work must extend that emitted surface instead of reconstructing
  class lineage or protocol truth from milestone-local notes or synthetic
  summaries

Category attachment and merged dispatch source of truth:

- compile publication exposes the coupled category-attachment and merged
  dispatch boundary through the emitted compile-manifest key
  `runtime_category_attachment_merged_dispatch_surface`
- that surface must point at the category-attachment protocol-conformance
  contract, realization/lookup semantics surface, class/metaclass/protocol
  realization surface, coupled registration artifacts, and the private
  realized-class graph, realized-class entry, protocol-conformance, and
  method-cache query symbols that the live executable probes consume
- later reflection visibility hardening and runnable conformance work must
  extend that emitted surface instead of rederiving category merge truth from
  ad hoc probe payloads or stale milestone notes

Reflection visibility and runtime coherence diagnostics source of truth:

- compile publication exposes the coupled reflection-visibility and runtime
  coherence diagnostics boundary through the emitted compile-manifest key
  `runtime_reflection_visibility_coherence_diagnostics_surface`
- that surface must point at the reflection query surface, category attachment
  and merged dispatch surface, dispatch/accessor ABI surface, property metadata
  reflection contract, ownership-backed property metadata contract, and the
  private property-registry, property-entry, realized-class, protocol-
  conformance, and method-cache query symbols that the live executable probes
  consume
- later reflection hardening and property/runtime consistency work must extend
  that emitted surface instead of reconstructing coherence guarantees from ad
  hoc probe payloads, milestone-local summaries, or source-only manifests

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

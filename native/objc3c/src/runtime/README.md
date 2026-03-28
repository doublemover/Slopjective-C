# ObjC3 Native Runtime Surface

Live runtime surface:

- archive: `artifacts/lib/objc3_runtime.lib`
- public header: `native/objc3c/src/runtime/objc3_runtime.h`
- primary entrypoints:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_reset_for_testing`

Ownership boundary:

- the compiler owns source-derived metadata and emitted IR/object payloads
- the runtime owns installed registration, lookup, dispatch, allocation, property access, and execution state once payloads are ingested

Current dispatch path:

1. IR lowers supported message sends to `objc3_runtime_dispatch_i32`
2. the runtime interns or resolves the selector through `objc3_runtime_lookup_selector`
3. dispatch probes the method cache and then the realized class/category/protocol slow path
4. resolved methods execute either live emitted method bodies or runtime builtins such as `alloc`, `init`, and synthesized property accessors
5. unresolved sends still fall back to the deterministic arithmetic path in `ComputeDispatchResult`

Installation lifecycle:

1. the compile path emits a coupled object, manifest, runtime registration manifest, and compile provenance set
2. the loader or linked probe retains the emitted registration table roots
3. `objc3_runtime_register_image` installs the emitted image descriptor and staged registration table into runtime-owned state
4. lookup and dispatch consume only that installed runtime-owned state plus runtime builtins
5. `objc3_runtime_reset_for_testing` is the deterministic lifecycle reset hook for acceptance and replay paths

Installation ABI and loader lifecycle surface:

- public installation ABI:
  - `objc3_runtime_register_image`
  - `objc3_runtime_copy_registration_state_for_testing`
  - `objc3_runtime_reset_for_testing`
- private loader lifecycle testing boundary:
  - `objc3_runtime_stage_registration_table_for_bootstrap`
  - `objc3_runtime_copy_image_walk_state_for_testing`
  - `objc3_runtime_replay_registered_images_for_testing`
  - `objc3_runtime_copy_reset_replay_state_for_testing`
- authoritative executable probe:
  - `tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp`
- emitted compile-manifest keys:
  - `runtime_installation_abi_surface`
  - `runtime_loader_lifecycle_surface`

Object-model realization surface:

- emitted compile-manifest key:
  - `runtime_object_model_realization_source_surface`
- public runtime ABI consumed:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
- private query boundary consumed:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- authoritative executable probes:
  - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
  - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
  - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`

Property/ivar/storage/accessor source surface:

- emitted compile-manifest key:
  - `runtime_property_ivar_storage_accessor_source_surface`
- authoritative composed source inputs:
  - `objc3c.executable.property.ivar.source.closure.v1`
  - `objc3c.executable.property.ivar.source.model.completion.v1`
  - `objc3c.executable.property.ivar.semantics.v1`
- authoritative live code paths:
  - `native/objc3c/src/ast/objc3_ast.h`
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
- authoritative source fields:
  - `Objc3PropertyDecl.ivar_binding_symbol`
  - `Objc3PropertyDecl.executable_synthesized_binding_kind`
  - `Objc3PropertyDecl.executable_synthesized_binding_symbol`
  - `Objc3PropertyDecl.property_attribute_profile`
  - `Objc3PropertyDecl.effective_getter_selector`
  - `Objc3PropertyDecl.effective_setter_available`
  - `Objc3PropertyDecl.effective_setter_selector`
  - `Objc3PropertyDecl.accessor_ownership_profile`
  - `Objc3PropertyDecl.executable_ivar_layout_symbol`
  - `Objc3PropertyDecl.executable_ivar_layout_slot_index`
  - `Objc3PropertyDecl.executable_ivar_layout_size_bytes`
  - `Objc3PropertyDecl.executable_ivar_layout_alignment_bytes`
- authoritative executable probes:
  - `tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp`
  - `tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp`
  - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
  - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
  - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`
  - `tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp`
- semantic boundary:
  - later storage legality, synthesis, lowering, runtime realization, and
    property/reflection conformance work must consume the AST/sema-approved
    source boundary instead of re-deriving semantics from sidecars or
    milestone-local notes
  - lowering may serialize these fields into emitted artifacts but must not
    invent property storage, accessor selectors, or ownership semantics beyond
    the frozen source model
  - no public runtime ABI widening or milestone-specific scaffolding is part
    of this boundary

Realization lowering and reflection artifact surface:

- emitted compile-manifest key:
  - `runtime_realization_lowering_reflection_artifact_surface`
- coupled emitted artifacts:
  - `<emit-prefix>.manifest.json`
  - `<emit-prefix>.runtime-registration-manifest.json`
  - `<emit-prefix>.runtime-registration-descriptor.json`
  - `<emit-prefix>.obj`
  - `<emit-prefix>.ll`
  - `<emit-prefix>.compile-provenance.json`
- private reflection artifact query boundary:
  - `objc3_runtime_copy_property_registry_state_for_testing`
  - `objc3_runtime_copy_property_entry_for_testing`
- authoritative executable probes:
  - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
  - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
  - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
  - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
  - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`
- semantic boundary:
  - compile manifest, registration descriptor, object, LLVM IR, and compile
    provenance co-publish realization lowering and reflection artifacts
  - property metadata and ownership artifacts remain coupled to lowered
    dispatch/accessor and executable realization-record outputs

Dispatch-table and reflection-record lowering surface:

- emitted compile-manifest key:
  - `runtime_dispatch_table_reflection_record_lowering_surface`
- coupled emitted artifacts:
  - `<emit-prefix>.manifest.json`
  - `<emit-prefix>.runtime-registration-manifest.json`
  - `<emit-prefix>.runtime-registration-descriptor.json`
  - `<emit-prefix>.obj`
  - `<emit-prefix>.ll`
  - `<emit-prefix>.compile-provenance.json`
- authoritative lowering roots:
  - `@__objc3_sec_selector_pool`
  - `__objc3_sec_class_descriptors`
  - `__objc3_sec_protocol_descriptors`
  - `__objc3_sec_category_descriptors`
  - `__objc3_sec_property_descriptors`
  - `__objc3_sec_ivar_descriptors`
- authoritative executable probes:
  - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
  - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
  - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`
  - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
- semantic boundary:
  - selector-pool-backed dispatch thunks and runtime dispatch sites co-publish
    stable selector table roots in LLVM IR and manifest artifacts
  - realization records and runtime metadata section aggregates co-publish
    class, protocol, category, property, and ivar record roots in emitted
    artifacts

Cross-module realized-metadata replay preservation surface:

- authoritative emitted artifact:
  - `module.cross-module-runtime-link-plan.json`
- coupled emitted artifacts:
  - `<emit-prefix>.obj`
  - `<emit-prefix>.runtime-import-surface.json`
  - `<emit-prefix>.runtime-registration-manifest.json`
  - `<emit-prefix>.cross-module-runtime-link-plan.json`
  - `<emit-prefix>.cross-module-runtime-linker-options.rsp`
- authoritative executable probe:
  - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
- semantic boundary:
  - the cross-module link plan preserves local and imported descriptor-count,
    translation-unit-identity, and registration-ordinal facts from the emitted
    runtime registration manifests
  - the same link plan preserves imported reset/replay readiness and bootstrap
    replay symbols so runnable replay proof stays coupled to emitted packaging
    artifacts

Object-model runtime ABI and query surface:

- emitted compile-manifest key:
  - `runtime_object_model_abi_query_surface`
- public runtime ABI boundary:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_reset_for_testing`
- private object-model query boundary:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_property_registry_state_for_testing`
  - `objc3_runtime_copy_property_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
  - `objc3_runtime_copy_selector_lookup_table_state_for_testing`
  - `objc3_runtime_copy_selector_lookup_entry_for_testing`
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
  - `objc3_runtime_copy_dispatch_state_for_testing`
- semantic boundary:
  - the public runtime header stays frozen at registration, lookup, dispatch,
    and reset
  - object-model lookup and reflection proof stays on the private testing
    snapshot boundary consumed by the live executable probes

Realization lookup and reflection implementation surface:

- emitted compile-manifest key:
  - `runtime_realization_lookup_reflection_implementation_surface`
- aggregate runtime query snapshot:
  - `objc3_runtime_copy_object_model_query_state_for_testing`
- supporting live query symbols:
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_property_registry_state_for_testing`
  - `objc3_runtime_copy_property_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
  - `objc3_runtime_copy_selector_lookup_table_state_for_testing`
  - `objc3_runtime_copy_selector_lookup_entry_for_testing`
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
  - `objc3_runtime_copy_dispatch_state_for_testing`
- authoritative executable probe:
  - `tests/tooling/runtime/m259_d002_realization_lookup_reflection_runtime_probe.cpp`

Reflection query surface:

- emitted compile-manifest key:
  - `runtime_reflection_query_surface`
- private query boundary:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_property_registry_state_for_testing`
  - `objc3_runtime_copy_property_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- authoritative executable probes:
  - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
  - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
  - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`

Realization and lookup semantics surface:

- emitted compile-manifest key:
  - `runtime_realization_lookup_semantics_surface`
- private lookup query boundary:
  - `objc3_runtime_copy_selector_lookup_table_state_for_testing`
  - `objc3_runtime_copy_selector_lookup_entry_for_testing`
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- authoritative executable probes:
  - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
  - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
  - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`
- explicit non-goal:
  - no public reflection ABI widening beyond the current runtime header

Class/metaclass/protocol realization surface:

- emitted compile-manifest key:
  - `runtime_class_metaclass_protocol_realization_surface`
- private realization query boundary:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- authoritative executable probes:
  - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
  - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
- semantic boundary:
  - registration installs runtime-backed class records before live dispatch and
    reflection
  - realized class entries publish stable class, metaclass, superclass, and
    super-metaclass owner identities
  - realized class entries plus runtime conformance queries publish direct and
    attached protocol conformance

Category attachment and merged dispatch surface:

- emitted compile-manifest key:
  - `runtime_category_attachment_merged_dispatch_surface`
- private category query boundary:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
- authoritative executable probes:
  - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
  - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
  - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
- semantic boundary:
  - registration attaches category-owned instance and protocol members onto live
    realized classes before dispatch
  - attached-category implementations override base-class instance lookup
    before superclass and protocol fallback
  - attached categories publish owner and name through realized class entries
    and protocol-conformance queries

Reflection visibility and runtime coherence diagnostics surface:

- emitted compile-manifest key:
  - `runtime_reflection_visibility_coherence_diagnostics_surface`
- private coherence query boundary:
  - `objc3_runtime_copy_property_registry_state_for_testing`
  - `objc3_runtime_copy_property_entry_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
  - `objc3_runtime_copy_method_cache_state_for_testing`
- authoritative executable probes:
  - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
  - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
  - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
  - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`
- semantic boundary:
  - private testing snapshots remain the only reflection visibility surface over
    runtime-owned class, property, and protocol state
  - missing class/property lookups must fail closed without mutating property
    registry or realized-class state
  - reflected selector, owner-identity, slot-layout, and ownership-profile
    metadata must remain coherent with live dispatch and attached-protocol state

Current synthesized-property path:

1. frontend metadata carries effective getter/setter selectors, binding symbols, and ivar layout records
2. IR emission synthesizes missing getter/setter bodies in `Objc3IREmitter::EmitSyntheticMethod`
3. property descriptors carry runtime implementation pointers for those accessors
4. runtime realization builds `RealizedPropertyAccessor` records from emitted descriptors and ivar layout
5. getter/setter dispatch executes against realized per-instance slot storage through the current-property helper surface

Current corrective focus:

- keep dispatch on the canonical `objc3_runtime_lookup_selector` / `objc3_runtime_dispatch_i32` path
- tighten the remaining gap between emitted dispatch IR and runtime selector/materialization behavior
- remove stale transitional lowering artifacts that no longer participate in live execution
- prove native output only from real compiler invocations and executable probes

Authoritative code paths for the current tranche:

- runtime registration and dispatch:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime.h`
- message-send lowering:
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- compile and artifact publication:
  - `native/objc3c/src/driver/objc3_compilation_driver.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- runtime/lowering boundary comments:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
- published architecture map:
  - `docs/objc3c-native.md`
  - `native/objc3c/src/runtime/ARCHITECTURE.md`

Non-goals for the current corrective tranche:

- no public runtime ABI widening beyond the existing header
- no new compatibility-only dispatch path
- no alternate loader lifecycle outside the existing register-image plus testing-reset surface
- no milestone-specific proof sidecars or bookkeeping surfaces in the live product tree
- no synthetic `.ll` or hand-authored artifact used as authoritative runtime proof

The live runtime docs describe the current executable surface only. Historical milestone freeze text is archived under `tmp/archive/`.

Integrated runtime architecture proof:

- runner:
  - `scripts/check_objc3c_runtime_architecture_proof_packet.py`
- public action:
  - `python scripts/objc3c_public_workflow_runner.py proof-runtime-architecture`
- integrated packet:
  - `tmp/reports/runtime/architecture-proof/summary.json`

Integrated runtime architecture validation:

- runner:
  - `scripts/check_objc3c_runtime_architecture_integration.py`
- public action:
  - `python scripts/objc3c_public_workflow_runner.py validate-runtime-architecture`
- integrated summary:
  - `tmp/reports/runtime/architecture-integration/summary.json`

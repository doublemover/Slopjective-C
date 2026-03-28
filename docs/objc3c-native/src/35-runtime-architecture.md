<!-- markdownlint-disable-file MD041 -->

## Runtime Execution Architecture (Current)

The live executable path is a single compile-to-runtime pipeline. Later runtime
closure work must extend this path, not bypass it.

## Working Boundary

- compiler-owned compile path:
  - `scripts/build_objc3c_native.ps1`
  - `scripts/objc3c_native_compile.ps1`
  - `native/objc3c/src/main.cpp`
  - `native/objc3c/src/driver/objc3_driver_main.cpp`
  - `native/objc3c/src/driver/objc3_compilation_driver.cpp`
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- runtime-owned installation and execution path:
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- authoritative emitted artifacts:
  - `<prefix>.obj`
  - `<prefix>.ll`
  - `<prefix>.manifest.json`
  - `<prefix>.runtime-registration-manifest.json`
  - `<prefix>.runtime-registration-descriptor.json`
  - `<prefix>.compile-provenance.json`
- validation-owned proof path:
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/check_objc3c_native_execution_smoke.ps1`
  - `scripts/objc3c_runtime_launch_contract.ps1`
  - `scripts/shared_compiler_runtime_acceptance_harness.py`

## Execution Flow

1. `scripts/build_objc3c_native.ps1` publishes `artifacts/bin/objc3c-native.exe`
   and `artifacts/lib/objc3_runtime.lib`.
2. `scripts/objc3c_native_compile.ps1` or the native executable drives the live
   `.objc3` compile path through driver, lowering, IR emission, and object
   emission.
3. The compile path emits the object, LLVM IR, registration manifest, and
   compile provenance as one coupled artifact set.
4. The runtime installs emitted image state through
   `objc3_runtime_register_image`, resolves selectors through
   `objc3_runtime_lookup_selector`, and executes calls through
   `objc3_runtime_dispatch_i32`.
5. Acceptance and replay proof only count when the emitted object, registration
   manifest, and linked probe all come from that same compile path.

## State Publication Surface

- front-door emitted surface:
  - `<prefix>.manifest.json`
  - key: `runtime_state_publication_surface`
- coupled runtime-owned surface:
  - `<prefix>.runtime-registration-manifest.json`
- required compile-owned artifacts:
  - `<prefix>.obj`
  - `<prefix>.ll`
- required public runtime ABI boundary:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_reset_for_testing`

The compile manifest is the authoritative front-door runtime state publication
surface. It must point at the coupled registration manifest, emitted object and
IR artifacts, the runtime archive path, the registration entrypoint, the runtime
state snapshot symbol, and the published descriptor counts.

## Bootstrap Registration Source Surface

- authoritative compile-manifest key:
  - `runtime_bootstrap_registration_source_surface`
- coupled emitted artifacts:
  - `<prefix>.manifest.json`
  - `<prefix>.runtime-registration-manifest.json`
  - `<prefix>.runtime-registration-descriptor.json`
  - `<prefix>.obj`
  - `<prefix>.ll`
- source inputs it composes:
  - `objc_runtime_registration_descriptor_image_root_source_surface`
  - `objc_runtime_registration_descriptor_frontend_closure`
  - `objc_runtime_translation_unit_registration_manifest`
  - `objc_runtime_bootstrap_lowering_contract`

This is the authoritative bootstrap registration source audit boundary. It
freezes the emitted registration descriptor artifact, image-root identity,
registration entrypoint, constructor root, translation-unit identity key, and
registration order ordinal as one coupled compile output instead of leaving
downstream work to reconstruct that source of truth from separate manifest
fragments.

## Bootstrap Lowering And Registration Artifact Surface

- authoritative compile-manifest key:
  - `runtime_bootstrap_lowering_registration_artifact_surface`
- authoritative composed source inputs:
  - `objc_runtime_bootstrap_lowering_contract`
  - `objc_runtime_translation_unit_registration_manifest`
  - `objc_runtime_startup_bootstrap_semantics`
  - `objc_runtime_registration_descriptor_frontend_closure`
- authoritative emitted artifacts:
  - `<prefix>.manifest.json`
  - `<prefix>.runtime-registration-manifest.json`
  - `<prefix>.runtime-registration-descriptor.json`
  - `<prefix>.obj`
  - `<prefix>.ll`
- authoritative lowered registration-descriptor fields:
  - `constructor_init_stub_symbol`
  - `bootstrap_registration_table_symbol`
  - `bootstrap_image_local_init_state_symbol`
  - `bootstrap_registration_table_layout_model`
  - `bootstrap_image_local_initialization_model`
  - `bootstrap_registration_table_abi_version`
  - `bootstrap_registration_table_pointer_field_count`
  - `translation_unit_registration_order_ordinal`
- authoritative lowered symbol fields:
  - `constructor_root_symbol`
  - `init_stub_symbol_prefix`
  - `registration_table_symbol_prefix`
  - `image_local_init_state_symbol_prefix`
  - `registration_entrypoint_symbol`
- authoritative loader-table freeze fields:
  - `registration_table_layout_model`
  - `registration_table_abi_version`
  - `registration_table_pointer_field_count`
- authoritative emission-state fields:
  - `constructor_root_emission_state`
  - `init_stub_emission_state`
  - `registration_table_emission_state`
  - `bootstrap_ir_materialization_landed`
  - `image_local_initialization_landed`

This is the authoritative bootstrap lowering and registration artifact
boundary. It freezes the lowered constructor-root/init-stub/registration-table
symbol family and loader-table ABI facts against the emitted manifest, object,
and IR artifacts instead of leaving later loader-table work to infer those
details from the semantic packet alone. The registration descriptor must now
carry the exact derived loader-table symbols/layout that appear in the emitted
LLVM IR, and acceptance must fail if the coupled `module.ll` stops lowering the
constructor-root to loader-table edge.

## Multi-Image Startup Ordering Source Surface

- authoritative compile-manifest key:
  - `runtime_multi_image_startup_ordering_source_surface`
- authoritative composed source inputs:
  - `objc_runtime_bootstrap_legality_semantics`
  - `objc_runtime_bootstrap_failure_restart_semantics`
  - `objc_runtime_bootstrap_api_contract`
  - `objc_runtime_bootstrap_reset_contract`
  - `objc_runtime_bootstrap_registrar_contract`
  - `objc_runtime_bootstrap_archive_static_link_replay_corpus`
- authoritative live proof path:
  - fixture: `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
  - probe: `tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp`
  - acceptance command: `python scripts/check_objc3c_runtime_acceptance.py`
  - public workflow command: `python scripts/objc3c_public_workflow_runner.py validate-runtime-architecture`
- authoritative runtime fields:
  - `last_rejected_module_name`
  - `last_rejected_translation_unit_identity_key`
  - `next_expected_registration_order_ordinal`
  - `last_successful_registration_order_ordinal`
  - `last_rejected_registration_order_ordinal`
- fail-closed diagnostic statuses:
  - duplicate translation-unit identity key: `-2`
  - out-of-order registration ordinal: `-3`
- diagnostic models:
  - duplicate install rejection publishes rejected module, identity key, and ordinal without advancing runtime state
  - out-of-order install rejection publishes rejected module, identity key, and ordinal without advancing runtime state

This is the authoritative multi-image startup ordering source boundary. It
freezes the duplicate-registration policy, monotonic registration-order model,
restart/replay symbols, duplicate/out-of-order rejection diagnostics, runtime
header paths, and the live installation lifecycle proof command surface as one
coupled artifact instead of leaving later bootstrap work to infer ordering
truth from scattered semantic and runtime-side reports.

## Object-Model Realization Source Surface

- authoritative compile-manifest key:
  - `runtime_object_model_realization_source_surface`
- authoritative composed source inputs:
  - `objc3c.executable.realization.records.v1`
  - `objc3c.runtime.class.realization.freeze.v1`
  - `objc3c.runtime.metaclass.graph.root.class.baseline.v1`
  - `objc3c.runtime.category.attachment.protocol.conformance.v1`
  - `objc3c.runtime.canonical.runnable.object.sample.support.v1`
- authoritative runtime boundary:
  - public ABI:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
  - private query boundary:
    - `objc3_runtime_copy_realized_class_graph_state_for_testing`
    - `objc3_runtime_copy_realized_class_entry_for_testing`
    - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3`
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3`
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`

This is the authoritative object-model realization source boundary. It freezes
the emitted class/metaclass/protocol/category realization contracts, coupled
registration artifacts, and the live runtime query/dispatch entrypoints that
later realization and reflection work must consume. Downstream work must extend
this emitted surface rather than rederiving realized graph truth from
sidecars, stale milestone notes, or synthetic probes.

## Block/ARC Unified Source Surface

- authoritative compile-manifest key:
  - `runtime_block_arc_unified_source_surface`
- authoritative composed source inputs:
  - `objc3c.executable.block.source.closure.v1`
  - `objc3c.executable.block.source.model.completion.v1`
  - `objc3c.executable.block.source.storage.annotation.v1`
  - `objc3c.executable.block.runtime.semantic.rules.v1`
  - `objc3c.executable.block.capture.legality.escape.and.invocation.v1`
  - `objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1`
  - `objc3c.executable.block.object.and.invoke.thunk.lowering.v1`
  - `objc3c.executable.block.byref.helper.lowering.v1`
  - `objc3c.executable.block.escape.runtime.hook.lowering.v1`
  - `objc3c.arc.source.mode.boundary.freeze.v1`
  - `objc3c.arc.mode.handling.v1`
  - `objc3c.arc.semantic.rules.v1`
  - `objc3c.arc.inference.lifetime.v1`
  - `objc3c.arc.interaction.semantics.v1`
- authoritative live code paths:
  - `native/objc3c/src/ast/objc3_ast.h`
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m261_block_source_model_completion_positive.objc3`
    - `tests/tooling/fixtures/native/m261_block_source_storage_annotations_positive.objc3`
    - `tests/tooling/fixtures/native/m261_capture_legality_escape_invocation_bad_call.objc3`
    - `tests/tooling/fixtures/native/m261_capture_legality_escape_invocation_missing_capture.objc3`
    - `tests/tooling/fixtures/native/m261_byref_cell_copy_dispose_runtime_positive.objc3`
    - `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_argument_positive.objc3`
    - `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_return_positive.objc3`
    - `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_byref_negative.objc3`
    - `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_owned_capture_negative.objc3`
    - `tests/tooling/fixtures/native/m261_executable_block_object_invoke_thunk_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_mode_handling_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_cleanup_scope_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_implicit_cleanup_void_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m261_d002_block_runtime_copy_dispose_invoke_probe.cpp`
    - `tests/tooling/runtime/m261_d003_block_runtime_byref_forwarding_probe.cpp`
    - `tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp`

This is the authoritative block/ARC unified source boundary. It freezes the
live frontend semantic packet, the emitted LLVM IR block-runtime summary
anchors, and the private runtime entrypoints used for escaping block promotion,
block invocation, and ARC debug snapshots as one coupled compile artifact.
Downstream block lowering, byref forwarding, ownership transfer, and ARC
automation work must extend this emitted surface instead of reconstructing
truth from sidecar-only notes, probe-local assumptions, or milestone-local
scaffolding.

## Ownership Transfer And Capture-Family Source Surface

- authoritative compile-manifest key:
  - `runtime_ownership_transfer_capture_family_source_surface`
- authoritative composed source inputs:
  - `frontend.pipeline.semantic_surface.objc_part8_resource_move_and_use_after_move_semantics`
  - `frontend.pipeline.semantic_surface.objc_part8_capture_list_and_retainable_family_legality_completion`
  - `objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1`
  - `objc3c.arc.inference.lifetime.v1`
  - `objc3c.arc.interaction.semantics.v1`
- authoritative live code paths:
  - `native/objc3c/src/ast/objc3_ast.h`
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m261_owned_object_capture_helper_positive.objc3`
    - `tests/tooling/fixtures/native/m261_owned_object_capture_runtime_positive.objc3`
    - `tests/tooling/fixtures/native/m261_nonowning_object_capture_helper_elided_positive.objc3`
    - `tests/tooling/fixtures/native/m261_nonowning_object_capture_runtime_positive.objc3`
    - `tests/tooling/fixtures/native/m261_weak_object_capture_mutation_negative.objc3`
    - `tests/tooling/fixtures/native/m261_unowned_object_capture_mutation_negative.objc3`
    - `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_owned_capture_negative.objc3`
    - `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_cleanup_scope_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_implicit_cleanup_void_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3`
    - `tests/tooling/fixtures/native/m271_b004_capture_list_and_retainable_family_legality_completion_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m261_d002_block_runtime_copy_dispose_invoke_probe.cpp`
    - `tests/tooling/runtime/m261_d003_block_runtime_byref_forwarding_probe.cpp`
    - `tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp`

This is the authoritative ownership-transfer and capture-family source
boundary. It freezes cleanup ownership transfer, explicit capture ownership
modes, retainable-family legality, and the normalized owned/weak/unowned block
capture profile before lowering or runtime lifetime expansion. Downstream
semantic, lowering, and runtime work must extend this emitted surface instead
of reinterpreting capture-family truth from ad hoc lowering state or
milestone-local notes.

## Block/ARC Lowering And Helper Surface

- authoritative compile-manifest key:
  - `runtime_block_arc_lowering_helper_surface`
- authoritative upstream contracts:
  - `objc3c.runtime.block.arc.unified.source.surface.v1`
  - `objc3c.runtime.ownership.transfer.capture.family.source.surface.v1`
  - `objc3c.executable.block.object.and.invoke.thunk.lowering.v1`
  - `objc3c.executable.block.byref.helper.lowering.v1`
  - `objc3c.executable.block.escape.runtime.hook.lowering.v1`
  - `objc3c.arc.mode.handling.v1`
  - `objc3c.arc.semantic.rules.v1`
  - `objc3c.arc.inference.lifetime.v1`
- authoritative live lowering packets:
  - semantic surfaces:
    - `frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface`
    - `frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface`
    - `frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface`
    - `frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface`
  - manifest replay summaries:
    - `lowering_block_abi_invoke_trampoline`
    - `lowering_block_storage_escape`
    - `lowering_block_copy_dispose`
  - LLVM IR summary anchors:
    - `llvm_ir_summary.executable_block_object_invoke_thunk_lowering`
    - `llvm_ir_summary.executable_block_byref_helper_lowering`
    - `llvm_ir_summary.executable_block_escape_runtime_hook_lowering`
    - `llvm_ir_summary.arc_cleanup_weak_lifetime_hooks`
    - `llvm_ir_summary.arc_block_autorelease_return_lowering`
  - private runtime hooks:
    - `runtime_api.objc3_runtime_promote_block_i32`
    - `runtime_api.objc3_runtime_invoke_block_i32`
    - `runtime_api.objc3_runtime_copy_arc_debug_state_for_testing`
- authoritative live code paths:
  - `native/objc3c/src/ast/objc3_ast.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m261_byref_cell_copy_dispose_runtime_positive.objc3`
    - `tests/tooling/fixtures/native/m261_owned_object_capture_runtime_positive.objc3`
    - `tests/tooling/fixtures/native/m261_nonowning_object_capture_runtime_positive.objc3`
    - `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_argument_positive.objc3`
    - `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_return_positive.objc3`
    - `tests/tooling/fixtures/native/m261_executable_block_object_invoke_thunk_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_mode_handling_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_cleanup_scope_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_implicit_cleanup_void_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m261_d002_block_runtime_copy_dispose_invoke_probe.cpp`
    - `tests/tooling/runtime/m261_d003_block_runtime_byref_forwarding_probe.cpp`
    - `tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp`

This is the authoritative lowering/helper boundary for `M281` lane C. It
freezes the live semantic lowering packets, manifest replay keys, emitted LLVM
helper summaries, and private runtime hook symbols that together describe the
current executable block/ARC lowering story. Cross-module preservation and
runtime-ABI widening work must extend this emitted surface instead of inferring
lane-C truth from one-off probes, sidecar notes, or milestone-local scaffolds.

## Cross-Module Block Ownership Artifact Preservation

- authoritative runtime-import-surface key:
  - `objc_runtime_block_ownership_artifact_preservation`
- authoritative cross-module link-plan keys:
  - `runtime_cross_module_block_ownership_artifact_preservation_surface_contract_id`
  - `block_ownership_cross_module_preservation_ready`
- authoritative source contracts:
  - `objc3c.runtime.block.ownership.artifact.preservation.v1`
  - `objc3c.runtime.block.arc.lowering.helper.surface.v1`
  - `objc3c.executable.block.object.and.invoke.thunk.lowering.v1`
  - `objc3c.executable.block.byref.helper.lowering.v1`
  - `objc3c.executable.block.escape.runtime.hook.lowering.v1`
  - `objc3c.runtime.support.library.link.wiring.v1`
- preserved cross-module facts:
  - local/imported/transitive block literal counts
  - invoke-thunk symbolized-site counts
  - copy/dispose helper required-site counts
  - copy/dispose helper symbolized-site counts
  - escape-to-heap site counts
  - byref-layout symbolized-site counts
  - runtime-support-library link-wiring readiness
  - deterministic replay key
- authoritative proof path:
  - fixtures:
    - `tests/tooling/fixtures/native/m261_byref_cell_copy_dispose_runtime_positive.objc3`
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3`
  - emitted artifacts:
    - `module.runtime-import-surface.json`
    - `module.cross-module-runtime-link-plan.json`

This is the authoritative cross-module block-ownership preservation boundary.
It freezes the fact that emitted runtime-import surfaces and the emitted
cross-module link plan, not milestone-local notes or ad hoc LLVM inspection,
carry the preserved invoke-thunk, byref-helper, copy/dispose, escape, and
runtime-link facts for imported block-heavy modules.

## Property/Ivar/Storage/Accessor Source Surface

- authoritative compile-manifest key:
  - `runtime_property_ivar_storage_accessor_source_surface`
- authoritative composed source inputs:
  - `objc3c.executable.property.ivar.source.closure.v1`
  - `objc3c.executable.property.ivar.source.model.completion.v1`
  - `objc3c.executable.property.ivar.semantics.v1`
- authoritative live code paths:
  - `native/objc3c/src/ast/objc3_ast.h`
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
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
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3`
    - `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp`
    - `tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp`
    - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`
    - `tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp`

This is the authoritative property/ivar/storage/accessor source boundary. It
freezes the AST/sema-approved binding, accessor, ownership, and ivar-layout
facts plus the exact live code paths that later storage, lowering, runtime,
and reflection work must use. Downstream work may serialize these fields into
emitted artifacts, but it must not invent storage semantics, accessor
selectors, or ownership behavior outside this emitted source surface. Public
runtime ABI widening and milestone-specific scaffolding are explicit
non-goals.

## Property Atomicity/Synthesis/Reflection Source Surface

- authoritative compile-manifest key:
  - `runtime_property_atomicity_synthesis_reflection_source_surface`
- authoritative composed source inputs:
  - `runtime_property_ivar_storage_accessor_source_surface`
  - `objc3c.runtime.property.metadata.reflection.v1`
  - `objc3c.runtime.backed.object.ownership.attribute.surface.v1`
- authoritative live code paths:
  - `native/objc3c/src/ast/objc3_ast.h`
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
- authoritative source fields:
  - `Objc3PropertyDecl.is_atomic`
  - `Objc3PropertyDecl.is_nonatomic`
  - `Objc3PropertyDecl.has_atomicity_conflict`
  - `Objc3PropertyDecl.property_attribute_profile`
  - `objc3_runtime_property_entry_snapshot.property_attribute_profile`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m257_property_atomic_ownership_negative.objc3`
    - `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
  - probes:
    - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`

This is the authoritative property atomicity/synthesis/reflection source
boundary. It freezes the live sema, pipeline, and runtime reflection path that
currently carries atomicity through `property_attribute_profile` and the
private property snapshot boundary. Atomic ownership-aware storage remains a
fail-closed semantic rule until later implementation issues land; widening the
public runtime ABI or inventing a parallel atomic helper surface are explicit
non-goals.

## Dispatch And Synthesized Accessor Lowering Surface

- authoritative compile-manifest key:
  - `dispatch_and_synthesized_accessor_lowering_surface`
- authoritative composed source inputs:
  - `runtime_property_ivar_storage_accessor_source_surface`
  - `objc3c.executable.property.accessor.layout.lowering.v1`
  - `objc3c.executable.ivar.layout.emission.v1`
  - `objc3c.executable.synthesized.accessor.property.lowering.v1`
  - `objc3c.runtime.storage.accessor.abi.surface.v1`
- coupled emitted artifacts:
  - `<emit-prefix>.obj`
  - `<emit-prefix>.ll`
  - `<emit-prefix>.manifest.json`
  - `<emit-prefix>.runtime-registration-manifest.json`
- authoritative lowering code paths:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
- frozen semantic models:
  - lowering metadata:
    - `runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path`
  - helper selection:
    - `plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_synthesis_default_ivar_binding_no_redeclaration.objc3`
    - `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp`
    - `tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp`
    - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`
    - `tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp`
- explicit non-goals:
  - `no-public-runtime-abi-widening`
  - `no-milestone-specific-scaffolding`
  - `no-sidecar-only-lowering-proof`

This is the authoritative accessor-storage lowering and metadata boundary. It
freezes the fact that the real compiler co-publishes dispatch/accessor helper
selection, synthesized-accessor counts, property/ivar descriptor counts, and
the coupled manifest/object/LLVM IR/runtime-registration artifacts on the live
compiler path. Downstream work must extend this emitted surface instead of
reconstructing lowering truth from sidecar-only notes, probe-local deductions,
or ad hoc IR inspection.

## Executable Property Accessor Layout Lowering Surface

- authoritative compile-manifest key:
  - `executable_property_accessor_layout_lowering_surface`
- authoritative composed source inputs:
  - `runtime_property_ivar_storage_accessor_source_surface`
  - `dispatch_and_synthesized_accessor_lowering_surface`
- coupled emitted artifacts:
  - `<emit-prefix>.obj`
  - `<emit-prefix>.ll`
  - `<emit-prefix>.manifest.json`
  - `<emit-prefix>.runtime-registration-manifest.json`
- authoritative lowering code paths:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- frozen semantic models:
  - property table:
    - `property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records`
  - ivar layout:
    - `ivar-descriptor-bundles-carry-sema-approved-layout-symbol-slot-size-alignment-records`
  - accessor binding:
    - `effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_synthesis_default_ivar_binding_no_redeclaration.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp`
    - `tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`
    - `tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp`
- explicit non-goals:
  - `no-public-runtime-abi-widening`
  - `no-milestone-specific-scaffolding`
  - `no-layout-or-accessor-body-rederivation-outside-the-live-lowering-path`

This is the authoritative executable property/accessor/layout lowering
boundary. It freezes the fact that the real compiler lowers sema-approved
property and ivar records into emitted descriptor inventories without inventing
body or layout truth outside the live lowering path.

## Executable Ivar Layout Emission Surface

- authoritative compile-manifest key:
  - `executable_ivar_layout_emission_surface`
- authoritative composed source inputs:
  - `executable_property_accessor_layout_lowering_surface`
- coupled emitted artifacts:
  - `<emit-prefix>.obj`
  - `<emit-prefix>.ll`
  - `<emit-prefix>.manifest.json`
  - `<emit-prefix>.runtime-registration-manifest.json`
- authoritative lowering code paths:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- frozen semantic models:
  - descriptor model:
    - `ivar-descriptor-records-carry-layout-symbol-offset-global-slot-offset-size-alignment`
  - offset globals:
    - `one-retained-i64-offset-global-per-emitted-ivar-binding`
  - layout tables:
    - `declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_source_model_completion_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
- explicit non-goals:
  - `no-public-runtime-abi-widening`
  - `no-milestone-specific-scaffolding`
  - `no-runtime-layout-rederivation`

This is the authoritative executable ivar-layout emission boundary. It freezes
the fact that offset globals and layout tables are emitted from the live
lowering path and are consumed as emitted artifacts rather than recomputed at
runtime or in sidecars.

## Executable Synthesized Accessor Property Lowering Surface

- authoritative compile-manifest key:
  - `executable_synthesized_accessor_property_lowering_surface`
- authoritative composed source inputs:
  - `executable_property_accessor_layout_lowering_surface`
  - `dispatch_and_synthesized_accessor_lowering_surface`
- coupled emitted artifacts:
  - `<emit-prefix>.obj`
  - `<emit-prefix>.ll`
  - `<emit-prefix>.manifest.json`
  - `<emit-prefix>.runtime-registration-manifest.json`
- authoritative lowering code paths:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
- frozen semantic models:
  - source model:
    - `implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists`
  - storage model:
    - `synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals`
  - property descriptor model:
    - `property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_synthesis_default_ivar_binding_no_redeclaration.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp`
    - `tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`
    - `tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp`
- explicit non-goals:
  - `no-public-runtime-abi-widening`
  - `no-milestone-specific-scaffolding`
  - `no-storage-global-fallbacks-or-sidecar-body-proof`

This is the authoritative synthesized-accessor body lowering boundary. It
freezes the fact that missing effective accessors become emitted method bodies
on the real compiler path and stay coupled to the emitted runtime-helper calls
and descriptor inventories rather than being inferred from source-only proof.

## Realization Lowering And Reflection Artifact Surface

- authoritative compile-manifest key:
  - `runtime_realization_lowering_reflection_artifact_surface`
- authoritative composed source inputs:
  - `runtime_object_model_realization_source_surface`
  - `objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1`
  - `objc3c.executable.realization.records.v1`
  - `objc3c.runtime.property.metadata.reflection.v1`
  - `objc3c.runtime.backed.object.ownership.attribute.surface.v1`
- coupled emitted artifacts:
  - `<emit-prefix>.manifest.json`
  - `<emit-prefix>.runtime-registration-manifest.json`
  - `<emit-prefix>.runtime-registration-descriptor.json`
  - `<emit-prefix>.obj`
  - `<emit-prefix>.ll`
  - `<emit-prefix>.compile-provenance.json`
- authoritative runtime boundary:
  - public ABI:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
  - private reflection artifact query boundary:
    - `objc3_runtime_copy_property_registry_state_for_testing`
    - `objc3_runtime_copy_property_entry_for_testing`
- frozen semantic models:
  - lowering artifact boundary:
    - `compile-manifest-registration-descriptor-object-and-llvm-ir-co-publish-realization-lowering-and-reflection-artifacts`
  - reflection artifact handoff:
    - `property-metadata-and-ownership-artifacts-remain-coupled-to-lowered-dispatch-accessor-and-executable-realization-record-outputs`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
  - probes:
    - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`

This is the authoritative realization-lowering and reflection-artifact
boundary. It freezes the fact that the real compiler co-publishes the manifest,
descriptor, object, LLVM IR, and provenance artifacts for lowered realization
records and reflection metadata, and that those artifacts stay coupled to the
same dispatch/accessor lowering path consumed by live executable probes.
Downstream work must extend this emitted surface instead of inferring lowering
truth from ad hoc IR inspection, source-only manifests, or milestone-local
notes.

## Dispatch-Table And Reflection-Record Lowering Surface

- authoritative compile-manifest key:
  - `runtime_dispatch_table_reflection_record_lowering_surface`
- authoritative composed source inputs:
  - `runtime_object_model_realization_source_surface`
  - `objc3c.runtime.state.publication.surface.v1`
  - `objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1`
  - `objc3c.method.dispatch.selector.thunk.lowering.v1`
  - `objc3c.executable.realization.records.v1`
- coupled emitted artifacts:
  - `<emit-prefix>.manifest.json`
  - `<emit-prefix>.runtime-registration-manifest.json`
  - `<emit-prefix>.runtime-registration-descriptor.json`
  - `<emit-prefix>.obj`
  - `<emit-prefix>.ll`
  - `<emit-prefix>.compile-provenance.json`
- authoritative lowering roots:
  - selector table root:
    - `@__objc3_sec_selector_pool`
  - reflection record roots:
    - `__objc3_sec_class_descriptors`
    - `__objc3_sec_protocol_descriptors`
    - `__objc3_sec_category_descriptors`
    - `__objc3_sec_property_descriptors`
    - `__objc3_sec_ivar_descriptors`
- frozen semantic models:
  - dispatch-table lowering:
    - `selector-pool-backed-dispatch-thunks-and-runtime-dispatch-sites-co-publish-stable-selector-table-roots-in-llvm-ir-and-manifest-artifacts`
  - reflection-record lowering:
    - `realization-records-and-runtime-metadata-section-aggregates-co-publish-class-protocol-category-property-and-ivar-record-roots-in-emitted-artifacts`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
  - probes:
    - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`

This is the authoritative dispatch-table and reflection-record lowering
boundary. It freezes the fact that the real compiler co-publishes selector-pool
roots, runtime dispatch-thunk lowering, executable realization records, and
metadata aggregate roots through the same manifest, object, and LLVM IR
artifacts consumed by live executable probes. Downstream work must extend this
emitted surface instead of rediscovering selector-table or reflection-record
truth from ad hoc IR inspection or milestone-local notes.

## Reflection Query Surface

- authoritative compile-manifest key:
  - `runtime_reflection_query_surface`
- authoritative composed source inputs:
  - `runtime_object_model_realization_source_surface`
  - `objc3c.runtime.dispatch_accessor.abi.surface.v1`
  - `objc3c.runtime.property.metadata.reflection.v1`
  - `objc3c.runtime.backed.object.ownership.attribute.surface.v1`
- authoritative query API boundary model:
  - `private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi`
- authoritative private query symbols:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_property_registry_state_for_testing`
  - `objc3_runtime_copy_property_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`

This is the authoritative reflection-query source and API boundary. It freezes
the fact that current reflection queries are runtime-backed, compile-coupled,
and exposed only through the private testing snapshots over realized
class/property/protocol metadata. Downstream work must extend this surface
instead of inventing a public reflection ABI or recovering reflection answers
from source-side manifests alone.

## Realization And Lookup Semantics Surface

- authoritative compile-manifest key:
  - `runtime_realization_lookup_semantics_surface`
- authoritative composed source inputs:
  - `runtime_object_model_realization_source_surface`
  - `runtime_reflection_query_surface`
  - `objc3c.runtime.dispatch_accessor.abi.surface.v1`
- authoritative runtime boundary:
  - public ABI:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
  - private lookup query boundary:
    - `objc3_runtime_copy_selector_lookup_table_state_for_testing`
    - `objc3_runtime_copy_selector_lookup_entry_for_testing`
    - `objc3_runtime_copy_method_cache_state_for_testing`
    - `objc3_runtime_copy_method_cache_entry_for_testing`
    - `objc3_runtime_copy_realized_class_entry_for_testing`
    - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- frozen semantic models:
  - lookup resolution order:
    - `seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-deterministic-fallback`
  - selector materialization:
    - `metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup`
  - unresolved selector behavior:
    - `negative-cache-entry-preserved-and-deterministic-fallback-returned`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3`
  - probes:
    - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`

This is the authoritative realization and lookup semantics boundary. It freezes
the live selector-table, method-cache, realized-class, category-attachment, and
protocol-conformance lookup rules against compile-coupled executable probes
instead of letting later runtime work infer lookup order from individual probe
payloads or stale planning notes.

## Class/Metaclass/Protocol Realization Surface

- authoritative compile-manifest key:
  - `runtime_class_metaclass_protocol_realization_surface`
- authoritative composed source inputs:
  - `runtime_object_model_realization_source_surface`
  - `runtime_reflection_query_surface`
  - `runtime_realization_lookup_semantics_surface`
- authoritative runtime boundary:
  - public ABI:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
  - private realization query boundary:
    - `objc3_runtime_copy_realized_class_graph_state_for_testing`
    - `objc3_runtime_copy_realized_class_entry_for_testing`
    - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- frozen semantic models:
  - class realization:
    - `registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection`
  - metaclass lineage:
    - `realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities`
  - protocol conformance:
    - `realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3`
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3`
    - `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
  - probes:
    - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`

This is the authoritative class/metaclass/protocol realization boundary. It
freezes the fact that live registration publishes runtime-backed class graphs,
stable metaclass lineage, and runtime-queryable direct/attached protocol
conformance through the same compile-coupled executable path used for dispatch
and reflection. Downstream work must extend this emitted surface instead of
recovering class lineage or protocol truth from ad hoc sidecars, stale
milestone notes, or synthetic summaries.

## Category Attachment And Merged Dispatch Surface

- authoritative compile-manifest key:
  - `runtime_category_attachment_merged_dispatch_surface`
- authoritative composed source inputs:
  - `objc3c.runtime.category.attachment.protocol.conformance.v1`
  - `runtime_realization_lookup_semantics_surface`
  - `runtime_class_metaclass_protocol_realization_surface`
- authoritative runtime boundary:
  - public ABI:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
  - private category query boundary:
    - `objc3_runtime_copy_realized_class_graph_state_for_testing`
    - `objc3_runtime_copy_realized_class_entry_for_testing`
    - `objc3_runtime_copy_protocol_conformance_query_for_testing`
    - `objc3_runtime_copy_method_cache_state_for_testing`
    - `objc3_runtime_copy_method_cache_entry_for_testing`
- frozen semantic models:
  - category attachment:
    - `registration-attaches-category-owned-instance-and-protocol-members-onto-live-realized-classes-before-dispatch`
  - merged dispatch resolution:
    - `attached-category-implementations-override-base-class-instance-lookup-before-superclass-and-protocol-fallback`
  - attached protocol visibility:
    - `attached-categories-publish-owner-and-name-through-realized-class-entries-and-protocol-conformance-queries`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3`
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3`
    - `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
  - probes:
    - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`

This is the authoritative category-attachment and merged-dispatch boundary. It
freezes the fact that live registration attaches category-owned methods and
protocol conformance before runtime dispatch, that attached implementations win
before superclass/protocol fallback, and that attached category owner/name
facts remain queryable through the same compile-coupled executable path used by
dispatch and reflection. Downstream work must extend this emitted surface
instead of reconstructing category merge truth from ad hoc sidecars or stale
planning notes.

## Reflection Visibility And Runtime Coherence Diagnostics Surface

- authoritative compile-manifest key:
  - `runtime_reflection_visibility_coherence_diagnostics_surface`
- authoritative composed source inputs:
  - `runtime_reflection_query_surface`
  - `runtime_category_attachment_merged_dispatch_surface`
  - `objc3c.runtime.dispatch_accessor.abi.surface.v1`
  - `objc3c.runtime.property.metadata.reflection.v1`
  - `objc3c.runtime.backed.object.ownership.attribute.surface.v1`
- authoritative runtime boundary:
  - public ABI:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
  - private coherence query boundary:
    - `objc3_runtime_copy_property_registry_state_for_testing`
    - `objc3_runtime_copy_property_entry_for_testing`
    - `objc3_runtime_copy_realized_class_entry_for_testing`
    - `objc3_runtime_copy_protocol_conformance_query_for_testing`
    - `objc3_runtime_copy_method_cache_state_for_testing`
- frozen semantic models:
  - reflection visibility boundary:
    - `private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state`
  - fail-closed lookup diagnostics:
    - `missing-class-and-property-lookups-publish-found-zero-without-mutating-property-registry-or-realized-class-state`
  - runtime coherence diagnostics:
    - `reflected-property-selectors-owner-identities-slot-layout-and-ownership-profiles-must-match-live-dispatch-realized-class-and-attached-protocol-state`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`

This is the authoritative reflection-visibility and runtime-coherence
diagnostics boundary. It freezes the fact that reflection visibility remains on
the private testing surface, that negative class/property queries fail closed
without mutating runtime state, and that reflected selector/owner/layout/
ownership metadata must remain coherent with live dispatch and attached-protocol
results. Downstream work must extend this emitted surface instead of
reconstructing coherence from source-only manifests, ad hoc probe payloads, or
milestone-local notes.

## Cross-Module Realized-Metadata Replay Preservation

- authoritative emitted artifact:
  - `module.cross-module-runtime-link-plan.json`
- machine-readable key:
  - `runtime_cross_module_realized_metadata_replay_preservation_surface`
- authoritative source contracts:
  - `objc3c.cross.module.runtime.packaging.link.plan.v1`
  - `objc3c.runtime.object.model.realization.source.surface.v1`
  - `objc3c.runtime.realization.lowering.reflection.artifact.surface.v1`
  - `objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1`
- emitted preservation facts:
  - imported and local descriptor counts:
    - class
    - protocol
    - category
    - property
    - ivar
    - total
  - transitive descriptor-count totals
  - translation-unit identity keys
  - registration ordinals
  - replay/reset readiness and bootstrap replay symbols
- authoritative proof path:
  - fixtures:
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3`
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3`
  - probe:
    - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`

This is the authoritative cross-module realized-metadata replay boundary. It
freezes the fact that the emitted cross-module link plan, not milestone-local
notes or ad hoc probe interpretation, carries the preserved descriptor-count,
identity, registration-order, and replay-readiness facts for imported and local
runtime images. Downstream work must consume that emitted artifact and its
reported surface instead of reconstructing cross-image replay truth from source
text or sidecar-only summaries.

## Object-Model Runtime ABI And Query Surface

- machine-readable key:
  - `runtime_object_model_abi_query_surface`
- authoritative source contracts:
  - `objc3c.runtime.object.model.realization.source.surface.v1`
  - `objc3c.runtime.realization.lowering.reflection.artifact.surface.v1`
  - `objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1`
  - `objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1`
  - `objc3c.runtime.reflection.query.surface.v1`
  - `objc3c.runtime.realization.lookup.semantics.v1`
  - `objc3c.runtime.class.metaclass.protocol.realization.v1`
  - `objc3c.runtime.category.attachment.merged.dispatch.surface.v1`
  - `objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1`
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
- authoritative proof path:
  - fixtures:
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3`
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3`
    - `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3`
    - `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`
    - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
    - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`

This is the authoritative object-model runtime ABI/query boundary. It freezes
the fact that the public runtime header stays at registration, selector lookup,
dispatch, and reset, while object-model lookup/reflection proof remains on the
private testing snapshot boundary used by the live runtime probes. Downstream
work must consume this emitted surface instead of widening the public ABI or
reconstructing query truth from milestone-local probe assumptions.

## Realization Lookup And Reflection Implementation

- machine-readable key:
  - `runtime_realization_lookup_reflection_implementation_surface`
- authoritative source contracts:
  - `objc3c.runtime.object.model.abi.query.surface.v1`
  - `objc3c.runtime.reflection.query.surface.v1`
  - `objc3c.runtime.realization.lookup.semantics.v1`
  - `objc3c.runtime.class.metaclass.protocol.realization.v1`
  - `objc3c.runtime.category.attachment.merged.dispatch.surface.v1`
  - `objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1`
- aggregate runtime query symbol:
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
- authoritative proof path:
  - fixture:
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
  - probes:
    - `tests/tooling/runtime/m259_d002_realization_lookup_reflection_runtime_probe.cpp`
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`

This is the authoritative live implementation boundary for realization lookup
and reflection. It freezes the fact that the runtime publishes one aggregate
object-model query snapshot over the same class/property/protocol/method-cache
state already exercised by the canonical object-model probes. Downstream work
must consume that aggregate query surface instead of reconstructing lookup and
reflection truth from disconnected probe-local reads.

## Installation ABI And Loader Lifecycle

- public installation ABI:
  - `objc3_runtime_register_image`
  - `objc3_runtime_copy_registration_state_for_testing`
  - `objc3_runtime_reset_for_testing`
- private loader lifecycle testing boundary:
  - `objc3_runtime_stage_registration_table_for_bootstrap`
  - `objc3_runtime_copy_image_walk_state_for_testing`
  - `objc3_runtime_replay_registered_images_for_testing`
  - `objc3_runtime_copy_reset_replay_state_for_testing`
- authoritative acceptance case:
  - `installation-lifecycle`
- fail-closed legality statuses:
  - duplicate translation-unit identity key: `-2`
  - out-of-order registration ordinal: `-3`
  - invalid registration roots / discovery root legality: `-4`
- machine-readable keys:
  - `runtime_installation_abi_surface`
  - `runtime_loader_lifecycle_surface`

The installation ABI is the stable front door for ingesting emitted runtime
images and reading installation state. Loader lifecycle proof remains on the
private testing boundary, where runtime acceptance verifies startup
installation, duplicate/out-of-order rejection without state advance, explicit
rejection of malformed staged registration roots and discovery roots without
state advance, reset retention of the bootstrap catalog, and deterministic
replay of registered images from the retained catalog.

## Acceptance Suite Surface

- authoritative suite:
  - `scripts/check_objc3c_runtime_acceptance.py`
- authoritative report:
  - `tmp/reports/runtime/acceptance/summary.json`
- machine-readable key:
  - `acceptance_suite_surface`

The acceptance suite surface defines which cases may claim published runtime
state. It is authoritative only when the suite consumes the emitted compile
manifest, the coupled registration manifest, and the linked runtime probe path
together. The composite public workflow report carries this same surface
forward when a composite action runs runtime acceptance.

## Shared Executable Acceptance Harness

- harness:
  - `scripts/shared_compiler_runtime_acceptance_harness.py`
- live executable suites:
  - `runtime-acceptance`
  - `public-test-fast`
  - `public-test-full`
- harness summary root:
  - `tmp/reports/runtime/shared-executable-acceptance-harness`

The shared executable acceptance harness is an integration runner over the live
runtime acceptance and public workflow entrypoints. It does not create a new
proof surface. Instead, it executes those suites, requires their emitted
reports to publish the runtime claim boundary, runtime state publication
surface, and acceptance suite surface contracts, and then writes a shared
summary that points back to those child executable reports.

## Integrated Proof Packet

- runner:
  - `scripts/check_objc3c_runtime_architecture_proof_packet.py`
- public action:
  - `python scripts/objc3c_public_workflow_runner.py proof-runtime-architecture`
- packet path:
  - `tmp/reports/runtime/architecture-proof/summary.json`

The integrated runtime architecture proof packet is a generic integration
artifact over the shared harness, the public workflow report, and the direct
runtime acceptance report. It only passes when all three agree on the runtime
claim boundary, runtime state publication surface, acceptance suite surface,
runtime installation ABI surface, and runtime loader lifecycle surface.

## Integrated Validation Path

- runner:
  - `scripts/check_objc3c_runtime_architecture_integration.py`
- public action:
  - `python scripts/objc3c_public_workflow_runner.py validate-runtime-architecture`
- summary path:
  - `tmp/reports/runtime/architecture-integration/summary.json`

The integrated runtime architecture validation path runs the shared harness over
`public-test-full`, then requires the resulting full public-workflow report to
stay aligned with the runtime architecture proof packet and the direct runtime
acceptance report. It fails closed if the full workflow drops the smoke,
runtime-acceptance, or replay child steps, or if any published runtime
architecture surface drifts between the full workflow and the proof packet.

## Claim Boundary

- runnable:
  - behavior proven by real emitted objects linked against
    `artifacts/lib/objc3_runtime.lib`
  - runtime state derived from emitted registration tables and descriptors
  - probe observations produced through the public runtime entrypoints or
    compile-coupled manifests
- claim-only until later closure work lands:
  - any surface described only by comments, sidecars, or private placeholders
  - synthetic `.ll` or hand-authored artifacts with no matching compile output
  - proof that depends on compatibility shims without a coupled emitted object
  - future runtime capability that would require widening
    `native/objc3c/src/runtime/objc3_runtime.h`

## Explicit Non-Goals

- no milestone-specific compile wrappers, proof packets, or closeout sidecars
- no parallel dispatch or installation ABI outside the current runtime header
- no authoritative proof from replay text alone without emitted object and probe
- no widening of public runtime claims beyond what the live acceptance and probe
  path can execute today

The live runtime-acceptance, replay-proof, and composite public-workflow
reports publish this same claim boundary as machine-readable JSON so later work
cannot silently overclaim from sidecars or synthetic artifacts.

The runtime-owned subsystem dependency model is anchored in
`native/objc3c/src/runtime/ARCHITECTURE.md` and enforced by
`python scripts/check_objc3c_dependency_boundaries.py --strict`.

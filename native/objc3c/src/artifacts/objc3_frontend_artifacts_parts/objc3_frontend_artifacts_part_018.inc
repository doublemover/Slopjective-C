           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"realized_class_graph_snapshot_symbol\":\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_property_ivar_storage_accessor_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"property_ast_anchor\":\""
           << kObjc3RuntimeMetadataPropertyAstAnchor
           << "\",\"ivar_ast_anchor\":\""
           << kObjc3RuntimeMetadataIvarAstAnchor
           << "\",\"source_closure_contract_id\":\""
           << kObjc3ExecutablePropertyIvarSourceClosureContractId
           << "\",\"source_model_completion_contract_id\":\""
           << kObjc3ExecutablePropertyIvarSourceModelCompletionContractId
           << "\",\"source_semantics_contract_id\":\""
           << kObjc3ExecutablePropertyIvarSemanticsContractId
           << "\",\"source_surface_model\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceModel
           << "\",\"layout_model\":\""
           << kObjc3ExecutablePropertyIvarLayoutModel
           << "\",\"attribute_model\":\""
           << kObjc3ExecutablePropertyAttributeModel
           << "\",\"synthesis_semantics_model\":\""
           << kObjc3ExecutablePropertySynthesisSemanticsModel
           << "\",\"default_ivar_binding_resolution_model\":\""
           << kObjc3ExecutablePropertyDefaultIvarBindingResolutionModel
           << "\",\"accessor_semantics_model\":\""
           << kObjc3ExecutablePropertyAccessorSemanticsModel
           << "\",\"accessor_selector_uniqueness_model\":\""
           << kObjc3ExecutablePropertyAccessorSelectorUniquenessModel
           << "\",\"ownership_atomicity_interaction_model\":\""
           << kObjc3ExecutablePropertyOwnershipAtomicityInteractionModel
           << "\",\"storage_semantics_model\":\""
           << kObjc3ExecutablePropertyStorageSemanticsModel
           << "\",\"layout_init_order_field\":\"Objc3PropertyDecl.executable_ivar_init_order_index\""
           << ",\"layout_destroy_order_field\":\"Objc3PropertyDecl.executable_ivar_destroy_order_index\""
           << ",\"synthesizes_accessors_field\":\"Objc3RuntimeMetadataPropertySourceRecord.synthesizes_executable_accessors\""
           << ",\"getter_runtime_helper_field\":\"Objc3RuntimeMetadataPropertySourceRecord.getter_storage_runtime_helper_symbol\""
           << ",\"setter_runtime_helper_field\":\"Objc3RuntimeMetadataPropertySourceRecord.setter_storage_runtime_helper_symbol\""
           << ",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"executable_property_accessor_layout_lowering_contract_id\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"executable_ivar_layout_emission_contract_id\":\""
           << kObjc3ExecutableIvarLayoutEmissionContractId
           << "\",\"executable_synthesized_accessor_property_lowering_contract_id\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\""
           << ",\"accessor_storage_lowering_metadata_model\":\""
           << kObjc3AccessorStorageLoweringMetadataModel
           << "\",\"accessor_storage_lowering_helper_selection_model\":\""
           << kObjc3AccessorStorageLoweringHelperSelectionModel
           << "\",\"compatibility_semantics_model\":\""
           << kObjc3ExecutablePropertyCompatibilitySemanticsModel
           << "\",\"ast_source_path\":\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"sema_source_path\":\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"frontend_pipeline_source_path\":\"native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/synthesized_accessor_probe.cpp\""
           << ",\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-lowering-owned-storage-or-accessor-semantics-invention\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_block_arc_unified_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_surface_model\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceModel
           << "\",\"source_contract_ids\":[\""
           << Expr::kObjc3ExecutableBlockSourceClosureContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockSourceModelCompletionContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockRuntimeSemanticRulesContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockCaptureLegalityImplementationContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
           << "\",\""
           << Expr::kObjc3ArcSourceModeBoundaryContractId
           << "\",\""
           << Expr::kObjc3ArcModeHandlingContractId
           << "\",\""
           << Expr::kObjc3ArcSemanticRulesContractId
           << "\",\""
           << Expr::kObjc3ArcInferenceLifetimeContractId
           << "\",\""
           << Expr::kObjc3ArcInteractionSemanticsContractId
           << "\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_source_fields\":[\"frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_literal_capture_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_source_model_completion_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_source_storage_annotation_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface\""
           << ",\"llvm_ir_summary.executable_block_object_invoke_thunk_lowering\""
           << ",\"llvm_ir_summary.executable_block_byref_helper_lowering\""
           << ",\"llvm_ir_summary.executable_block_escape_runtime_hook_lowering\""
           << ",\"llvm_ir_summary.runtime_block_api_object_layout\""
           << ",\"llvm_ir_summary.runtime_block_allocation_copy_dispose_invoke_support\""
           << ",\"llvm_ir_summary.runtime_block_byref_forwarding_heap_promotion_ownership_interop\""
           << ",\"runtime_api.objc3_runtime_promote_block_i32\""
           << ",\"runtime_api.objc3_runtime_invoke_block_i32\""
           << ",\"runtime_api.objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\"]"
           << ",\"block_runtime_boundary_model\":\""
           << Expr::kObjc3ExecutableBlockByrefMutationOwnershipModel
           << "\""
           << ",\"arc_runtime_boundary_model\":\""
           << Expr::kObjc3ArcInteractionSemanticsSemanticModel
           << "\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/block_source_model_completion_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/block_source_storage_annotations_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/capture_legality_escape_invocation_bad_call.objc3\""
           << ",\"tests/tooling/fixtures/native/capture_legality_escape_invocation_missing_capture.objc3\""
           << ",\"tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_mode_handling_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp\""
           << ",\"tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\""
           << ",\"tests/tooling/runtime/block_arc_runtime_abi_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-block-object-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-sidecar-only-block-or-arc-proof\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_ownership_transfer_capture_family_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"block_arc_unified_source_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"source_surface_model\":\""
           << kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceModel
           << "\",\"ownership_resource_move_use_after_move_surface_path\":\"frontend.pipeline.semantic_surface.objc_ownership_resource_move_and_use_after_move_semantics\""
           << ",\"ownership_capture_list_retainable_family_surface_path\":\"frontend.pipeline.semantic_surface.objc_ownership_capture_list_and_retainable_family_legality_completion\""
           << ",\"block_capture_ownership_contract_id\":\""
           << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
           << "\",\"arc_inference_lifetime_contract_id\":\""
           << Expr::kObjc3ArcInferenceLifetimeContractId
           << "\",\"arc_interaction_semantics_contract_id\":\""
           << Expr::kObjc3ArcInteractionSemanticsContractId
           << "\",\"block_capture_ownership_profile_field\":\"Expr.block_runtime_capture_ownership_profile\""
           << ",\"block_capture_owned_count_field\":\"Expr.block_runtime_owned_object_capture_count\""
           << ",\"block_capture_weak_count_field\":\"Expr.block_runtime_weak_object_capture_count\""
           << ",\"block_capture_unowned_count_field\":\"Expr.block_runtime_unowned_object_capture_count\""
           << ",\"cleanup_ownership_transfer_field\":\"cleanup_ownership_transfer_enforced\""
           << ",\"explicit_capture_ownership_mode_field\":\"explicit_capture_ownership_mode_enforced\""
           << ",\"retainable_family_conflict_field\":\"retainable_family_conflict_enforced\""
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/nonowning_object_capture_helper_elided_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/weak_object_capture_mutation_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/unowned_object_capture_mutation_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp\""
           << ",\"tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-parallel-semantics-path\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-lowering-owned-reinterpretation-of-capture-family-truth\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_block_arc_lowering_helper_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"block_arc_unified_source_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"runtime_block_arc_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiSurfaceContractId
           << "\",\"ownership_transfer_capture_family_source_surface_contract_id\":\""
           << kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceContractId
           << "\",\"block_object_invoke_thunk_lowering_contract_id\":\""
           << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
           << "\",\"block_byref_helper_lowering_contract_id\":\""
           << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
           << "\",\"block_escape_runtime_hook_lowering_contract_id\":\""
           << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
           << "\",\"arc_mode_handling_contract_id\":\""
           << Expr::kObjc3ArcModeHandlingContractId
           << "\",\"arc_semantic_rules_contract_id\":\""
           << Expr::kObjc3ArcSemanticRulesContractId
           << "\",\"arc_inference_lifetime_contract_id\":\""
           << Expr::kObjc3ArcInferenceLifetimeContractId
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"lowering_helper_surface_model\":\""
           << kObjc3RuntimeBlockArcLoweringHelperSurfaceModel
           << "\",\"semantic_surface_paths\":[\"frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface\"]"
           << ",\"manifest_lowering_paths\":[\"lowering_block_abi_invoke_trampoline\""
           << ",\"lowering_block_storage_escape\""
           << ",\"lowering_block_copy_dispose\"]"
           << ",\"llvm_ir_summary_paths\":[\"llvm_ir_summary.executable_block_object_invoke_thunk_lowering\""
           << ",\"llvm_ir_summary.executable_block_byref_helper_lowering\""
           << ",\"llvm_ir_summary.executable_block_escape_runtime_hook_lowering\""
           << ",\"llvm_ir_summary.arc_cleanup_weak_lifetime_hooks\""
           << ",\"llvm_ir_summary.arc_block_autorelease_return_lowering\"]"
           << ",\"runtime_api_paths\":[\"runtime_api.objc3_runtime_promote_block_i32\""
           << ",\"runtime_api.objc3_runtime_invoke_block_i32\""
           << ",\"runtime_api.objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_mode_handling_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp\""
           << ",\"tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\""
           << ",\"tests/tooling/runtime/block_arc_runtime_abi_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-cross-module-packaging-claims\""
           << ",\"no-public-block-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_block_arc_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"block_arc_unified_source_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"block_arc_lowering_helper_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_block_arc_runtime_abi_boundary\":[\""
           << kObjc3RuntimePromoteBlockI32Symbol << "\",\""
           << kObjc3RuntimeInvokeBlockI32Symbol << "\",\""
           << kObjc3RuntimeRetainI32Symbol << "\",\""
           << kObjc3RuntimeReleaseI32Symbol << "\",\""
           << kObjc3RuntimeAutoreleaseI32Symbol << "\",\""
           << kObjc3RuntimePushAutoreleasepoolScopeSymbol << "\",\""
           << kObjc3RuntimePopAutoreleasepoolScopeSymbol << "\",\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol << "\",\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol << "\",\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol << "\",\""
           << "objc3_runtime_bind_current_property_context_for_testing"
           << "\",\"objc3_runtime_clear_current_property_context_for_testing"
           << "\",\"" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol << "\",\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol << "\",\""
           << "objc3_runtime_copy_arc_debug_state_for_testing"
           << "\",\"objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\"]"
           << ",\"block_arc_runtime_abi_snapshot_symbol\":\"objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\""
           << ",\"arc_debug_state_snapshot_symbol\":\"objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"runtime_abi_boundary_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel
           << "\",\"block_runtime_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiBlockModel
           << "\",\"arc_runtime_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiArcModel
           << "\",\"fail_closed_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiFailClosedModel
           << "\",\"authoritative_probe_path\":\"tests/tooling/runtime/block_arc_runtime_abi_probe.cpp\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_property_ivar_accessor_reflection_implementation_surface\":{\"contract_id\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"implementation_snapshot_symbol\":\"objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"bind_current_property_context_symbol\":\"objc3_runtime_bind_current_property_context_for_testing\""
           << ",\"clear_current_property_context_symbol\":\"objc3_runtime_clear_current_property_context_for_testing\""
           << ",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"implementation_model\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationModel
           << "\",\"reflection_model\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationReflectionModel
           << "\",\"fail_closed_model\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationFailClosedModel
           << "\",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"executable_property_accessor_layout_lowering_surface\":{\"contract_id\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"property_table_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringPropertyTableModel
           << "\",\"ivar_layout_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringIvarLayoutModel
           << "\",\"accessor_binding_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringAccessorBindingModel
           << "\",\"scope_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringScopeModel
           << "\",\"fail_closed_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringFailClosedModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/synthesized_accessor_probe.cpp\""
           << ",\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-layout-or-accessor-body-rederivation-outside-the-live-lowering-path\"]"
           << ",\"property_metadata_entries\":"
           << executable_accessor_layout_lowering_summary.property_metadata_entries
           << ",\"ivar_metadata_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_metadata_entries
           << ",\"property_descriptor_entries\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"ivar_descriptor_entries\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"property_attribute_profile_entries\":"
           << executable_accessor_layout_lowering_summary
                  .property_attribute_profile_entries
           << ",\"accessor_ownership_profile_entries\":"
           << executable_accessor_layout_lowering_summary
                  .accessor_ownership_profile_entries
           << ",\"synthesized_binding_entries\":"
           << executable_accessor_layout_lowering_summary
                  .synthesized_binding_entries
           << ",\"ivar_layout_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_entries
           << ",\"ivar_layout_owner_entries\":"
           << executable_accessor_layout_lowering_summary
                  .ivar_layout_owner_entries
           << ",\"descriptor_counts_match_source_graph\":"
           << ((executable_accessor_layout_lowering_summary.property_metadata_entries ==
                        runtime_metadata_section_publication.property_descriptor_count &&
                executable_accessor_layout_lowering_summary.ivar_metadata_entries ==
                        runtime_metadata_section_publication.ivar_descriptor_count)
                   ? "true"
                   : "false")
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"executable_ivar_layout_emission_surface\":{\"contract_id\":\""
           << kObjc3ExecutableIvarLayoutEmissionContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"executable_property_accessor_layout_lowering_surface_contract_id\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"descriptor_model\":\""
           << kObjc3ExecutableIvarLayoutDescriptorModel
           << "\",\"offset_global_model\":\""
           << kObjc3ExecutableIvarOffsetGlobalModel
           << "\",\"layout_table_model\":\""
           << kObjc3ExecutableIvarLayoutTableModel
           << "\",\"scope_model\":\""
           << kObjc3ExecutableIvarLayoutEmissionScopeModel
           << "\",\"fail_closed_model\":\""
           << kObjc3ExecutableIvarLayoutEmissionFailClosedModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-runtime-layout-rederivation\"]"
           << ",\"offset_global_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_entries
           << ",\"layout_table_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_owner_entries
           << ",\"layout_owner_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_owner_entries
           << ",\"ivar_descriptor_entries\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"executable_synthesized_accessor_property_lowering_surface\":{\"contract_id\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"executable_property_accessor_layout_lowering_surface_contract_id\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"source_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel
           << "\",\"storage_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel
           << "\",\"property_descriptor_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel
           << "\",\"fail_closed_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringFailClosedModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/synthesized_accessor_probe.cpp\""
           << ",\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-storage-global-fallbacks-or-sidecar-body-proof\"]"
           << ",\"implementation_owned_property_entries\":"
           << executable_accessor_layout_lowering_summary
                  .implementation_owned_property_entries
           << ",\"synthesized_getter_entries\":"
           << executable_accessor_layout_lowering_summary.synthesized_getter_entries
           << ",\"synthesized_setter_entries\":"
           << executable_accessor_layout_lowering_summary.synthesized_setter_entries
           << ",\"synthesized_accessor_entries\":"
           << executable_accessor_layout_lowering_summary
                  .synthesized_accessor_entries
           << ",\"property_descriptor_entries\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_property_atomicity_synthesis_reflection_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimePropertyAtomicitySynthesisReflectionSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"property_storage_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"atomic_modifier_field\":\"Objc3PropertyDecl.is_atomic\""
           << ",\"nonatomic_modifier_field\":\"Objc3PropertyDecl.is_nonatomic\""
           << ",\"atomicity_conflict_field\":\"Objc3PropertyDecl.has_atomicity_conflict\""
           << ",\"property_attribute_profile_field\":\"Objc3PropertyDecl.property_attribute_profile\""
           << ",\"reflection_attribute_profile_field\":\"objc3_runtime_property_entry_snapshot.property_attribute_profile\""
           << ",\"ast_source_path\":\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"sema_source_path\":\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"sema_pass_manager_source_path\":\"native/objc3c/src/sema/objc3_sema_pass_manager.cpp\""
           << ",\"frontend_pipeline_source_path\":\"native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_internal_header_path\":\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"source_surface_model\":\""
           << kObjc3RuntimePropertyAtomicitySynthesisReflectionSourceSurfaceModel
           << "\",\"atomicity_fail_closed_model\":\""
           << kObjc3ExecutablePropertyOwnershipAtomicityInteractionModel
           << "\",\"reflection_boundary_model\":\""
           << kObjc3RuntimePropertyAtomicityReflectionBoundaryModel
           << "\",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/property_atomic_ownership_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-atomic-property-runtime-abi-widening\""
           << ",\"no-runtime-managed-atomic-storage-semantics-before-lane-b-and-lane-d-implementation\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_realization_lowering_reflection_artifact_surface\":{\"contract_id\":\""
           << kObjc3RuntimeRealizationLoweringReflectionArtifactSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\"objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1\""
           << ",\"executable_realization_records_contract_id\":\""
           << kObjc3ExecutableRealizationRecordsContractId
           << "\",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"lowering_artifact_boundary_model\":\"compile-manifest-registration-descriptor-object-and-llvm-ir-co-publish-realization-lowering-and-reflection-artifacts\""
           << ",\"reflection_artifact_handoff_model\":\"property-metadata-and-ownership-artifacts-remain-coupled-to-lowered-dispatch-accessor-and-executable-realization-record-outputs\""
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_compile_output_truthfulness\":true"
           << "},\n";
  manifest << "  \"runtime_dispatch_table_reflection_record_lowering_surface\":{\"contract_id\":\""
           << kObjc3RuntimeDispatchTableReflectionRecordLoweringSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_state_publication_surface_contract_id\":\""
           << kObjc3RuntimeStatePublicationSurfaceContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\"objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1\""
           << ",\"method_dispatch_and_selector_thunk_lowering_contract_id\":\"objc3c.method.dispatch.selector.thunk.lowering.v1\""
           << ",\"executable_realization_records_contract_id\":\""
           << kObjc3ExecutableRealizationRecordsContractId
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"selector_pool_section_root_symbol\":\"@__objc3_sec_selector_pool\""
           << ",\"class_aggregate_symbol\":\""
           << runtime_metadata_section_publication.class_aggregate_symbol
           << "\",\"protocol_aggregate_symbol\":\""
           << runtime_metadata_section_publication.protocol_aggregate_symbol
           << "\",\"category_aggregate_symbol\":\""
           << runtime_metadata_section_publication.category_aggregate_symbol
           << "\",\"property_aggregate_symbol\":\""
           << runtime_metadata_section_publication.property_aggregate_symbol
           << "\",\"ivar_aggregate_symbol\":\""
           << runtime_metadata_section_publication.ivar_aggregate_symbol
           << "\",\"message_send_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"class_descriptor_count\":"
           << runtime_metadata_section_publication.class_descriptor_count
           << ",\"protocol_descriptor_count\":"
           << runtime_metadata_section_publication.protocol_descriptor_count
           << ",\"category_descriptor_count\":"
           << runtime_metadata_section_publication.category_descriptor_count
           << ",\"property_descriptor_count\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"ivar_descriptor_count\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"dispatch_table_lowering_model\":\"selector-pool-backed-dispatch-thunks-and-runtime-dispatch-sites-co-publish-stable-selector-table-roots-in-llvm-ir-and-manifest-artifacts\""
           << ",\"reflection_record_lowering_model\":\"realization-records-and-runtime-metadata-section-aggregates-co-publish-class-protocol-category-property-and-ivar-record-roots-in-emitted-artifacts\""
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_compile_output_truthfulness\":true"
           << "},\n";
  manifest << "  \"runtime_object_model_abi_query_surface\":{\"contract_id\":\""
           << kObjc3RuntimeObjectModelAbiQuerySurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_realization_lowering_reflection_artifact_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLoweringReflectionArtifactSurfaceContractId
           << "\",\"runtime_dispatch_table_reflection_record_lowering_surface_contract_id\":\""
           << kObjc3RuntimeDispatchTableReflectionRecordLoweringSurfaceContractId
           << "\",\"runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id\":\""
           << kObjc3RuntimeCrossModuleRealizedMetadataReplayPreservationSurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"runtime_class_metaclass_protocol_realization_surface_contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"runtime_reflection_visibility_coherence_diagnostics_surface_contract_id\":\""
           << kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol
           << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol
           << "\"]"
           << ",\"private_object_model_query_boundary\":[\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"objc3_runtime_copy_selector_lookup_table_state_for_testing\""
           << ",\"objc3_runtime_copy_selector_lookup_entry_for_testing\""
           << ",\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"objc3_runtime_copy_dispatch_state_for_testing\"]"
           << ",\"object_model_query_boundary_model\":\""
           << kObjc3RuntimeObjectModelQueryBoundaryModel
           << "\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_unified_concurrency_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_contract_ids\":[\"objc3c.concurrency.async.source.closure.v1\""
           << ",\"objc3c.concurrency.actor.member.isolation.source.closure.v1\""
           << ",\"objc3c.concurrency.task.group.cancellation.source.closure.v1\""
           << ",\"objc3c.concurrency.async.effect.suspension.semantic.model.v1\""
           << ",\"objc3c.concurrency.task.executor.cancellation.semantic.model.v1\""
           << ",\"objc3c.concurrency.actor.isolation.sendable.semantic.model.v1\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_source_fields\":[\"frontend.pipeline.semantic_surface.objc_concurrency_async_source_closure\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_member_and_isolation_source_closure\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_cancellation_source_closure\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model\"]"
           << ",\"source_surface_model\":\"unified-concurrency-source-surface-freezes-live-async-actor-task-source-and-sema-boundaries-before-lowering-runtime-and-public-abi-expansion\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_concurrency_runtime_boundary\":[\"objc3_runtime_allocate_async_continuation_i32\""
           << ",\"objc3_runtime_handoff_async_continuation_to_executor_i32\""
           << ",\"objc3_runtime_resume_async_continuation_i32\""
           << ",\"objc3_runtime_spawn_task_i32\""
           << ",\"objc3_runtime_enter_task_group_scope_i32\""
           << ",\"objc3_runtime_add_task_group_task_i32\""
           << ",\"objc3_runtime_wait_task_group_next_i32\""
           << ",\"objc3_runtime_cancel_task_group_i32\""
           << ",\"objc3_runtime_task_is_cancelled_i32\""
           << ",\"objc3_runtime_task_on_cancel_i32\""
           << ",\"objc3_runtime_actor_enter_isolation_thunk_i32\""
           << ",\"objc3_runtime_actor_enter_nonisolated_i32\""
           << ",\"objc3_runtime_actor_hop_to_executor_i32\""
           << ",\"objc3_runtime_actor_record_replay_proof_i32\""
           << ",\"objc3_runtime_actor_record_race_guard_i32\""
           << ",\"objc3_runtime_actor_bind_executor_i32\""
           << ",\"objc3_runtime_actor_mailbox_enqueue_i32\""
           << ",\"objc3_runtime_actor_mailbox_drain_next_i32\""
           << ",\"objc3_runtime_copy_async_continuation_state_for_testing\""
           << ",\"objc3_runtime_copy_task_runtime_state_for_testing\""
           << ",\"objc3_runtime_copy_actor_runtime_state_for_testing\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/async_await_executor_source_closure_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/actor_member_isolation_surface_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/task_executor_cancellation_source_closure_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_lowering_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_lowering_runtime_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-sidecar-only-concurrency-proof\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_async_task_actor_normalization_completion_surface\":{\"contract_id\":\""
           << kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"normalized_semantic_contract_ids\":[\"objc3c.concurrency.async.effect.suspension.semantic.model.v1\""
           << ",\"objc3c.concurrency.task.executor.cancellation.semantic.model.v1\""
           << ",\"objc3c.concurrency.actor.isolation.sendable.semantic.model.v1\"]"
           << ",\"lowering_contract_ids\":[\"objc3c.concurrency.continuation.abi.async.lowering.contract.v1\""
           << ",\"objc3c.concurrency.task.runtime.lowering.contract.v1\""
           << ",\"objc3c.concurrency.actor.lowering.and.metadata.contract.v1\"]"
           << ",\"lowering_lane_contract_ids\":[\"objc3c.async.continuation.lowering.v1\""
           << ",\"objc3c.await.lowering.suspension.state.lowering.v1\""
           << ",\"objc3c.task.runtime.interop.cancellation.lowering.v1\""
           << ",\"objc3c.concurrency.replay.race.guard.lowering.v1\""
           << ",\"objc3c.actor.lowering.metadata.contract.v1\""
           << ",\"objc3c.actor.isolation.sendability.lowering.v1\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_surface_fields\":[\"frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract\"]"
           << ",\"normalization_completion_model\":\"normalized-async-task-actor-sema-and-lowering-packets-freeze-the-live-boundary-before-runtime-abi-and-runnable-execution-closure\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/async_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/actor_isolation_sendable_semantic_model_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/task_executor_cancellation_semantic_model_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_lowering_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_lowering_runtime_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-runnable-task-or-actor-execution-claim\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_unified_concurrency_lowering_metadata_surface\":{\"contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencyLoweringMetadataSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"normalization_completion_surface_contract_id\":\""
           << kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId
           << "\",\"lowering_contract_ids\":[\"objc3c.concurrency.continuation.abi.async.lowering.contract.v1\""
           << ",\"objc3c.concurrency.task.runtime.lowering.contract.v1\""
           << ",\"objc3c.concurrency.actor.lowering.and.metadata.contract.v1\"]"
           << ",\"lowering_detail_contract_ids\":[\"objc3c.concurrency.async.direct.call.lowering.v1\""
           << ",\"objc3c.concurrency.task.runtime.abi.completion.v1\""
           << ",\"objc3c.concurrency.actor.isolation.sendability.enforcement.v1\"]"
           << ",\"lowering_lane_contract_ids\":[\"objc3c.async.continuation.lowering.v1\""
           << ",\"objc3c.await.lowering.suspension.state.lowering.v1\""
           << ",\"objc3c.task.runtime.interop.cancellation.lowering.v1\""
           << ",\"objc3c.concurrency.replay.race.guard.lowering.v1\""
           << ",\"objc3c.actor.lowering.metadata.contract.v1\""
           << ",\"objc3c.actor.isolation.sendability.lowering.v1\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\"]"
           << ",\"authoritative_surface_fields\":[\"frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_async_function_await_and_continuation_lowering\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_runtime_abi_completion\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendability_enforcement\"]"
           << ",\"lowering_metadata_surface_model\":\"unified-concurrency-lowering-and-metadata-surface-freezes-live-async-task-actor-lowering-packets-and-emitted-metadata-boundaries-before-runtime-abi-and-runnable-execution-closure\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/async_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/task_runtime_async_entry_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/actor_lowering_metadata_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_lowering_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_lowering_runtime_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-runnable-task-or-actor-execution-claim\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_unified_concurrency_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencyRuntimeAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"unified_concurrency_source_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"async_task_actor_normalization_completion_surface_contract_id\":\""
           << kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId
           << "\",\"unified_concurrency_lowering_metadata_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencyLoweringMetadataSurfaceContractId
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_unified_concurrency_runtime_abi_boundary\":[\""
           << kObjc3RuntimeAllocateAsyncContinuationI32Symbol << "\",\""
           << kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol << "\",\""
           << kObjc3RuntimeResumeAsyncContinuationI32Symbol << "\",\""
           << kObjc3RuntimeSpawnTaskI32Symbol << "\",\""
           << kObjc3RuntimeEnterTaskGroupScopeI32Symbol << "\",\""
           << kObjc3RuntimeAddTaskGroupTaskI32Symbol << "\",\""
           << kObjc3RuntimeWaitTaskGroupNextI32Symbol << "\",\""
           << kObjc3RuntimeCancelTaskGroupI32Symbol << "\",\""
           << kObjc3RuntimeTaskIsCancelledI32Symbol << "\",\""
           << kObjc3RuntimeTaskOnCancelI32Symbol << "\",\""
           << kObjc3RuntimeExecutorHopI32Symbol << "\",\""
           << kObjc3RuntimeActorEnterIsolationThunkI32Symbol << "\",\""
           << kObjc3RuntimeActorEnterNonisolatedI32Symbol << "\",\""
           << kObjc3RuntimeActorHopToExecutorI32Symbol << "\",\""
           << kObjc3RuntimeActorRecordReplayProofI32Symbol << "\",\""
           << kObjc3RuntimeActorRecordRaceGuardI32Symbol << "\",\""
           << kObjc3RuntimeActorBindExecutorI32Symbol << "\",\""
           << kObjc3RuntimeActorMailboxEnqueueI32Symbol << "\",\""
           << kObjc3RuntimeActorMailboxDrainNextI32Symbol << "\",\""
           << "objc3_runtime_copy_async_continuation_state_for_testing"
           << "\",\"objc3_runtime_copy_task_runtime_state_for_testing"
           << "\",\"objc3_runtime_copy_actor_runtime_state_for_testing\"]"
           << ",\"async_continuation_state_snapshot_symbol\":\"objc3_runtime_copy_async_continuation_state_for_testing\""
           << ",\"task_runtime_state_snapshot_symbol\":\"objc3_runtime_copy_task_runtime_state_for_testing\""
           << ",\"actor_runtime_state_snapshot_symbol\":\"objc3_runtime_copy_actor_runtime_state_for_testing\""
           << ",\"runtime_abi_boundary_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyRuntimeAbiBoundaryModel
           << "\",\"continuation_runtime_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyContinuationRuntimeModel
           << "\",\"task_runtime_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyTaskRuntimeModel
           << "\",\"actor_runtime_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyActorRuntimeModel
           << "\",\"fail_closed_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyRuntimeAbiFailClosedModel
           << "\",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_abi_completion_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_realization_lookup_reflection_implementation_surface\":{\"contract_id\":\""
           << kObjc3RuntimeRealizationLookupReflectionImplementationSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_abi_query_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelAbiQuerySurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"runtime_class_metaclass_protocol_realization_surface_contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"runtime_reflection_visibility_coherence_diagnostics_surface_contract_id\":\""
           << kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"object_model_query_state_snapshot_symbol\":\"objc3_runtime_copy_object_model_query_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"selector_lookup_table_state_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_table_state_for_testing\""
           << ",\"selector_lookup_entry_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_entry_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"dispatch_state_snapshot_symbol\":\"objc3_runtime_copy_dispatch_state_for_testing\""
           << ",\"realization_lookup_reflection_implementation_model\":\""
           << kObjc3RuntimeRealizationLookupReflectionImplementationModel
           << "\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_reflection_query_surface\":{\"contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"query_api_boundary_model\":\"private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi\""
           << ",\"private_query_symbols\":[\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"objc3_runtime_copy_protocol_conformance_query_for_testing\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_realization_lookup_semantics_surface\":{\"contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"selector_lookup_table_state_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_table_state_for_testing\""
           << ",\"selector_lookup_entry_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_entry_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"lookup_resolution_order_model\":\"seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-strict-dispatch-error\""
           << ",\"selector_materialization_model\":\"metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup\""
           << ",\"unresolved_selector_behavior_model\":\"negative-cache-entry-preserved-and-typed-strict-dispatch-error-returned\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_class_metaclass_protocol_realization_surface\":{\"contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"realized_class_graph_snapshot_symbol\":\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"class_realization_model\":\"registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection\""
           << ",\"metaclass_lineage_model\":\"realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities\""
           << ",\"protocol_conformance_model\":\"realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_category_attachment_merged_dispatch_surface\":{\"contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_category_attachment_protocol_conformance_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"runtime_class_metaclass_protocol_realization_surface_contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"realized_class_graph_snapshot_symbol\":\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"category_attachment_model\":\"registration-attaches-category-owned-instance-and-protocol-members-onto-live-realized-classes-before-dispatch\""
           << ",\"merged_dispatch_resolution_model\":\"attached-category-implementations-override-base-class-instance-lookup-before-superclass-and-protocol-strict-error\""
           << ",\"attached_protocol_visibility_model\":\"attached-categories-publish-owner-and-name-through-realized-class-entries-and-protocol-conformance-queries\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_reflection_visibility_coherence_diagnostics_surface\":{\"contract_id\":\""
           << kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"reflection_visibility_boundary_model\":\"private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state\""
           << ",\"fail_closed_lookup_diagnostic_model\":\"missing-class-and-property-lookups-publish-found-zero-without-mutating-property-registry-or-realized-class-state\""
           << ",\"runtime_coherence_diagnostic_model\":\"reflected-property-selectors-owner-identities-slot-layout-and-ownership-profiles-must-match-live-dispatch-realized-class-and-attached-protocol-state\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_installation_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeInstallationAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"bootstrap_api_contract_id\":\""
           << runtime_bootstrap_api.contract_id
           << "\",\"bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"public_installation_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeBootstrapStateSnapshotSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol
           << "\"],\"private_loader_testing_boundary\":[\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol << "\",\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol << "\",\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol << "\",\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\"],\"installation_requires_coupled_registration_manifest\":true"
           << ",\"register_image_consumes_staged_registration_table_once\":true"
           << ",\"deterministic_reset_replay_supported\":true"
           << "},\n";
  manifest << "  \"runtime_loader_lifecycle_surface\":{\"contract_id\":\""
           << kObjc3RuntimeLoaderLifecycleSurfaceContractId
           << "\",\"runtime_installation_abi_surface_contract_id\":\""
           << kObjc3RuntimeInstallationAbiSurfaceContractId
           << "\",\"bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_semantics.contract_id
           << "\",\"bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"authoritative_probe_path\":\""
           << kObjc3RuntimeInstallationLifecycleProbePath
           << "\",\"loader_testing_boundary_symbols\":[\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol << "\",\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol << "\",\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol << "\",\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\"],\"lifecycle_phases\":[\"startup-installed-runtime-state\""
           << ",\"duplicate-registration-rejected-without-state-advance\""
           << ",\"out-of-order-registration-rejected-without-state-advance\""
           << ",\"invalid-anchor-root-rejected-without-state-advance\""
           << ",\"invalid-discovery-root-rejected-without-state-advance\""
           << ",\"reset-retained-bootstrap-catalog\""
           << ",\"replay-restored-installed-runtime-state\""
           << "],\"rejected_registration_status_codes\":{"
           << "\"duplicate_translation_unit_identity_key\":"
           << kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode
           << ",\"out_of_order_registration\":"
           << kObjc3RuntimeBootstrapOutOfOrderStatusCode
           << ",\"invalid_registration_roots\":"
           << kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode
           << "},\"retained_bootstrap_catalog_required\":true"
           << ",\"deterministic_replay_required\":true"
           << ",\"requires_linked_fixture_or_loader_retained_roots\":true"
           << "},\n";
  manifest << "  \"runtime_release_candidate_claim_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeReleaseCandidateClaimAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"claimability_semantics_release_policy_surface_contract_id\":\"objc3c.runtime.claimability.semantics.release.policy.surface.v1\""
           << ",\"final_claim_publication_deprecated_path_shutdown_surface_contract_id\":\"objc3c.runtime.final.claim.publication.deprecated.path.shutdown.surface.v1\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_release_candidate_claim_testing_boundary\":[\""
           << kObjc3RuntimeReleaseCandidateClaimSnapshotSymbol << "\"]"
           << ",\"release_candidate_claim_snapshot_symbol\":\""
           << kObjc3RuntimeReleaseCandidateClaimSnapshotSymbol
           << "\",\"release_candidate_claim_snapshot_type\":\""
           << kObjc3RuntimeReleaseCandidateClaimSnapshotType
           << "\",\"conformance_publication_contract_id\":\"objc3c.driver.conformance.report.publication.v1\""
           << ",\"conformance_claim_operations_contract_id\":\"objc3c.toolchain.conformance.claim.operations.v1\""
           << ",\"release_evidence_operation_contract_id\":\"objc3c.tooling.release.evidence.toolchain.operations.v1\""
           << ",\"dashboard_status_publication_contract_id\":\"objc3c.tooling.dashboard.status.publication.v1\""
           << ",\"release_candidate_matrix_contract_id\":\"objc3c.tooling.release.candidate.execution.matrix.v1\""
           << ",\"claimed_profile_ids\":[\"core\",\"strict\",\"strict-concurrency\",\"strict-system\"]"
           << ",\"targeted_profile_ids\":[\"strict\",\"strict-concurrency\",\"strict-system\"]"
           << ",\"authoritative_probe_path\":\""
           << kObjc3RuntimeReleaseCandidateClaimProbePath
           << "\",\"runtime_claim_boundary_model\":\""
           << kObjc3RuntimeReleaseCandidateClaimBoundaryModel

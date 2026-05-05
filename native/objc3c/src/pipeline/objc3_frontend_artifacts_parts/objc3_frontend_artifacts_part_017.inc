           << block_source_storage_annotation_contract.block_literal_sites
           << ",\"capture_entries_total\":"
           << block_source_storage_annotation_contract.capture_entries_total
           << ",\"mutated_capture_entries_total\":"
           << block_source_storage_annotation_contract
                  .mutated_capture_entries_total
           << ",\"byref_capture_entries_total\":"
           << block_source_storage_annotation_contract
                  .byref_capture_entries_total
           << ",\"copy_helper_intent_sites\":"
           << block_source_storage_annotation_contract.copy_helper_intent_sites
           << ",\"dispose_helper_intent_sites\":"
           << block_source_storage_annotation_contract
                  .dispose_helper_intent_sites
           << ",\"heap_candidate_sites\":"
           << block_source_storage_annotation_contract.heap_candidate_sites
           << ",\"expression_sites\":"
           << block_source_storage_annotation_contract.expression_sites
           << ",\"global_initializer_sites\":"
           << block_source_storage_annotation_contract
                  .global_initializer_sites
           << ",\"binding_initializer_sites\":"
           << block_source_storage_annotation_contract
                  .binding_initializer_sites
           << ",\"assignment_value_sites\":"
           << block_source_storage_annotation_contract.assignment_value_sites
           << ",\"return_value_sites\":"
           << block_source_storage_annotation_contract.return_value_sites
           << ",\"call_argument_sites\":"
           << block_source_storage_annotation_contract.call_argument_sites
           << ",\"message_argument_sites\":"
           << block_source_storage_annotation_contract.message_argument_sites
           << ",\"non_normalized_sites\":"
           << block_source_storage_annotation_contract.non_normalized_sites
           << ",\"contract_violation_sites\":"
           << block_source_storage_annotation_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_source_storage_annotation_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_source_storage_annotation_contract.deterministic ? "true"
                                                                      : "false")
           << "}"
           << ",\"objc_block_abi_invoke_trampoline_lowering_surface\":{\"block_literal_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.block_literal_sites
           << ",\"invoke_argument_slots_total\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total
           << ",\"capture_word_count_total\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_word_count_total
           << ",\"parameter_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total
           << ",\"descriptor_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites
           << ",\"invoke_trampoline_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites
           << ",\"missing_invoke_trampoline_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites
           << ",\"non_normalized_layout_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites
           << ",\"contract_violation_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_storage_escape_lowering_surface\":{\"block_literal_sites\":"
           << block_storage_escape_lowering_contract.block_literal_sites
           << ",\"mutable_capture_count_total\":"
           << block_storage_escape_lowering_contract.mutable_capture_count_total
           << ",\"byref_slot_count_total\":"
           << block_storage_escape_lowering_contract.byref_slot_count_total
           << ",\"parameter_entries_total\":"
           << block_storage_escape_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_storage_escape_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_storage_escape_lowering_contract.body_statement_entries_total
           << ",\"requires_byref_cells_sites\":"
           << block_storage_escape_lowering_contract.requires_byref_cells_sites
           << ",\"escape_analysis_enabled_sites\":"
           << block_storage_escape_lowering_contract.escape_analysis_enabled_sites
           << ",\"escape_to_heap_sites\":"
           << block_storage_escape_lowering_contract.escape_to_heap_sites
           << ",\"escape_profile_normalized_sites\":"
           << block_storage_escape_lowering_contract.escape_profile_normalized_sites
           << ",\"byref_layout_symbolized_sites\":"
           << block_storage_escape_lowering_contract.byref_layout_symbolized_sites
           << ",\"contract_violation_sites\":"
           << block_storage_escape_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_copy_dispose_lowering_surface\":{\"block_literal_sites\":"
           << block_copy_dispose_lowering_contract.block_literal_sites
           << ",\"mutable_capture_count_total\":"
           << block_copy_dispose_lowering_contract.mutable_capture_count_total
           << ",\"byref_slot_count_total\":"
           << block_copy_dispose_lowering_contract.byref_slot_count_total
           << ",\"parameter_entries_total\":"
           << block_copy_dispose_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_copy_dispose_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_copy_dispose_lowering_contract.body_statement_entries_total
           << ",\"copy_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_required_sites
           << ",\"dispose_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_required_sites
           << ",\"profile_normalized_sites\":"
           << block_copy_dispose_lowering_contract.profile_normalized_sites
           << ",\"copy_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_symbolized_sites
           << ",\"dispose_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites
           << ",\"contract_violation_sites\":"
           << block_copy_dispose_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_determinism_perf_baseline_lowering_surface\":{\"block_literal_sites\":"
           << block_determinism_perf_baseline_lowering_contract.block_literal_sites
           << ",\"baseline_weight_total\":"
           << block_determinism_perf_baseline_lowering_contract.baseline_weight_total
           << ",\"parameter_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.body_statement_entries_total
           << ",\"deterministic_capture_sites\":"
           << block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites
           << ",\"heavy_tier_sites\":"
           << block_determinism_perf_baseline_lowering_contract.heavy_tier_sites
           << ",\"normalized_profile_sites\":"
           << block_determinism_perf_baseline_lowering_contract.normalized_profile_sites
           << ",\"contract_violation_sites\":"
           << block_determinism_perf_baseline_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_lightweight_generic_constraint_lowering_surface\":{\"generic_constraint_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_constraint_sites
           << ",\"generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_suffix_sites
           << ",\"object_pointer_type_sites\":"
           << lightweight_generic_constraint_lowering_contract.object_pointer_type_sites
           << ",\"terminated_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites
           << ",\"pointer_declarator_sites\":"
           << lightweight_generic_constraint_lowering_contract.pointer_declarator_sites
           << ",\"normalized_constraint_sites\":"
           << lightweight_generic_constraint_lowering_contract.normalized_constraint_sites
           << ",\"contract_violation_sites\":"
           << lightweight_generic_constraint_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_nullability_flow_warning_precision_lowering_surface\":{\"nullability_flow_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_flow_sites
           << ",\"object_pointer_type_sites\":"
           << nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites
           << ",\"nullability_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites
           << ",\"nullable_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites
           << ",\"nonnull_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites
           << ",\"normalized_sites\":"
           << nullability_flow_warning_precision_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << nullability_flow_warning_precision_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_protocol_qualified_object_type_lowering_surface\":{\"protocol_qualified_object_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites
           << ",\"protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_composition_sites
           << ",\"object_pointer_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.object_pointer_type_sites
           << ",\"terminated_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites
           << ",\"pointer_declarator_sites\":"
           << protocol_qualified_object_type_lowering_contract.pointer_declarator_sites
           << ",\"normalized_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites
           << ",\"contract_violation_sites\":"
           << protocol_qualified_object_type_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_variance_bridge_cast_lowering_surface\":{\"variance_bridge_cast_sites\":"
           << variance_bridge_cast_lowering_contract.variance_bridge_cast_sites
           << ",\"protocol_composition_sites\":"
           << variance_bridge_cast_lowering_contract.protocol_composition_sites
           << ",\"ownership_qualifier_sites\":"
           << variance_bridge_cast_lowering_contract.ownership_qualifier_sites
           << ",\"object_pointer_type_sites\":"
           << variance_bridge_cast_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << variance_bridge_cast_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << variance_bridge_cast_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << variance_bridge_cast_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_generic_metadata_abi_lowering_surface\":{\"generic_metadata_abi_sites\":"
           << generic_metadata_abi_lowering_contract.generic_metadata_abi_sites
           << ",\"generic_suffix_sites\":"
           << generic_metadata_abi_lowering_contract.generic_suffix_sites
           << ",\"protocol_composition_sites\":"
           << generic_metadata_abi_lowering_contract.protocol_composition_sites
           << ",\"ownership_qualifier_sites\":"
           << generic_metadata_abi_lowering_contract.ownership_qualifier_sites
           << ",\"object_pointer_type_sites\":"
           << generic_metadata_abi_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << generic_metadata_abi_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << generic_metadata_abi_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << generic_metadata_abi_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
             << "\",\"deterministic_handoff\":"
             << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
             << "}"
             << ",\"objc_type_system_optional_keypath_lowering_contract\":"
            << BuildTypeSystemOptionalKeypathLoweringContractJson(
                  type_system_optional_keypath_lowering_contract,
                  type_system_type_semantic_model_summary,
                  type_system_type_semantic_model_summary.replay_key,
                  message_send_selector_lowering_replay_key,
                    dispatch_abi_marshalling_replay_key,
                    nil_receiver_semantics_foldability_replay_key,
                    type_system_optional_keypath_lowering_replay_key)
             << ",\"objc_control_flow_control_flow_safety_lowering_contract\":"
            << BuildControlFlowControlFlowSafetyLoweringContractJson(
                  control_flow_control_flow_safety_lowering_contract,
                  control_flow_control_flow_semantic_model_summary,
                  control_flow_control_flow_semantic_model_summary.replay_key,
                  control_flow_control_flow_safety_lowering_replay_key)
            // runtime-helper freeze anchor: lane-D publishes one
            // canonical runtime/helper boundary packet above the live lowering
            // contract and the runtime link wiring surface while full key-path
            // execution helpers remain deferred to D002.
            << ",\"objc_type_system_optional_keypath_runtime_helper_contract\":"
            << BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
                   type_system_optional_keypath_lowering_contract,
                   runtime_support_library_link_wiring,
                   type_system_optional_keypath_lowering_replay_key)
             << ",\"objc_module_import_graph_lowering_surface\":{\"module_import_graph_sites\":"
             << module_import_graph_lowering_contract.module_import_graph_sites
           << ",\"import_edge_candidate_sites\":"
           << module_import_graph_lowering_contract.import_edge_candidate_sites
           << ",\"namespace_segment_sites\":"
           << module_import_graph_lowering_contract.namespace_segment_sites
           << ",\"object_pointer_type_sites\":"
           << module_import_graph_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << module_import_graph_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << module_import_graph_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << module_import_graph_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << "}"
           // runtime-aware import/module surface anchor: lane-A
           // freezes one frontend-published contract above the current module
           // import graph lowering surface and below any real imported
           // runtime-owned declaration or metadata-reference realization.
           << ",\"objc_runtime_aware_import_module_surface_contract\":"
           << BuildRuntimeAwareImportModuleSurfaceSummaryJson(
                  program,
                  pipeline_result.parser_contract_snapshot,
                  module_import_graph_lowering_contract)
           << ",\"objc_runtime_aware_import_module_frontend_closure\":"
           << BuildRuntimeAwareImportModuleFrontendClosureSummaryJson(
                  runtime_aware_import_module_frontend_closure)
           // cross-module semantic preservation anchor: lane-B
           // freezes the semantic facts that later imported metadata handling
           // must preserve across module boundaries without claiming that the
           // imported runtime metadata semantics are landed yet.
           << ",\"objc_cross_module_runtime_metadata_semantic_preservation_contract\":"
           << BuildCrossModuleRuntimeMetadataSemanticPreservationSummaryJson(
                  cross_module_runtime_metadata_semantic_preservation)
           << ",\"objc_imported_runtime_metadata_semantic_rules\":"
           << BuildImportedRuntimeMetadataSemanticRulesSummaryJson(
                  imported_runtime_metadata_semantic_rules)
           // serialized metadata import/lowering anchor: lane-C
           // freezes the boundary where emitted runtime-import-surface
           // artifacts already influence frontend semantic surfaces, but
           // imported metadata payloads still are not rehydrated, reused
           // incrementally, or lowered into IR.
           << ",\"objc_serialized_runtime_metadata_import_lowering_contract\":"
           << BuildSerializedRuntimeMetadataImportLoweringSummaryJson(
                  serialized_runtime_metadata_import_lowering)
           // serialized metadata artifact reuse anchor: lane-C now
           // emits and reloads a transitive serialized runtime-metadata payload
           // through the runtime-import-surface artifact so downstream modules
           // can recover object-model semantics without reparsing source.
           << ",\"objc_serialized_runtime_metadata_artifact_reuse\":"
           << BuildSerializedRuntimeMetadataArtifactReuseSummaryJson(
                  serialized_runtime_metadata_artifact_reuse)
           // cross-module build/runtime orchestration anchor:
           // lane-D freezes the truthful boundary where the transitive
           // runtime-import-surface reuse payload and the local registration
           // manifest are both authoritative inputs, while cross-module link
           // packaging and runtime-registration aggregation remain unlanded.
           // cross-module runtime packaging anchor:
           // driver/runtime packaging path now materializes the ordered
           // cross-module link plan and merged linker response file from those
           // same authoritative artifacts, so this semantic surface remains the
           // canonical replay boundary for downstream packaging consumers.
           // cross-module object-model gate anchor: lane-E consumes
           // the A002/B002/C002/D002 summary chain and freezes the current
           // runnable two-image proof boundary before E002 broadens the
           // execution matrix.
           // runnable import/module execution-matrix anchor:
           // the same emitted frontend surface remains the canonical replay
           // boundary while lane-E closes the runnable matrix around it.
           << ",\"objc_cross_module_build_runtime_orchestration_contract\":"
           << BuildCrossModuleBuildRuntimeOrchestrationSummaryJson(
                  cross_module_build_runtime_orchestration)
           << ",\"objc_namespace_collision_shadowing_lowering_surface\":{\"namespace_collision_shadowing_sites\":" 
           << namespace_collision_shadowing_lowering_contract
                  .namespace_collision_shadowing_sites
           << ",\"namespace_segment_sites\":"
           << namespace_collision_shadowing_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << namespace_collision_shadowing_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_public_private_api_partition_lowering_surface\":{\"public_private_api_partition_sites\":"
           << public_private_api_partition_lowering_contract
                  .public_private_api_partition_sites
           << ",\"namespace_segment_sites\":"
           << public_private_api_partition_lowering_contract
                  .namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << public_private_api_partition_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << public_private_api_partition_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << public_private_api_partition_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << public_private_api_partition_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << public_private_api_partition_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_incremental_module_cache_invalidation_lowering_surface\":{\"incremental_module_cache_invalidation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .incremental_module_cache_invalidation_sites
           << ",\"namespace_segment_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_cross_module_conformance_lowering_surface\":{\"cross_module_conformance_sites\":"
           << cross_module_conformance_lowering_contract
                  .cross_module_conformance_sites
           << ",\"namespace_segment_sites\":"
           << cross_module_conformance_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << cross_module_conformance_lowering_contract.import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << cross_module_conformance_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << cross_module_conformance_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << cross_module_conformance_lowering_contract.normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << cross_module_conformance_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_throws_propagation_lowering_surface\":{\"throws_propagation_sites\":"
           << throws_propagation_lowering_contract.throws_propagation_sites
           << ",\"namespace_segment_sites\":"
           << throws_propagation_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << throws_propagation_lowering_contract.import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << throws_propagation_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << throws_propagation_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << throws_propagation_lowering_contract.normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << throws_propagation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << throws_propagation_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_result_like_lowering_surface\":{\"result_like_sites\":"
           << result_like_lowering_contract.result_like_sites
           << ",\"result_success_sites\":"
           << result_like_lowering_contract.result_success_sites
           << ",\"result_failure_sites\":"
           << result_like_lowering_contract.result_failure_sites
           << ",\"result_branch_sites\":"
           << result_like_lowering_contract.result_branch_sites
           << ",\"result_payload_sites\":"
           << result_like_lowering_contract.result_payload_sites
           << ",\"normalized_sites\":"
           << result_like_lowering_contract.normalized_sites
           << ",\"branch_merge_sites\":"
           << result_like_lowering_contract.branch_merge_sites
           << ",\"contract_violation_sites\":"
           << result_like_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << result_like_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (result_like_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_ns_error_bridging_lowering_surface\":{\"ns_error_bridging_sites\":"
           << ns_error_bridging_lowering_contract.ns_error_bridging_sites
           << ",\"ns_error_parameter_sites\":"
           << ns_error_bridging_lowering_contract.ns_error_parameter_sites
           << ",\"ns_error_out_parameter_sites\":"
           << ns_error_bridging_lowering_contract.ns_error_out_parameter_sites
           << ",\"ns_error_bridge_path_sites\":"
           << ns_error_bridging_lowering_contract.ns_error_bridge_path_sites
           << ",\"failable_call_sites\":"
           << ns_error_bridging_lowering_contract.failable_call_sites
           << ",\"normalized_sites\":"
           << ns_error_bridging_lowering_contract.normalized_sites
           << ",\"bridge_boundary_sites\":"
           << ns_error_bridging_lowering_contract.bridge_boundary_sites
           << ",\"contract_violation_sites\":"
           << ns_error_bridging_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << ns_error_bridging_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (ns_error_bridging_lowering_contract.deterministic ? "true"
                                                                 : "false")
           << "}"
           << ",\"objc_unwind_cleanup_lowering_surface\":{\"unwind_cleanup_sites\":"
           << unwind_cleanup_lowering_contract.unwind_cleanup_sites
           << ",\"unwind_edge_sites\":"
           << unwind_cleanup_lowering_contract.unwind_edge_sites
           << ",\"cleanup_scope_sites\":"
           << unwind_cleanup_lowering_contract.cleanup_scope_sites
           << ",\"cleanup_emit_sites\":"
           << unwind_cleanup_lowering_contract.cleanup_emit_sites
           << ",\"landing_pad_sites\":"
           << unwind_cleanup_lowering_contract.landing_pad_sites
           << ",\"cleanup_resume_sites\":"
           << unwind_cleanup_lowering_contract.cleanup_resume_sites
           << ",\"normalized_sites\":"
           << unwind_cleanup_lowering_contract.normalized_sites
           << ",\"guard_blocked_sites\":"
           << unwind_cleanup_lowering_contract.guard_blocked_sites
           << ",\"contract_violation_sites\":"
           << unwind_cleanup_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << unwind_cleanup_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (unwind_cleanup_lowering_contract.deterministic ? "true"
                                                              : "false")
           << "}"
           << ",\"objc_error_handling_throws_abi_propagation_lowering\":{\"contract_id\":\""
           << kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId
           << "\",\"source_model\":\""
           << kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel
           << "\",\"abi_model\":\""
           << kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel
           << "\",\"throws_replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"result_like_replay_key\":\""
           << result_like_lowering_replay_key
           << "\",\"ns_error_replay_key\":\""
           << ns_error_bridging_lowering_replay_key
           << "\",\"unwind_replay_key\":\""
           << unwind_cleanup_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (deterministic_error_handling_throws_abi_propagation_lowering ? "true"
                                                                   : "false")
           << ",\"ready_for_runtime_execution\":true"
           << ",\"fail_closed_model\":\""
           << kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel
           << "\",\"next_issue\":\"objc3c.errors.resultbridging.artifactsurface.v1\"}"
           << ",\"objc_error_handling_result_and_bridging_artifact_replay\":"
           << BuildErrorHandlingResultAndBridgingArtifactReplaySummaryJson(
                  error_handling_result_and_bridging_artifact_replay_summary)
           << ",\"objc_object_pointer_nullability_generics_surface\":{\"object_pointer_type_spellings\":"
           << object_pointer_nullability_generics_summary.object_pointer_type_spellings
           << ",\"pointer_declarator_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_entries
           << ",\"pointer_declarator_depth_total\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
           << ",\"pointer_declarator_token_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_token_entries
           << ",\"nullability_suffix_entries\":"
           << object_pointer_nullability_generics_summary.nullability_suffix_entries
           << ",\"generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.generic_suffix_entries
           << ",\"terminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.terminated_generic_suffix_entries
           << ",\"unterminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries
           << ",\"deterministic_handoff\":"
           << (object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_type_system_type_source_closure\":"
           << BuildTypeSystemTypeSourceClosureSummaryJson(
                  type_system_type_source_closure_summary)
           << ",\"objc_control_flow_control_flow_source_closure\":"
           << BuildControlFlowControlFlowSourceClosureSummaryJson(
                  control_flow_control_flow_source_closure_summary)
           << ",\"objc_error_handling_error_source_closure\":"
           << BuildErrorHandlingErrorSourceClosureSummaryJson(
                  error_handling_error_source_closure_summary)
            << ",\"objc_concurrency_async_source_closure\":"
            << BuildConcurrencyAsyncSourceClosureSummaryJson(
                   concurrency_async_source_closure_summary)
            << ",\"objc_ownership_resource_borrowed_and_capture_list_source_closure\":"
            << BuildOwnershipSystemExtensionSourceClosureSummaryJson(
                   ownership_system_extension_source_closure_summary)
            << ",\"objc_ownership_cleanup_resource_and_capture_source_completion\":"
            << BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
                   ownership_cleanup_resource_capture_source_completion_summary)
            << ",\"objc_ownership_retainable_c_family_source_completion\":"
            << BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
                   ownership_retainable_c_family_source_completion_summary)
            << ",\"objc_dispatch_dispatch_intent_and_dynamism_source_closure\":"
            << BuildDispatchDispatchIntentSourceClosureSummaryJson(
                   dispatch_dispatch_intent_source_closure_summary)
            << ",\"objc_dispatch_dispatch_intent_attribute_and_defaulting_source_completion\":"
            << BuildDispatchDispatchIntentSourceCompletionSummaryJson(
                   dispatch_dispatch_intent_source_completion_summary)
            << ",\"objc_metaprogramming_derive_macro_property_behavior_source_closure\":"
            << BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson(
                   metaprogramming_metaprogramming_source_closure_summary)
            << ",\"objc_metaprogramming_macro_package_and_provenance_source_completion\":"
            << BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson(
                   metaprogramming_macro_package_provenance_source_completion_summary)
            << ",\"objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion\":"
                   << BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson(
                   metaprogramming_property_behavior_source_completion_summary)
            << ",\"objc_interop_foreign_declaration_and_import_source_closure\":"
                   << BuildInteropForeignImportSourceClosureSummaryJson(
                   interop_foreign_import_source_closure_summary)
            << ",\"objc_interop_cpp_and_swift_interop_annotation_source_completion\":"
                   << BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson(
                   interop_cpp_swift_interop_annotation_source_completion_summary)
           << ",\"objc_tooling_diagnostics_fixit_and_migrator_source_inventory\":"
                  << BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
                  tooling_diagnostics_migrator_source_inventory_summary)
           << ",\"objc_tooling_migration_and_canonicalization_source_completion\":"
                  << BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
                  tooling_migration_canonicalization_source_completion_summary)
           << ",\"objc_tooling_diagnostic_taxonomy_and_portability_contract\":"
                  << BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
                  tooling_diagnostic_taxonomy_portability_contract_summary)
           << ",\"objc_tooling_feature_specific_fixit_synthesis\":"
                  << BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
                  tooling_feature_specific_fixit_synthesis_summary)
           << ",\"objc_tooling_legacy_canonical_migration_semantics\":"
                  << BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson(
                      tooling_legacy_canonical_migration_semantics_summary)
           << ",\"objc_tooling_machine_readable_conformance_report_contract\":"
                  << BuildToolingMachineReadableConformanceReportContractSummaryJson(
                      tooling_machine_readable_conformance_report_contract_summary)
           << ",\"objc_tooling_feature_aware_conformance_report_emission\":"
                  << BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(
                      tooling_feature_aware_conformance_report_emission_summary)
           << ",\"objc_tooling_corpus_sharding_release_evidence_packaging\":"
                  << BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(
                      tooling_corpus_sharding_release_evidence_packaging_summary)
           << ",\"objc_interop_interop_semantic_model\":"
           << BuildInteropInteropSemanticModelSummaryJson(
                  interop_interop_semantic_model_summary)
           << ",\"objc_interop_c_and_objc_runtime_parity_semantics\":"
           << BuildInteropInteropRuntimeParitySummaryJson(
                  interop_interop_runtime_parity_summary)
           << ",\"objc_interop_cpp_ownership_throws_and_async_interactions\":"
           << BuildInteropCppInteropInteractionSummaryJson(
                  interop_cpp_interop_interaction_summary)
           << ",\"objc_interop_swift_metadata_and_isolation_mapping\":"
           << BuildInteropSwiftInteropIsolationSummaryJson(
                  interop_swift_interop_isolation_summary)
           << ",\"objc_interop_foreign_surface_interface_and_module_preservation\":"
           << BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
                  interop_foreign_surface_interface_preservation_summary)
           << ",\"objc_interop_header_module_and_bridge_generation\":"
           << BuildInteropHeaderModuleBridgeGenerationSummaryJson(
                  interop_header_module_bridge_generation_summary)
           << ",\"objc_interop_interop_lowering_and_abi_contract\":"
           << BuildInteropInteropLoweringContractJson(
                  interop_interop_semantic_model_summary,
                  interop_interop_runtime_parity_summary,
                  interop_cpp_interop_interaction_summary,
                  interop_swift_interop_isolation_summary,
                  interop_foreign_surface_interface_preservation_summary,
                  interop_interop_lowering_contract,
                  interop_interop_lowering_replay_key)
           << ",\"objc_interop_foreign_call_and_lifetime_lowering\":"
           << BuildInteropForeignCallLifetimeLoweringContractJson(
                  interop_interop_lowering_contract,
                  interop_cpp_interop_interaction_summary,
                  interop_foreign_surface_interface_preservation_summary,
                  interop_foreign_call_lifetime_lowering_contract,
                  interop_foreign_call_lifetime_lowering_replay_key)
           << ",\"objc_interop_ffi_metadata_and_interface_preservation\":"
           << BuildInteropFfiMetadataInterfacePreservationContractJson(
                  interop_foreign_call_lifetime_lowering_contract,
                  interop_foreign_call_lifetime_lowering_replay_key,
                  interop_foreign_surface_interface_preservation_summary,
                  interop_ffi_metadata_interface_preservation_contract,
                  interop_ffi_metadata_interface_preservation_replay_key)
            << ",\"objc_metaprogramming_expansion_and_behavior_semantic_model\":"
            << BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
                   metaprogramming_expansion_behavior_semantic_model_summary)
            << ",\"objc_metaprogramming_derive_expansion_inventory\":"
            << BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
                   metaprogramming_derive_expansion_inventory_summary)
            << ",\"objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics\":"
            << BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
                   metaprogramming_macro_safety_sandbox_determinism_summary)
            << ",\"objc_metaprogramming_property_behavior_legality_and_interaction_completion\":"
            << BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
                   metaprogramming_property_behavior_legality_compatibility_summary)
            << ",\"objc_metaprogramming_expansion_and_lowering_contract\":"
            << BuildMetaprogrammingExpansionLoweringContractJson(
                   metaprogramming_property_behavior_source_completion_summary,
                   metaprogramming_derive_expansion_inventory_summary,
                   metaprogramming_macro_safety_sandbox_determinism_summary,
                   metaprogramming_property_behavior_legality_compatibility_summary,
                   metaprogramming_expansion_lowering_contract,
                   metaprogramming_expansion_lowering_replay_key)
            << ",\"objc_metaprogramming_synthesized_ast_and_ir_emission\":"
            << BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
                   metaprogramming_expansion_lowering_contract,
                   metaprogramming_synthesized_artifact_emission_contract,
                   metaprogramming_synthesized_artifact_emission_replay_key)
           << ",\"objc_metaprogramming_module_interface_and_replay_preservation\":"
           << BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
                  metaprogramming_module_interface_replay_preservation_summary)
           << ",\"objc_dispatch_dynamism_and_dispatch_control_semantic_model\":"
           << BuildDispatchDispatchIntentSemanticModelSummaryJson(
                  dispatch_dispatch_intent_semantic_model_summary)
            << ",\"objc_dispatch_override_finality_and_sealing_legality\":"
            << BuildDispatchDispatchIntentLegalitySummaryJson(
                   dispatch_dispatch_intent_legality_summary)
            << ",\"objc_dispatch_dynamism_control_compatibility_diagnostics\":"
            << BuildDispatchDispatchIntentCompatibilitySummaryJson(
                   dispatch_dispatch_intent_compatibility_summary)
           << ",\"objc_dispatch_dispatch_control_lowering_contract\":"
           << BuildDispatchDispatchControlLoweringContractJson(
                  dispatch_dispatch_intent_semantic_model_summary,
                  dispatch_dispatch_intent_legality_summary,
                  dispatch_dispatch_intent_compatibility_summary,
                  dispatch_dispatch_control_lowering_contract,
                  dispatch_dispatch_control_lowering_replay_key)
           << ",\"objc_dispatch_dispatch_metadata_and_interface_preservation\":"
           << BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
                  dispatch_dispatch_metadata_interface_preservation_summary)
            << ",\"objc_concurrency_actor_member_and_isolation_source_closure\":"
            << BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson(
                   concurrency_actor_member_isolation_source_closure_summary)
           << ",\"objc_concurrency_actor_isolation_and_sendable_semantic_model\":"
           << BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson(
                  concurrency_actor_isolation_sendable_semantic_model_summary)
           << ",\"objc_concurrency_actor_isolation_and_sendability_enforcement\":"
           << BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson(
                  concurrency_actor_isolation_sendability_enforcement_summary)
           << ",\"objc_concurrency_actor_race_hazard_and_escape_diagnostics\":"
           << BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson(
                  concurrency_actor_race_hazard_escape_diagnostics_summary)
           << ",\"objc_concurrency_actor_lowering_and_metadata_contract\":"
           << BuildConcurrencyActorLoweringMetadataContractJson(
                  concurrency_actor_member_isolation_source_closure_summary,
                  concurrency_actor_isolation_sendability_enforcement_summary,
                  concurrency_actor_race_hazard_escape_diagnostics_summary,
                  concurrency_actor_lowering_metadata_contract,
                  concurrency_actor_lowering_metadata_replay_key)
           << ",\"objc_concurrency_task_group_and_cancellation_source_closure\":"
           << BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson(
                  concurrency_task_group_cancellation_source_closure_summary)
           << ",\"objc_concurrency_async_effect_and_suspension_semantic_model\":"
           << BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
                  concurrency_async_effect_suspension_semantic_model_summary)
           << ",\"objc_concurrency_task_executor_and_cancellation_semantic_model\":"
           << BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
                  concurrency_task_executor_cancellation_semantic_model_summary)
           << ",\"objc_ownership_system_extension_semantic_model\":"
           << BuildOwnershipSystemExtensionSemanticModelSummaryJson(
                  ownership_system_extension_semantic_model_summary)
           << ",\"objc_effects_ownership_semantic_model\":"
           << BuildEffectsOwnershipSemanticModelSummaryJson(
                  effects_ownership_semantic_model_summary)
           << ",\"objc_cross_module_semantic_contracts_and_diagnostics\":"
           << BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
                  cross_module_semantic_contracts_diagnostics_summary)
           << ",\"objc_ownership_resource_move_and_use_after_move_semantics\":"
           << BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
                  ownership_resource_move_use_after_move_semantics_summary)
           << ",\"objc_ownership_borrowed_pointer_escape_analysis\":"
           << BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
                  ownership_borrowed_pointer_escape_analysis_summary)
           << ",\"objc_ownership_capture_list_and_retainable_family_legality_completion\":"
           << BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
                  ownership_capture_list_retainable_family_legality_completion_summary)
           << ",\"objc_ownership_system_extension_lowering_contract\":"
           << BuildOwnershipSystemExtensionLoweringContractJson(
                  ownership_system_extension_semantic_model_summary,
                  ownership_resource_move_use_after_move_semantics_summary,
                  ownership_borrowed_pointer_escape_analysis_summary,
                  ownership_capture_list_retainable_family_legality_completion_summary,
                  ownership_system_extension_lowering_contract,
                  ownership_system_extension_lowering_replay_key)
           << ",\"objc_ownership_borrowed_pointer_and_retainable_family_abi_completion\":"
           << BuildOwnershipBorrowedRetainableAbiCompletionJson(
                  ownership_system_extension_lowering_contract,
                  ownership_system_extension_source_closure_summary,
                  ownership_retainable_c_family_source_completion_summary,
                  ownership_system_extension_lowering_replay_key,
                  ownership_borrowed_retainable_abi_completion_replay_key)
            << ",\"objc_concurrency_structured_task_and_cancellation_semantics\":"
            << BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
                   concurrency_structured_task_cancellation_semantic_summary)
           << ",\"objc_concurrency_executor_hop_and_affinity_compatibility_completion\":"
           << BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
                  concurrency_executor_hop_affinity_compatibility_summary)
           << ",\"objc_concurrency_await_suspension_and_resume_semantics\":"
           << BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
                  concurrency_await_suspension_resume_semantic_summary)
           << ",\"objc_concurrency_async_diagnostics_and_compatibility_completion\":"
           << BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
                  concurrency_async_diagnostics_compatibility_summary)
           << ",\"objc_concurrency_continuation_abi_and_async_lowering_contract\":"
           << BuildConcurrencyContinuationAbiAsyncLoweringContractJson(
                  concurrency_async_continuation_lowering_contract,
                  concurrency_await_lowering_suspension_state_lowering_contract,
                  concurrency_async_continuation_lowering_replay_key,
                  concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_concurrency_task_runtime_lowering_contract\":"
           << BuildConcurrencyTaskRuntimeLoweringContractJson(
                  concurrency_task_executor_cancellation_semantic_model_summary,
                  concurrency_structured_task_cancellation_semantic_summary,
                  concurrency_executor_hop_affinity_compatibility_summary,
                  concurrency_actor_isolation_sendability_lowering_contract,
                  concurrency_actor_isolation_sendability_lowering_replay_key,
                  concurrency_task_runtime_interop_cancellation_lowering_contract,
                  concurrency_task_runtime_interop_cancellation_lowering_replay_key,
                  concurrency_concurrency_replay_race_guard_lowering_contract,
                  concurrency_concurrency_replay_race_guard_lowering_replay_key)
           << ",\"objc_concurrency_task_group_and_runtime_abi_completion\":"
           << BuildConcurrencyTaskRuntimeAbiCompletionJson(
                  concurrency_task_runtime_interop_cancellation_lowering_replay_key,
                  concurrency_concurrency_replay_race_guard_lowering_replay_key)
           << ",\"objc_concurrency_async_function_await_and_continuation_lowering\":"
           << BuildConcurrencyAsyncDirectCallLoweringJson(
                  concurrency_async_source_closure_summary,
                  concurrency_async_continuation_lowering_contract,
                  concurrency_await_lowering_suspension_state_lowering_contract,
                  concurrency_async_continuation_lowering_replay_key,
                  concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_concurrency_suspension_autorelease_and_cleanup_integration\":"
           << BuildConcurrencySuspensionCleanupIntegrationJson(
                  control_flow_control_flow_safety_lowering_contract,
                  control_flow_control_flow_safety_lowering_replay_key,
                  autoreleasepool_scope_lowering_contract,
                  autoreleasepool_scope_lowering_replay_key,
                  concurrency_async_continuation_lowering_contract,
                  concurrency_await_lowering_suspension_state_lowering_contract,
                  concurrency_async_continuation_lowering_replay_key,
                  concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_error_handling_error_semantic_model\":"
           << BuildErrorHandlingErrorSemanticModelSummaryJson(
                  error_handling_error_semantic_model_summary)
           << ",\"objc_error_handling_try_do_catch_semantics\":"
           << BuildErrorHandlingTryDoCatchSemanticSummaryJson(
                  error_handling_try_do_catch_semantic_summary)
           << ",\"objc_error_handling_error_bridge_legality\":"
           << BuildErrorHandlingErrorBridgeLegalitySummaryJson(
                  error_handling_error_bridge_legality_summary)
           << ",\"objc_control_flow_control_flow_semantic_model\":"
           << BuildControlFlowControlFlowSemanticModelSummaryJson(
                  control_flow_control_flow_semantic_model_summary)
           << ",\"objc_type_system_type_semantic_model\":"
           << BuildTypeSystemTypeSemanticModelSummaryJson(
                  type_system_type_semantic_model_summary)
           << ",\"objc_symbol_graph_scope_resolution_surface\":{\"global_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.global_symbol_nodes
           << ",\"function_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.function_symbol_nodes
           << ",\"interface_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_symbol_nodes
           << ",\"implementation_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
           << ",\"interface_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
           << ",\"implementation_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes
           << ",\"interface_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
           << ",\"implementation_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
           << ",\"top_level_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.top_level_scope_symbols
           << ",\"nested_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.nested_scope_symbols
           << ",\"scope_frames_total\":"
           << symbol_graph_scope_resolution_summary.scope_frames_total
           << ",\"implementation_interface_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites
           << ",\"implementation_interface_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits
           << ",\"implementation_interface_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses
           << ",\"method_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.method_resolution_sites
           << ",\"method_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.method_resolution_hits
           << ",\"method_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.method_resolution_misses
           << ",\"deterministic_symbol_graph_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff ? "true" : "false")
           << ",\"deterministic_scope_resolution_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff ? "true" : "false")
           << ",\"deterministic_handoff_key\":\""
           << symbol_graph_scope_resolution_summary.deterministic_handoff_key
           << "\"}"
           << ",\"function_signature_surface\":{\"scalar_return_i32\":" << scalar_return_i32
           << ",\"scalar_return_bool\":" << scalar_return_bool
           << ",\"scalar_return_void\":" << scalar_return_void << ",\"scalar_param_i32\":" << scalar_param_i32
           << ",\"scalar_param_bool\":" << scalar_param_bool << "}}\n";
  manifest << "    }\n";
  manifest << "  },\n";
  manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args
           << ",\"selector_global_ordering\":\"lexicographic\"},\n";
  manifest << "  \"lowering_vector_abi\":{\"replay_key\":\"" << Objc3SimdVectorTypeLoweringReplayKey()
           << "\",\"lane_contract\":\"" << kObjc3SimdVectorLaneContract
           << "\",\"vector_signature_functions\":" << vector_signature_functions << "},\n";
  manifest << "  \"lowering_property_synthesis_ivar_binding\":{\"replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\",\"lane_contract\":\"" << kObjc3PropertySynthesisIvarBindingLaneContract
           << "\",\"deterministic_handoff\":"
           << (property_synthesis_ivar_binding_contract.deterministic ? "true" : "false")
           << "},\n";
  std::string runtime_state_publication_emit_prefix = "module";
  if (runtime_translation_unit_registration_manifest
          .manifest_artifact_relative_path.ends_with(
              kObjc3RuntimeTranslationUnitRegistrationManifestArtifactSuffix)) {
    runtime_state_publication_emit_prefix =
        runtime_translation_unit_registration_manifest
            .manifest_artifact_relative_path.substr(
                0,
                runtime_translation_unit_registration_manifest
                        .manifest_artifact_relative_path.size() -
                    std::char_traits<char>::length(
                        kObjc3RuntimeTranslationUnitRegistrationManifestArtifactSuffix));
  }
  const auto accessor_storage_lowering_metadata_summary =
      BuildAccessorStorageLoweringMetadataSummary(runtime_metadata_source_records);
  const auto executable_accessor_layout_lowering_summary =
      BuildExecutableAccessorLayoutLoweringSummary(
          executable_metadata_source_graph);
  manifest << "  \"dispatch_and_synthesized_accessor_lowering_surface\":{\"contract_id\":"
           << "\"" << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId << "\""
           << ",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll\""
           << ",\"runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":"
           << runtime_link_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_dispatch_declaration_parameter_count\":"
           << runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_dispatch_symbol_matches_lowering\":"
           << ((runtime_link_host_link_contract.runtime_dispatch_symbol ==
                        options.lowering.runtime_dispatch_symbol &&
                runtime_link_host_link_contract.runtime_dispatch_symbol ==
                        runtime_support_library_link_wiring.runtime_dispatch_symbol)
                       ? "true"
                       : "false")
           << ",\"live_runtime_dispatch_sites\":"
           << (dispatch_surface_classification_contract.instance_dispatch_sites +
               dispatch_surface_classification_contract.class_dispatch_sites +
               dispatch_surface_classification_contract.super_dispatch_sites +
               dispatch_surface_classification_contract.dynamic_dispatch_sites)
           << ",\"direct_dispatch_sites\":"
           << dispatch_surface_classification_contract.direct_dispatch_sites
           << ",\"message_send_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << property_synthesis_ivar_binding_contract.interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << property_synthesis_ivar_binding_contract.implementation_property_redeclaration_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_resolved
           << ",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\""
           << ",\"accessor_storage_lowering_metadata_model\":\""
           << kObjc3AccessorStorageLoweringMetadataModel
           << "\",\"accessor_storage_lowering_helper_selection_model\":\""
           << kObjc3AccessorStorageLoweringHelperSelectionModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3\""
           << ",\"tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
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
           << ",\"no-sidecar-only-lowering-proof\"]"
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"synthesized_accessor_owner_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .synthesized_accessor_owner_entries
           << ",\"synthesized_getter_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .synthesized_getter_entries
           << ",\"synthesized_setter_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .synthesized_setter_entries
           << ",\"current_property_read_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .current_property_read_entries
           << ",\"current_property_write_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .current_property_write_entries
           << ",\"current_property_exchange_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .current_property_exchange_entries
           << ",\"weak_current_property_load_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .weak_current_property_load_entries
           << ",\"weak_current_property_store_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .weak_current_property_store_entries
           << ",\"property_descriptor_count\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"ivar_descriptor_count\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << ",\"deterministic_handoff\":"
           << (property_synthesis_ivar_binding_contract.deterministic &&
                       dispatch_surface_classification_contract.deterministic &&
                       message_send_selector_lowering_contract.deterministic &&
                       runtime_link_host_link_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"dispatch_accessor_runtime_abi_surface\":{\"contract_id\":"
           << "\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"abi_boundary_model\":"
           << "\"public-dispatch-entrypoint-plus-private-testing-snapshot-and-property-helper-surface\""
           << ",\"public_header_path\":\"native/objc3c/src/runtime/objc3_runtime.h\""
           << ",\"private_header_path\":\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\"dispatch_state_snapshot_symbol\":\"objc3_runtime_copy_dispatch_state_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"arc_debug_state_snapshot_symbol\":\"objc3_runtime_copy_arc_debug_state_for_testing\""
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
           << "\",\"retain_symbol\":\"" << kObjc3RuntimeRetainI32Symbol
           << "\",\"release_symbol\":\"" << kObjc3RuntimeReleaseI32Symbol
           << "\",\"autorelease_symbol\":\""
           << kObjc3RuntimeAutoreleaseI32Symbol
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << ((property_synthesis_ivar_binding_contract.deterministic &&
                dispatch_surface_classification_contract.deterministic &&
                message_send_selector_lowering_contract.deterministic &&
                runtime_link_host_link_contract.deterministic)
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"storage_accessor_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\",\"abi_boundary_model\":\"private-bootstrap-internal-property-helper-and-reflection-snapshot-surface-without-public-header-widening\""
           << ",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"private_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
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
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << ((property_synthesis_ivar_binding_contract.deterministic &&
                runtime_link_host_link_contract.deterministic)
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"runtime_state_publication_surface\":{\"contract_id\":\""
           << kObjc3RuntimeStatePublicationSurfaceContractId
           << "\",\"publication_surface_kind\":"
           << "\"compile-manifest-plus-registration-manifest\""
           << ",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"runtime_state_snapshot_symbol\":\""
           << runtime_bootstrap_semantics.runtime_state_snapshot_symbol
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol
           << "\"],\"class_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.class_descriptor_count
           << ",\"protocol_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.protocol_descriptor_count
           << ",\"category_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.category_descriptor_count
           << ",\"property_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.property_descriptor_count
           << ",\"ivar_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.ivar_descriptor_count
           << ",\"total_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.total_descriptor_count
           << ",\"publication_requires_coupled_registration_manifest\":true"
           << ",\"publication_requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_bootstrap_registration_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrationSourceSurfaceContractId
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
           << runtime_registration_descriptor_image_root_source_surface.contract_id
           << "\",\"frontend_closure_contract_id\":\""
           << runtime_registration_descriptor_frontend_closure.contract_id
           << "\",\"registration_manifest_contract_id\":\""
           << runtime_translation_unit_registration_manifest.contract_id
           << "\",\"bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_descriptor_identifier\":\""
           << runtime_registration_descriptor_frontend_closure
                  .registration_descriptor_identifier
           << "\",\"image_root_identifier\":\""
           << runtime_registration_descriptor_frontend_closure
                  .image_root_identifier
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"constructor_root_symbol\":\""
           << runtime_translation_unit_registration_manifest.constructor_root_symbol
           << "\",\"translation_unit_identity_key\":\""
           << runtime_bootstrap_legality_semantics.translation_unit_identity_key
           << "\",\"translation_unit_registration_order_ordinal\":"
           << runtime_translation_unit_registration_manifest
                  .translation_unit_registration_order_ordinal
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_bootstrap_lowering_registration_artifact_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBootstrapLoweringRegistrationArtifactSurfaceContractId
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
           << "\",\"bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"registration_manifest_contract_id\":\""
           << runtime_translation_unit_registration_manifest.contract_id
           << "\",\"bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_semantics.contract_id
           << "\",\"registration_descriptor_frontend_closure_contract_id\":\""
           << runtime_registration_descriptor_frontend_closure.contract_id
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"constructor_root_symbol\":\""
           << runtime_bootstrap_lowering.constructor_root_symbol
           << "\",\"init_stub_symbol_prefix\":\""
           << runtime_bootstrap_lowering.constructor_init_stub_symbol_prefix
           << "\",\"registration_table_symbol_prefix\":\""
           << runtime_bootstrap_lowering.registration_table_symbol_prefix
           << "\",\"image_local_init_state_symbol_prefix\":\""
           << runtime_bootstrap_lowering.image_local_init_state_symbol_prefix
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_bootstrap_lowering.registration_entrypoint_symbol
           << "\",\"global_ctor_list_model\":\""
           << runtime_bootstrap_lowering.global_ctor_list_model
           << "\",\"registration_table_layout_model\":\""
           << runtime_bootstrap_lowering.registration_table_layout_model
           << "\",\"image_local_initialization_model\":\""
           << runtime_bootstrap_lowering.image_local_initialization_model
           << "\",\"registration_table_abi_version\":"
           << runtime_bootstrap_lowering.registration_table_abi_version
           << ",\"registration_table_pointer_field_count\":"
           << runtime_bootstrap_lowering
                  .registration_table_pointer_field_count
           << ",\"constructor_root_emission_state\":\""
           << runtime_bootstrap_lowering.constructor_root_emission_state
           << "\",\"init_stub_emission_state\":\""
           << runtime_bootstrap_lowering.init_stub_emission_state
           << "\",\"registration_table_emission_state\":\""
           << runtime_bootstrap_lowering.registration_table_emission_state
           << "\",\"lowered_registration_descriptor_fields\":["
           << "\"constructor_init_stub_symbol\","
           << "\"bootstrap_registration_table_symbol\","
           << "\"bootstrap_image_local_init_state_symbol\","
           << "\"bootstrap_registration_table_layout_model\","
           << "\"bootstrap_image_local_initialization_model\","
           << "\"bootstrap_registration_table_abi_version\","
           << "\"bootstrap_registration_table_pointer_field_count\","
           << "\"translation_unit_registration_order_ordinal\""
           << "],\"loader_table_ir_proof_fields\":["
           << "\"constructor_root_symbol\","
           << "\"constructor_init_stub_symbol\","
           << "\"bootstrap_registration_table_symbol\","
           << "\"bootstrap_image_local_init_state_symbol\","
           << "\"translation_unit_registration_order_ordinal\""
           << "]"
           << ",\"bootstrap_ir_materialization_landed\":"
           << (runtime_bootstrap_lowering.bootstrap_ir_materialization_landed
                   ? "true"
                   : "false")
           << ",\"image_local_initialization_landed\":"
           << (runtime_bootstrap_lowering.image_local_initialization_landed
                   ? "true"
                   : "false")
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_emitted_loader_table_ir\":true"
           << "},\n";
  manifest << "  \"runtime_multi_image_startup_ordering_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeMultiImageStartupOrderingSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"bootstrap_legality_semantics_contract_id\":\""
           << runtime_bootstrap_legality_semantics.contract_id
           << "\",\"bootstrap_failure_restart_contract_id\":\""
           << runtime_bootstrap_failure_restart_semantics.contract_id
           << "\",\"bootstrap_api_contract_id\":\""
           << runtime_bootstrap_api.contract_id
           << "\",\"bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"archive_static_link_replay_corpus_contract_id\":\""
           << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_bootstrap_api.registration_entrypoint_symbol
           << "\",\"state_snapshot_symbol\":\""
           << runtime_bootstrap_api.state_snapshot_symbol
           << "\",\"reset_for_testing_symbol\":\""
           << runtime_bootstrap_api.reset_for_testing_symbol
           << "\",\"replay_registered_images_symbol\":\""
           << runtime_bootstrap_failure_restart_semantics
                  .replay_registered_images_symbol
           << "\",\"reset_replay_state_snapshot_symbol\":\""
           << runtime_bootstrap_failure_restart_semantics
                  .reset_replay_state_snapshot_symbol
           << "\",\"translation_unit_identity_key\":\""
           << runtime_bootstrap_legality_semantics.translation_unit_identity_key
           << "\",\"translation_unit_registration_order_ordinal\":"
           << runtime_bootstrap_legality_semantics
                  .translation_unit_registration_order_ordinal
           << ",\"duplicate_registration_status_code\":"
           << runtime_bootstrap_semantics.duplicate_registration_status_code
           << ",\"out_of_order_status_code\":"
           << runtime_bootstrap_semantics.out_of_order_status_code
           << ",\"duplicate_install_diagnostic_model\":\""
           << kObjc3RuntimeBootstrapDuplicateInstallDiagnosticModel
           << "\",\"out_of_order_install_diagnostic_model\":\""
           << kObjc3RuntimeBootstrapOutOfOrderDiagnosticModel
           << "\",\"last_rejected_module_name_field\":\""
           << kObjc3RuntimeBootstrapRejectedModuleNameField
           << "\",\"last_rejected_translation_unit_identity_key_field\":\""
           << kObjc3RuntimeBootstrapRejectedTranslationUnitIdentityKeyField
           << "\",\"next_expected_registration_order_field\":\""
           << kObjc3RuntimeBootstrapNextExpectedRegistrationOrderField
           << "\",\"last_successful_registration_order_field\":\""
           << kObjc3RuntimeBootstrapLastSuccessfulRegistrationOrderField
           << "\",\"last_rejected_registration_order_field\":\""
           << kObjc3RuntimeBootstrapLastRejectedRegistrationOrderField
           << "\",\"requires_linked_runtime_probe\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_object_model_realization_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
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
           << "\",\"executable_realization_records_contract_id\":\""
           << kObjc3ExecutableRealizationRecordsContractId
           << "\",\"runtime_class_realization_contract_id\":\""
           << kObjc3RuntimeClassRealizationContractId
           << "\",\"runtime_metaclass_graph_contract_id\":\""
           << kObjc3RuntimeMetaclassGraphRootClassContractId
           << "\",\"runtime_category_attachment_protocol_conformance_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId
           << "\",\"canonical_runnable_object_support_contract_id\":\""
           << kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol

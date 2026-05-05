      << (summary.diagnostic_taxonomy_ready ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingLegacyCanonicalMigrationSemanticsReplayKey(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ";legacy-sites=" << summary.current_run_legacy_literal_sites
      << ";candidates=" << summary.current_run_canonicalization_candidate_sites
      << ";families=" << summary.fixit_family_count
      << ";fail-closed=" << (summary.fail_closed ? "true" : "false")
      << ";rejection-code=" << summary.canonical_mode_rejection_code
      << ";ready="
      << (summary.ready_for_lowering_and_runtime ? "true" : "false");
  return out.str();
}

Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
BuildToolingLegacyCanonicalMigrationSemanticsSummary(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &compatibility_summary,
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &fixit_summary) {
  Objc3ToolingLegacyCanonicalMigrationSemanticsSummary summary;
  summary.effective_language_profile =
      compatibility_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled =
      compatibility_summary.canonical_literal_rejection_diagnostics_enabled;
  summary.current_run_legacy_literal_sites =
      fixit_summary.migration_fixit_candidate_sites;
  summary.current_run_canonicalization_candidate_sites =
      fixit_summary.migration_fixit_candidate_sites;
  summary.fixit_family_count = fixit_summary.fixit_family_count;
  summary.fail_closed = compatibility_summary.fail_closed;
  summary.selected_configuration_valid =
      compatibility_summary.selected_configuration_valid;
  summary.language_profile_semantics_landed =
      compatibility_summary.language_profile_semantics_landed;
  summary.canonical_literal_rejection_semantics_landed =
      compatibility_summary.canonical_literal_rejection_semantics_landed;
  summary.canonical_mode_rejection_ready =
      compatibility_summary.fail_closed &&
      compatibility_summary.language_profile_semantics_landed &&
      compatibility_summary.canonical_literal_rejection_semantics_landed &&
      summary.canonical_mode_rejection_code ==
          kObjc3ToolingLegacyCanonicalMigrationDiagnosticCode;
  summary.feature_specific_fixit_surface_ready =
      fixit_summary.ready_for_lowering_and_runtime;
  summary.deterministic_handoff =
      IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
          compatibility_summary) &&
      summary.feature_specific_fixit_surface_ready &&
      summary.selected_configuration_valid &&
      !compatibility_summary.selected_configuration_downgraded &&
      !compatibility_summary.selected_configuration_rejected &&
      summary.canonical_mode_rejection_ready;
  summary.ready_for_lowering_and_runtime = summary.deterministic_handoff;
  if (!summary.feature_specific_fixit_surface_ready) {
    summary.failure_reason =
        "tooling feature-specific fix-it synthesis prerequisite is not ready";
  } else if (!IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
                 compatibility_summary)) {
    summary.failure_reason =
        "frontend compatibility/strictness semantics prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingLegacyCanonicalMigrationSemanticsReplayKey(summary);
  return summary;
}

std::string BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"compatibility_semantics_contract_id\":\""
      << EscapeJsonString(summary.compatibility_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"language_profile_model\":\""
      << EscapeJsonString(summary.language_profile_model)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ",\"current_run_legacy_literal_sites\":"
      << summary.current_run_legacy_literal_sites
      << ",\"current_run_canonicalization_candidate_sites\":"
      << summary.current_run_canonicalization_candidate_sites
      << ",\"fixit_family_count\":" << summary.fixit_family_count
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"selected_configuration_valid\":"
      << (summary.selected_configuration_valid ? "true" : "false")
      << ",\"language_profile_semantics_landed\":"
      << (summary.language_profile_semantics_landed ? "true" : "false")
      << ",\"canonical_literal_rejection_semantics_landed\":"
      << (summary.canonical_literal_rejection_semantics_landed ? "true" : "false")
      << ",\"canonical_mode_rejection_code\":\""
      << EscapeJsonString(summary.canonical_mode_rejection_code)
      << "\",\"canonical_mode_rejection_ready\":"
      << (summary.canonical_mode_rejection_ready ? "true" : "false")
      << ",\"feature_specific_fixit_surface_ready\":"
      << (summary.feature_specific_fixit_surface_ready ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropInteropSemanticModelSummaryJson(
    const Objc3InteropInteropSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"foreign_callable_sites\":" << summary.foreign_callable_sites
      << ",\"import_module_annotation_sites\":"
      << summary.import_module_annotation_sites
      << ",\"imported_module_name_sites\":"
      << summary.imported_module_name_sites
      << ",\"export_header_annotation_sites\":"
      << summary.export_header_annotation_sites
      << ",\"export_header_name_sites\":"
      << summary.export_header_name_sites
      << ",\"mixed_image_annotation_sites\":"
      << summary.mixed_image_annotation_sites
      << ",\"mixed_image_name_sites\":"
      << summary.mixed_image_name_sites
      << ",\"package_entry_annotation_sites\":"
      << summary.package_entry_annotation_sites
      << ",\"package_entry_name_sites\":"
      << summary.package_entry_name_sites
      << ",\"swift_name_annotation_sites\":"
      << summary.swift_name_annotation_sites
      << ",\"swift_private_annotation_sites\":"
      << summary.swift_private_annotation_sites
      << ",\"cpp_name_annotation_sites\":"
      << summary.cpp_name_annotation_sites
      << ",\"header_name_annotation_sites\":"
      << summary.header_name_annotation_sites
      << ",\"abi_alignment_annotation_sites\":"
      << summary.abi_alignment_annotation_sites
      << ",\"foreign_type_annotation_sites\":"
      << summary.foreign_type_annotation_sites
      << ",\"named_annotation_payload_sites\":"
      << summary.named_annotation_payload_sites
      << ",\"retainable_family_callable_sites\":"
      << summary.retainable_family_callable_sites
      << ",\"bridge_callable_sites\":" << summary.bridge_callable_sites
      << ",\"async_executor_affinity_sites\":"
      << summary.async_executor_affinity_sites
      << ",\"actor_hazard_sites\":" << summary.actor_hazard_sites
      << ",\"interop_metadata_annotation_sites\":"
      << summary.interop_metadata_annotation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"foreign_annotation_source_supported\":"
      << (summary.foreign_annotation_source_supported ? "true" : "false")
      << ",\"ownership_interaction_profile_frozen\":"
      << (summary.ownership_interaction_profile_frozen ? "true" : "false")
      << ",\"error_bridge_profile_reused\":"
      << (summary.error_bridge_profile_reused ? "true" : "false")
      << ",\"async_affinity_profile_reused\":"
      << (summary.async_affinity_profile_reused ? "true" : "false")
      << ",\"actor_hazard_profile_reused\":"
      << (summary.actor_hazard_profile_reused ? "true" : "false")
      << ",\"metadata_payload_profile_frozen\":"
      << (summary.metadata_payload_profile_frozen ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropInteropRuntimeParitySummaryJson(
    const Objc3InteropInteropRuntimeParitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"foreign_callable_sites\":" << summary.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << summary.c_foreign_callable_sites
      << ",\"objc_method_foreign_callable_sites\":"
      << summary.objc_method_foreign_callable_sites
      << ",\"import_module_annotation_sites\":"
      << summary.import_module_annotation_sites
      << ",\"import_module_foreign_callable_sites\":"
      << summary.import_module_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << summary.objc_runtime_parity_callable_sites
      << ",\"foreign_definition_rejection_sites\":"
      << summary.foreign_definition_rejection_sites
      << ",\"import_without_foreign_rejection_sites\":"
      << summary.import_without_foreign_rejection_sites
      << ",\"implementation_annotation_rejection_sites\":"
      << summary.implementation_annotation_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"declaration_only_foreign_c_enforced\":"
      << (summary.declaration_only_foreign_c_enforced ? "true" : "false")
      << ",\"import_module_requires_foreign_enforced\":"
      << (summary.import_module_requires_foreign_enforced ? "true" : "false")
      << ",\"implementation_annotations_fail_closed\":"
      << (summary.implementation_annotations_fail_closed ? "true" : "false")
      << ",\"objc_runtime_parity_classified\":"
      << (summary.objc_runtime_parity_classified ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropCppInteropInteractionSummaryJson(
    const Objc3InteropCppInteropInteractionSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"cpp_interop_callable_sites\":"
      << summary.cpp_interop_callable_sites
      << ",\"cpp_named_callable_sites\":" << summary.cpp_named_callable_sites
      << ",\"header_named_callable_sites\":"
      << summary.header_named_callable_sites
      << ",\"ownership_interaction_sites\":"
      << summary.ownership_interaction_sites
      << ",\"throws_interaction_sites\":"
      << summary.throws_interaction_sites
      << ",\"async_interaction_sites\":"
      << summary.async_interaction_sites
      << ",\"ownership_rejection_sites\":"
      << summary.ownership_rejection_sites
      << ",\"throws_rejection_sites\":"
      << summary.throws_rejection_sites
      << ",\"async_rejection_sites\":"
      << summary.async_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"cpp_annotation_profile_reused\":"
      << (summary.cpp_annotation_profile_reused ? "true" : "false")
      << ",\"ownership_interactions_fail_closed\":"
      << (summary.ownership_interactions_fail_closed ? "true" : "false")
      << ",\"throws_interactions_fail_closed\":"
      << (summary.throws_interactions_fail_closed ? "true" : "false")
      << ",\"async_interactions_fail_closed\":"
      << (summary.async_interactions_fail_closed ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropSwiftInteropIsolationSummaryJson(
    const Objc3InteropSwiftInteropIsolationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"swift_interop_callable_sites\":"
      << summary.swift_interop_callable_sites
      << ",\"swift_named_callable_sites\":"
      << summary.swift_named_callable_sites
      << ",\"swift_private_callable_sites\":"
      << summary.swift_private_callable_sites
      << ",\"swift_private_without_name_sites\":"
      << summary.swift_private_without_name_sites
      << ",\"actor_owned_swift_callable_sites\":"
      << summary.actor_owned_swift_callable_sites
      << ",\"nonisolated_swift_callable_sites\":"
      << summary.nonisolated_swift_callable_sites
      << ",\"implementation_swift_callable_sites\":"
      << summary.implementation_swift_callable_sites
      << ",\"swift_private_without_name_rejection_sites\":"
      << summary.swift_private_without_name_rejection_sites
      << ",\"actor_isolation_mapping_rejection_sites\":"
      << summary.actor_isolation_mapping_rejection_sites
      << ",\"nonisolated_mapping_rejection_sites\":"
      << summary.nonisolated_mapping_rejection_sites
      << ",\"implementation_surface_rejection_sites\":"
      << summary.implementation_surface_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"swift_metadata_profile_reused\":"
      << (summary.swift_metadata_profile_reused ? "true" : "false")
      << ",\"swift_private_requires_name_enforced\":"
      << (summary.swift_private_requires_name_enforced ? "true" : "false")
      << ",\"actor_isolation_mapping_fail_closed\":"
      << (summary.actor_isolation_mapping_fail_closed ? "true" : "false")
      << ",\"nonisolated_mapping_fail_closed\":"
      << (summary.nonisolated_mapping_fail_closed ? "true" : "false")
      << ",\"implementation_surface_fail_closed\":"
      << (summary.implementation_surface_fail_closed ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"derive_marker_sites\":" << summary.derive_marker_sites
      << ",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
      << ",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"interface_property_behavior_sites\":"
      << summary.interface_property_behavior_sites
      << ",\"implementation_property_behavior_sites\":"
      << summary.implementation_property_behavior_sites
      << ",\"protocol_property_behavior_sites\":"
      << summary.protocol_property_behavior_sites
      << ",\"synthesized_binding_visible_sites\":"
      << summary.synthesized_binding_visible_sites
      << ",\"synthesized_getter_visible_sites\":"
      << summary.synthesized_getter_visible_sites
      << ",\"synthesized_setter_visible_sites\":"
      << summary.synthesized_setter_visible_sites
      << ",\"property_behavior_contract_violation_sites\":"
      << summary.property_behavior_contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"derive_macro_source_supported\":"
      << (summary.derive_macro_source_supported ? "true" : "false")
      << ",\"macro_package_provenance_surface_reused\":"
      << (summary.macro_package_provenance_surface_reused ? "true" : "false")
      << ",\"property_behavior_source_supported\":"
      << (summary.property_behavior_source_supported ? "true" : "false")
      << ",\"synthesized_visibility_surface_reused\":"
      << (summary.synthesized_visibility_surface_reused ? "true" : "false")
      << ",\"derive_synthesis_deferred\":"
      << (summary.derive_synthesis_deferred ? "true" : "false")
      << ",\"macro_execution_deferred\":"
      << (summary.macro_execution_deferred ? "true" : "false")
      << ",\"property_behavior_runtime_deferred\":"
      << (summary.property_behavior_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_core_implementation\":"
      << (summary.ready_for_core_implementation ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"derive_request_sites\":" << summary.derive_request_sites
      << ",\"supported_derive_request_sites\":"
      << summary.supported_derive_request_sites
      << ",\"unsupported_derive_request_sites\":"
      << summary.unsupported_derive_request_sites
      << ",\"unsupported_topology_sites\":"
      << summary.unsupported_topology_sites
      << ",\"equatable_alias_sites\":" << summary.equatable_alias_sites
      << ",\"equality_derive_sites\":" << summary.equality_derive_sites
      << ",\"hash_derive_sites\":" << summary.hash_derive_sites
      << ",\"debug_description_derive_sites\":"
      << summary.debug_description_derive_sites
      << ",\"selector_conflict_sites\":" << summary.selector_conflict_sites
      << ",\"generated_method_entry_count\":"
      << summary.generated_method_entry_count
      << ",\"expansion_inventory_rows_lexicographic\":"
      << BuildStringArrayJson(summary.expansion_inventory_rows_lexicographic)
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"supported_derive_inventory_landed\":"
      << (summary.supported_derive_inventory_landed ? "true" : "false")
      << ",\"unsupported_derive_fail_closed\":"
      << (summary.unsupported_derive_fail_closed ? "true" : "false")
      << ",\"unsupported_topology_fail_closed\":"
      << (summary.unsupported_topology_fail_closed ? "true" : "false")
      << ",\"selector_conflicts_fail_closed\":"
      << (summary.selector_conflicts_fail_closed ? "true" : "false")
      << ",\"runtime_materialization_deferred\":"
      << (summary.runtime_materialization_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
      << ",\"safe_macro_callable_sites\":"
      << summary.safe_macro_callable_sites
      << ",\"incomplete_macro_metadata_sites\":"
      << summary.incomplete_macro_metadata_sites
      << ",\"orphan_macro_metadata_sites\":"
      << summary.orphan_macro_metadata_sites
      << ",\"invalid_package_sites\":" << summary.invalid_package_sites
      << ",\"invalid_provenance_sites\":"
      << summary.invalid_provenance_sites
      << ",\"nondeterministic_callable_sites\":"
      << summary.nondeterministic_callable_sites
      << ",\"unsupported_callable_topology_sites\":"
      << summary.unsupported_callable_topology_sites
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"metadata_completeness_enforced\":"
      << (summary.metadata_completeness_enforced ? "true" : "false")
      << ",\"sandbox_namespace_enforced\":"
      << (summary.sandbox_namespace_enforced ? "true" : "false")
      << ",\"provenance_determinism_enforced\":"
      << (summary.provenance_determinism_enforced ? "true" : "false")
      << ",\"callable_determinism_enforced\":"
      << (summary.callable_determinism_enforced ? "true" : "false")
      << ",\"macro_execution_deferred\":"
      << (summary.macro_execution_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"supported_behavior_sites\":" << summary.supported_behavior_sites
      << ",\"unsupported_behavior_sites\":" << summary.unsupported_behavior_sites
      << ",\"observed_behavior_sites\":" << summary.observed_behavior_sites
      << ",\"projected_behavior_sites\":" << summary.projected_behavior_sites
      << ",\"observed_on_protocol_sites\":" << summary.observed_on_protocol_sites
      << ",\"observed_readonly_conflict_sites\":"
      << summary.observed_readonly_conflict_sites
      << ",\"projected_writable_conflict_sites\":"
      << summary.projected_writable_conflict_sites
      << ",\"non_object_behavior_sites\":" << summary.non_object_behavior_sites
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"supported_behavior_inventory_landed\":"
      << (summary.supported_behavior_inventory_landed ? "true" : "false")
      << ",\"unsupported_behavior_fail_closed\":"
      << (summary.unsupported_behavior_fail_closed ? "true" : "false")
      << ",\"owner_topology_fail_closed\":"
      << (summary.owner_topology_fail_closed ? "true" : "false")
      << ",\"interaction_legality_fail_closed\":"
      << (summary.interaction_legality_fail_closed ? "true" : "false")
      << ",\"storage_legality_fail_closed\":"
      << (summary.storage_legality_fail_closed ? "true" : "false")
      << ",\"runtime_materialization_deferred\":"
      << (summary.runtime_materialization_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchIntentSemanticModelSummaryJson(
    const Objc3DispatchDispatchIntentSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"prefixed_container_attribute_sites\":"
      << summary.prefixed_container_attribute_sites
      << ",\"direct_members_container_sites\":"
      << summary.direct_members_container_sites
      << ",\"final_container_sites\":" << summary.final_container_sites
      << ",\"sealed_container_sites\":" << summary.sealed_container_sites
      << ",\"effective_direct_member_sites\":"
      << summary.effective_direct_member_sites
      << ",\"direct_members_defaulted_method_sites\":"
      << summary.direct_members_defaulted_method_sites
      << ",\"direct_members_dynamic_opt_out_sites\":"
      << summary.direct_members_dynamic_opt_out_sites
      << ",\"override_lookup_sites\":" << summary.override_lookup_sites
      << ",\"override_lookup_hits\":" << summary.override_lookup_hits
      << ",\"override_lookup_misses\":" << summary.override_lookup_misses
      << ",\"override_conflicts\":" << summary.override_conflicts
      << ",\"unresolved_base_interfaces\":"
      << summary.unresolved_base_interfaces
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"dispatch_intent_source_supported\":"
      << (summary.dispatch_intent_source_supported ? "true" : "false")
      << ",\"override_semantic_surface_reused\":"
      << (summary.override_semantic_surface_reused ? "true" : "false")
      << ",\"direct_dispatch_reserved_non_goal\":"
      << (summary.direct_dispatch_reserved_non_goal ? "true" : "false")
      << ",\"final_sealed_enforcement_deferred\":"
      << (summary.final_sealed_enforcement_deferred ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_core_implementation\":"
      << (summary.ready_for_core_implementation ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchIntentLegalitySummaryJson(
    const Objc3DispatchDispatchIntentLegalitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"subclass_sites\":" << summary.subclass_sites
      << ",\"override_sites\":" << summary.override_sites
      << ",\"illegal_final_superclass_sites\":"
      << summary.illegal_final_superclass_sites
      << ",\"illegal_sealed_superclass_sites\":"
      << summary.illegal_sealed_superclass_sites
      << ",\"illegal_final_override_sites\":"
      << summary.illegal_final_override_sites
      << ",\"illegal_direct_override_sites\":"
      << summary.illegal_direct_override_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"final_superclass_fail_closed\":"
      << (summary.final_superclass_fail_closed ? "true" : "false")
      << ",\"sealed_superclass_fail_closed\":"
      << (summary.sealed_superclass_fail_closed ? "true" : "false")
      << ",\"final_override_fail_closed\":"
      << (summary.final_override_fail_closed ? "true" : "false")
      << ",\"direct_override_fail_closed\":"
      << (summary.direct_override_fail_closed ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchIntentCompatibilitySummaryJson(
    const Objc3DispatchDispatchIntentCompatibilitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"callable_dispatch_intent_sites\":"
      << summary.callable_dispatch_intent_sites
      << ",\"container_dispatch_intent_sites\":"
      << summary.container_dispatch_intent_sites
      << ",\"illegal_direct_dynamic_conflict_sites\":"
      << summary.illegal_direct_dynamic_conflict_sites
      << ",\"illegal_final_dynamic_conflict_sites\":"
      << summary.illegal_final_dynamic_conflict_sites
      << ",\"illegal_non_method_callable_sites\":"
      << summary.illegal_non_method_callable_sites
      << ",\"illegal_protocol_method_sites\":"
      << summary.illegal_protocol_method_sites
      << ",\"illegal_category_method_sites\":"
      << summary.illegal_category_method_sites
      << ",\"illegal_category_container_sites\":"
      << summary.illegal_category_container_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"callable_conflict_fail_closed\":"
      << (summary.callable_conflict_fail_closed ? "true" : "false")
      << ",\"unsupported_callable_topology_fail_closed\":"
      << (summary.unsupported_callable_topology_fail_closed ? "true"
                                                            : "false")
      << ",\"unsupported_container_topology_fail_closed\":"
      << (summary.unsupported_container_topology_fail_closed ? "true"
                                                             : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"actor_interface_sites\":" << summary.actor_interface_sites
      << ",\"actor_method_sites\":" << summary.actor_method_sites
      << ",\"actor_property_sites\":" << summary.actor_property_sites
      << ",\"objc_nonisolated_annotation_sites\":"
      << summary.objc_nonisolated_annotation_sites
      << ",\"actor_member_executor_annotation_sites\":"
      << summary.actor_member_executor_annotation_sites
      << ",\"actor_async_method_sites\":" << summary.actor_async_method_sites
      << ",\"actor_member_metadata_sites\":"
      << summary.actor_member_metadata_sites
      << ",\"actor_declaration_source_supported\":"
      << (summary.actor_declaration_source_supported ? "true" : "false")
      << ",\"actor_member_source_supported\":"
      << (summary.actor_member_source_supported ? "true" : "false")
      << ",\"isolation_annotation_source_supported\":"
      << (summary.isolation_annotation_source_supported ? "true" : "false")
      << ",\"actor_metadata_surface_supported\":"
      << (summary.actor_metadata_surface_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_attribute_sites\":" << summary.executor_attribute_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"task_group_scope_sites\":" << summary.task_group_scope_sites
      << ",\"task_group_add_task_sites\":" << summary.task_group_add_task_sites
      << ",\"task_group_wait_next_sites\":" << summary.task_group_wait_next_sites
      << ",\"task_group_cancel_all_sites\":" << summary.task_group_cancel_all_sites
      << ",\"cancellation_check_sites\":" << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":" << summary.cancellation_handler_sites
      << ",\"task_creation_source_supported\":"
      << (summary.task_creation_source_supported ? "true" : "false")
      << ",\"task_group_source_supported\":"
      << (summary.task_group_source_supported ? "true" : "false")
      << ",\"cancellation_source_supported\":"
      << (summary.cancellation_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildTypeSystemTypeSemanticModelSummaryJson(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"optional_binding_sites\":" << summary.optional_binding_sites
      << ",\"optional_binding_clause_sites\":"
      << summary.optional_binding_clause_sites
      << ",\"guard_binding_sites\":" << summary.guard_binding_sites
      << ",\"optional_send_sites\":" << summary.optional_send_sites
      << ",\"nil_coalescing_sites\":" << summary.nil_coalescing_sites
      << ",\"optional_propagation_sites\":"
      << summary.optional_propagation_sites
      << ",\"optional_flow_refinement_sites\":"
      << summary.optional_flow_refinement_sites
      << ",\"guard_binding_exit_enforcement_sites\":"
      << summary.guard_binding_exit_enforcement_sites
      << ",\"typed_keypath_literal_sites\":"
      << summary.typed_keypath_literal_sites
      << ",\"typed_keypath_self_root_sites\":"
      << summary.typed_keypath_self_root_sites
      << ",\"typed_keypath_class_root_sites\":"
      << summary.typed_keypath_class_root_sites
      << ",\"object_pointer_semantic_sites\":"
      << summary.object_pointer_semantic_sites
      << ",\"protocol_composition_semantic_sites\":"
      << summary.protocol_composition_semantic_sites
      << ",\"generic_suffix_semantic_sites\":"
      << summary.generic_suffix_semantic_sites
      << ",\"generic_erasure_semantic_sites\":"
      << summary.generic_erasure_semantic_sites
      << ",\"nullability_suffix_semantic_sites\":"
      << summary.nullability_suffix_semantic_sites
      << ",\"nullability_semantic_sites\":"
      << summary.nullability_semantic_sites
      << ",\"canonical_type_entries\":"
      << summary.canonical_type_entries
      << ",\"canonical_object_type_entries\":"
      << summary.canonical_object_type_entries
      << ",\"canonical_protocol_qualified_entries\":"
      << summary.canonical_protocol_qualified_entries
      << ",\"canonical_generic_argument_entries\":"
      << summary.canonical_generic_argument_entries
      << ",\"canonical_nullable_entries\":"
      << summary.canonical_nullable_entries
      << ",\"canonical_nonnull_entries\":"
      << summary.canonical_nonnull_entries
      << ",\"canonical_implicitly_unwrapped_entries\":"
      << summary.canonical_implicitly_unwrapped_entries
      << ",\"canonical_null_resettable_entries\":"
      << summary.canonical_null_resettable_entries
      << ",\"canonical_unspecified_nullability_entries\":"
      << summary.canonical_unspecified_nullability_entries
      << ",\"canonical_invalid_type_entries\":"
      << summary.canonical_invalid_type_entries
      << ",\"invalid_generic_suffix_semantic_sites\":"
      << summary.invalid_generic_suffix_semantic_sites
      << ",\"invalid_nullability_suffix_semantic_sites\":"
      << summary.invalid_nullability_suffix_semantic_sites
      << ",\"invalid_protocol_composition_semantic_sites\":"
      << summary.invalid_protocol_composition_semantic_sites
      << ",\"optional_binding_contract_violation_sites\":"
      << summary.optional_binding_contract_violation_sites
      << ",\"optional_send_contract_violation_sites\":"
      << summary.optional_send_contract_violation_sites
      << ",\"optional_flow_contract_violation_sites\":"
      << summary.optional_flow_contract_violation_sites
      << ",\"typed_keypath_root_legality_violation_sites\":"
      << summary.typed_keypath_root_legality_violation_sites
      << ",\"typed_keypath_member_path_contract_violation_sites\":"
      << summary.typed_keypath_member_path_contract_violation_sites
      << ",\"typed_keypath_contract_violation_sites\":"
      << summary.typed_keypath_contract_violation_sites
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\"}";
  return out.str();
}

struct Objc3TypeSystemGenericContractInventory {
  std::size_t interface_count = 0;
  std::size_t generic_interface_count = 0;
  std::size_t generic_parameter_count = 0;
  std::size_t generic_variance_annotation_count = 0;
  std::size_t generic_argument_reference_count = 0;
  std::size_t protocol_qualified_generic_argument_count = 0;
};

bool IsProtocolQualifiedGenericArgumentSpelling(const std::string &argument) {
  return argument.find("id<") != std::string::npos ||
         argument.find("Class<") != std::string::npos;
}

void AccumulateGenericContractInventory(
    const Objc3SemanticCanonicalType &type,
    Objc3TypeSystemGenericContractInventory &inventory) {
  inventory.generic_argument_reference_count +=
      type.generic_arguments_source_order.size();
  for (const std::string &argument : type.generic_arguments_source_order) {
    if (IsProtocolQualifiedGenericArgumentSpelling(argument)) {
      ++inventory.protocol_qualified_generic_argument_count;
    }
  }
}

Objc3TypeSystemGenericContractInventory BuildTypeSystemGenericContractInventory(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3TypeSystemGenericContractInventory inventory;
  inventory.interface_count = handoff.interfaces_lexicographic.size();
  for (const Objc3SemanticInterfaceTypeMetadata &interface_metadata :
       handoff.interfaces_lexicographic) {
    if (!interface_metadata.generic_parameter_names_source_order.empty()) {
      ++inventory.generic_interface_count;
    }
    inventory.generic_parameter_count +=
        interface_metadata.generic_parameter_names_source_order.size();
    inventory.generic_variance_annotation_count +=
        interface_metadata.generic_parameter_variance_source_order.size();
    for (const Objc3SemanticPropertyTypeMetadata &property :
         interface_metadata.properties_lexicographic) {
      AccumulateGenericContractInventory(property.canonical_type, inventory);
    }
    for (const Objc3SemanticMethodTypeMetadata &method :
         interface_metadata.methods_lexicographic) {
      AccumulateGenericContractInventory(method.return_canonical_type,
                                         inventory);
      for (const Objc3SemanticCanonicalType &param :
           method.param_canonical_types) {
        AccumulateGenericContractInventory(param, inventory);
      }
    }
  }
  for (const Objc3SemanticFunctionTypeMetadata &function :
       handoff.functions_lexicographic) {
    AccumulateGenericContractInventory(function.return_canonical_type,
                                       inventory);
    for (const Objc3SemanticCanonicalType &param :
         function.param_canonical_types) {
      AccumulateGenericContractInventory(param, inventory);
    }
  }
  for (const Objc3SemanticImplementationTypeMetadata &implementation :
       handoff.implementations_lexicographic) {
    for (const Objc3SemanticPropertyTypeMetadata &property :
         implementation.properties_lexicographic) {
      AccumulateGenericContractInventory(property.canonical_type, inventory);
    }
    for (const Objc3SemanticMethodTypeMetadata &method :
         implementation.methods_lexicographic) {
      AccumulateGenericContractInventory(method.return_canonical_type,
                                         inventory);
      for (const Objc3SemanticCanonicalType &param :
           method.param_canonical_types) {
        AccumulateGenericContractInventory(param, inventory);
      }
    }
  }
  return inventory;
}

std::string BuildTypeSystemGenericContractPreservationReplayKey(
    const Objc3TypeSystemGenericContractInventory &inventory,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  std::ostringstream out;
  out << kObjc3TypeSystemGenericContractPreservationContractId
      << ";source_contract=" << kObjc3TypeSystemTypeSemanticModelContractId
      << ";type_semantic_replay=" << semantic_summary.replay_key
      << ";interfaces=" << inventory.interface_count
      << ";generic_interfaces=" << inventory.generic_interface_count
      << ";generic_parameters=" << inventory.generic_parameter_count
      << ";variance_annotations="
      << inventory.generic_variance_annotation_count
      << ";generic_argument_refs="
      << inventory.generic_argument_reference_count
      << ";protocol_qualified_generic_arguments="
      << inventory.protocol_qualified_generic_argument_count;
  return out.str();
}

std::string BuildTypeSystemGenericContractPreservationJson(
    const Objc3SemanticTypeMetadataHandoff &handoff,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  const Objc3TypeSystemGenericContractInventory inventory =
      BuildTypeSystemGenericContractInventory(handoff);
  const bool deterministic = IsDeterministicSemanticTypeMetadataHandoff(handoff);
  const bool ready =
      deterministic && semantic_summary.ready_for_lowering_and_runtime &&
      semantic_summary.deterministic && !semantic_summary.replay_key.empty();
  const std::string replay_key =
      BuildTypeSystemGenericContractPreservationReplayKey(inventory,
                                                          semantic_summary);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemGenericContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemTypeSemanticModelContractId)
      << "\",\"preservation_model\":\"runtime-import-surface-preserves-interface-generic-parameter-variance-adoption-and-specialization-reference-facts\""
      << ",\"interface_count\":" << inventory.interface_count
      << ",\"generic_interface_count\":" << inventory.generic_interface_count
      << ",\"generic_parameter_count\":" << inventory.generic_parameter_count
      << ",\"generic_variance_annotation_count\":"
      << inventory.generic_variance_annotation_count
      << ",\"generic_argument_reference_count\":"
      << inventory.generic_argument_reference_count
      << ",\"protocol_qualified_generic_argument_count\":"
      << inventory.protocol_qualified_generic_argument_count
      << ",\"ready\":" << (ready ? "true" : "false")
      << ",\"deterministic\":" << (deterministic ? "true" : "false")
      << ",\"type_semantic_replay_key\":\""
      << EscapeJsonString(semantic_summary.replay_key)
      << "\",\"generic_contract_interfaces\":[";
  for (std::size_t i = 0; i < handoff.interfaces_lexicographic.size(); ++i) {
    const Objc3SemanticInterfaceTypeMetadata &interface_metadata =
        handoff.interfaces_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"name\":\"" << EscapeJsonString(interface_metadata.name)
        << "\",\"super_name\":\""
        << EscapeJsonString(interface_metadata.super_name)
        << "\",\"generic_parameter_names_source_order\":"
        << BuildStringArrayJson(
               interface_metadata.generic_parameter_names_source_order)
        << ",\"generic_parameter_variance_source_order\":"
        << BuildStringArrayJson(
               interface_metadata.generic_parameter_variance_source_order)
        << ",\"adopted_protocols_lexicographic\":"
        << BuildStringArrayJson(
               interface_metadata.adopted_protocols_lexicographic)
        << "}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}

std::string BuildTypeSystemNullabilityContractPreservationReplayKey(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  std::ostringstream out;
  out << kObjc3TypeSystemNullabilityContractPreservationContractId
      << ";source_contract=" << kObjc3TypeSystemTypeSemanticModelContractId
      << ";type_semantic_replay=" << summary.replay_key
      << ";canonical_types=" << summary.canonical_type_entries
      << ";object_types=" << summary.canonical_object_type_entries
      << ";nullable=" << summary.canonical_nullable_entries
      << ";nonnull=" << summary.canonical_nonnull_entries
      << ";iuo=" << summary.canonical_implicitly_unwrapped_entries
      << ";null_resettable=" << summary.canonical_null_resettable_entries
      << ";unspecified=" << summary.canonical_unspecified_nullability_entries
      << ";invalid=" << summary.canonical_invalid_type_entries;
  return out.str();
}

std::string BuildTypeSystemNullabilityContractPreservationJson(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  const std::string replay_key =
      BuildTypeSystemNullabilityContractPreservationReplayKey(summary);
  const bool nullability_count_consistent =
      summary.canonical_type_entries ==
      summary.canonical_nullable_entries + summary.canonical_nonnull_entries +
          summary.canonical_implicitly_unwrapped_entries +
          summary.canonical_null_resettable_entries +
          summary.canonical_unspecified_nullability_entries;
  const bool ready = summary.ready_for_lowering_and_runtime &&
                     summary.deterministic &&
                     nullability_count_consistent &&
                     !summary.replay_key.empty();
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(
             kObjc3TypeSystemNullabilityContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemTypeSemanticModelContractId)
      << "\",\"preservation_model\":\"runtime-import-surface-preserves-canonical-nullability-counts-and-type-semantic-replay-boundary\""
      << ",\"canonical_type_count\":" << summary.canonical_type_entries
      << ",\"object_type_count\":" << summary.canonical_object_type_entries
      << ",\"nullable_entry_count\":" << summary.canonical_nullable_entries
      << ",\"nonnull_entry_count\":" << summary.canonical_nonnull_entries
      << ",\"implicitly_unwrapped_entry_count\":"
      << summary.canonical_implicitly_unwrapped_entries
      << ",\"null_resettable_entry_count\":"
      << summary.canonical_null_resettable_entries
      << ",\"unspecified_nullability_entry_count\":"
      << summary.canonical_unspecified_nullability_entries
      << ",\"invalid_nullability_entry_count\":"
      << summary.canonical_invalid_type_entries
      << ",\"ready\":" << (ready ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"type_semantic_replay_key\":\""
      << EscapeJsonString(summary.replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}

struct Objc3TypeSystemProtocolContractInventory {
  std::size_t protocol_decl_count = 0;
  std::size_t protocol_forward_declaration_count = 0;
  std::size_t protocol_inheritance_edge_count = 0;
  std::size_t protocol_required_method_count = 0;
  std::size_t protocol_optional_method_count = 0;
  std::size_t protocol_required_property_count = 0;
  std::size_t protocol_optional_property_count = 0;
  std::size_t class_protocol_adoption_count = 0;
  std::size_t category_protocol_adoption_count = 0;
};

std::string Objc3ProtocolRequirementKindName(
    Objc3ProtocolRequirementKind kind) {
  switch (kind) {
    case Objc3ProtocolRequirementKind::Required:
      return "required";
    case Objc3ProtocolRequirementKind::Optional:
      return "optional";
    case Objc3ProtocolRequirementKind::NotApplicable:
      break;
  }
  return "not-applicable";
}

Objc3TypeSystemProtocolContractInventory
BuildTypeSystemProtocolContractInventory(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_records) {
  Objc3TypeSystemProtocolContractInventory inventory;
  inventory.protocol_decl_count = program.protocols.size();
  for (const Objc3ProtocolDecl &protocol : program.protocols) {
    if (protocol.is_forward_declaration) {
      ++inventory.protocol_forward_declaration_count;
    }
    inventory.protocol_inheritance_edge_count +=
        protocol.inherited_protocols_lexicographic.size();
    for (const Objc3MethodDecl &method : protocol.methods) {
      if (method.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++inventory.protocol_optional_method_count;
      } else {
        ++inventory.protocol_required_method_count;
      }
    }
    for (const Objc3PropertyDecl &property : protocol.properties) {
      if (property.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++inventory.protocol_optional_property_count;
      } else {
        ++inventory.protocol_required_property_count;
      }
    }
  }
  for (const Objc3RuntimeMetadataClassSourceRecord &class_record :
       runtime_records.classes_lexicographic) {
    inventory.class_protocol_adoption_count +=
        class_record.adopted_protocols_lexicographic.size();
  }
  for (const Objc3RuntimeMetadataCategorySourceRecord &category_record :
       runtime_records.categories_lexicographic) {
    inventory.category_protocol_adoption_count +=
        category_record.adopted_protocols_lexicographic.size();
  }
  return inventory;
}

std::string BuildTypeSystemProtocolContractPreservationReplayKey(
    const Objc3TypeSystemProtocolContractInventory &inventory,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  std::ostringstream out;
  out << kObjc3TypeSystemProtocolContractPreservationContractId
      << ";source_contract=" << kObjc3TypeSystemTypeSemanticModelContractId
      << ";type_semantic_replay=" << semantic_summary.replay_key
      << ";protocols=" << inventory.protocol_decl_count
      << ";forward_declarations="
      << inventory.protocol_forward_declaration_count
      << ";inherited_edges=" << inventory.protocol_inheritance_edge_count
      << ";required_methods=" << inventory.protocol_required_method_count
      << ";optional_methods=" << inventory.protocol_optional_method_count
      << ";required_properties=" << inventory.protocol_required_property_count
      << ";optional_properties=" << inventory.protocol_optional_property_count
      << ";class_adoptions=" << inventory.class_protocol_adoption_count
      << ";category_adoptions=" << inventory.category_protocol_adoption_count;
  return out.str();
}

std::string BuildTypeSystemProtocolContractPreservationJson(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_records,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  const Objc3TypeSystemProtocolContractInventory inventory =
      BuildTypeSystemProtocolContractInventory(program, runtime_records);
  const bool deterministic = runtime_records.deterministic;
  const bool ready =
      deterministic && semantic_summary.ready_for_lowering_and_runtime &&
      semantic_summary.deterministic && !semantic_summary.replay_key.empty() &&
      inventory.protocol_decl_count >=
          inventory.protocol_forward_declaration_count;
  const std::string replay_key =
      BuildTypeSystemProtocolContractPreservationReplayKey(inventory,
                                                           semantic_summary);
  std::vector<const Objc3ProtocolDecl *> protocols;
  protocols.reserve(program.protocols.size());
  for (const Objc3ProtocolDecl &protocol : program.protocols) {
    protocols.push_back(&protocol);
  }
  std::sort(protocols.begin(), protocols.end(),
            [](const Objc3ProtocolDecl *lhs, const Objc3ProtocolDecl *rhs) {
              return lhs->name < rhs->name;
            });

  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(
             kObjc3TypeSystemProtocolContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemTypeSemanticModelContractId)
      << "\",\"preservation_model\":\"runtime-import-surface-preserves-protocol-requirement-partitions-inheritance-and-conformance-edges\""
      << ",\"protocol_decl_count\":" << inventory.protocol_decl_count
      << ",\"protocol_forward_declaration_count\":"
      << inventory.protocol_forward_declaration_count
      << ",\"protocol_inheritance_edge_count\":"
      << inventory.protocol_inheritance_edge_count
      << ",\"protocol_required_method_count\":"
      << inventory.protocol_required_method_count
      << ",\"protocol_optional_method_count\":"
      << inventory.protocol_optional_method_count
      << ",\"protocol_required_property_count\":"
      << inventory.protocol_required_property_count
      << ",\"protocol_optional_property_count\":"
      << inventory.protocol_optional_property_count
      << ",\"class_protocol_adoption_count\":"
      << inventory.class_protocol_adoption_count
      << ",\"category_protocol_adoption_count\":"
      << inventory.category_protocol_adoption_count
      << ",\"ready\":" << (ready ? "true" : "false")
      << ",\"deterministic\":" << (deterministic ? "true" : "false")
      << ",\"type_semantic_replay_key\":\""
      << EscapeJsonString(semantic_summary.replay_key)
      << "\",\"protocol_contract_protocols\":[";
  for (std::size_t i = 0; i < protocols.size(); ++i) {
    const Objc3ProtocolDecl &protocol = *protocols[i];
    if (i != 0u) {
      out << ",";
    }
    std::size_t required_methods = 0;
    std::size_t optional_methods = 0;
    for (const Objc3MethodDecl &method : protocol.methods) {
      if (method.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++optional_methods;
      } else {
        ++required_methods;
      }
    }
    std::size_t required_properties = 0;
    std::size_t optional_properties = 0;
    for (const Objc3PropertyDecl &property : protocol.properties) {
      if (property.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++optional_properties;
      } else {
        ++required_properties;
      }
    }
    out << "{\"name\":\"" << EscapeJsonString(protocol.name)
        << "\",\"is_forward_declaration\":"
        << (protocol.is_forward_declaration ? "true" : "false")
        << ",\"inherited_protocols_lexicographic\":"
        << BuildStringArrayJson(protocol.inherited_protocols_lexicographic)
        << ",\"required_method_count\":" << required_methods
        << ",\"optional_method_count\":" << optional_methods
        << ",\"required_property_count\":" << required_properties
        << ",\"optional_property_count\":" << optional_properties
        << ",\"methods\":[";
    for (std::size_t method_index = 0; method_index < protocol.methods.size();
         ++method_index) {
      const Objc3MethodDecl &method = protocol.methods[method_index];
      if (method_index != 0u) {
        out << ",";
      }
      out << "{\"selector\":\"" << EscapeJsonString(method.selector)
          << "\",\"requirement_kind\":\""
          << Objc3ProtocolRequirementKindName(method.protocol_requirement_kind)
          << "\",\"is_class_method\":"
          << (method.is_class_method ? "true" : "false") << "}";
    }
    out << "],\"properties\":[";
    for (std::size_t property_index = 0;
         property_index < protocol.properties.size(); ++property_index) {
      const Objc3PropertyDecl &property = protocol.properties[property_index];
      if (property_index != 0u) {
        out << ",";
      }
      out << "{\"name\":\"" << EscapeJsonString(property.name)
          << "\",\"requirement_kind\":\""
          << Objc3ProtocolRequirementKindName(
                 property.protocol_requirement_kind)
          << "\"}";
    }
    out << "]}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}

std::string BuildEffectsOwnershipSemanticModelSummaryJson(
    const Objc3EffectsOwnershipSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"arc_ownership_qualified_sites\":"
      << summary.arc_ownership_qualified_sites
      << ",\"retain_insertion_sites\":" << summary.retain_insertion_sites
      << ",\"release_insertion_sites\":" << summary.release_insertion_sites
      << ",\"autorelease_insertion_sites\":"
      << summary.autorelease_insertion_sites
      << ",\"weak_zeroing_sites\":" << summary.weak_zeroing_sites
      << ",\"unowned_reference_sites\":" << summary.unowned_reference_sites
      << ",\"weak_unowned_conflict_sites\":"
      << summary.weak_unowned_conflict_sites
      << ",\"autoreleasepool_scope_sites\":"
      << summary.autoreleasepool_scope_sites
      << ",\"cleanup_order_exit_sites\":" << summary.cleanup_order_exit_sites
      << ",\"block_literal_sites\":" << summary.block_literal_sites
      << ",\"stack_to_heap_promotion_sites\":"
      << summary.stack_to_heap_promotion_sites
      << ",\"byref_forwarding_cell_sites\":"
      << summary.byref_forwarding_cell_sites
      << ",\"copy_helper_required_sites\":"
      << summary.copy_helper_required_sites
      << ",\"dispose_helper_required_sites\":"
      << summary.dispose_helper_required_sites
      << ",\"copy_helper_symbolized_sites\":"
      << summary.copy_helper_symbolized_sites
      << ",\"dispose_helper_symbolized_sites\":"
      << summary.dispose_helper_symbolized_sites
      << ",\"captured_object_lifetime_sites\":"
      << summary.captured_object_lifetime_sites
      << ",\"throws_propagation_sites\":"
      << summary.throws_propagation_sites
      << ",\"unwind_cleanup_sites\":" << summary.unwind_cleanup_sites
      << ",\"bridged_error_sites\":" << summary.bridged_error_sites
      << ",\"nested_cleanup_sites\":" << summary.nested_cleanup_sites
      << ",\"foreign_boundary_sites\":" << summary.foreign_boundary_sites
      << ",\"async_continuation_sites\":"
      << summary.async_continuation_sites
      << ",\"continuation_resume_sites\":"
      << summary.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << summary.continuation_suspend_sites
      << ",\"async_state_machine_sites\":"
      << summary.async_state_machine_sites
      << ",\"cancellation_propagation_sites\":"
      << summary.cancellation_propagation_sites
      << ",\"actor_isolation_sites\":" << summary.actor_isolation_sites
      << ",\"actor_hop_sites\":" << summary.actor_hop_sites
      << ",\"sendability_check_sites\":" << summary.sendability_check_sites
      << ",\"reentrancy_policy_sites\":"
      << summary.reentrancy_policy_sites
      << ",\"imported_actor_api_sites\":"
      << summary.imported_actor_api_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"arc_semantics_landed\":"
      << (summary.arc_semantics_landed ? "true" : "false")
      << ",\"block_escape_semantics_landed\":"
      << (summary.block_escape_semantics_landed ? "true" : "false")
      << ",\"throws_cleanup_semantics_landed\":"
      << (summary.throws_cleanup_semantics_landed ? "true" : "false")
      << ",\"async_task_semantics_landed\":"
      << (summary.async_task_semantics_landed ? "true" : "false")
      << ",\"actor_semantics_landed\":"
      << (summary.actor_semantics_landed ? "true" : "false")
      << ",\"foreign_boundary_semantics_landed\":"
      << (summary.foreign_boundary_semantics_landed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
    const Objc3CrossModuleSemanticContractsDiagnosticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"module_import_graph_sites\":"
      << summary.module_import_graph_sites
      << ",\"import_edge_candidate_sites\":"
      << summary.import_edge_candidate_sites
      << ",\"namespace_segment_sites\":" << summary.namespace_segment_sites
      << ",\"object_pointer_type_sites\":"
      << summary.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << summary.pointer_declarator_sites
      << ",\"namespace_collision_shadowing_sites\":"
      << summary.namespace_collision_shadowing_sites
      << ",\"public_private_api_partition_sites\":"
      << summary.public_private_api_partition_sites
      << ",\"incremental_module_cache_invalidation_sites\":"
      << summary.incremental_module_cache_invalidation_sites
      << ",\"cross_module_conformance_sites\":"
      << summary.cross_module_conformance_sites
      << ",\"normalized_cross_module_sites\":"
      << summary.normalized_cross_module_sites
      << ",\"cache_invalidation_candidate_sites\":"
      << summary.cache_invalidation_candidate_sites
      << ",\"diagnostic_recovery_sites\":"
      << summary.diagnostic_recovery_sites
      << ",\"diagnostic_emit_sites\":" << summary.diagnostic_emit_sites
      << ",\"recovery_anchor_sites\":" << summary.recovery_anchor_sites
      << ",\"recovery_boundary_sites\":" << summary.recovery_boundary_sites
      << ",\"fail_closed_diagnostic_sites\":"
      << summary.fail_closed_diagnostic_sites
      << ",\"diagnostic_normalized_sites\":"
      << summary.diagnostic_normalized_sites
      << ",\"diagnostic_gate_blocked_sites\":"
      << summary.diagnostic_gate_blocked_sites
      << ",\"interop_import_module_annotation_sites\":"
      << summary.interop_import_module_annotation_sites
      << ",\"interop_imported_module_name_sites\":"

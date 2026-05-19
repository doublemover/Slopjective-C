Objc3InteropInteropSemanticModelSummary BuildInteropInteropSemanticModelSummary(
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &foreign_source_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_source_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &ownership_summary,
    const Objc3ErrorHandlingErrorBridgeLegalitySummary &error_summary,
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &async_summary,
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &actor_summary) {
  // semantic freeze anchor: Part 11 interop legality is modeled as a
  // single summary over already-landed Part 6/7/8 semantic surfaces plus the
  // new foreign/import and Swift/C++ annotation source state, while ABI
  // lowering and runnable bridge generation remain later work.
  Objc3InteropInteropSemanticModelSummary summary;
  summary.foreign_callable_sites = foreign_source_summary.foreign_callable_sites;
  summary.import_module_annotation_sites =
      foreign_source_summary.import_module_annotation_sites;
  summary.imported_module_name_sites =
      foreign_source_summary.imported_module_name_sites;
  summary.export_header_annotation_sites =
      foreign_source_summary.export_header_annotation_sites;
  summary.export_header_name_sites =
      foreign_source_summary.export_header_name_sites;
  summary.mixed_image_annotation_sites =
      foreign_source_summary.mixed_image_annotation_sites;
  summary.mixed_image_name_sites =
      foreign_source_summary.mixed_image_name_sites;
  summary.package_entry_annotation_sites =
      foreign_source_summary.package_entry_annotation_sites;
  summary.package_entry_name_sites =
      foreign_source_summary.package_entry_name_sites;
  summary.swift_name_annotation_sites =
      interop_source_summary.swift_name_annotation_sites;
  summary.swift_private_annotation_sites =
      interop_source_summary.swift_private_annotation_sites;
  summary.cpp_name_annotation_sites =
      interop_source_summary.cpp_name_annotation_sites;
  summary.header_name_annotation_sites =
      interop_source_summary.header_name_annotation_sites;
  summary.abi_alignment_annotation_sites =
      interop_source_summary.abi_alignment_annotation_sites;
  summary.foreign_type_annotation_sites =
      interop_source_summary.foreign_type_annotation_sites;
  summary.named_annotation_payload_sites =
      interop_source_summary.named_annotation_payload_sites;
  summary.retainable_family_callable_sites =
      ownership_summary.retainable_family_callable_sites;
  summary.bridge_callable_sites = error_summary.bridge_callable_sites;
  summary.async_executor_affinity_sites = async_summary.executor_affinity_sites;
  summary.actor_hazard_sites = actor_summary.task_handoff_sites;
  summary.interop_metadata_annotation_sites =
      foreign_source_summary.interop_annotation_sites +
      interop_source_summary.interop_metadata_annotation_sites;

  summary.source_dependency_required = true;
  summary.foreign_annotation_source_supported =
      foreign_source_summary.foreign_declaration_source_supported &&
      foreign_source_summary.imported_surface_source_supported &&
      foreign_source_summary.interop_annotation_source_supported &&
      interop_source_summary.swift_annotation_source_supported &&
      interop_source_summary.cpp_annotation_source_supported &&
      interop_source_summary.interop_metadata_source_supported &&
      foreign_source_summary.deterministic_handoff &&
      interop_source_summary.deterministic_handoff;
  summary.ownership_interaction_profile_frozen =
      IsReadyObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary(
          ownership_summary) &&
      summary.retainable_family_callable_sites >=
          ownership_summary.retainable_family_operation_callable_sites;
  summary.error_bridge_profile_reused =
      error_summary.source_dependency_required &&
      error_summary.bridge_legality_landed &&
      error_summary.try_bridge_filter_landed &&
      error_summary.unsupported_combinations_fail_closed &&
      error_summary.native_emit_remains_fail_closed &&
      error_summary.deterministic &&
      error_summary.failure_reason.empty() &&
      summary.bridge_callable_sites >=
          error_summary.try_eligible_bridge_callable_sites;
  summary.async_affinity_profile_reused =
      IsReadyObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummary(async_summary) &&
      summary.async_executor_affinity_sites >=
          async_summary.supported_async_callable_sites;
  summary.actor_hazard_profile_reused =
      IsReadyObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary(actor_summary) &&
      summary.actor_hazard_sites >=
          actor_summary.illegal_missing_actor_isolation_sites;
  summary.metadata_payload_profile_frozen =
      summary.imported_module_name_sites <=
          summary.import_module_annotation_sites &&
      summary.export_header_name_sites <=
          summary.export_header_annotation_sites &&
      summary.mixed_image_name_sites <=
          summary.mixed_image_annotation_sites &&
      summary.package_entry_name_sites <=
          summary.package_entry_annotation_sites &&
      summary.named_annotation_payload_sites ==
          summary.swift_name_annotation_sites +
              summary.cpp_name_annotation_sites +
              summary.header_name_annotation_sites +
              summary.foreign_type_annotation_sites;
  summary.ffi_abi_lowering_deferred = true;
  summary.runtime_bridge_generation_deferred = true;
  summary.deterministic =
      summary.foreign_annotation_source_supported &&
      summary.ownership_interaction_profile_frozen &&
      summary.error_bridge_profile_reused &&
      summary.async_affinity_profile_reused &&
      summary.actor_hazard_profile_reused &&
      summary.metadata_payload_profile_frozen;
  summary.ready_for_semantic_expansion = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "interop interop source, ownership, error, async, actor, and metadata profiles must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";foreign=" << summary.foreign_callable_sites
      << ";import-module=" << summary.import_module_annotation_sites << ":"
      << summary.imported_module_name_sites
      << ";export-header=" << summary.export_header_annotation_sites << ":"
      << summary.export_header_name_sites
      << ";mixed-image=" << summary.mixed_image_annotation_sites << ":"
      << summary.mixed_image_name_sites
      << ";package-entry=" << summary.package_entry_annotation_sites << ":"
      << summary.package_entry_name_sites
      << ";swift=" << summary.swift_name_annotation_sites << ":"
      << summary.swift_private_annotation_sites
      << ";cpp=" << summary.cpp_name_annotation_sites << ":"
      << summary.header_name_annotation_sites
      << ";abi-align=" << summary.abi_alignment_annotation_sites
      << ";foreign-type=" << summary.foreign_type_annotation_sites
      << ";named-payload=" << summary.named_annotation_payload_sites
      << ";retainable-family=" << summary.retainable_family_callable_sites
      << ";bridge=" << summary.bridge_callable_sites
      << ";async-affinity=" << summary.async_executor_affinity_sites
      << ";actor-hazard=" << summary.actor_hazard_sites
      << ";interop-metadata=" << summary.interop_metadata_annotation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

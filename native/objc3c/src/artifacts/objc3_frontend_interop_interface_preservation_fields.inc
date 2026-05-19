namespace {

std::size_t CountInteropInterfaceAnnotationSites(
    const Objc3InteropForeignSurfaceInterfacePreservationSummary &summary,
    bool imported) {
  if (imported) {
    return summary.imported_import_module_annotation_count +
           summary.imported_imported_module_name_count +
           summary.imported_swift_name_annotation_count +
           summary.imported_swift_private_annotation_count +
           summary.imported_cpp_name_annotation_count +
           summary.imported_header_name_annotation_count +
           summary.imported_named_annotation_payload_count;
  }
  return summary.local_import_module_annotation_count +
         summary.local_imported_module_name_count +
         summary.local_swift_name_annotation_count +
         summary.local_swift_private_annotation_count +
         summary.local_cpp_name_annotation_count +
         summary.local_header_name_annotation_count +
         summary.local_named_annotation_payload_count;
}

void ApplyInteropForeignSurfaceLocalPreservationFields(
    const Objc3Program &program,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &foreign_import_source_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &cpp_swift_source_summary,
    bool runtime_import_artifact_ready,
    Objc3InteropForeignSurfaceInterfacePreservationSummary &summary) {
  summary.foreign_import_source_replay_key =
      foreign_import_source_summary.replay_key;
  summary.cpp_swift_source_replay_key = cpp_swift_source_summary.replay_key;
  summary.local_import_module_names_lexicographic =
      CollectInteropLocalImportModuleNames(program);
  summary.local_foreign_callable_count =
      foreign_import_source_summary.foreign_callable_sites;
  summary.local_import_module_annotation_count =
      foreign_import_source_summary.import_module_annotation_sites;
  summary.local_imported_module_name_count =
      foreign_import_source_summary.imported_module_name_sites;
  summary.local_swift_name_annotation_count =
      cpp_swift_source_summary.swift_name_annotation_sites;
  summary.local_swift_private_annotation_count =
      cpp_swift_source_summary.swift_private_annotation_sites;
  summary.local_cpp_name_annotation_count =
      cpp_swift_source_summary.cpp_name_annotation_sites;
  summary.local_header_name_annotation_count =
      cpp_swift_source_summary.header_name_annotation_sites;
  summary.local_named_annotation_payload_count =
      cpp_swift_source_summary.named_annotation_payload_sites;
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready &&
      foreign_import_source_summary.ready_for_semantic_expansion &&
      cpp_swift_source_summary.ready_for_semantic_expansion;
  summary.deterministic =
      foreign_import_source_summary.deterministic_handoff &&
      cpp_swift_source_summary.deterministic_handoff &&
      summary.local_import_module_names_lexicographic.size() <=
          summary.local_imported_module_name_count;
}

void AccumulateInteropForeignSurfaceImportedPreservationFields(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3InteropForeignSurfaceInterfacePreservationSummary &summary) {
  if (!surface.interop_foreign_surface_interface_preservation_present) {
    return;
  }
  ++summary.imported_module_count;
  if (!surface.frontend_closure_summary.module_name.empty()) {
    summary.imported_provider_module_names_lexicographic.push_back(
        surface.frontend_closure_summary.module_name);
  }
  summary.imported_foreign_callable_count +=
      surface.interop_local_foreign_callable_count;
  summary.imported_import_module_annotation_count +=
      surface.interop_local_import_module_annotation_count;
  summary.imported_imported_module_name_count +=
      surface.interop_local_imported_module_name_count;
  summary.imported_swift_name_annotation_count +=
      surface.interop_local_swift_name_annotation_count;
  summary.imported_swift_private_annotation_count +=
      surface.interop_local_swift_private_annotation_count;
  summary.imported_cpp_name_annotation_count +=
      surface.interop_local_cpp_name_annotation_count;
  summary.imported_header_name_annotation_count +=
      surface.interop_local_header_name_annotation_count;
  summary.imported_named_annotation_payload_count +=
      surface.interop_local_named_annotation_payload_count;
  summary.deterministic =
      summary.deterministic && surface.interop_deterministic;
}

void ApplyInteropFfiLocalPreservationFields(
    const Objc3InteropForeignCallLifetimeLoweringContract &lowering_contract,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    bool runtime_import_artifact_ready,
    Objc3InteropFfiMetadataInterfacePreservationContract &contract) {
  contract.local_foreign_callable_count =
      lowering_contract.foreign_callable_sites;
  contract.local_metadata_preservation_sites =
      lowering_contract.metadata_preservation_sites;
  contract.local_interface_annotation_sites =
      CountInteropInterfaceAnnotationSites(preservation_summary, false);
  contract.runtime_import_artifact_ready =
      runtime_import_artifact_ready && lowering_contract.deterministic &&
      preservation_summary.runtime_import_artifact_ready;
  contract.deterministic =
      lowering_contract.deterministic && preservation_summary.deterministic;
}

void AccumulateInteropFfiImportedPreservationFields(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3InteropFfiMetadataInterfacePreservationContract &contract) {
  if (!surface.interop_ffi_metadata_interface_preservation_present) {
    return;
  }
  ++contract.imported_module_count;
  contract.imported_foreign_callable_count +=
      surface.interop_ffi_local_foreign_callable_count;
  contract.imported_metadata_preservation_sites +=
      surface.interop_ffi_local_metadata_preservation_sites;
  contract.imported_interface_annotation_sites +=
      surface.interop_ffi_local_interface_annotation_sites;
  contract.deterministic =
      contract.deterministic && surface.interop_ffi_deterministic;
}

void ApplyInteropHeaderBridgeLocalFields(
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &ffi_preservation_contract,
    const std::string &ffi_preservation_replay_key,
    Objc3InteropHeaderModuleBridgeGenerationSummary &summary) {
  summary.local_import_module_names_lexicographic =
      preservation_summary.local_import_module_names_lexicographic;
  summary.local_foreign_callable_count =
      ffi_preservation_contract.local_foreign_callable_count;
  summary.local_import_module_name_count =
      preservation_summary.local_imported_module_name_count;
  summary.local_cpp_name_annotation_count =
      preservation_summary.local_cpp_name_annotation_count;
  summary.local_header_name_annotation_count =
      preservation_summary.local_header_name_annotation_count;
  summary.local_swift_name_annotation_count =
      preservation_summary.local_swift_name_annotation_count;
  summary.runtime_generation_ready =
      ffi_preservation_contract.runtime_import_artifact_ready &&
      ffi_preservation_contract.separate_compilation_preservation_ready &&
      ffi_preservation_contract.deterministic &&
      summary.local_foreign_callable_count > 0;
  summary.deterministic =
      preservation_summary.deterministic &&
      ffi_preservation_contract.deterministic &&
      summary.local_import_module_names_lexicographic.size() <=
          summary.local_import_module_name_count;
  summary.preservation_replay_key = ffi_preservation_replay_key;
}

void AccumulateInteropHeaderBridgeImportedFields(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3InteropHeaderModuleBridgeGenerationSummary &summary) {
  if (!surface.interop_header_module_bridge_generation_present) {
    return;
  }
  ++summary.imported_module_count;
  if (!surface.frontend_closure_summary.module_name.empty()) {
    summary.imported_provider_module_names_lexicographic.push_back(
        surface.frontend_closure_summary.module_name);
  }
  summary.deterministic =
      summary.deterministic &&
      surface.interop_header_module_bridge_deterministic;
}

}  // namespace

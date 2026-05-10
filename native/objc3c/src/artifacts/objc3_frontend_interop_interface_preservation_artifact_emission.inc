Objc3InteropForeignSurfaceInterfacePreservationSummary
BuildInteropForeignSurfaceInterfacePreservationSummary(
    const Objc3Program &program,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &foreign_import_source_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &cpp_swift_source_summary,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3InteropForeignSurfaceInterfacePreservationSummary summary;
  ApplyInteropForeignSurfaceLocalPreservationFields(
      program,
      foreign_import_source_summary,
      cpp_swift_source_summary,
      runtime_import_artifact_ready,
      summary);
  for (const auto &surface : imported_runtime_module_surfaces) {
    AccumulateInteropForeignSurfaceImportedPreservationFields(surface, summary);
  }
  FinalizeInteropForeignSurfaceInterfacePreservationReadinessAndReplay(
      summary);
  return summary;
}

Objc3InteropFfiMetadataInterfacePreservationContract
BuildInteropFfiMetadataInterfacePreservationContract(
    const Objc3InteropForeignCallLifetimeLoweringContract &lowering_contract,
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    bool runtime_import_artifact_ready,
    std::string &replay_key_out) {
  Objc3InteropFfiMetadataInterfacePreservationContract contract;
  ApplyInteropFfiLocalPreservationFields(lowering_contract,
                                         preservation_summary,
                                         runtime_import_artifact_ready,
                                         contract);
  for (const auto &surface : imported_runtime_module_surfaces) {
    AccumulateInteropFfiImportedPreservationFields(surface, contract);
  }
  FinalizeInteropFfiPreservationReadinessAndReplay(lowering_replay_key,
                                                   preservation_summary,
                                                   contract,
                                                   replay_key_out);
  return contract;
}

Objc3InteropHeaderModuleBridgeGenerationSummary
BuildInteropHeaderModuleBridgeGenerationSummary(
    const Objc3Program &program,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &ffi_preservation_contract,
    const std::string &ffi_preservation_replay_key,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3InteropHeaderModuleBridgeGenerationSummary summary;
  ApplyInteropHeaderBridgeLocalFields(preservation_summary,
                                      ffi_preservation_contract,
                                      ffi_preservation_replay_key,
                                      summary);
  for (const auto &surface : imported_runtime_module_surfaces) {
    AccumulateInteropHeaderBridgeImportedFields(surface, summary);
  }
  FinalizeInteropHeaderBridgeReadinessAndReplay(summary);
  (void)program;
  return summary;
}

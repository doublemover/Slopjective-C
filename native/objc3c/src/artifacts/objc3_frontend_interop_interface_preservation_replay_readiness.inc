namespace {

void FinalizeInteropForeignSurfaceInterfacePreservationReadinessAndReplay(
    Objc3InteropForeignSurfaceInterfacePreservationSummary &summary) {
  std::sort(summary.imported_provider_module_names_lexicographic.begin(),
            summary.imported_provider_module_names_lexicographic.end());
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready &&
      summary.imported_provider_module_names_lexicographic.size() ==
          summary.imported_module_count;
  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_preservation_ready="
             << (summary.separate_compilation_preservation_ready ? "true"
                                                                 : "false")
             << ";imported_module_count=" << summary.imported_module_count
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";foreign_import_source_replay_key="
             << summary.foreign_import_source_replay_key
             << ";cpp_swift_source_replay_key="
             << summary.cpp_swift_source_replay_key
             << ";local_foreign_callable_count="
             << summary.local_foreign_callable_count
             << ";local_import_module_annotation_count="
             << summary.local_import_module_annotation_count
             << ";local_imported_module_name_count="
             << summary.local_imported_module_name_count
             << ";local_swift_name_annotation_count="
             << summary.local_swift_name_annotation_count
             << ";local_swift_private_annotation_count="
             << summary.local_swift_private_annotation_count
             << ";local_cpp_name_annotation_count="
             << summary.local_cpp_name_annotation_count
             << ";local_header_name_annotation_count="
             << summary.local_header_name_annotation_count
             << ";local_named_annotation_payload_count="
             << summary.local_named_annotation_payload_count
             << ";imported_foreign_callable_count="
             << summary.imported_foreign_callable_count
             << ";imported_import_module_annotation_count="
             << summary.imported_import_module_annotation_count
             << ";imported_imported_module_name_count="
             << summary.imported_imported_module_name_count
             << ";imported_swift_name_annotation_count="
             << summary.imported_swift_name_annotation_count
             << ";imported_swift_private_annotation_count="
             << summary.imported_swift_private_annotation_count
             << ";imported_cpp_name_annotation_count="
             << summary.imported_cpp_name_annotation_count
             << ";imported_header_name_annotation_count="
             << summary.imported_header_name_annotation_count
             << ";imported_named_annotation_payload_count="
             << summary.imported_named_annotation_payload_count;
  summary.replay_key = replay_key.str();
}

void FinalizeInteropFfiPreservationReadinessAndReplay(
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    Objc3InteropFfiMetadataInterfacePreservationContract &contract,
    std::string &replay_key_out) {
  contract.separate_compilation_preservation_ready =
      contract.runtime_import_artifact_ready;

  std::ostringstream replay_key;
  replay_key
      << Objc3InteropFfiMetadataInterfacePreservationReplayKey(contract)
      << ";lowering_replay_key=" << lowering_replay_key
      << ";preservation_replay_key=" << preservation_summary.replay_key;
  replay_key_out = replay_key.str();
}

void FinalizeInteropHeaderBridgeReadinessAndReplay(
    Objc3InteropHeaderModuleBridgeGenerationSummary &summary) {
  std::sort(summary.imported_provider_module_names_lexicographic.begin(),
            summary.imported_provider_module_names_lexicographic.end());
  summary.cross_module_packaging_ready =
      summary.runtime_generation_ready &&
      summary.imported_provider_module_names_lexicographic.size() ==
          summary.imported_module_count;

  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << ";source_contract_id=" << summary.source_contract_id
             << ";preservation_contract_id="
             << summary.preservation_contract_id
             << ";header_artifact_relative_path="
             << summary.header_artifact_relative_path
             << ";module_artifact_relative_path="
             << summary.module_artifact_relative_path
             << ";bridge_artifact_relative_path="
             << summary.bridge_artifact_relative_path
             << ";local_foreign_callable_count="
             << summary.local_foreign_callable_count
             << ";local_import_module_name_count="
             << summary.local_import_module_name_count
             << ";local_cpp_name_annotation_count="
             << summary.local_cpp_name_annotation_count
             << ";local_header_name_annotation_count="
             << summary.local_header_name_annotation_count
             << ";local_swift_name_annotation_count="
             << summary.local_swift_name_annotation_count
             << ";imported_module_count=" << summary.imported_module_count
             << ";runtime_generation_ready="
             << (summary.runtime_generation_ready ? "true" : "false")
             << ";cross_module_packaging_ready="
             << (summary.cross_module_packaging_ready ? "true" : "false")
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";preservation_replay_key="
             << summary.preservation_replay_key;
  summary.replay_key = replay_key.str();
}

}  // namespace

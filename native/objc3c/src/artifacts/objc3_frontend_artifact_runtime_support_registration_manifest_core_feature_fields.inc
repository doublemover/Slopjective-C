void AppendObjc3RuntimeSupportLibraryCoreFeatureManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature) {
  manifest
      << ",\"runtime_support_library_core_feature_contract_id\":\""
      << runtime_support_library_core_feature.contract_id
      << "\",\"runtime_support_library_core_feature_support_library_contract_id\":\""
      << runtime_support_library_core_feature.support_library_contract_id
      << "\",\"runtime_support_library_core_feature_metadata_publication_contract_id\":\""
      << runtime_support_library_core_feature.metadata_publication_contract_id
      << "\",\"runtime_support_library_core_feature_fail_closed\":"
      << (runtime_support_library_core_feature.fail_closed ? "true" : "false")
      << ",\"runtime_support_library_core_feature_sources_present\":"
      << (runtime_support_library_core_feature
                  .native_runtime_library_sources_present
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_header_present\":"
      << (runtime_support_library_core_feature.native_runtime_library_header_present
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_archive_build_enabled\":"
      << (runtime_support_library_core_feature
                  .native_runtime_library_archive_build_enabled
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_entrypoints_implemented\":"
      << (runtime_support_library_core_feature
                  .native_runtime_library_entrypoints_implemented
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_selector_lookup_stateful\":"
      << (runtime_support_library_core_feature.selector_lookup_stateful ? "true"
                                                                        : "false")
      << ",\"runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper\":"
      << (runtime_support_library_core_feature
                  .deterministic_dispatch_formula_matches_runtime_test_helper
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_reset_for_testing_supported\":"
      << (runtime_support_library_core_feature.reset_for_testing_supported
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_strict_dispatch_errors_required\":"
      << (runtime_support_library_core_feature.strict_dispatch_errors_required
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_driver_link_wiring_pending\":"
      << (runtime_support_library_core_feature.driver_link_wiring_pending
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_ready_for_driver_link_wiring\":"
      << (runtime_support_library_core_feature.ready_for_driver_link_wiring
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_target_name\":\""
      << runtime_support_library_core_feature.cmake_target_name
      << "\",\"runtime_support_library_core_feature_public_header_path\":\""
      << runtime_support_library_core_feature.public_header_path
      << "\",\"runtime_support_library_core_feature_source_root\":\""
      << runtime_support_library_core_feature.source_root
      << "\",\"runtime_support_library_core_feature_implementation_source_path\":\""
      << runtime_support_library_core_feature.implementation_source_path
      << "\",\"runtime_support_library_core_feature_library_kind\":\""
      << runtime_support_library_core_feature.library_kind
      << "\",\"runtime_support_library_core_feature_archive_basename\":\""
      << runtime_support_library_core_feature.archive_basename
      << "\",\"runtime_support_library_core_feature_archive_relative_path\":\""
      << runtime_support_library_core_feature.archive_relative_path
      << "\",\"runtime_support_library_core_feature_probe_source_path\":\""
      << runtime_support_library_core_feature.probe_source_path
      << "\",\"runtime_support_library_core_feature_register_image_symbol\":\""
      << runtime_support_library_core_feature.register_image_symbol
      << "\",\"runtime_support_library_core_feature_lookup_selector_symbol\":\""
      << runtime_support_library_core_feature.lookup_selector_symbol
      << "\",\"runtime_support_library_core_feature_dispatch_i32_symbol\":\""
      << runtime_support_library_core_feature.dispatch_i32_symbol
      << "\",\"runtime_support_library_core_feature_reset_for_testing_symbol\":\""
      << runtime_support_library_core_feature.reset_for_testing_symbol
      << "\",\"runtime_support_library_core_feature_driver_link_mode\":\""
      << runtime_support_library_core_feature.driver_link_mode
      << "\"";
}

void AppendObjc3RuntimeSupportLibraryCoreFeatureFailureManifestField(
    std::ostream &manifest,
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature) {
  manifest << ",\"runtime_support_library_core_feature_failure_reason\":\""
           << runtime_support_library_core_feature.failure_reason
           << "\"";
}

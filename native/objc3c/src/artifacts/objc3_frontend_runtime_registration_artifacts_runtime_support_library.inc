Objc3RuntimeSupportLibraryContractSummary
BuildRuntimeSupportLibraryContractSummary() {
  Objc3RuntimeSupportLibraryContractSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.target_name_frozen = true;
  summary.exported_entrypoints_frozen = true;
  summary.ownership_boundaries_frozen = true;
  summary.build_constraints_frozen = true;
  summary.strict_dispatch_errors_required = true;
  summary.native_runtime_library_present = false;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_runtime_library_skeleton = true;
  return summary;
}

Objc3RuntimeSupportLibraryCoreFeatureSummary
BuildRuntimeSupportLibraryCoreFeatureSummary(
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library) {
  Objc3RuntimeSupportLibraryCoreFeatureSummary summary;
  summary.support_library_contract_id = runtime_support_library.contract_id;
  summary.metadata_publication_contract_id =
      runtime_support_library.metadata_publication_contract_id;
  summary.fail_closed = true;
  summary.native_runtime_library_sources_present = true;
  summary.native_runtime_library_header_present = true;
  summary.native_runtime_library_archive_build_enabled = true;
  summary.native_runtime_library_entrypoints_implemented = true;
  summary.selector_lookup_stateful = true;
  summary.deterministic_dispatch_formula_matches_runtime_test_helper = true;
  summary.reset_for_testing_supported = true;
  summary.strict_dispatch_errors_required = true;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_driver_link_wiring =
      IsReadyObjc3RuntimeSupportLibraryContractSummary(runtime_support_library);
  return summary;
}

Objc3RuntimeSupportLibraryLinkWiringSummary
BuildRuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature) {
  Objc3RuntimeSupportLibraryLinkWiringSummary summary;
  summary.support_library_core_feature_contract_id =
      runtime_support_library_core_feature.contract_id;
  summary.fail_closed = true;
  summary.runtime_library_archive_available =
      runtime_support_library_core_feature
          .native_runtime_library_archive_build_enabled;
  summary.driver_emits_runtime_link_contract = true;
  summary.execution_smoke_consumes_runtime_library = true;
  summary.strict_dispatch_errors_required = true;
  summary.ready_for_runtime_library_consumption =
      IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
          runtime_support_library_core_feature);
  return summary;
}

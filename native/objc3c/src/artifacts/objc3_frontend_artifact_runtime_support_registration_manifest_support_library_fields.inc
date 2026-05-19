void AppendObjc3RuntimeSupportLibraryManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library) {
  manifest
      << ",\"runtime_support_library_contract_id\":\""
      << runtime_support_library.contract_id
      << "\",\"runtime_support_library_metadata_publication_contract_id\":\""
      << runtime_support_library.metadata_publication_contract_id
      << "\",\"runtime_support_library_boundary_frozen\":"
      << (runtime_support_library.boundary_frozen ? "true" : "false")
      << ",\"runtime_support_library_fail_closed\":"
      << (runtime_support_library.fail_closed ? "true" : "false")
      << ",\"runtime_support_library_target_name_frozen\":"
      << (runtime_support_library.target_name_frozen ? "true" : "false")
      << ",\"runtime_support_library_exported_entrypoints_frozen\":"
      << (runtime_support_library.exported_entrypoints_frozen ? "true"
                                                              : "false")
      << ",\"runtime_support_library_ownership_boundaries_frozen\":"
      << (runtime_support_library.ownership_boundaries_frozen ? "true"
                                                              : "false")
      << ",\"runtime_support_library_build_constraints_frozen\":"
      << (runtime_support_library.build_constraints_frozen ? "true" : "false")
      << ",\"runtime_support_library_strict_dispatch_errors_required\":"
      << (runtime_support_library.strict_dispatch_errors_required ? "true"
                                                                  : "false")
      << ",\"runtime_support_library_native_library_present\":"
      << (runtime_support_library.native_runtime_library_present ? "true"
                                                                 : "false")
      << ",\"runtime_support_library_driver_link_wiring_pending\":"
      << (runtime_support_library.driver_link_wiring_pending ? "true" : "false")
      << ",\"runtime_support_library_ready_for_skeleton\":"
      << (runtime_support_library.ready_for_runtime_library_skeleton ? "true"
                                                                     : "false")
      << ",\"runtime_support_library_target_name\":\""
      << runtime_support_library.cmake_target_name
      << "\",\"runtime_support_library_public_header_path\":\""
      << runtime_support_library.public_header_path
      << "\",\"runtime_support_library_source_root\":\""
      << runtime_support_library.source_root
      << "\",\"runtime_support_library_library_kind\":\""
      << runtime_support_library.library_kind
      << "\",\"runtime_support_library_archive_basename\":\""
      << runtime_support_library.archive_basename
      << "\",\"runtime_support_library_register_image_symbol\":\""
      << runtime_support_library.register_image_symbol
      << "\",\"runtime_support_library_lookup_selector_symbol\":\""
      << runtime_support_library.lookup_selector_symbol
      << "\",\"runtime_support_library_dispatch_i32_symbol\":\""
      << runtime_support_library.dispatch_i32_symbol
      << "\",\"runtime_support_library_reset_for_testing_symbol\":\""
      << runtime_support_library.reset_for_testing_symbol
      << "\",\"runtime_support_library_driver_link_mode\":\""
      << runtime_support_library.driver_link_mode
      << "\",\"runtime_support_library_compiler_ownership_boundary\":\""
      << runtime_support_library.compiler_ownership_boundary
      << "\",\"runtime_support_library_runtime_ownership_boundary\":\""
      << runtime_support_library.runtime_ownership_boundary
      << "\",\"runtime_support_library_failure_reason\":\""
      << runtime_support_library.failure_reason
      << "\"";
}

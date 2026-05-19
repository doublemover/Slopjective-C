void AppendObjc3RuntimeSupportLibraryLinkWiringManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  manifest
      << ",\"runtime_support_library_link_wiring_contract_id\":\""
      << runtime_support_library_link_wiring.contract_id
      << "\",\"runtime_support_library_link_wiring_core_feature_contract_id\":\""
      << runtime_support_library_link_wiring
             .support_library_core_feature_contract_id
      << "\",\"runtime_support_library_link_wiring_fail_closed\":"
      << (runtime_support_library_link_wiring.fail_closed ? "true" : "false")
      << ",\"runtime_support_library_link_wiring_archive_available\":"
      << (runtime_support_library_link_wiring.runtime_library_archive_available
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_driver_emits_runtime_link_contract\":"
      << (runtime_support_library_link_wiring.driver_emits_runtime_link_contract
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library\":"
      << (runtime_support_library_link_wiring
                  .execution_smoke_consumes_runtime_library
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_strict_dispatch_errors_required\":"
      << (runtime_support_library_link_wiring.strict_dispatch_errors_required
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_ready_for_runtime_library_consumption\":"
      << (runtime_support_library_link_wiring
                  .ready_for_runtime_library_consumption
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_archive_relative_path\":\""
      << runtime_support_library_link_wiring.archive_relative_path
      << "\",\"runtime_support_library_link_wiring_runtime_dispatch_symbol\":\""
      << runtime_support_library_link_wiring.runtime_dispatch_symbol
      << "\",\"runtime_support_library_link_wiring_execution_smoke_script_path\":\""
      << runtime_support_library_link_wiring.execution_smoke_script_path
      << "\",\"runtime_support_library_link_wiring_driver_link_mode\":\""
      << runtime_support_library_link_wiring.driver_link_mode
      << "\",\"runtime_support_library_link_wiring_failure_reason\":\""
      << runtime_support_library_link_wiring.failure_reason
      << "\"";
}

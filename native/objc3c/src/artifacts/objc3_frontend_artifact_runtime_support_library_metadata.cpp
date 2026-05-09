#include "artifacts/objc3_frontend_artifact_runtime_support_library_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendRuntimeSupportLibraryMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library,
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  ir_frontend_metadata.runtime_support_library_contract_id =
      runtime_support_library.contract_id;
  ir_frontend_metadata.runtime_support_library_metadata_publication_contract_id =
      runtime_support_library.metadata_publication_contract_id;
  ir_frontend_metadata.runtime_support_library_boundary_frozen =
      runtime_support_library.boundary_frozen;
  ir_frontend_metadata.runtime_support_library_fail_closed =
      runtime_support_library.fail_closed;
  ir_frontend_metadata.runtime_support_library_target_name_frozen =
      runtime_support_library.target_name_frozen;
  ir_frontend_metadata.runtime_support_library_exported_entrypoints_frozen =
      runtime_support_library.exported_entrypoints_frozen;
  ir_frontend_metadata.runtime_support_library_ownership_boundaries_frozen =
      runtime_support_library.ownership_boundaries_frozen;
  ir_frontend_metadata.runtime_support_library_build_constraints_frozen =
      runtime_support_library.build_constraints_frozen;
  ir_frontend_metadata
      .runtime_support_library_strict_dispatch_errors_required =
      runtime_support_library.strict_dispatch_errors_required;
  ir_frontend_metadata.runtime_support_library_native_library_present =
      runtime_support_library.native_runtime_library_present;
  ir_frontend_metadata.runtime_support_library_driver_link_wiring_pending =
      runtime_support_library.driver_link_wiring_pending;
  ir_frontend_metadata.runtime_support_library_ready_for_skeleton =
      runtime_support_library.ready_for_runtime_library_skeleton;
  ir_frontend_metadata.runtime_support_library_target_name =
      runtime_support_library.cmake_target_name;
  ir_frontend_metadata.runtime_support_library_public_header_path =
      runtime_support_library.public_header_path;
  ir_frontend_metadata.runtime_support_library_source_root =
      runtime_support_library.source_root;
  ir_frontend_metadata.runtime_support_library_library_kind =
      runtime_support_library.library_kind;
  ir_frontend_metadata.runtime_support_library_archive_basename =
      runtime_support_library.archive_basename;
  ir_frontend_metadata.runtime_support_library_register_image_symbol =
      runtime_support_library.register_image_symbol;
  ir_frontend_metadata.runtime_support_library_lookup_selector_symbol =
      runtime_support_library.lookup_selector_symbol;
  ir_frontend_metadata.runtime_support_library_dispatch_i32_symbol =
      runtime_support_library.dispatch_i32_symbol;
  ir_frontend_metadata.runtime_support_library_reset_for_testing_symbol =
      runtime_support_library.reset_for_testing_symbol;
  ir_frontend_metadata.runtime_support_library_driver_link_mode =
      runtime_support_library.driver_link_mode;
  ir_frontend_metadata.runtime_support_library_compiler_ownership_boundary =
      runtime_support_library.compiler_ownership_boundary;
  ir_frontend_metadata.runtime_support_library_runtime_ownership_boundary =
      runtime_support_library.runtime_ownership_boundary;
  ir_frontend_metadata.runtime_support_library_core_feature_contract_id =
      runtime_support_library_core_feature.contract_id;
  ir_frontend_metadata
      .runtime_support_library_core_feature_support_library_contract_id =
      runtime_support_library_core_feature.support_library_contract_id;
  ir_frontend_metadata
      .runtime_support_library_core_feature_metadata_publication_contract_id =
      runtime_support_library_core_feature.metadata_publication_contract_id;
  ir_frontend_metadata.runtime_support_library_core_feature_fail_closed =
      runtime_support_library_core_feature.fail_closed;
  ir_frontend_metadata.runtime_support_library_core_feature_sources_present =
      runtime_support_library_core_feature
          .native_runtime_library_sources_present;
  ir_frontend_metadata.runtime_support_library_core_feature_header_present =
      runtime_support_library_core_feature.native_runtime_library_header_present;
  ir_frontend_metadata
      .runtime_support_library_core_feature_archive_build_enabled =
      runtime_support_library_core_feature
          .native_runtime_library_archive_build_enabled;
  ir_frontend_metadata
      .runtime_support_library_core_feature_entrypoints_implemented =
      runtime_support_library_core_feature
          .native_runtime_library_entrypoints_implemented;
  ir_frontend_metadata
      .runtime_support_library_core_feature_selector_lookup_stateful =
      runtime_support_library_core_feature.selector_lookup_stateful;
  ir_frontend_metadata
      .runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper =
      runtime_support_library_core_feature
          .deterministic_dispatch_formula_matches_runtime_test_helper;
  ir_frontend_metadata
      .runtime_support_library_core_feature_reset_for_testing_supported =
      runtime_support_library_core_feature.reset_for_testing_supported;
  ir_frontend_metadata
      .runtime_support_library_core_feature_strict_dispatch_errors_required =
      runtime_support_library_core_feature.strict_dispatch_errors_required;
  ir_frontend_metadata
      .runtime_support_library_core_feature_driver_link_wiring_pending =
      runtime_support_library_core_feature.driver_link_wiring_pending;
  ir_frontend_metadata
      .runtime_support_library_core_feature_ready_for_driver_link_wiring =
      runtime_support_library_core_feature.ready_for_driver_link_wiring;
  ir_frontend_metadata.runtime_support_library_core_feature_target_name =
      runtime_support_library_core_feature.cmake_target_name;
  ir_frontend_metadata.runtime_support_library_core_feature_public_header_path =
      runtime_support_library_core_feature.public_header_path;
  ir_frontend_metadata.runtime_support_library_core_feature_source_root =
      runtime_support_library_core_feature.source_root;
  ir_frontend_metadata
      .runtime_support_library_core_feature_implementation_source_path =
      runtime_support_library_core_feature.implementation_source_path;
  ir_frontend_metadata.runtime_support_library_core_feature_library_kind =
      runtime_support_library_core_feature.library_kind;
  ir_frontend_metadata.runtime_support_library_core_feature_archive_basename =
      runtime_support_library_core_feature.archive_basename;
  ir_frontend_metadata.runtime_support_library_core_feature_archive_relative_path =
      runtime_support_library_core_feature.archive_relative_path;
  ir_frontend_metadata.runtime_support_library_core_feature_probe_source_path =
      runtime_support_library_core_feature.probe_source_path;
  ir_frontend_metadata.runtime_support_library_core_feature_register_image_symbol =
      runtime_support_library_core_feature.register_image_symbol;
  ir_frontend_metadata
      .runtime_support_library_core_feature_lookup_selector_symbol =
      runtime_support_library_core_feature.lookup_selector_symbol;
  ir_frontend_metadata.runtime_support_library_core_feature_dispatch_i32_symbol =
      runtime_support_library_core_feature.dispatch_i32_symbol;
  ir_frontend_metadata
      .runtime_support_library_core_feature_reset_for_testing_symbol =
      runtime_support_library_core_feature.reset_for_testing_symbol;
  ir_frontend_metadata.runtime_support_library_core_feature_driver_link_mode =
      runtime_support_library_core_feature.driver_link_mode;
  ir_frontend_metadata.runtime_support_library_link_wiring_contract_id =
      runtime_support_library_link_wiring.contract_id;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_core_feature_contract_id =
      runtime_support_library_link_wiring
          .support_library_core_feature_contract_id;
  ir_frontend_metadata.runtime_support_library_link_wiring_fail_closed =
      runtime_support_library_link_wiring.fail_closed;
  ir_frontend_metadata.runtime_support_library_link_wiring_archive_available =
      runtime_support_library_link_wiring.runtime_library_archive_available;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_driver_emits_runtime_link_contract =
      runtime_support_library_link_wiring.driver_emits_runtime_link_contract;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library =
      runtime_support_library_link_wiring
          .execution_smoke_consumes_runtime_library;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_strict_dispatch_errors_required =
      runtime_support_library_link_wiring.strict_dispatch_errors_required;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_ready_for_runtime_library_consumption =
      runtime_support_library_link_wiring
          .ready_for_runtime_library_consumption;
  ir_frontend_metadata.runtime_support_library_link_wiring_archive_relative_path =
      runtime_support_library_link_wiring.archive_relative_path;
  ir_frontend_metadata.runtime_support_library_link_wiring_runtime_dispatch_symbol =
      runtime_support_library_link_wiring.runtime_dispatch_symbol;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_execution_smoke_script_path =
      runtime_support_library_link_wiring.execution_smoke_script_path;
  ir_frontend_metadata.runtime_support_library_link_wiring_driver_link_mode =
      runtime_support_library_link_wiring.driver_link_mode;
}

}  // namespace objc3::artifacts::frontend

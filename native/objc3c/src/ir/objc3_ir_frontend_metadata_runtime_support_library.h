#pragma once

#include <string>

struct Objc3IRFrontendRuntimeSupportLibraryMetadata {
  std::string runtime_support_library_contract_id;
  std::string runtime_support_library_metadata_publication_contract_id;
  bool runtime_support_library_boundary_frozen = false;
  bool runtime_support_library_fail_closed = false;
  bool runtime_support_library_target_name_frozen = false;
  bool runtime_support_library_exported_entrypoints_frozen = false;
  bool runtime_support_library_ownership_boundaries_frozen = false;
  bool runtime_support_library_build_constraints_frozen = false;
  bool runtime_support_library_strict_dispatch_errors_required = false;
  bool runtime_support_library_native_library_present = false;
  bool runtime_support_library_driver_link_wiring_pending = false;
  bool runtime_support_library_ready_for_skeleton = false;
  std::string runtime_support_library_target_name;
  std::string runtime_support_library_public_header_path;
  std::string runtime_support_library_source_root;
  std::string runtime_support_library_library_kind;
  std::string runtime_support_library_archive_basename;
  std::string runtime_support_library_register_image_symbol;
  std::string runtime_support_library_lookup_selector_symbol;
  std::string runtime_support_library_dispatch_i32_symbol;
  std::string runtime_support_library_reset_for_testing_symbol;
  std::string runtime_support_library_driver_link_mode;
  std::string runtime_support_library_compiler_ownership_boundary;
  std::string runtime_support_library_runtime_ownership_boundary;
  std::string runtime_support_library_core_feature_contract_id;
  std::string runtime_support_library_core_feature_support_library_contract_id;
  std::string
      runtime_support_library_core_feature_metadata_publication_contract_id;
  bool runtime_support_library_core_feature_fail_closed = false;
  bool runtime_support_library_core_feature_sources_present = false;
  bool runtime_support_library_core_feature_header_present = false;
  bool runtime_support_library_core_feature_archive_build_enabled = false;
  bool runtime_support_library_core_feature_entrypoints_implemented = false;
  bool runtime_support_library_core_feature_selector_lookup_stateful = false;
  bool runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper =
      false;
  bool runtime_support_library_core_feature_reset_for_testing_supported = false;
  bool runtime_support_library_core_feature_strict_dispatch_errors_required =
      false;
  bool runtime_support_library_core_feature_driver_link_wiring_pending = false;
  bool runtime_support_library_core_feature_ready_for_driver_link_wiring = false;
  std::string runtime_support_library_core_feature_target_name;
  std::string runtime_support_library_core_feature_public_header_path;
  std::string runtime_support_library_core_feature_source_root;
  std::string runtime_support_library_core_feature_implementation_source_path;
  std::string runtime_support_library_core_feature_library_kind;
  std::string runtime_support_library_core_feature_archive_basename;
  std::string runtime_support_library_core_feature_archive_relative_path;
  std::string runtime_support_library_core_feature_probe_source_path;
  std::string runtime_support_library_core_feature_register_image_symbol;
  std::string runtime_support_library_core_feature_lookup_selector_symbol;
  std::string runtime_support_library_core_feature_dispatch_i32_symbol;
  std::string runtime_support_library_core_feature_reset_for_testing_symbol;
  std::string runtime_support_library_core_feature_driver_link_mode;
  std::string runtime_support_library_link_wiring_contract_id;
  std::string runtime_support_library_link_wiring_core_feature_contract_id;
  bool runtime_support_library_link_wiring_fail_closed = false;
  bool runtime_support_library_link_wiring_archive_available = false;
  bool runtime_support_library_link_wiring_driver_emits_runtime_link_contract =
      false;
  bool runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library =
      false;
  bool runtime_support_library_link_wiring_strict_dispatch_errors_required =
      false;
  bool runtime_support_library_link_wiring_ready_for_runtime_library_consumption =
      false;
  std::string runtime_support_library_link_wiring_archive_relative_path;
  std::string runtime_support_library_link_wiring_runtime_dispatch_symbol;
  std::string runtime_support_library_link_wiring_execution_smoke_script_path;
  std::string runtime_support_library_link_wiring_driver_link_mode;
};

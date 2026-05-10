#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendRuntimeSupportMetadata {
  std::string runtime_metadata_object_inspection_contract_id;
  std::string runtime_metadata_object_inspection_publication_contract_id;
  bool runtime_metadata_object_inspection_matrix_published = false;
  bool runtime_metadata_object_inspection_fail_closed = false;
  bool runtime_metadata_object_inspection_uses_llvm_readobj = false;
  bool runtime_metadata_object_inspection_uses_llvm_objdump = false;
  std::size_t runtime_metadata_object_inspection_matrix_row_count = 0;
  std::string runtime_metadata_object_inspection_fixture_path;
  std::string runtime_metadata_object_inspection_emit_prefix;
  std::string runtime_metadata_object_inspection_object_relative_path;
  std::string runtime_metadata_object_inspection_section_inventory_row_key;
  std::string runtime_metadata_object_inspection_section_inventory_command;
  std::string runtime_metadata_object_inspection_symbol_inventory_row_key;
  std::string runtime_metadata_object_inspection_symbol_inventory_command;
  std::string executable_metadata_debug_projection_contract_id;
  std::string executable_metadata_debug_projection_typed_handoff_contract_id;
  std::string executable_metadata_debug_projection_source_graph_contract_id;
  std::string executable_metadata_debug_projection_named_metadata_name;
  std::string executable_metadata_debug_projection_manifest_surface_path;
  std::string executable_metadata_debug_projection_typed_handoff_surface_path;
  std::string executable_metadata_debug_projection_source_graph_surface_path;
  bool executable_metadata_debug_projection_matrix_published = false;
  bool executable_metadata_debug_projection_fail_closed = false;
  bool executable_metadata_debug_projection_manifest_debug_surface_published =
      false;
  bool executable_metadata_debug_projection_ir_named_metadata_published = false;
  bool executable_metadata_debug_projection_replay_anchor_deterministic = false;
  bool executable_metadata_debug_projection_active_typed_handoff_ready = false;
  std::size_t executable_metadata_debug_projection_matrix_row_count = 0;
  std::string executable_metadata_debug_projection_replay_key;
  std::string executable_metadata_debug_projection_active_typed_handoff_replay_key;
  std::string executable_metadata_debug_projection_row0_descriptor;
  std::string executable_metadata_debug_projection_row1_descriptor;
  std::string executable_metadata_debug_projection_row2_descriptor;
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
  std::string runtime_support_library_core_feature_metadata_publication_contract_id;
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

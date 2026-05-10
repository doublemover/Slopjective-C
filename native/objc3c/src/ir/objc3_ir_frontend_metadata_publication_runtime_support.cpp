#include "ir/objc3_ir_frontend_metadata_publication_runtime_support.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeSupportMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!50 = !{!\""
      << EscapeCStringLiteral(metadata.runtime_metadata_object_inspection_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_publication_contract_id)
      << "\", i1 "
      << (metadata.runtime_metadata_object_inspection_matrix_published ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_object_inspection_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_object_inspection_uses_llvm_readobj ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_object_inspection_uses_llvm_objdump ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_object_inspection_matrix_row_count)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_fixture_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_emit_prefix)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_object_relative_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_section_inventory_row_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_section_inventory_command)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_symbol_inventory_row_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_symbol_inventory_command)
      << "\"}\n";
  out << "!51 = !{!\""
      << EscapeCStringLiteral(metadata.runtime_support_library_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_metadata_publication_contract_id)
      << "\", i1 "
      << (metadata.runtime_support_library_boundary_frozen ? 1 : 0)
      << ", i1 " << (metadata.runtime_support_library_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_target_name_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_exported_entrypoints_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_ownership_boundaries_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_build_constraints_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_strict_dispatch_errors_required ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_native_library_present ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_driver_link_wiring_pending ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_ready_for_skeleton ? 1 : 0)
      << ", !\"" << EscapeCStringLiteral(metadata.runtime_support_library_target_name)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_public_header_path)
      << "\", !\"" << EscapeCStringLiteral(metadata.runtime_support_library_source_root)
      << "\", !\"" << EscapeCStringLiteral(metadata.runtime_support_library_library_kind)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_archive_basename)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_register_image_symbol)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_lookup_selector_symbol)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_dispatch_i32_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_reset_for_testing_symbol)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_driver_link_mode)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_compiler_ownership_boundary)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_runtime_ownership_boundary)
      << "\"}\n";
  out << "!52 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_support_library_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_support_library_core_feature_metadata_publication_contract_id)
      << "\", i1 "
      << (metadata.runtime_support_library_core_feature_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_sources_present ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_header_present ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_archive_build_enabled ? 1
                                                                              : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_entrypoints_implemented ? 1
                                                                                : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_selector_lookup_stateful
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper
              ? 1
              : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_reset_for_testing_supported
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_core_feature_strict_dispatch_errors_required
              ? 1
              : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_driver_link_wiring_pending
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_core_feature_ready_for_driver_link_wiring
              ? 1
              : 0)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_target_name)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_public_header_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_source_root)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_support_library_core_feature_implementation_source_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_library_kind)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_archive_basename)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_archive_relative_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_probe_source_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_register_image_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_lookup_selector_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_dispatch_i32_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_reset_for_testing_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_driver_link_mode)
      << "\"}\n";
  out << "!53 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_core_feature_contract_id)
      << "\", i1 "
      << (metadata.runtime_support_library_link_wiring_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_link_wiring_archive_available ? 1 : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_link_wiring_driver_emits_runtime_link_contract
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_link_wiring_strict_dispatch_errors_required
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_link_wiring_ready_for_runtime_library_consumption
              ? 1
              : 0)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_archive_relative_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_runtime_dispatch_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_execution_smoke_script_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_driver_link_mode)
      << "\"}\n";
  out << "!54 = !{!\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_typed_handoff_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_source_graph_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_named_metadata_name)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_manifest_surface_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .executable_metadata_debug_projection_typed_handoff_surface_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_source_graph_surface_path)
      << "\", i1 "
      << (metadata.executable_metadata_debug_projection_matrix_published ? 1 : 0)
      << ", i1 "
      << (metadata.executable_metadata_debug_projection_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_manifest_debug_surface_published
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_ir_named_metadata_published
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_replay_anchor_deterministic
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_active_typed_handoff_ready
              ? 1
              : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_metadata_debug_projection_matrix_row_count)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .executable_metadata_debug_projection_active_typed_handoff_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_row0_descriptor)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_row1_descriptor)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_row2_descriptor)
      << "\"}\n";
}

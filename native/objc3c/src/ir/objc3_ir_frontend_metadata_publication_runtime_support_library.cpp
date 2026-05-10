#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeSupportLibraryMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
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
}

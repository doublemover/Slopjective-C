#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeSupportLibraryCoreFeatureMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
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
}

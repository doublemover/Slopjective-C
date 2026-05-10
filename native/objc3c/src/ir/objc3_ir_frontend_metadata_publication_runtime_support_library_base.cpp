#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeSupportLibraryBaseMetadataNode(
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
}

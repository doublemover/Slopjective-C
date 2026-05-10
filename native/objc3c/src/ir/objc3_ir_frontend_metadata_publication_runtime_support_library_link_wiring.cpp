#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_link_wiring.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeSupportLibraryLinkWiringMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
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

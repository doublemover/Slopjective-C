#include "ir/objc3_ir_lowering_extension_metadata_publication_interop.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRInteropLoweringMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!108 = !{!\""
      << EscapeCStringLiteral(metadata.lowering_interop_interop_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_c_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_objc_runtime_parity_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_ownership_bridge_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_error_surface_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_async_boundary_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_swift_concurrency_metadata_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_interface_preserved_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_interop_lowering_interface_preserved_metadata_annotation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_interop_interop_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!109 = !{!\""
      << EscapeCStringLiteral(metadata.lowering_interop_foreign_call_lifetime_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_c_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_foreign_call_lifetime_lowering_objc_runtime_parity_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_ownership_bridge_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_lifetime_bridge_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_metadata_preservation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_interop_foreign_call_lifetime_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!110 = !{!\""
      << EscapeCStringLiteral(
             metadata.lowering_interop_ffi_metadata_interface_preservation_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_local_foreign_callable_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_local_metadata_preservation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_local_interface_annotation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_ffi_metadata_interface_preservation_imported_module_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_imported_foreign_callable_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_imported_metadata_preservation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_imported_interface_annotation_sites)
      << ", i1 "
      << (metadata
                  .interop_ffi_metadata_interface_preservation_runtime_import_artifact_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .interop_ffi_metadata_interface_preservation_separate_compilation_preservation_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata.deterministic_interop_ffi_metadata_interface_preservation_handoff
              ? 1
              : 0)
      << "}\n\n";
  out << "!111 = !{!\""
      << EscapeCStringLiteral(Objc3InteropBridgePackagingToolchainSummary())
      << "\"}\n\n";
  out << "!112 = !{!\""
      << EscapeCStringLiteral(
             Objc3InteropHeaderModuleBridgeGenerationBoundarySummary())
      << "\"}\n\n";
}

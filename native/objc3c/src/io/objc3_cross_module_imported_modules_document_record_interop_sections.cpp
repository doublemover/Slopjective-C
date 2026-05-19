#include "io/objc3_cross_module_imported_modules_document_record_interop_sections.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void EmitObjc3CrossModuleImportedModuleRecordInteropSectionsJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input) {
  out << "      \"interop_ffi_metadata_interface_preservation_present\": "
      << (imported_input.interop_ffi_metadata_interface_preservation_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_ffi_runtime_import_artifact_ready\": "
      << (imported_input.interop_ffi_runtime_import_artifact_ready ? "true"
                                                                  : "false")
      << ",\n"
      << "      \"interop_ffi_separate_compilation_preservation_ready\": "
      << (imported_input.interop_ffi_separate_compilation_preservation_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_ffi_deterministic\": "
      << (imported_input.interop_ffi_deterministic ? "true" : "false")
      << ",\n"
      << "      \"interop_ffi_contract_id\": \""
      << EscapeJsonString(imported_input.interop_ffi_contract_id)
      << "\",\n"
      << "      \"interop_ffi_source_contract_id\": \""
      << EscapeJsonString(imported_input.interop_ffi_source_contract_id)
      << "\",\n"
      << "      \"interop_ffi_preservation_contract_id\": \""
      << EscapeJsonString(imported_input.interop_ffi_preservation_contract_id)
      << "\",\n"
      << "      \"interop_ffi_replay_key\": \""
      << EscapeJsonString(imported_input.interop_ffi_replay_key)
      << "\",\n"
      << "      \"interop_ffi_lowering_replay_key\": \""
      << EscapeJsonString(imported_input.interop_ffi_lowering_replay_key)
      << "\",\n"
      << "      \"interop_ffi_preservation_replay_key\": \""
      << EscapeJsonString(imported_input.interop_ffi_preservation_replay_key)
      << "\",\n"
      << "      \"interop_ffi_local_foreign_callable_count\": "
      << imported_input.interop_ffi_local_foreign_callable_count << ",\n"
      << "      \"interop_ffi_local_metadata_preservation_sites\": "
      << imported_input.interop_ffi_local_metadata_preservation_sites
      << ",\n"
      << "      \"interop_ffi_local_interface_annotation_sites\": "
      << imported_input.interop_ffi_local_interface_annotation_sites
      << ",\n"
      << "      \"interop_header_module_bridge_generation_present\": "
      << (imported_input.interop_header_module_bridge_generation_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_header_module_bridge_runtime_generation_ready\": "
      << (imported_input.interop_header_module_bridge_runtime_generation_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_header_module_bridge_cross_module_packaging_ready\": "
      << (imported_input
                  .interop_header_module_bridge_cross_module_packaging_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_header_module_bridge_deterministic\": "
      << (imported_input.interop_header_module_bridge_deterministic ? "true"
                                                                   : "false")
      << ",\n"
      << "      \"interop_header_module_bridge_contract_id\": \""
      << EscapeJsonString(
             imported_input.interop_header_module_bridge_contract_id)
      << "\",\n"
      << "      \"interop_header_module_bridge_source_contract_id\": \""
      << EscapeJsonString(
             imported_input.interop_header_module_bridge_source_contract_id)
      << "\",\n"
      << "      \"interop_header_module_bridge_preservation_contract_id\": \""
      << EscapeJsonString(
             imported_input
                 .interop_header_module_bridge_preservation_contract_id)
      << "\",\n"
      << "      \"interop_header_module_bridge_replay_key\": \""
      << EscapeJsonString(
             imported_input.interop_header_module_bridge_replay_key)
      << "\",\n"
      << "      \"interop_header_module_bridge_preservation_replay_key\": \""
      << EscapeJsonString(
             imported_input
                 .interop_header_module_bridge_preservation_replay_key)
      << "\",\n"
      << "      \"interop_bridge_header_artifact_relative_path\": \""
      << EscapeJsonString(
             imported_input.interop_bridge_header_artifact_relative_path)
      << "\",\n"
      << "      \"interop_bridge_module_artifact_relative_path\": \""
      << EscapeJsonString(
             imported_input.interop_bridge_module_artifact_relative_path)
      << "\",\n"
      << "      \"interop_bridge_artifact_relative_path\": \""
      << EscapeJsonString(imported_input.interop_bridge_artifact_relative_path)
      << "\",\n"
      << "      \"interop_header_module_bridge_local_foreign_callable_count\": "
      << imported_input.interop_header_module_bridge_local_foreign_callable_count
      << ",\n";
}

#pragma once

#include "artifacts/objc3_frontend_interop_semantic_json_summary_fields_common.h"

namespace objc3::artifacts::frontend::interop_semantic_json_detail {

inline void
AppendInteropForeignSurfaceInterfacePreservationSummaryJsonFields(
    std::ostringstream &out,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary &summary) {
  out << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"foreign_import_source_contract_id\":\""
      << EscapeJsonString(summary.foreign_import_source_contract_id)
      << "\",\"cpp_swift_source_contract_id\":\""
      << EscapeJsonString(summary.cpp_swift_source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"foreign_import_source_replay_key\":\""
      << EscapeJsonString(summary.foreign_import_source_replay_key)
      << "\",\"cpp_swift_source_replay_key\":\""
      << EscapeJsonString(summary.cpp_swift_source_replay_key)
      << "\",\"local_import_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.local_import_module_names_lexicographic)
      << ",\"imported_provider_module_names_lexicographic\":"
      << BuildStringArrayJson(
             summary.imported_provider_module_names_lexicographic)
      << ",\"local_foreign_callable_count\":"
      << summary.local_foreign_callable_count
      << ",\"local_import_module_annotation_count\":"
      << summary.local_import_module_annotation_count
      << ",\"local_imported_module_name_count\":"
      << summary.local_imported_module_name_count
      << ",\"local_swift_name_annotation_count\":"
      << summary.local_swift_name_annotation_count
      << ",\"local_swift_private_annotation_count\":"
      << summary.local_swift_private_annotation_count
      << ",\"local_cpp_name_annotation_count\":"
      << summary.local_cpp_name_annotation_count
      << ",\"local_header_name_annotation_count\":"
      << summary.local_header_name_annotation_count
      << ",\"local_named_annotation_payload_count\":"
      << summary.local_named_annotation_payload_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"imported_foreign_callable_count\":"
      << summary.imported_foreign_callable_count
      << ",\"imported_import_module_annotation_count\":"
      << summary.imported_import_module_annotation_count
      << ",\"imported_imported_module_name_count\":"
      << summary.imported_imported_module_name_count
      << ",\"imported_swift_name_annotation_count\":"
      << summary.imported_swift_name_annotation_count
      << ",\"imported_swift_private_annotation_count\":"
      << summary.imported_swift_private_annotation_count
      << ",\"imported_cpp_name_annotation_count\":"
      << summary.imported_cpp_name_annotation_count
      << ",\"imported_header_name_annotation_count\":"
      << summary.imported_header_name_annotation_count
      << ",\"imported_named_annotation_payload_count\":"
      << summary.imported_named_annotation_payload_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"";
}

inline void AppendInteropHeaderModuleBridgeGenerationSummaryJsonFields(
    std::ostringstream &out,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &summary) {
  out << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(summary.preservation_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"generation_model\":\""
      << EscapeJsonString(summary.generation_model)
      << "\",\"packaging_model\":\""
      << EscapeJsonString(summary.packaging_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"header_artifact_relative_path\":\""
      << EscapeJsonString(summary.header_artifact_relative_path)
      << "\",\"module_artifact_relative_path\":\""
      << EscapeJsonString(summary.module_artifact_relative_path)
      << "\",\"bridge_artifact_relative_path\":\""
      << EscapeJsonString(summary.bridge_artifact_relative_path)
      << "\",\"local_import_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.local_import_module_names_lexicographic)
      << ",\"imported_provider_module_names_lexicographic\":"
      << BuildStringArrayJson(
             summary.imported_provider_module_names_lexicographic)
      << ",\"local_foreign_callable_count\":"
      << summary.local_foreign_callable_count
      << ",\"local_import_module_name_count\":"
      << summary.local_import_module_name_count
      << ",\"local_cpp_name_annotation_count\":"
      << summary.local_cpp_name_annotation_count
      << ",\"local_header_name_annotation_count\":"
      << summary.local_header_name_annotation_count
      << ",\"local_swift_name_annotation_count\":"
      << summary.local_swift_name_annotation_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"runtime_generation_ready\":"
      << (summary.runtime_generation_ready ? "true" : "false")
      << ",\"cross_module_packaging_ready\":"
      << (summary.cross_module_packaging_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"preservation_replay_key\":\""
      << EscapeJsonString(summary.preservation_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"";
}

}  // namespace objc3::artifacts::frontend::interop_semantic_json_detail

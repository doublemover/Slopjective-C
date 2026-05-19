#pragma once

#include "artifacts/objc3_frontend_interop_semantic_json_summary_fields_common.h"

namespace objc3::artifacts::frontend::interop_semantic_json_detail {

inline void AppendInteropInteropSemanticModelSummaryJsonFields(
    std::ostringstream &out,
    const Objc3InteropInteropSemanticModelSummary &summary) {
  out << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"foreign_callable_sites\":" << summary.foreign_callable_sites
      << ",\"import_module_annotation_sites\":"
      << summary.import_module_annotation_sites
      << ",\"imported_module_name_sites\":"
      << summary.imported_module_name_sites
      << ",\"export_header_annotation_sites\":"
      << summary.export_header_annotation_sites
      << ",\"export_header_name_sites\":"
      << summary.export_header_name_sites
      << ",\"mixed_image_annotation_sites\":"
      << summary.mixed_image_annotation_sites
      << ",\"mixed_image_name_sites\":" << summary.mixed_image_name_sites
      << ",\"package_entry_annotation_sites\":"
      << summary.package_entry_annotation_sites
      << ",\"package_entry_name_sites\":"
      << summary.package_entry_name_sites
      << ",\"swift_name_annotation_sites\":"
      << summary.swift_name_annotation_sites
      << ",\"swift_private_annotation_sites\":"
      << summary.swift_private_annotation_sites
      << ",\"cpp_name_annotation_sites\":"
      << summary.cpp_name_annotation_sites
      << ",\"header_name_annotation_sites\":"
      << summary.header_name_annotation_sites
      << ",\"abi_alignment_annotation_sites\":"
      << summary.abi_alignment_annotation_sites
      << ",\"foreign_type_annotation_sites\":"
      << summary.foreign_type_annotation_sites
      << ",\"named_annotation_payload_sites\":"
      << summary.named_annotation_payload_sites
      << ",\"retainable_family_callable_sites\":"
      << summary.retainable_family_callable_sites
      << ",\"bridge_callable_sites\":" << summary.bridge_callable_sites
      << ",\"async_executor_affinity_sites\":"
      << summary.async_executor_affinity_sites
      << ",\"actor_hazard_sites\":" << summary.actor_hazard_sites
      << ",\"interop_metadata_annotation_sites\":"
      << summary.interop_metadata_annotation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"foreign_annotation_source_supported\":"
      << (summary.foreign_annotation_source_supported ? "true" : "false")
      << ",\"ownership_interaction_profile_frozen\":"
      << (summary.ownership_interaction_profile_frozen ? "true" : "false")
      << ",\"error_bridge_profile_reused\":"
      << (summary.error_bridge_profile_reused ? "true" : "false")
      << ",\"async_affinity_profile_reused\":"
      << (summary.async_affinity_profile_reused ? "true" : "false")
      << ",\"actor_hazard_profile_reused\":"
      << (summary.actor_hazard_profile_reused ? "true" : "false")
      << ",\"metadata_payload_profile_frozen\":"
      << (summary.metadata_payload_profile_frozen ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"";
}

}  // namespace objc3::artifacts::frontend::interop_semantic_json_detail

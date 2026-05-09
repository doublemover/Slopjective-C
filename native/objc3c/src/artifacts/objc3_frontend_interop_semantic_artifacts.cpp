#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

std::string BuildInteropInteropSemanticModelSummaryJson(
    const Objc3InteropInteropSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
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
      << "\"}";
  return out.str();
}

std::string BuildInteropInteropRuntimeParitySummaryJson(
    const Objc3InteropInteropRuntimeParitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"foreign_callable_sites\":" << summary.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << summary.c_foreign_callable_sites
      << ",\"objc_method_foreign_callable_sites\":"
      << summary.objc_method_foreign_callable_sites
      << ",\"import_module_annotation_sites\":"
      << summary.import_module_annotation_sites
      << ",\"import_module_foreign_callable_sites\":"
      << summary.import_module_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << summary.objc_runtime_parity_callable_sites
      << ",\"foreign_definition_rejection_sites\":"
      << summary.foreign_definition_rejection_sites
      << ",\"import_without_foreign_rejection_sites\":"
      << summary.import_without_foreign_rejection_sites
      << ",\"implementation_annotation_rejection_sites\":"
      << summary.implementation_annotation_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"declaration_only_foreign_c_enforced\":"
      << (summary.declaration_only_foreign_c_enforced ? "true" : "false")
      << ",\"import_module_requires_foreign_enforced\":"
      << (summary.import_module_requires_foreign_enforced ? "true" : "false")
      << ",\"implementation_annotations_fail_closed\":"
      << (summary.implementation_annotations_fail_closed ? "true" : "false")
      << ",\"objc_runtime_parity_classified\":"
      << (summary.objc_runtime_parity_classified ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropCppInteropInteractionSummaryJson(
    const Objc3InteropCppInteropInteractionSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"cpp_interop_callable_sites\":"
      << summary.cpp_interop_callable_sites
      << ",\"cpp_named_callable_sites\":" << summary.cpp_named_callable_sites
      << ",\"header_named_callable_sites\":"
      << summary.header_named_callable_sites
      << ",\"ownership_interaction_sites\":"
      << summary.ownership_interaction_sites
      << ",\"throws_interaction_sites\":" << summary.throws_interaction_sites
      << ",\"async_interaction_sites\":" << summary.async_interaction_sites
      << ",\"ownership_rejection_sites\":"
      << summary.ownership_rejection_sites
      << ",\"throws_rejection_sites\":" << summary.throws_rejection_sites
      << ",\"async_rejection_sites\":" << summary.async_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"cpp_annotation_profile_reused\":"
      << (summary.cpp_annotation_profile_reused ? "true" : "false")
      << ",\"ownership_interactions_fail_closed\":"
      << (summary.ownership_interactions_fail_closed ? "true" : "false")
      << ",\"throws_interactions_fail_closed\":"
      << (summary.throws_interactions_fail_closed ? "true" : "false")
      << ",\"async_interactions_fail_closed\":"
      << (summary.async_interactions_fail_closed ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropSwiftInteropIsolationSummaryJson(
    const Objc3InteropSwiftInteropIsolationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"swift_interop_callable_sites\":"
      << summary.swift_interop_callable_sites
      << ",\"swift_named_callable_sites\":"
      << summary.swift_named_callable_sites
      << ",\"swift_private_callable_sites\":"
      << summary.swift_private_callable_sites
      << ",\"swift_private_without_name_sites\":"
      << summary.swift_private_without_name_sites
      << ",\"actor_owned_swift_callable_sites\":"
      << summary.actor_owned_swift_callable_sites
      << ",\"nonisolated_swift_callable_sites\":"
      << summary.nonisolated_swift_callable_sites
      << ",\"implementation_swift_callable_sites\":"
      << summary.implementation_swift_callable_sites
      << ",\"swift_private_without_name_rejection_sites\":"
      << summary.swift_private_without_name_rejection_sites
      << ",\"actor_isolation_mapping_rejection_sites\":"
      << summary.actor_isolation_mapping_rejection_sites
      << ",\"nonisolated_mapping_rejection_sites\":"
      << summary.nonisolated_mapping_rejection_sites
      << ",\"implementation_surface_rejection_sites\":"
      << summary.implementation_surface_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"swift_metadata_profile_reused\":"
      << (summary.swift_metadata_profile_reused ? "true" : "false")
      << ",\"swift_private_requires_name_enforced\":"
      << (summary.swift_private_requires_name_enforced ? "true" : "false")
      << ",\"actor_isolation_mapping_fail_closed\":"
      << (summary.actor_isolation_mapping_fail_closed ? "true" : "false")
      << ",\"nonisolated_mapping_fail_closed\":"
      << (summary.nonisolated_mapping_fail_closed ? "true" : "false")
      << ",\"implementation_surface_fail_closed\":"
      << (summary.implementation_surface_fail_closed ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
    const Objc3InteropForeignSurfaceInterfacePreservationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
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
      << "\"}";
  return out.str();
}

std::string BuildInteropHeaderModuleBridgeGenerationSummaryJson(
    const Objc3InteropHeaderModuleBridgeGenerationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
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
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend

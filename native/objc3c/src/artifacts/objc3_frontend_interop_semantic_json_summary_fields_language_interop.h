#pragma once

#include "artifacts/objc3_frontend_interop_semantic_json_summary_fields_common.h"

namespace objc3::artifacts::frontend::interop_semantic_json_detail {

inline void AppendInteropCppInteropInteractionSummaryJsonFields(
    std::ostringstream &out,
    const Objc3InteropCppInteropInteractionSummary &summary) {
  out << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
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
      << "\"";
}

inline void AppendInteropSwiftInteropIsolationSummaryJsonFields(
    std::ostringstream &out,
    const Objc3InteropSwiftInteropIsolationSummary &summary) {
  out << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
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
      << "\"";
}

}  // namespace objc3::artifacts::frontend::interop_semantic_json_detail

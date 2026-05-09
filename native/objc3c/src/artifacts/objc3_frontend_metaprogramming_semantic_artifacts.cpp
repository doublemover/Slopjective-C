#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"

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

std::string BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"derive_marker_sites\":" << summary.derive_marker_sites
      << ",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
      << ",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"interface_property_behavior_sites\":"
      << summary.interface_property_behavior_sites
      << ",\"implementation_property_behavior_sites\":"
      << summary.implementation_property_behavior_sites
      << ",\"protocol_property_behavior_sites\":"
      << summary.protocol_property_behavior_sites
      << ",\"synthesized_binding_visible_sites\":"
      << summary.synthesized_binding_visible_sites
      << ",\"synthesized_getter_visible_sites\":"
      << summary.synthesized_getter_visible_sites
      << ",\"synthesized_setter_visible_sites\":"
      << summary.synthesized_setter_visible_sites
      << ",\"property_behavior_contract_violation_sites\":"
      << summary.property_behavior_contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"derive_macro_source_supported\":"
      << (summary.derive_macro_source_supported ? "true" : "false")
      << ",\"macro_package_provenance_surface_reused\":"
      << (summary.macro_package_provenance_surface_reused ? "true" : "false")
      << ",\"property_behavior_source_supported\":"
      << (summary.property_behavior_source_supported ? "true" : "false")
      << ",\"synthesized_visibility_surface_reused\":"
      << (summary.synthesized_visibility_surface_reused ? "true" : "false")
      << ",\"derive_synthesis_deferred\":"
      << (summary.derive_synthesis_deferred ? "true" : "false")
      << ",\"macro_execution_deferred\":"
      << (summary.macro_execution_deferred ? "true" : "false")
      << ",\"property_behavior_runtime_deferred\":"
      << (summary.property_behavior_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_core_implementation\":"
      << (summary.ready_for_core_implementation ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"derive_request_sites\":" << summary.derive_request_sites
      << ",\"supported_derive_request_sites\":"
      << summary.supported_derive_request_sites
      << ",\"unsupported_derive_request_sites\":"
      << summary.unsupported_derive_request_sites
      << ",\"unsupported_topology_sites\":"
      << summary.unsupported_topology_sites
      << ",\"equatable_alias_sites\":" << summary.equatable_alias_sites
      << ",\"equality_derive_sites\":" << summary.equality_derive_sites
      << ",\"hash_derive_sites\":" << summary.hash_derive_sites
      << ",\"debug_description_derive_sites\":"
      << summary.debug_description_derive_sites
      << ",\"selector_conflict_sites\":" << summary.selector_conflict_sites
      << ",\"generated_method_entry_count\":"
      << summary.generated_method_entry_count
      << ",\"expansion_inventory_rows_lexicographic\":"
      << BuildStringArrayJson(summary.expansion_inventory_rows_lexicographic)
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"supported_derive_inventory_landed\":"
      << (summary.supported_derive_inventory_landed ? "true" : "false")
      << ",\"unsupported_derive_fail_closed\":"
      << (summary.unsupported_derive_fail_closed ? "true" : "false")
      << ",\"unsupported_topology_fail_closed\":"
      << (summary.unsupported_topology_fail_closed ? "true" : "false")
      << ",\"selector_conflicts_fail_closed\":"
      << (summary.selector_conflicts_fail_closed ? "true" : "false")
      << ",\"runtime_materialization_deferred\":"
      << (summary.runtime_materialization_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
      << ",\"safe_macro_callable_sites\":"
      << summary.safe_macro_callable_sites
      << ",\"incomplete_macro_metadata_sites\":"
      << summary.incomplete_macro_metadata_sites
      << ",\"orphan_macro_metadata_sites\":"
      << summary.orphan_macro_metadata_sites
      << ",\"invalid_package_sites\":" << summary.invalid_package_sites
      << ",\"invalid_provenance_sites\":"
      << summary.invalid_provenance_sites
      << ",\"nondeterministic_callable_sites\":"
      << summary.nondeterministic_callable_sites
      << ",\"unsupported_callable_topology_sites\":"
      << summary.unsupported_callable_topology_sites
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"metadata_completeness_enforced\":"
      << (summary.metadata_completeness_enforced ? "true" : "false")
      << ",\"sandbox_namespace_enforced\":"
      << (summary.sandbox_namespace_enforced ? "true" : "false")
      << ",\"provenance_determinism_enforced\":"
      << (summary.provenance_determinism_enforced ? "true" : "false")
      << ",\"callable_determinism_enforced\":"
      << (summary.callable_determinism_enforced ? "true" : "false")
      << ",\"macro_execution_deferred\":"
      << (summary.macro_execution_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string
BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"supported_behavior_sites\":" << summary.supported_behavior_sites
      << ",\"unsupported_behavior_sites\":" << summary.unsupported_behavior_sites
      << ",\"observed_behavior_sites\":" << summary.observed_behavior_sites
      << ",\"projected_behavior_sites\":" << summary.projected_behavior_sites
      << ",\"observed_on_protocol_sites\":"
      << summary.observed_on_protocol_sites
      << ",\"observed_readonly_conflict_sites\":"
      << summary.observed_readonly_conflict_sites
      << ",\"projected_writable_conflict_sites\":"
      << summary.projected_writable_conflict_sites
      << ",\"non_object_behavior_sites\":" << summary.non_object_behavior_sites
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"supported_behavior_inventory_landed\":"
      << (summary.supported_behavior_inventory_landed ? "true" : "false")
      << ",\"unsupported_behavior_fail_closed\":"
      << (summary.unsupported_behavior_fail_closed ? "true" : "false")
      << ",\"owner_topology_fail_closed\":"
      << (summary.owner_topology_fail_closed ? "true" : "false")
      << ",\"interaction_legality_fail_closed\":"
      << (summary.interaction_legality_fail_closed ? "true" : "false")
      << ",\"storage_legality_fail_closed\":"
      << (summary.storage_legality_fail_closed ? "true" : "false")
      << ",\"runtime_materialization_deferred\":"
      << (summary.runtime_materialization_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend

#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "artifacts/objc3_frontend_artifact_dispatch_contract_constants.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildDispatchDispatchIntentSemanticModelSummaryJson(
    const Objc3DispatchDispatchIntentSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"prefixed_container_attribute_sites\":"
      << summary.prefixed_container_attribute_sites
      << ",\"direct_members_container_sites\":"
      << summary.direct_members_container_sites
      << ",\"final_container_sites\":" << summary.final_container_sites
      << ",\"sealed_container_sites\":" << summary.sealed_container_sites
      << ",\"effective_direct_member_sites\":"
      << summary.effective_direct_member_sites
      << ",\"direct_members_defaulted_method_sites\":"
      << summary.direct_members_defaulted_method_sites
      << ",\"direct_members_dynamic_opt_out_sites\":"
      << summary.direct_members_dynamic_opt_out_sites
      << ",\"override_lookup_sites\":" << summary.override_lookup_sites
      << ",\"override_lookup_hits\":" << summary.override_lookup_hits
      << ",\"override_lookup_misses\":" << summary.override_lookup_misses
      << ",\"override_conflicts\":" << summary.override_conflicts
      << ",\"unresolved_base_interfaces\":"
      << summary.unresolved_base_interfaces
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"dispatch_intent_source_supported\":"
      << (summary.dispatch_intent_source_supported ? "true" : "false")
      << ",\"override_semantic_surface_reused\":"
      << (summary.override_semantic_surface_reused ? "true" : "false")
      << ",\"direct_dispatch_reserved_non_goal\":"
      << (summary.direct_dispatch_reserved_non_goal ? "true" : "false")
      << ",\"final_sealed_enforcement_deferred\":"
      << (summary.final_sealed_enforcement_deferred ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_core_implementation\":"
      << (summary.ready_for_core_implementation ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchIntentLegalitySummaryJson(
    const Objc3DispatchDispatchIntentLegalitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"subclass_sites\":" << summary.subclass_sites
      << ",\"override_sites\":" << summary.override_sites
      << ",\"illegal_final_superclass_sites\":"
      << summary.illegal_final_superclass_sites
      << ",\"illegal_sealed_superclass_sites\":"
      << summary.illegal_sealed_superclass_sites
      << ",\"illegal_final_override_sites\":"
      << summary.illegal_final_override_sites
      << ",\"illegal_direct_override_sites\":"
      << summary.illegal_direct_override_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"final_superclass_fail_closed\":"
      << (summary.final_superclass_fail_closed ? "true" : "false")
      << ",\"sealed_superclass_fail_closed\":"
      << (summary.sealed_superclass_fail_closed ? "true" : "false")
      << ",\"final_override_fail_closed\":"
      << (summary.final_override_fail_closed ? "true" : "false")
      << ",\"direct_override_fail_closed\":"
      << (summary.direct_override_fail_closed ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchIntentCompatibilitySummaryJson(
    const Objc3DispatchDispatchIntentCompatibilitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"callable_dispatch_intent_sites\":"
      << summary.callable_dispatch_intent_sites
      << ",\"container_dispatch_intent_sites\":"
      << summary.container_dispatch_intent_sites
      << ",\"illegal_direct_dynamic_conflict_sites\":"
      << summary.illegal_direct_dynamic_conflict_sites
      << ",\"illegal_final_dynamic_conflict_sites\":"
      << summary.illegal_final_dynamic_conflict_sites
      << ",\"illegal_non_method_callable_sites\":"
      << summary.illegal_non_method_callable_sites
      << ",\"illegal_protocol_method_sites\":"
      << summary.illegal_protocol_method_sites
      << ",\"illegal_category_method_sites\":"
      << summary.illegal_category_method_sites
      << ",\"illegal_category_container_sites\":"
      << summary.illegal_category_container_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"callable_conflict_fail_closed\":"
      << (summary.callable_conflict_fail_closed ? "true" : "false")
      << ",\"unsupported_callable_topology_fail_closed\":"
      << (summary.unsupported_callable_topology_fail_closed ? "true"
                                                            : "false")
      << ",\"unsupported_container_topology_fail_closed\":"
      << (summary.unsupported_container_topology_fail_closed ? "true"
                                                             : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchControlLoweringContractJson(
    const Objc3DispatchDispatchIntentSemanticModelSummary &semantic_summary,
    const Objc3DispatchDispatchIntentLegalitySummary &legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary &compatibility_summary,
    const Objc3DispatchDispatchControlLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3DispatchDispatchControlLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"legality_contract_id\":\""
      << EscapeJsonString(legality_summary.contract_id)
      << "\",\"compatibility_contract_id\":\""
      << EscapeJsonString(compatibility_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"direct_call_candidate_sites\":"
      << contract.direct_call_candidate_sites
      << ",\"direct_members_defaulted_sites\":"
      << contract.direct_members_defaulted_sites
      << ",\"dynamic_opt_out_sites\":" << contract.dynamic_opt_out_sites
      << ",\"final_container_sites\":" << contract.final_container_sites
      << ",\"sealed_container_sites\":" << contract.sealed_container_sites
      << ",\"override_legality_sites\":" << contract.override_legality_sites
      << ",\"metadata_preserved_callable_sites\":"
      << contract.metadata_preserved_callable_sites
      << ",\"metadata_preserved_container_sites\":"
      << contract.metadata_preserved_container_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend

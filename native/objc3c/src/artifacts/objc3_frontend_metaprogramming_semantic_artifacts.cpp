#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"

#include <algorithm>
#include <string_view>
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

Objc3MetaprogrammingExpansionLoweringContract
BuildMetaprogrammingExpansionLoweringContract(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_source_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &derive_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &macro_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &property_legality_summary) {
  Objc3MetaprogrammingExpansionLoweringContract contract;
  contract.derive_inventory_sites =
      derive_summary.supported_derive_request_sites;
  contract.derived_selector_artifact_sites =
      derive_summary.generated_method_entry_count;
  contract.macro_replay_visible_sites =
      macro_summary.expansion_visible_macro_sites;
  contract.property_behavior_sites =
      property_legality_summary.property_behavior_sites;
  contract.synthesized_binding_sites =
      property_source_summary.synthesized_binding_visible_sites;
  contract.synthesized_getter_sites =
      property_source_summary.synthesized_getter_visible_sites;
  contract.synthesized_setter_sites =
      property_source_summary.synthesized_setter_visible_sites;
  contract.replay_visible_metadata_sites =
      contract.derived_selector_artifact_sites +
      contract.macro_replay_visible_sites + contract.property_behavior_sites +
      contract.synthesized_binding_sites + contract.synthesized_getter_sites +
      contract.synthesized_setter_sites;
  contract.guard_blocked_sites =
      derive_summary.unsupported_derive_request_sites +
      derive_summary.unsupported_topology_sites +
      derive_summary.selector_conflict_sites +
      macro_summary.incomplete_macro_metadata_sites +
      macro_summary.orphan_macro_metadata_sites +
      macro_summary.invalid_package_sites +
      macro_summary.invalid_provenance_sites +
      macro_summary.nondeterministic_callable_sites +
      macro_summary.unsupported_callable_topology_sites +
      property_legality_summary.unsupported_behavior_sites +
      property_legality_summary.observed_on_protocol_sites +
      property_legality_summary.observed_readonly_conflict_sites +
      property_legality_summary.projected_writable_conflict_sites +
      property_legality_summary.non_object_behavior_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      derive_summary.deterministic &&
      derive_summary.ready_for_lowering_and_runtime &&
      macro_summary.deterministic &&
      macro_summary.ready_for_lowering_and_runtime &&
      property_legality_summary.deterministic &&
      property_legality_summary.ready_for_lowering_and_runtime &&
      property_source_summary.deterministic_handoff &&
      property_source_summary.ready_for_semantic_expansion;
  return contract;
}

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

std::string BuildMetaprogrammingExpansionLoweringContractJson(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_source_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &derive_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &macro_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &property_legality_summary,
    const Objc3MetaprogrammingExpansionLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3MetaprogrammingExpansionLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringSurfacePath)
      << "\",\"property_source_contract_id\":\""
      << EscapeJsonString(property_source_summary.contract_id)
      << "\",\"derive_contract_id\":\""
      << EscapeJsonString(derive_summary.contract_id)
      << "\",\"macro_contract_id\":\""
      << EscapeJsonString(macro_summary.contract_id)
      << "\",\"property_legality_contract_id\":\""
      << EscapeJsonString(property_legality_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"derive_inventory_sites\":" << contract.derive_inventory_sites
      << ",\"derived_selector_artifact_sites\":"
      << contract.derived_selector_artifact_sites
      << ",\"macro_replay_visible_sites\":"
      << contract.macro_replay_visible_sites
      << ",\"property_behavior_sites\":" << contract.property_behavior_sites
      << ",\"synthesized_binding_sites\":"
      << contract.synthesized_binding_sites
      << ",\"synthesized_getter_sites\":"
      << contract.synthesized_getter_sites
      << ",\"synthesized_setter_sites\":"
      << contract.synthesized_setter_sites
      << ",\"replay_visible_metadata_sites\":"
      << contract.replay_visible_metadata_sites
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

std::string BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
    const Objc3MetaprogrammingExpansionLoweringContract &dependency_contract,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionSurfacePath)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringContractId)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionLaneContract)
      << "\",\"emission_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingSynthesizedArtifactEmissionModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionDeferredModel)
      << "\",\"dependency_replay_key\":\""
      << EscapeJsonString(Objc3MetaprogrammingExpansionLoweringReplayKey(
             dependency_contract))
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"derive_inventory_sites\":" << contract.derive_inventory_sites
      << ",\"emitted_derive_method_sites\":"
      << contract.emitted_derive_method_sites
      << ",\"emitted_macro_artifact_sites\":"
      << contract.emitted_macro_artifact_sites
      << ",\"emitted_property_behavior_artifact_sites\":"
      << contract.emitted_property_behavior_artifact_sites
      << ",\"emitted_global_artifact_sites\":"
      << contract.emitted_global_artifact_sites
      << ",\"emitted_runtime_method_list_sites\":"
      << contract.emitted_runtime_method_list_sites
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

std::size_t CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle> &bundles,
    std::string_view owner_kind) {
  return static_cast<std::size_t>(std::count_if(
      bundles.begin(), bundles.end(),
      [owner_kind](const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &bundle) {
        return bundle.owner_kind == owner_kind;
      }));
}

Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
BuildMetaprogrammingModuleInterfaceReplayPreservationSummary(
    const Objc3MetaprogrammingExpansionLoweringContract &expansion_contract,
    const std::string &expansion_replay_key,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
        &synthesized_contract,
    const std::string &synthesized_replay_key,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &property_behavior_bundles,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary summary;
  summary.expansion_lowering_replay_key = expansion_replay_key;
  summary.synthesized_emission_replay_key = synthesized_replay_key;
  summary.local_derive_method_count =
      synthesized_contract.emitted_derive_method_sites;
  summary.local_macro_artifact_count =
      synthesized_contract.emitted_macro_artifact_sites;
  summary.local_interface_property_behavior_artifact_count =
      CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
          property_behavior_bundles, "class-interface");
  summary.local_implementation_property_behavior_artifact_count =
      CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
          property_behavior_bundles, "class-implementation");
  summary.local_runtime_method_list_count =
      synthesized_contract.emitted_runtime_method_list_sites;
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready &&
      IsValidObjc3MetaprogrammingExpansionLoweringContract(expansion_contract) &&
      IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
          synthesized_contract);
  summary.deterministic =
      expansion_contract.deterministic && synthesized_contract.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.metaprogramming_module_interface_replay_preservation_present) {
      continue;
    }
    ++summary.imported_module_count;
    if (!surface.frontend_closure_summary.module_name.empty()) {
      summary.imported_module_names_lexicographic.push_back(
          surface.frontend_closure_summary.module_name);
    }
    summary.imported_derive_method_count +=
        surface.metaprogramming_local_derive_method_count;
    summary.imported_macro_artifact_count +=
        surface.metaprogramming_local_macro_artifact_count;
    summary.imported_interface_property_behavior_artifact_count +=
        surface.metaprogramming_local_interface_property_behavior_artifact_count;
    summary.imported_implementation_property_behavior_artifact_count +=
        surface.metaprogramming_local_implementation_property_behavior_artifact_count;
    summary.imported_runtime_method_list_count +=
        surface.metaprogramming_local_runtime_method_list_count;
    summary.deterministic =
        summary.deterministic && surface.metaprogramming_deterministic;
  }
  std::sort(summary.imported_module_names_lexicographic.begin(),
            summary.imported_module_names_lexicographic.end());
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready &&
      summary.imported_module_names_lexicographic.size() ==
          summary.imported_module_count;
  std::ostringstream replay_key;
  replay_key << Objc3MetaprogrammingModuleInterfaceReplayPreservationSummary()
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_preservation_ready="
             << (summary.separate_compilation_preservation_ready ? "true"
                                                                 : "false")
             << ";imported_module_count=" << summary.imported_module_count
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";expansion_lowering_replay_key=" << expansion_replay_key
             << ";synthesized_emission_replay_key=" << synthesized_replay_key
             << ";local_derive_method_count="
             << summary.local_derive_method_count
             << ";local_macro_artifact_count="
             << summary.local_macro_artifact_count
             << ";local_interface_property_behavior_artifact_count="
             << summary.local_interface_property_behavior_artifact_count
             << ";local_implementation_property_behavior_artifact_count="
             << summary.local_implementation_property_behavior_artifact_count
             << ";local_runtime_method_list_count="
             << summary.local_runtime_method_list_count
             << ";imported_derive_method_count="
             << summary.imported_derive_method_count
             << ";imported_macro_artifact_count="
             << summary.imported_macro_artifact_count
             << ";imported_interface_property_behavior_artifact_count="
             << summary.imported_interface_property_behavior_artifact_count
             << ";imported_implementation_property_behavior_artifact_count="
             << summary.imported_implementation_property_behavior_artifact_count
             << ";imported_runtime_method_list_count="
             << summary.imported_runtime_method_list_count;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"expansion_lowering_replay_key\":\""
      << EscapeJsonString(summary.expansion_lowering_replay_key)
      << "\",\"synthesized_emission_replay_key\":\""
      << EscapeJsonString(summary.synthesized_emission_replay_key)
      << "\",\"imported_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.imported_module_names_lexicographic)
      << ",\"local_derive_method_count\":"
      << summary.local_derive_method_count
      << ",\"local_macro_artifact_count\":"
      << summary.local_macro_artifact_count
      << ",\"local_interface_property_behavior_artifact_count\":"
      << summary.local_interface_property_behavior_artifact_count
      << ",\"local_implementation_property_behavior_artifact_count\":"
      << summary.local_implementation_property_behavior_artifact_count
      << ",\"local_runtime_method_list_count\":"
      << summary.local_runtime_method_list_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"imported_derive_method_count\":"
      << summary.imported_derive_method_count
      << ",\"imported_macro_artifact_count\":"
      << summary.imported_macro_artifact_count
      << ",\"imported_interface_property_behavior_artifact_count\":"
      << summary.imported_interface_property_behavior_artifact_count
      << ",\"imported_implementation_property_behavior_artifact_count\":"
      << summary.imported_implementation_property_behavior_artifact_count
      << ",\"imported_runtime_method_list_count\":"
      << summary.imported_runtime_method_list_count
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

Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary
BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
        &module_interface_summary,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    const Objc3FrontendOptions &options) {
  Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary summary;
  if (!options.metaprogramming_cache_root_relative_path.empty()) {
    summary.cache_root_relative_path =
        options.metaprogramming_cache_root_relative_path;
  }
  summary.metaprogramming_replay_key = module_interface_summary.replay_key;
  summary.local_macro_artifact_count =
      module_interface_summary.local_macro_artifact_count;
  summary.local_property_behavior_artifact_count =
      module_interface_summary.local_interface_property_behavior_artifact_count +
      module_interface_summary
          .local_implementation_property_behavior_artifact_count;
  summary.runtime_import_artifact_ready =
      module_interface_summary.runtime_import_artifact_ready;
  summary.deterministic = module_interface_summary.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.metaprogramming_macro_host_process_cache_runtime_integration_present) {
      continue;
    }
    ++summary.imported_module_count;
    summary.deterministic =
        summary.deterministic &&
        surface.metaprogramming_macro_host_process_cache_deterministic;
  }
  summary.separate_compilation_ready =
      summary.runtime_import_artifact_ready &&
      module_interface_summary.separate_compilation_preservation_ready;
  std::ostringstream replay_key;
  replay_key << Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary()
             << ";metaprogramming_replay_key=" << module_interface_summary.replay_key
             << ";local_macro_artifact_count="
             << summary.local_macro_artifact_count
             << ";local_property_behavior_artifact_count="
             << summary.local_property_behavior_artifact_count
             << ";imported_module_count=" << summary.imported_module_count
             << ";cache_root_relative_path=" << summary.cache_root_relative_path
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_ready="
             << (summary.separate_compilation_ready ? "true" : "false")
             << ";deterministic="
             << (summary.deterministic ? "true" : "false");
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson(
    const Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"host_executable_relative_path\":\""
      << EscapeJsonString(summary.host_executable_relative_path)
      << "\",\"cache_root_relative_path\":\""
      << EscapeJsonString(summary.cache_root_relative_path)
      << "\",\"host_model\":\"" << EscapeJsonString(summary.host_model)
      << "\",\"toolchain_model\":\""
      << EscapeJsonString(summary.toolchain_model)
      << "\",\"cache_model\":\"" << EscapeJsonString(summary.cache_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"metaprogramming_replay_key\":\""
      << EscapeJsonString(summary.metaprogramming_replay_key)
      << "\",\"local_macro_artifact_count\":"
      << summary.local_macro_artifact_count
      << ",\"local_property_behavior_artifact_count\":"
      << summary.local_property_behavior_artifact_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_ready\":"
      << (summary.separate_compilation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend

#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "sema/model/semantic_symbol_metaprogramming_interop_summaries.h"
#include "sema/objc3_sema_contract_metaprogramming_surfaces.h"

namespace objc3::artifacts::frontend {

Objc3MetaprogrammingSynthesizedArtifactEmissionContract
BuildMetaprogrammingSynthesizedArtifactEmissionContract(
    const Objc3MetaprogrammingExpansionLoweringContract &dependency_contract,
    const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle> &derive_bundles,
    const std::vector<Objc3IRMetaprogrammingMacroArtifactBundle> &macro_bundles,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &property_behavior_bundles) {
  Objc3MetaprogrammingSynthesizedArtifactEmissionContract contract;
  contract.derive_inventory_sites = dependency_contract.derive_inventory_sites;
  contract.emitted_derive_method_sites = derive_bundles.size();
  contract.emitted_macro_artifact_sites = macro_bundles.size();
  contract.emitted_property_behavior_artifact_sites =
      property_behavior_bundles.size();
  contract.emitted_global_artifact_sites =
      contract.emitted_derive_method_sites +
      contract.emitted_macro_artifact_sites +
      contract.emitted_property_behavior_artifact_sites;
  contract.emitted_runtime_method_list_sites =
      contract.emitted_derive_method_sites;
  contract.guard_blocked_sites = dependency_contract.guard_blocked_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      dependency_contract.deterministic &&
      contract.emitted_derive_method_sites <=
          dependency_contract.derived_selector_artifact_sites &&
      contract.emitted_macro_artifact_sites <=
          dependency_contract.macro_replay_visible_sites &&
      contract.emitted_property_behavior_artifact_sites <=
          dependency_contract.property_behavior_sites;
  return contract;
}

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

}  // namespace objc3::artifacts::frontend

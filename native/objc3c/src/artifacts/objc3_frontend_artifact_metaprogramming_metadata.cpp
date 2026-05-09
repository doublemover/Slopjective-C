#include "artifacts/objc3_frontend_artifact_metaprogramming_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendMetaprogrammingMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &metaprogramming_expansion_lowering_replay_key,
    const Objc3MetaprogrammingExpansionLoweringContract
        &metaprogramming_expansion_lowering_contract,
    const std::string &metaprogramming_synthesized_artifact_emission_replay_key,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
        &metaprogramming_synthesized_artifact_emission_contract,
    const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
        &metaprogramming_derived_method_bundles,
    const std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
        &metaprogramming_macro_artifact_bundles,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &metaprogramming_property_behavior_artifact_bundles,
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
        &metaprogramming_module_interface_replay_preservation_summary) {
  ir_frontend_metadata.lowering_metaprogramming_expansion_replay_key =
      metaprogramming_expansion_lowering_replay_key;
  ir_frontend_metadata.metaprogramming_expansion_lowering_derive_inventory_sites =
      metaprogramming_expansion_lowering_contract.derive_inventory_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_derived_selector_artifact_sites =
      metaprogramming_expansion_lowering_contract.derived_selector_artifact_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_macro_replay_visible_sites =
      metaprogramming_expansion_lowering_contract.macro_replay_visible_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_property_behavior_sites =
      metaprogramming_expansion_lowering_contract.property_behavior_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_synthesized_binding_sites =
      metaprogramming_expansion_lowering_contract.synthesized_binding_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_synthesized_getter_sites =
      metaprogramming_expansion_lowering_contract.synthesized_getter_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_synthesized_setter_sites =
      metaprogramming_expansion_lowering_contract.synthesized_setter_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_replay_visible_metadata_sites =
      metaprogramming_expansion_lowering_contract.replay_visible_metadata_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_guard_blocked_sites =
      metaprogramming_expansion_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_contract_violation_sites =
      metaprogramming_expansion_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_metaprogramming_expansion_lowering_handoff =
      metaprogramming_expansion_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_metaprogramming_synthesized_emission_replay_key =
      metaprogramming_synthesized_artifact_emission_replay_key;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_derive_method_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .emitted_derive_method_sites;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_macro_artifact_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .emitted_macro_artifact_sites;
  ir_frontend_metadata
      .metaprogramming_synthesized_emitted_property_behavior_artifact_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .emitted_property_behavior_artifact_sites;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_global_artifact_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .emitted_global_artifact_sites;
  ir_frontend_metadata
      .metaprogramming_synthesized_emitted_runtime_method_list_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .emitted_runtime_method_list_sites;
  ir_frontend_metadata.metaprogramming_synthesized_guard_blocked_sites =
      metaprogramming_synthesized_artifact_emission_contract.guard_blocked_sites;
  ir_frontend_metadata.metaprogramming_synthesized_contract_violation_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .contract_violation_sites;
  ir_frontend_metadata.deterministic_metaprogramming_synthesized_emission_handoff =
      metaprogramming_synthesized_artifact_emission_contract.deterministic;
  ir_frontend_metadata.metaprogramming_derived_method_bundles_lexicographic =
      metaprogramming_derived_method_bundles;
  ir_frontend_metadata.metaprogramming_macro_artifact_bundles_lexicographic =
      metaprogramming_macro_artifact_bundles;
  ir_frontend_metadata
      .metaprogramming_property_behavior_artifact_bundles_lexicographic =
      metaprogramming_property_behavior_artifact_bundles;

  ir_frontend_metadata
      .lowering_metaprogramming_module_interface_replay_preservation_key =
      metaprogramming_module_interface_replay_preservation_summary.replay_key;
  ir_frontend_metadata.metaprogramming_module_replay_local_derive_method_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_derive_method_count;
  ir_frontend_metadata.metaprogramming_module_replay_local_macro_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_macro_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_local_interface_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_interface_property_behavior_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_local_implementation_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_implementation_property_behavior_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_local_runtime_method_list_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_runtime_method_list_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_module_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_module_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_derive_method_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_derive_method_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_macro_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_macro_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_imported_interface_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_interface_property_behavior_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_imported_implementation_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_implementation_property_behavior_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_imported_runtime_method_list_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_runtime_method_list_count;
  ir_frontend_metadata.metaprogramming_module_replay_runtime_import_artifact_ready =
      metaprogramming_module_interface_replay_preservation_summary
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .metaprogramming_module_replay_separate_compilation_preservation_ready =
      metaprogramming_module_interface_replay_preservation_summary
          .separate_compilation_preservation_ready;
  ir_frontend_metadata
      .deterministic_metaprogramming_module_interface_replay_handoff =
      metaprogramming_module_interface_replay_preservation_summary.deterministic;
}

}  // namespace objc3::artifacts::frontend

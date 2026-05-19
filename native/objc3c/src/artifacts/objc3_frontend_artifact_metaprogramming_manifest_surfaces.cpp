#include "artifacts/objc3_frontend_artifact_metaprogramming_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"

namespace objc3::artifacts::frontend {

void WriteMetaprogrammingManifestSurfaces(
    std::ostream &manifest,
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary
        &metaprogramming_expansion_behavior_semantic_model_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary
        &metaprogramming_derive_expansion_inventory_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary
        &metaprogramming_macro_safety_sandbox_determinism_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &metaprogramming_property_behavior_legality_compatibility_summary,
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &metaprogramming_property_behavior_source_completion_summary,
    const Objc3MetaprogrammingExpansionLoweringContract
        &metaprogramming_expansion_lowering_contract,
    const std::string &metaprogramming_expansion_lowering_replay_key,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
        &metaprogramming_synthesized_artifact_emission_contract,
    const std::string &metaprogramming_synthesized_artifact_emission_replay_key,
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
        &metaprogramming_module_interface_replay_preservation_summary) {
  manifest
      << ",\"objc_metaprogramming_expansion_and_behavior_semantic_model\":"
      << BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
             metaprogramming_expansion_behavior_semantic_model_summary)
      << ",\"objc_metaprogramming_derive_expansion_inventory\":"
      << BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
             metaprogramming_derive_expansion_inventory_summary)
      << ",\"objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics\":"
      << BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
             metaprogramming_macro_safety_sandbox_determinism_summary)
      << ",\"objc_metaprogramming_property_behavior_legality_and_interaction_completion\":"
      << BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
             metaprogramming_property_behavior_legality_compatibility_summary)
      << ",\"objc_metaprogramming_expansion_and_lowering_contract\":"
      << BuildMetaprogrammingExpansionLoweringContractJson(
             metaprogramming_property_behavior_source_completion_summary,
             metaprogramming_derive_expansion_inventory_summary,
             metaprogramming_macro_safety_sandbox_determinism_summary,
             metaprogramming_property_behavior_legality_compatibility_summary,
             metaprogramming_expansion_lowering_contract,
             metaprogramming_expansion_lowering_replay_key)
      << ",\"objc_metaprogramming_synthesized_ast_and_ir_emission\":"
      << BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
             metaprogramming_expansion_lowering_contract,
             metaprogramming_synthesized_artifact_emission_contract,
             metaprogramming_synthesized_artifact_emission_replay_key)
      << ",\"objc_metaprogramming_module_interface_and_replay_preservation\":"
      << BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
             metaprogramming_module_interface_replay_preservation_summary);
}

}  // namespace objc3::artifacts::frontend

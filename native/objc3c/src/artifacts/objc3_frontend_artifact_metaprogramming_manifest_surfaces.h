#pragma once

#include <iosfwd>
#include <string>

struct Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary;
struct Objc3MetaprogrammingDeriveExpansionInventorySummary;
struct Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary;
struct Objc3MetaprogrammingExpansionLoweringContract;
struct Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary;
struct Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary;
struct Objc3MetaprogrammingSynthesizedArtifactEmissionContract;

namespace objc3::artifacts::frontend {

struct Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary;

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
        &metaprogramming_module_interface_replay_preservation_summary);

}  // namespace objc3::artifacts::frontend

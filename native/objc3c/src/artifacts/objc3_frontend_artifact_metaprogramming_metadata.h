#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"

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
        &metaprogramming_module_interface_replay_preservation_summary);

}  // namespace objc3::artifacts::frontend

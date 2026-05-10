#include "artifacts/objc3_frontend_artifact_dispatch_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"
#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

namespace objc3::artifacts::frontend {

void WriteDispatchManifestSurfaces(
    std::ostream &manifest,
    const Objc3DispatchDispatchIntentSemanticModelSummary
        &dispatch_dispatch_intent_semantic_model_summary,
    const Objc3DispatchDispatchIntentLegalitySummary
        &dispatch_dispatch_intent_legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary
        &dispatch_dispatch_intent_compatibility_summary,
    const Objc3DispatchDispatchControlLoweringContract
        &dispatch_dispatch_control_lowering_contract,
    const std::string &dispatch_dispatch_control_lowering_replay_key,
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
        &dispatch_dispatch_metadata_interface_preservation_summary) {
  manifest
      << ",\"objc_dispatch_dynamism_and_dispatch_control_semantic_model\":"
      << BuildDispatchDispatchIntentSemanticModelSummaryJson(
             dispatch_dispatch_intent_semantic_model_summary)
      << ",\"objc_dispatch_override_finality_and_sealing_legality\":"
      << BuildDispatchDispatchIntentLegalitySummaryJson(
             dispatch_dispatch_intent_legality_summary)
      << ",\"objc_dispatch_dynamism_control_compatibility_diagnostics\":"
      << BuildDispatchDispatchIntentCompatibilitySummaryJson(
             dispatch_dispatch_intent_compatibility_summary)
      << ",\"objc_dispatch_dispatch_control_lowering_contract\":"
      << BuildDispatchDispatchControlLoweringContractJson(
             dispatch_dispatch_intent_semantic_model_summary,
             dispatch_dispatch_intent_legality_summary,
             dispatch_dispatch_intent_compatibility_summary,
             dispatch_dispatch_control_lowering_contract,
             dispatch_dispatch_control_lowering_replay_key)
      << ",\"objc_dispatch_dispatch_metadata_and_interface_preservation\":"
      << BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
             dispatch_dispatch_metadata_interface_preservation_summary);
}

}  // namespace objc3::artifacts::frontend

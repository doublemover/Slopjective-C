#pragma once

#include <iosfwd>
#include <string>

#include "artifacts/objc3_frontend_runtime_import_artifacts.h"
#include "lower/contracts/dispatch_control_lowering_contracts.h"
#include "sema/objc3_sema_contract_dispatch_intent.h"

struct Objc3DispatchDispatchControlLoweringContract;
struct Objc3DispatchDispatchIntentCompatibilitySummary;
struct Objc3DispatchDispatchIntentLegalitySummary;
struct Objc3DispatchDispatchIntentSemanticModelSummary;

namespace objc3::artifacts::frontend {

struct Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary;

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
        &dispatch_dispatch_metadata_interface_preservation_summary);

}  // namespace objc3::artifacts::frontend

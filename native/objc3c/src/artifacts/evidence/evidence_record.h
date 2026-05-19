#pragma once

#include "artifacts/evidence/error_handling_replay_evidence.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::evidence {

using RuntimeAwareImportModuleClosureSummary =
    ::Objc3RuntimeAwareImportModuleFrontendClosureSummary;
using RuntimeBlockOwnershipPreservationSummary =
    ::Objc3RuntimeBlockOwnershipArtifactPreservationSummary;
using RuntimeStorageReflectionPreservationSummary =
    ::Objc3RuntimeStorageReflectionArtifactPreservationSummary;

[[nodiscard]] inline bool IsReady(
    const RuntimeAwareImportModuleClosureSummary &summary) {
  return ::IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(summary);
}

}  // namespace objc3::artifacts::evidence

#pragma once

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::evidence {

using CompatibilityStrictnessClaimSemanticsSummary =
    ::Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary;

[[nodiscard]] inline bool IsReady(
    const CompatibilityStrictnessClaimSemanticsSummary &summary) {
  return ::IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
      summary);
}

}  // namespace objc3::artifacts::evidence

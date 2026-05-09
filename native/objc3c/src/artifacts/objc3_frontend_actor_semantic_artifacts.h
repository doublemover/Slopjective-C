#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string
BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson(
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson(
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &summary);

[[nodiscard]] std::string
BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson(
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &summary);

}  // namespace objc3::artifacts::frontend

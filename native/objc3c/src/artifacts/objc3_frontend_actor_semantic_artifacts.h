#pragma once

#include <string>

#include "lower/contracts/concurrency_actor_contracts.h"
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

[[nodiscard]] std::string BuildConcurrencyActorLoweringMetadataContractJson(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &source_summary,
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &enforcement_summary,
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &hazard_summary,
    const Objc3ActorLoweringMetadataContract &contract,
    const std::string &replay_key);

}  // namespace objc3::artifacts::frontend

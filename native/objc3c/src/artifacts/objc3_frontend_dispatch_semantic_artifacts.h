#pragma once

#include <string>

#include "lower/contracts/dispatch_control_lowering_contracts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildDispatchDispatchIntentSemanticModelSummaryJson(
    const Objc3DispatchDispatchIntentSemanticModelSummary &summary);

[[nodiscard]] std::string BuildDispatchDispatchIntentLegalitySummaryJson(
    const Objc3DispatchDispatchIntentLegalitySummary &summary);

[[nodiscard]] std::string BuildDispatchDispatchIntentCompatibilitySummaryJson(
    const Objc3DispatchDispatchIntentCompatibilitySummary &summary);

[[nodiscard]] std::string BuildDispatchDispatchControlLoweringContractJson(
    const Objc3DispatchDispatchIntentSemanticModelSummary &semantic_summary,
    const Objc3DispatchDispatchIntentLegalitySummary &legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary &compatibility_summary,
    const Objc3DispatchDispatchControlLoweringContract &contract,
    const std::string &replay_key);

}  // namespace objc3::artifacts::frontend

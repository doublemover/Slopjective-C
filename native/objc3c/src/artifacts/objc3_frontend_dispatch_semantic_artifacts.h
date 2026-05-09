#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildDispatchDispatchIntentSemanticModelSummaryJson(
    const Objc3DispatchDispatchIntentSemanticModelSummary &summary);

[[nodiscard]] std::string BuildDispatchDispatchIntentLegalitySummaryJson(
    const Objc3DispatchDispatchIntentLegalitySummary &summary);

[[nodiscard]] std::string BuildDispatchDispatchIntentCompatibilitySummaryJson(
    const Objc3DispatchDispatchIntentCompatibilitySummary &summary);

}  // namespace objc3::artifacts::frontend

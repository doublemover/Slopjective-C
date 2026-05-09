#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildTypeSystemTypeSourceClosureSummaryJson(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary);

[[nodiscard]] std::string BuildControlFlowControlFlowSourceClosureSummaryJson(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary);

[[nodiscard]] std::string BuildErrorHandlingErrorSourceClosureSummaryJson(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary);

[[nodiscard]] std::string BuildConcurrencyAsyncSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary);

}  // namespace objc3::artifacts::frontend

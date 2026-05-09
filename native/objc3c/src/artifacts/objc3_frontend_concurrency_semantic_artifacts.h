#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string
BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary);

}  // namespace objc3::artifacts::frontend

#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string
BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &summary);

[[nodiscard]] std::string
BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary);

}  // namespace objc3::artifacts::frontend

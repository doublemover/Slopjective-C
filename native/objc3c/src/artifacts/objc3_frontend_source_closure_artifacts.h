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

[[nodiscard]] std::string
BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &summary);

[[nodiscard]] std::string
BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &summary);

[[nodiscard]] std::string BuildOwnershipSystemExtensionSourceClosureSummaryJson(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary);

[[nodiscard]] std::string
BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &summary);

[[nodiscard]] std::string
BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &summary);

[[nodiscard]] std::string BuildDispatchDispatchIntentSourceClosureSummaryJson(
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary &summary);

[[nodiscard]] std::string
BuildDispatchDispatchIntentSourceCompletionSummaryJson(
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary &summary);

[[nodiscard]] std::string
BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &summary);

[[nodiscard]] std::string
BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson(
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &summary);

[[nodiscard]] std::string
BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &summary);

[[nodiscard]] std::string BuildInteropForeignImportSourceClosureSummaryJson(
    const Objc3FrontendInteropForeignImportSourceClosureSummary &summary);

[[nodiscard]] std::string
BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson(
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &summary);

}  // namespace objc3::artifacts::frontend

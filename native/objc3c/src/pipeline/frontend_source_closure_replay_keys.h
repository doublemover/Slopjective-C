#pragma once

#include <string>

#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"
#include "sema/model/frontend_type_source_closure.h"
#include "sema/model/semantic_ownership.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

std::string BuildTypeSystemTypeSourceClosureReplayKey(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary);

std::string BuildControlFlowControlFlowSourceClosureReplayKey(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary);

std::string BuildErrorHandlingErrorSourceClosureReplayKey(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary);

std::string BuildConcurrencyAsyncSourceClosureReplayKey(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary);

std::string BuildConcurrencyActorMemberIsolationSourceClosureReplayKey(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &summary);

std::string BuildConcurrencyTaskGroupCancellationSourceClosureReplayKey(
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &summary);

std::string BuildOwnershipSystemExtensionSourceClosureReplayKey(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary);

std::string BuildOwnershipCleanupResourceCaptureSourceCompletionReplayKey(
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &summary);

std::string BuildOwnershipRetainableCFamilySourceCompletionReplayKey(
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &summary);

std::string BuildDispatchDispatchIntentSourceClosureReplayKey(
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary &summary);

std::string BuildDispatchDispatchIntentSourceCompletionReplayKey(
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary &summary);

std::string BuildMetaprogrammingMetaprogrammingSourceClosureReplayKey(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &summary);

std::string BuildMetaprogrammingMacroPackageProvenanceSourceCompletionReplayKey(
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &summary);

std::string BuildMetaprogrammingPropertyBehaviorSourceCompletionReplayKey(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &summary);

std::string BuildInteropForeignImportSourceClosureReplayKey(
    const Objc3FrontendInteropForeignImportSourceClosureSummary &summary);

std::string BuildInteropCppSwiftInteropAnnotationSourceCompletionReplayKey(
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &summary);

std::string BuildToolingDiagnosticsMigratorSourceInventoryReplayKey(
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &summary);

std::string BuildToolingMigrationCanonicalizationSourceCompletionReplayKey(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &summary);

std::string BuildToolingDiagnosticTaxonomyPortabilityContractReplayKey(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary &summary);

std::string BuildToolingFeatureSpecificFixitSynthesisReplayKey(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &summary);

std::string BuildSymbolGraphScopeResolutionHandoffKey(
    const Objc3FrontendSymbolGraphScopeResolutionSummary &summary);

}  // namespace objc3c::pipeline::orchestration

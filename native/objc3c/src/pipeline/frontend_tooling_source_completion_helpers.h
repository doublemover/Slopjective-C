#pragma once

#include "pipeline/frontend_source_closure_replay_keys.h"
#include "pipeline/results/canonical_literal_rejection_counts.h"
#include "pipeline/results/compile_options.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
BuildToolingDiagnosticsMigratorSourceInventorySummary(
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &error_handling_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &concurrency_async_summary,
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &concurrency_actor_summary,
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &concurrency_task_summary,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &ownership_summary,
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &ownership_cleanup_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &ownership_family_summary,
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary
        &dispatch_closure_summary,
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
        &dispatch_completion_summary,
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &metaprogramming_closure_summary,
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &metaprogramming_macro_summary,
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &metaprogramming_property_summary,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &interop_closure_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_completion_summary);

Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
BuildToolingMigrationCanonicalizationSourceCompletionSummary(
    const Objc3FrontendOptions &options,
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &inventory_summary);

}  // namespace objc3c::pipeline::orchestration

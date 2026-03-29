#pragma once

#include <string>
#include <vector>

#include "sema/objc3_sema_contract.h"

struct Objc3FrontendErrorHandlingErrorSourceClosureSummary;
struct Objc3FrontendConcurrencyAsyncSourceClosureSummary;
struct Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary;
struct Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary;
struct Objc3FrontendOwnershipSystemExtensionSourceClosureSummary;
struct Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary;
struct Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary;
struct Objc3FrontendDispatchDispatchIntentSourceCompletionSummary;
struct Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary;
struct Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary;
struct Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary;
struct Objc3FrontendInteropForeignImportSourceClosureSummary;
struct Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary;

Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,
                                                                bool legacy_compatibility_mode,
                                                                bool migration_assist_enabled,
                                                                bool allow_source_only_block_literals,
                                                                bool allow_source_only_defer_statements,
                                                                bool allow_source_only_error_runtime_surface,
                                                                bool arc_mode_enabled,
                                                                std::vector<std::string> &diagnostics);
Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface);
bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);
std::string ExplainNonDeterministicSemanticTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff);
Objc3TypeSystemTypeSemanticModelSummary BuildTypeSystemTypeSemanticModelSummary(
    const Objc3Program &ast, const Objc3SemanticIntegrationSurface &surface,
    std::size_t max_message_send_args);
Objc3ControlFlowControlFlowSemanticModelSummary BuildControlFlowControlFlowSemanticModelSummary(
    const Objc3Program &ast);
Objc3ErrorHandlingErrorSemanticModelSummary BuildErrorHandlingErrorSemanticModelSummary(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary
BuildConcurrencyAsyncEffectSuspensionSemanticModelSummary(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3ConcurrencyActorIsolationSendableSemanticModelSummary
BuildConcurrencyActorIsolationSendableSemanticModelSummary(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
BuildConcurrencyActorIsolationSendabilityEnforcementSummary(
    const Objc3Program &ast,
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary
BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummary(
    const Objc3Program &ast,
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
BuildConcurrencyTaskExecutorCancellationSemanticModelSummary(
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3OwnershipSystemExtensionSemanticModelSummary
BuildOwnershipSystemExtensionSemanticModelSummary(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
        &source_summary,
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &completion_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary);
Objc3InteropInteropSemanticModelSummary BuildInteropInteropSemanticModelSummary(
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &foreign_source_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_source_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &ownership_summary,
    const Objc3ErrorHandlingErrorBridgeLegalitySummary &error_summary,
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &async_summary,
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &actor_summary);
Objc3InteropInteropRuntimeParitySummary
BuildInteropInteropRuntimeParitySummary(
    const Objc3Program &program,
    const Objc3InteropInteropSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3InteropCppInteropInteractionSummary
BuildInteropCppInteropInteractionSummary(
    const Objc3Program &program,
    const Objc3InteropInteropRuntimeParitySummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3InteropSwiftInteropIsolationSummary
BuildInteropSwiftInteropIsolationSummary(
    const Objc3Program &program,
    const Objc3InteropCppInteropInteractionSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3DispatchDispatchIntentSemanticModelSummary
BuildDispatchDispatchIntentSemanticModelSummary(
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary
BuildMetaprogrammingExpansionBehaviorSemanticModelSummary(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &source_summary,
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &macro_summary,
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_summary,
    const std::vector<std::string> &diagnostics);
Objc3MetaprogrammingDeriveExpansionInventorySummary
BuildMetaprogrammingDeriveExpansionInventorySummary(
    const Objc3Program &program,
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary
BuildMetaprogrammingMacroSafetySandboxDeterminismSummary(
    const Objc3Program &program,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummary(
    const Objc3Program &program,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
  Objc3DispatchDispatchIntentLegalitySummary
  BuildDispatchDispatchIntentLegalitySummary(
      const Objc3Program &program,
      const Objc3DispatchDispatchIntentSemanticModelSummary &dependency_summary,
      const std::vector<std::string> &diagnostics);
  Objc3DispatchDispatchIntentCompatibilitySummary
  BuildDispatchDispatchIntentCompatibilitySummary(
      const Objc3Program &program,
      const Objc3DispatchDispatchIntentLegalitySummary &dependency_summary,
      const std::vector<std::string> &diagnostics);
  Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
  BuildOwnershipResourceMoveUseAfterMoveSemanticsSummary(
      const Objc3Program &program,
    const Objc3OwnershipSystemExtensionSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3OwnershipBorrowedPointerEscapeAnalysisSummary
BuildOwnershipBorrowedPointerEscapeAnalysisSummary(
    const Objc3Program &program,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummary(
    const Objc3Program &program,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
BuildConcurrencyStructuredTaskCancellationSemanticSummary(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
BuildConcurrencyExecutorHopAffinityCompatibilitySummary(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &dependency_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const std::vector<std::string> &diagnostics);
Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary
BuildConcurrencyAwaitSuspensionResumeSemanticSummary(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary
BuildConcurrencyAsyncDiagnosticsCompatibilitySummary(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &dependency_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const Objc3Program &ast,
    const std::vector<std::string> &diagnostics);
Objc3ErrorHandlingTryDoCatchSemanticSummary BuildErrorHandlingTryDoCatchSemanticSummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &surface,
    bool allow_source_only_error_runtime_surface,
    std::vector<std::string> &diagnostics);
Objc3ErrorHandlingErrorBridgeLegalitySummary BuildErrorHandlingErrorBridgeLegalitySummary(
    const Objc3Program &program,
    bool allow_source_only_error_runtime_surface,
    std::vector<std::string> &diagnostics);
Objc3AtomicMemoryOrderMappingSummary BuildAtomicMemoryOrderMappingSummary(const Objc3ParsedProgram &program);
Objc3VectorTypeLoweringSummary BuildVectorTypeLoweringSummary(const Objc3SemanticIntegrationSurface &surface);
void ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program,
                                             const std::unordered_map<std::string, FunctionInfo> &surface_functions,
                                             std::vector<std::string> &diagnostics);
void ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,
                            const Objc3SemanticValidationOptions &options,
                            std::vector<std::string> &diagnostics);
void RefreshSemanticIntegrationSurfaceAfterBodyValidation(const Objc3ParsedProgram &program,
                                                          Objc3SemanticIntegrationSurface &surface);

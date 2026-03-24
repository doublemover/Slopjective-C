#pragma once

#include <vector>

#include "sema/objc3_sema_contract.h"

struct Objc3FrontendPart6ErrorSourceClosureSummary;
struct Objc3FrontendPart7AsyncSourceClosureSummary;
struct Objc3FrontendPart7ActorMemberIsolationSourceClosureSummary;
struct Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary;
struct Objc3FrontendPart8SystemExtensionSourceClosureSummary;
struct Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary;
struct Objc3FrontendPart8RetainableCFamilySourceCompletionSummary;
struct Objc3FrontendPart9DispatchIntentSourceCompletionSummary;
struct Objc3FrontendPart10MetaprogrammingSourceClosureSummary;
struct Objc3FrontendPart10MacroPackageProvenanceSourceCompletionSummary;
struct Objc3FrontendPart10PropertyBehaviorSourceCompletionSummary;
struct Objc3FrontendPart11ForeignImportSourceClosureSummary;
struct Objc3FrontendPart11CppSwiftInteropAnnotationSourceCompletionSummary;

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
Objc3Part3TypeSemanticModelSummary BuildPart3TypeSemanticModelSummary(
    const Objc3Program &ast, const Objc3SemanticIntegrationSurface &surface,
    std::size_t max_message_send_args);
Objc3Part5ControlFlowSemanticModelSummary BuildPart5ControlFlowSemanticModelSummary(
    const Objc3Program &ast);
Objc3Part6ErrorSemanticModelSummary BuildPart6ErrorSemanticModelSummary(
    const Objc3FrontendPart6ErrorSourceClosureSummary &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3Part7AsyncEffectSuspensionSemanticModelSummary
BuildPart7AsyncEffectSuspensionSemanticModelSummary(
    const Objc3FrontendPart7AsyncSourceClosureSummary &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3Part7ActorIsolationSendableSemanticModelSummary
BuildPart7ActorIsolationSendableSemanticModelSummary(
    const Objc3FrontendPart7ActorMemberIsolationSourceClosureSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3Part7ActorIsolationSendabilityEnforcementSummary
BuildPart7ActorIsolationSendabilityEnforcementSummary(
    const Objc3Program &ast,
    const Objc3Part7ActorIsolationSendableSemanticModelSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part7ActorRaceHazardEscapeDiagnosticsSummary
BuildPart7ActorRaceHazardEscapeDiagnosticsSummary(
    const Objc3Program &ast,
    const Objc3Part7ActorIsolationSendabilityEnforcementSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part7TaskExecutorCancellationSemanticModelSummary
BuildPart7TaskExecutorCancellationSemanticModelSummary(
    const Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3Part8SystemExtensionSemanticModelSummary
BuildPart8SystemExtensionSemanticModelSummary(
    const Objc3FrontendPart8SystemExtensionSourceClosureSummary
        &source_summary,
    const Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary
        &completion_summary,
    const Objc3FrontendPart8RetainableCFamilySourceCompletionSummary
        &retainable_summary);
Objc3Part11InteropSemanticModelSummary BuildPart11InteropSemanticModelSummary(
    const Objc3FrontendPart11ForeignImportSourceClosureSummary
        &foreign_source_summary,
    const Objc3FrontendPart11CppSwiftInteropAnnotationSourceCompletionSummary
        &interop_source_summary,
    const Objc3Part8CaptureListRetainableFamilyLegalityCompletionSummary
        &ownership_summary,
    const Objc3Part6ErrorBridgeLegalitySummary &error_summary,
    const Objc3Part7AsyncDiagnosticsCompatibilitySummary &async_summary,
    const Objc3Part7ActorRaceHazardEscapeDiagnosticsSummary &actor_summary);
Objc3Part9DispatchIntentSemanticModelSummary
BuildPart9DispatchIntentSemanticModelSummary(
    const Objc3FrontendPart9DispatchIntentSourceCompletionSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
Objc3Part10ExpansionBehaviorSemanticModelSummary
BuildPart10ExpansionBehaviorSemanticModelSummary(
    const Objc3FrontendPart10MetaprogrammingSourceClosureSummary
        &source_summary,
    const Objc3FrontendPart10MacroPackageProvenanceSourceCompletionSummary
        &macro_summary,
    const Objc3FrontendPart10PropertyBehaviorSourceCompletionSummary
        &property_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part10DeriveExpansionInventorySummary
BuildPart10DeriveExpansionInventorySummary(
    const Objc3Program &program,
    const Objc3Part10ExpansionBehaviorSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part10MacroSafetySandboxDeterminismSummary
BuildPart10MacroSafetySandboxDeterminismSummary(
    const Objc3Program &program,
    const Objc3Part10DeriveExpansionInventorySummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part10PropertyBehaviorLegalityCompatibilitySummary
BuildPart10PropertyBehaviorLegalityCompatibilitySummary(
    const Objc3Program &program,
    const Objc3Part10MacroSafetySandboxDeterminismSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
  Objc3Part9DispatchIntentLegalitySummary
  BuildPart9DispatchIntentLegalitySummary(
      const Objc3Program &program,
      const Objc3Part9DispatchIntentSemanticModelSummary &dependency_summary,
      const std::vector<std::string> &diagnostics);
  Objc3Part9DispatchIntentCompatibilitySummary
  BuildPart9DispatchIntentCompatibilitySummary(
      const Objc3Program &program,
      const Objc3Part9DispatchIntentLegalitySummary &dependency_summary,
      const std::vector<std::string> &diagnostics);
  Objc3Part8ResourceMoveUseAfterMoveSemanticsSummary
  BuildPart8ResourceMoveUseAfterMoveSemanticsSummary(
      const Objc3Program &program,
    const Objc3Part8SystemExtensionSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part8BorrowedPointerEscapeAnalysisSummary
BuildPart8BorrowedPointerEscapeAnalysisSummary(
    const Objc3Program &program,
    const Objc3Part8ResourceMoveUseAfterMoveSemanticsSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part8CaptureListRetainableFamilyLegalityCompletionSummary
BuildPart8CaptureListRetainableFamilyLegalityCompletionSummary(
    const Objc3Program &program,
    const Objc3Part8BorrowedPointerEscapeAnalysisSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part7StructuredTaskCancellationSemanticSummary
BuildPart7StructuredTaskCancellationSemanticSummary(
    const Objc3Part7TaskExecutorCancellationSemanticModelSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part7ExecutorHopAffinityCompatibilitySummary
BuildPart7ExecutorHopAffinityCompatibilitySummary(
    const Objc3Part7StructuredTaskCancellationSemanticSummary
        &dependency_summary,
    const Objc3FrontendPart7AsyncSourceClosureSummary &source_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part7AwaitSuspensionResumeSemanticSummary
BuildPart7AwaitSuspensionResumeSemanticSummary(
    const Objc3Part7AsyncEffectSuspensionSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics);
Objc3Part7AsyncDiagnosticsCompatibilitySummary
BuildPart7AsyncDiagnosticsCompatibilitySummary(
    const Objc3Part7AwaitSuspensionResumeSemanticSummary &dependency_summary,
    const Objc3FrontendPart7AsyncSourceClosureSummary &source_summary,
    const Objc3Program &ast,
    const std::vector<std::string> &diagnostics);
Objc3Part6TryDoCatchSemanticSummary BuildPart6TryDoCatchSemanticSummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &surface,
    bool allow_source_only_error_runtime_surface,
    std::vector<std::string> &diagnostics);
Objc3Part6ErrorBridgeLegalitySummary BuildPart6ErrorBridgeLegalitySummary(
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

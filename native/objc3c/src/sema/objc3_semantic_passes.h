#pragma once

#include <vector>

#include "sema/objc3_sema_contract.h"

struct Objc3FrontendPart6ErrorSourceClosureSummary;
struct Objc3FrontendPart7AsyncSourceClosureSummary;
struct Objc3FrontendPart7ActorMemberIsolationSourceClosureSummary;
struct Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary;

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
Objc3Part7TaskExecutorCancellationSemanticModelSummary
BuildPart7TaskExecutorCancellationSemanticModelSummary(
    const Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface);
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

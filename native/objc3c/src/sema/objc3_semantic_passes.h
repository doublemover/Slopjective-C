#pragma once

#include <vector>

#include "sema/objc3_sema_contract.h"

Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,
                                                                bool legacy_compatibility_mode,
                                                                bool migration_assist_enabled,
                                                                bool allow_source_only_block_literals,
                                                                bool arc_mode_enabled,
                                                                std::vector<std::string> &diagnostics);
Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface);
bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);
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

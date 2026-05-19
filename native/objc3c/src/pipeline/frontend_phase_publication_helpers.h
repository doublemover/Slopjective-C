#pragma once

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
BuildToolingDiagnosticTaxonomyPortabilityContractSummary(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &migration_summary,
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface
        &diagnostic_surface);

Objc3ToolingFeatureSpecificFixitSynthesisSummary
BuildToolingFeatureSpecificFixitSynthesisSummary(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
        &taxonomy_summary);

Objc3FrontendSemanticDiagnosticTaxonomyPhaseResult
BuildObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
    const Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options);

void AdoptObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
    Objc3FrontendPipelineResult &result,
    Objc3FrontendSemanticDiagnosticTaxonomyPhaseResult phase);

void PopulateObjc3FrontendReadinessLoweringPhaseResult(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options);

}  // namespace objc3c::pipeline::orchestration

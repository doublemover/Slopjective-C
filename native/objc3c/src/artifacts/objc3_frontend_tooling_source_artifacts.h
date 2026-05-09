#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &summary);

[[nodiscard]] std::string
BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &summary);

[[nodiscard]] std::string
BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary &summary);

[[nodiscard]] std::string BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &summary);

[[nodiscard]] Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
BuildToolingLegacyCanonicalMigrationSemanticsSummary(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &compatibility_summary,
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &fixit_summary);

[[nodiscard]] std::string BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary &summary);

}  // namespace objc3::artifacts::frontend

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

}  // namespace objc3::artifacts::frontend

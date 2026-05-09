#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string
BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
    const Objc3CrossModuleSemanticContractsDiagnosticsSummary &summary);

}  // namespace objc3::artifacts::frontend

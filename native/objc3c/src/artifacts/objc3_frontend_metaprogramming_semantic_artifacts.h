#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string
BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &summary);

[[nodiscard]] std::string
BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &summary);

[[nodiscard]] std::string
BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &summary);

[[nodiscard]] std::string
BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &summary);

}  // namespace objc3::artifacts::frontend

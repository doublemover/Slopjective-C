#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildEffectsOwnershipSemanticModelSummaryJson(
    const Objc3EffectsOwnershipSemanticModelSummary &summary);

[[nodiscard]] std::string
BuildOwnershipSystemExtensionSemanticModelSummaryJson(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &summary);

[[nodiscard]] std::string
BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &summary);

[[nodiscard]] std::string
BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &summary);

[[nodiscard]] std::string
BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &summary);

}  // namespace objc3::artifacts::frontend

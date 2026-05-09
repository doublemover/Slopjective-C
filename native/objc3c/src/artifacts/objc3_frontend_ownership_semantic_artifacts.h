#pragma once

#include <string>

#include "lower/contracts/ownership_system_extension_contracts.h"
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

[[nodiscard]] std::string BuildOwnershipSystemExtensionLoweringContractJson(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &semantic_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &resource_summary,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &borrowed_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &family_summary,
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const std::string &replay_key);

[[nodiscard]] std::string BuildOwnershipBorrowedRetainableAbiCompletionReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary);

[[nodiscard]] std::string BuildOwnershipBorrowedRetainableAbiCompletionJson(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary,
    const std::string &lowering_replay_key,
    const std::string &abi_completion_replay_key);

}  // namespace objc3::artifacts::frontend

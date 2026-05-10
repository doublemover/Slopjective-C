#pragma once

#include <string>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] Objc3ControlFlowControlFlowSafetyLoweringContract
BuildControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSemanticModelSummary &summary);

[[nodiscard]] std::string BuildControlFlowControlFlowSemanticModelSummaryJson(
    const Objc3ControlFlowControlFlowSemanticModelSummary &summary);

[[nodiscard]] std::string BuildControlFlowControlFlowSafetyLoweringContractJson(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract,
    const Objc3ControlFlowControlFlowSemanticModelSummary &semantic_summary,
    const std::string &semantic_summary_replay_key,
    const std::string &replay_key);

}  // namespace objc3::artifacts::frontend

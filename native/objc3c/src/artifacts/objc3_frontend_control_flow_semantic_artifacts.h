#pragma once

#include <string>

#include "lower/contracts/control_flow_safety_lowering_contracts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildControlFlowControlFlowSemanticModelSummaryJson(
    const Objc3ControlFlowControlFlowSemanticModelSummary &summary);

[[nodiscard]] std::string BuildControlFlowControlFlowSafetyLoweringContractJson(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract,
    const Objc3ControlFlowControlFlowSemanticModelSummary &semantic_summary,
    const std::string &semantic_summary_replay_key,
    const std::string &replay_key);

}  // namespace objc3::artifacts::frontend

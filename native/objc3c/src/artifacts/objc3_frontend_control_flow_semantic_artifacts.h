#pragma once

#include <string>

#include "lower/contracts/control_flow_safety_lowering_contracts.h"
#include "sema/objc3_sema_contract_effects_flow_control_flow.h"

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

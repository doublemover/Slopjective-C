#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <string>

bool IsValidObjc3ErrorDiagnosticsRecoveryLoweringContract(
    const Objc3ErrorDiagnosticsRecoveryLoweringContract &contract) {
  if (contract.parser_diagnostic_sites > contract.error_diagnostic_sites ||
      contract.semantic_diagnostic_sites > contract.error_diagnostic_sites ||
      contract.fixit_hint_sites > contract.error_diagnostic_sites ||
      contract.recovery_candidate_sites > contract.error_diagnostic_sites ||
      contract.recovery_applied_sites > contract.recovery_candidate_sites ||
      contract.normalized_sites > contract.error_diagnostic_sites ||
      contract.guard_blocked_sites > contract.error_diagnostic_sites ||
      contract.contract_violation_sites > contract.error_diagnostic_sites) {
    return false;
  }
  if (contract.parser_diagnostic_sites + contract.semantic_diagnostic_sites >
      contract.error_diagnostic_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.guard_blocked_sites !=
      contract.error_diagnostic_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ErrorDiagnosticsRecoveryLoweringReplayKey(
    const Objc3ErrorDiagnosticsRecoveryLoweringContract &contract) {
  return std::string("error_diagnostic_sites=") +
             std::to_string(contract.error_diagnostic_sites) +
         ";parser_diagnostic_sites=" +
         std::to_string(contract.parser_diagnostic_sites) +
         ";semantic_diagnostic_sites=" +
         std::to_string(contract.semantic_diagnostic_sites) +
         ";fixit_hint_sites=" + std::to_string(contract.fixit_hint_sites) +
         ";recovery_candidate_sites=" +
         std::to_string(contract.recovery_candidate_sites) +
         ";recovery_applied_sites=" +
         std::to_string(contract.recovery_applied_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";guard_blocked_sites=" + std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract;
}

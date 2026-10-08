#include "lower/contracts/type_system_lowering_contracts.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

bool IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract) {
  if (contract.guard_clause_sites < contract.guard_statement_sites) {
    return false;
  }
  if (contract.live_guard_short_circuit_sites +
          contract.fail_closed_guard_short_circuit_sites !=
      contract.guard_statement_sites) {
    return false;
  }
  if (contract.live_match_dispatch_sites +
          contract.fail_closed_match_dispatch_sites !=
      contract.match_statement_sites) {
    return false;
  }
  if (contract.live_match_expression_dispatch_sites +
          contract.fail_closed_match_expression_dispatch_sites !=
      contract.match_expression_sites) {
    return false;
  }
  if (contract.live_defer_cleanup_sites +
          contract.fail_closed_defer_cleanup_sites !=
      contract.defer_statement_sites) {
    return false;
  }
  if (contract.deterministic_fail_closed_sites !=
      contract.fail_closed_guard_short_circuit_sites +
          contract.fail_closed_match_dispatch_sites +
          contract.fail_closed_match_expression_dispatch_sites +
          contract.fail_closed_defer_cleanup_sites) {
    return false;
  }
  if (contract.contract_violation_sites >
          contract.live_guard_short_circuit_sites +
          contract.live_match_dispatch_sites +
          contract.live_match_expression_dispatch_sites +
          contract.live_defer_cleanup_sites +
          contract.deterministic_fail_closed_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract) {
  return std::string("guard_statement_sites=") +
         std::to_string(contract.guard_statement_sites) +
         ";guard_clause_sites=" +
         std::to_string(contract.guard_clause_sites) +
         ";match_statement_sites=" +
         std::to_string(contract.match_statement_sites) +
         ";match_expression_sites=" +
         std::to_string(contract.match_expression_sites) +
         ";defer_statement_sites=" +
         std::to_string(contract.defer_statement_sites) +
         ";live_guard_short_circuit_sites=" +
         std::to_string(contract.live_guard_short_circuit_sites) +
         ";live_match_dispatch_sites=" +
         std::to_string(contract.live_match_dispatch_sites) +
         ";live_match_expression_dispatch_sites=" +
         std::to_string(contract.live_match_expression_dispatch_sites) +
         ";live_defer_cleanup_sites=" +
         std::to_string(contract.live_defer_cleanup_sites) +
         ";fail_closed_guard_short_circuit_sites=" +
         std::to_string(contract.fail_closed_guard_short_circuit_sites) +
         ";fail_closed_match_dispatch_sites=" +
         std::to_string(contract.fail_closed_match_dispatch_sites) +
         ";fail_closed_match_expression_dispatch_sites=" +
         std::to_string(contract.fail_closed_match_expression_dispatch_sites) +
         ";fail_closed_defer_cleanup_sites=" +
         std::to_string(contract.fail_closed_defer_cleanup_sites) +
         ";deterministic_fail_closed_sites=" +
         std::to_string(contract.deterministic_fail_closed_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3ControlFlowControlFlowSafetyLoweringLaneContract;
}

std::string Objc3ControlFlowControlFlowSafetyLoweringSummary() {
  std::ostringstream out;
  out << "contract_id=" << kObjc3ControlFlowControlFlowSafetyLoweringContractId
      << ";surface_path="
      << kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath
      << ";guard_model=" << kObjc3ControlFlowControlFlowSafetyLoweringGuardModel
      << ";match_model=" << kObjc3ControlFlowControlFlowSafetyLoweringMatchModel
      << ";defer_model=" << kObjc3ControlFlowControlFlowSafetyLoweringDeferModel
      << ";authority_model="
      << kObjc3ControlFlowControlFlowSafetyLoweringAuthorityModel
      << ";fail_closed_model="
      << kObjc3ControlFlowControlFlowSafetyLoweringFailClosedModel
      << ";lane_contract="
      << kObjc3ControlFlowControlFlowSafetyLoweringLaneContract;
  return out.str();
}

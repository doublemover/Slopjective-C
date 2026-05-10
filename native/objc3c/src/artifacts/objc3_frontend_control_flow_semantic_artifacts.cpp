#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

Objc3ControlFlowControlFlowSafetyLoweringContract
BuildControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  Objc3ControlFlowControlFlowSafetyLoweringContract contract;
  contract.guard_statement_sites = summary.guard_exit_enforcement_sites;
  contract.guard_clause_sites = summary.guard_binding_clause_semantic_sites +
                                summary.guard_condition_clause_semantic_sites;
  contract.match_statement_sites = summary.match_statement_semantic_sites;
  contract.defer_statement_sites = summary.defer_statement_semantic_sites;
  // Result-case payload matching remains fail-closed until that ABI tranche
  // lands, while the literal/default/wildcard/binding slice can lower live.
  const bool result_case_patterns_present =
      summary.match_result_case_scope_sites > 0u;
  contract.live_guard_short_circuit_sites = contract.guard_statement_sites;
  contract.live_match_dispatch_sites =
      result_case_patterns_present ? 0u : contract.match_statement_sites;
  contract.live_defer_cleanup_sites = contract.defer_statement_sites;
  contract.fail_closed_guard_short_circuit_sites = 0;
  contract.fail_closed_match_dispatch_sites =
      contract.match_statement_sites - contract.live_match_dispatch_sites;
  contract.fail_closed_defer_cleanup_sites = 0;
  contract.deterministic_fail_closed_sites =
      contract.fail_closed_guard_short_circuit_sites +
      contract.fail_closed_match_dispatch_sites +
      contract.fail_closed_defer_cleanup_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic = summary.deterministic &&
                           summary.ready_for_lowering_and_runtime;
  return contract;
}

}  // namespace objc3::artifacts::frontend

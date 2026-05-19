#include "pipeline/frontend_control_flow_source_closure_helpers.h"

#include "pipeline/frontend_source_closure_replay_keys.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendControlFlowControlFlowSourceClosureSummary
BuildControlFlowControlFlowSourceClosureSummary(
    const Objc3Program &program,
    const std::vector<Objc3LexToken> &tokens) {
  Objc3FrontendControlFlowControlFlowSourceClosureSummary summary;
  for (const auto &token : tokens) {
    if (token.kind == Objc3LexTokenKind::KwDefer) {
      ++summary.defer_keyword_sites;
    }
  }
  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      detail::CollectControlFlowControlFlowSourceClosureStmtSites(
          stmt.get(), summary);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        detail::CollectControlFlowControlFlowSourceClosureStmtSites(
            stmt.get(), summary);
      }
    }
  }
  summary.guard_binding_source_supported = true;
  summary.guard_condition_list_source_supported = true;
  summary.switch_case_pattern_source_supported = true;
  summary.defer_statement_source_supported = true;
  summary.match_statement_source_supported = true;
  summary.match_wildcard_pattern_source_supported = true;
  summary.match_literal_pattern_source_supported = true;
  summary.match_binding_pattern_source_supported = true;
  summary.match_result_case_pattern_source_supported = true;
  summary.defer_keyword_reserved = true;
  summary.defer_fail_closed = false;
  summary.match_expression_fail_closed = true;
  summary.guarded_pattern_fail_closed = true;
  summary.type_test_pattern_fail_closed = true;
  summary.deterministic_handoff =
      summary.guard_binding_clause_sites >= summary.guard_binding_sites &&
      summary.guard_boolean_condition_sites + summary.guard_binding_sites >=
          summary.guard_binding_sites &&
      summary.switch_default_pattern_sites <=
          summary.switch_case_pattern_sites +
              summary.switch_default_pattern_sites &&
      summary.match_default_sites <=
          summary.match_statement_sites + summary.match_default_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildControlFlowControlFlowSourceClosureReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration

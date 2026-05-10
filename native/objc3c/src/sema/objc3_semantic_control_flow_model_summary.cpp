#include "sema/objc3_semantic_passes.h"

#include "sema/objc3_semantic_match_exhaustiveness.h"

#include <memory>
#include <sstream>
#include <vector>

static void CollectControlFlowControlFlowSemanticStmtSitesFromList(
    const std::vector<std::unique_ptr<Stmt>> &statements, int loop_depth,
    int switch_depth, int defer_loop_base_depth, int defer_switch_base_depth,
    bool inside_defer_body,
    Objc3ControlFlowControlFlowSemanticModelSummary &summary);

static void CollectControlFlowControlFlowSemanticStmtSites(
    const Stmt *stmt, int loop_depth, int switch_depth,
    int defer_loop_base_depth, int defer_switch_base_depth,
    bool inside_defer_body,
    Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      const IfStmt *if_stmt = stmt->if_stmt.get();
      if (if_stmt->optional_binding_surface_enabled &&
          if_stmt->guard_binding_surface_enabled) {
        ++summary.guard_binding_semantic_sites;
        summary.guard_binding_clause_semantic_sites +=
            if_stmt->optional_binding_clause_count;
        summary.guard_refinement_sites += if_stmt->optional_binding_clause_count;
        ++summary.guard_exit_enforcement_sites;
        summary.guard_condition_clause_semantic_sites +=
            if_stmt->guard_boolean_condition_clause_count;
      } else if (if_stmt->guard_condition_list_surface_enabled) {
        ++summary.guard_condition_statement_sites;
        summary.guard_condition_clause_semantic_sites +=
            if_stmt->guard_boolean_condition_clause_count;
        ++summary.guard_exit_enforcement_sites;
      }
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          if_stmt->then_body, loop_depth, switch_depth, defer_loop_base_depth,
          defer_switch_base_depth, inside_defer_body, summary);
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          if_stmt->else_body, loop_depth, switch_depth, defer_loop_base_depth,
          defer_switch_base_depth, inside_defer_body, summary);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->do_while_stmt->body, loop_depth + 1, switch_depth,
          defer_loop_base_depth, defer_switch_base_depth, inside_defer_body,
          summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->for_stmt->body, loop_depth + 1, switch_depth,
          defer_loop_base_depth, defer_switch_base_depth, inside_defer_body,
          summary);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt->match_surface_enabled) {
        ++summary.match_statement_semantic_sites;
        const MatchExhaustivenessInfo exhaustiveness =
            ClassifyMatchExhaustiveness(*switch_stmt);
        if (exhaustiveness.exhaustive) {
          ++summary.match_exhaustive_statement_sites;
        } else {
          ++summary.match_non_exhaustive_diagnostic_sites;
        }
        if (exhaustiveness.bool_exhaustive) {
          ++summary.match_bool_exhaustive_sites;
        }
        if (exhaustiveness.result_case_exhaustive) {
          ++summary.match_result_case_exhaustive_sites;
        }
        for (const auto &case_stmt : switch_stmt->cases) {
          if (case_stmt.is_default) {
            ++summary.match_default_pattern_sites;
          } else {
            switch (case_stmt.match_pattern_kind) {
            case MatchPatternKind::Wildcard:
              ++summary.match_wildcard_pattern_sites;
              break;
            case MatchPatternKind::LiteralInteger:
            case MatchPatternKind::LiteralBool:
            case MatchPatternKind::LiteralNil:
              ++summary.match_literal_pattern_sites;
              break;
            case MatchPatternKind::Binding:
              if (!case_stmt.match_binding_name.empty()) {
                ++summary.match_binding_scope_sites;
              }
              break;
            case MatchPatternKind::ResultCase:
              if (!case_stmt.match_binding_name.empty()) {
                ++summary.match_result_case_scope_sites;
              }
              break;
            case MatchPatternKind::None:
              break;
            }
          }
          CollectControlFlowControlFlowSemanticStmtSitesFromList(
              case_stmt.body, loop_depth, switch_depth + 1,
              defer_loop_base_depth, defer_switch_base_depth,
              inside_defer_body, summary);
        }
        return;
      }
      for (const auto &case_stmt : switch_stmt->cases) {
        CollectControlFlowControlFlowSemanticStmtSitesFromList(
            case_stmt.body, loop_depth, switch_depth + 1,
            defer_loop_base_depth, defer_switch_base_depth,
            inside_defer_body, summary);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->while_stmt->body, loop_depth + 1, switch_depth,
          defer_loop_base_depth, defer_switch_base_depth, inside_defer_body,
          summary);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->block_stmt->body, loop_depth, switch_depth,
          defer_loop_base_depth, defer_switch_base_depth, inside_defer_body,
          summary);
    }
    return;
  case Stmt::Kind::Defer:
    ++summary.defer_statement_semantic_sites;
    ++summary.defer_scope_cleanup_order_sites;
    if (stmt->block_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->block_stmt->body, loop_depth, switch_depth, loop_depth,
          switch_depth, true, summary);
    }
    return;
  case Stmt::Kind::Break:
    ++summary.break_statement_sites;
    if (inside_defer_body && loop_depth <= defer_loop_base_depth &&
        switch_depth <= defer_switch_base_depth) {
      ++summary.defer_nonlocal_exit_diagnostic_sites;
    } else if (loop_depth <= 0 && switch_depth <= 0) {
      ++summary.break_restriction_diagnostic_sites;
    }
    return;
  case Stmt::Kind::Continue:
    ++summary.continue_statement_sites;
    if (inside_defer_body && loop_depth <= defer_loop_base_depth) {
      ++summary.defer_nonlocal_exit_diagnostic_sites;
    } else if (loop_depth <= 0) {
      ++summary.continue_restriction_diagnostic_sites;
    }
    return;
  case Stmt::Kind::Let:
  case Stmt::Kind::Assign:
  case Stmt::Kind::Expr:
  case Stmt::Kind::Empty:
    return;
  case Stmt::Kind::Return:
    if (inside_defer_body) {
      ++summary.defer_nonlocal_exit_diagnostic_sites;
    }
    return;
  }
}

static void CollectControlFlowControlFlowSemanticStmtSitesFromList(
    const std::vector<std::unique_ptr<Stmt>> &statements, int loop_depth,
    int switch_depth, int defer_loop_base_depth, int defer_switch_base_depth,
    bool inside_defer_body,
    Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  for (const auto &stmt : statements) {
    CollectControlFlowControlFlowSemanticStmtSites(stmt.get(), loop_depth,
                                             switch_depth,
                                             defer_loop_base_depth,
                                             defer_switch_base_depth,
                                             inside_defer_body, summary);
  }
}

Objc3ControlFlowControlFlowSemanticModelSummary
BuildControlFlowControlFlowSemanticModelSummary(const Objc3Program &ast) {
  Objc3ControlFlowControlFlowSemanticModelSummary summary;
  for (const auto &function : ast.functions) {
    CollectControlFlowControlFlowSemanticStmtSitesFromList(function.body, 0, 0, 0, 0,
                                                     false, summary);
  }
  for (const auto &implementation : ast.implementations) {
    for (const auto &method : implementation.methods) {
      CollectControlFlowControlFlowSemanticStmtSitesFromList(method.body, 0, 0, 0, 0,
                                                       false, summary);
    }
  }

  summary.source_dependency_required = true;
  summary.guard_refinement_semantics_landed =
      summary.guard_refinement_sites ==
      summary.guard_binding_clause_semantic_sites;
  summary.guard_exit_enforcement_landed =
      summary.guard_exit_enforcement_sites ==
      summary.guard_binding_semantic_sites +
          summary.guard_condition_statement_sites;
  summary.match_binding_scope_semantics_landed =
      summary.match_binding_scope_sites <=
      summary.match_statement_semantic_sites;
  summary.match_result_case_scope_semantics_landed =
      summary.match_result_case_scope_sites <=
      summary.match_statement_semantic_sites;
  summary.match_exhaustiveness_semantics_landed =
      summary.match_statement_semantic_sites ==
      summary.match_exhaustive_statement_sites +
          summary.match_non_exhaustive_diagnostic_sites;
  summary.match_exhaustiveness_deferred =
      summary.match_exhaustiveness_deferred_sites != 0;
  summary.defer_cleanup_order_semantics_landed = true;
  summary.defer_nonlocal_exit_semantics_landed = true;
  summary.defer_cleanup_order_deferred = false;
  summary.defer_nonlocal_exit_deferred = false;
  summary.non_local_exit_restrictions_landed =
      summary.break_restriction_diagnostic_sites <=
          summary.break_statement_sites &&
      summary.continue_restriction_diagnostic_sites <=
          summary.continue_statement_sites;
  summary.deterministic =
      summary.guard_binding_semantic_sites <=
          summary.guard_binding_clause_semantic_sites &&
      summary.guard_condition_statement_sites <=
          summary.guard_exit_enforcement_sites &&
      summary.guard_condition_clause_semantic_sites >=
          summary.guard_condition_statement_sites &&
      summary.match_default_pattern_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_wildcard_pattern_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_literal_pattern_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_binding_scope_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_result_case_scope_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_exhaustive_statement_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_non_exhaustive_diagnostic_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_exhaustiveness_deferred_sites == 0 &&
      summary.match_exhaustiveness_semantics_landed &&
      !summary.match_exhaustiveness_deferred &&
      summary.defer_scope_cleanup_order_sites <=
          summary.defer_statement_semantic_sites &&
      !summary.defer_cleanup_order_deferred &&
      !summary.defer_nonlocal_exit_deferred &&
      summary.defer_cleanup_order_semantics_landed &&
      summary.defer_nonlocal_exit_semantics_landed &&
      summary.non_local_exit_restrictions_landed &&
      summary.guard_refinement_semantics_landed &&
      summary.guard_exit_enforcement_landed &&
      summary.match_binding_scope_semantics_landed &&
      summary.match_result_case_scope_semantics_landed;
  summary.ready_for_lowering_and_runtime =
      summary.source_dependency_required &&
      summary.defer_cleanup_order_semantics_landed &&
      summary.defer_nonlocal_exit_semantics_landed &&
      !summary.defer_cleanup_order_deferred &&
      !summary.defer_nonlocal_exit_deferred &&
      summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";source-dependency=" << summary.frontend_dependency_contract_id
      << ";guard-sites=" << summary.guard_binding_semantic_sites
      << ";guard-clauses=" << summary.guard_binding_clause_semantic_sites
      << ";guard-condition-statements="
      << summary.guard_condition_statement_sites
      << ";guard-condition-clauses="
      << summary.guard_condition_clause_semantic_sites
      << ";guard-exit=" << summary.guard_exit_enforcement_sites
      << ";guard-refinement=" << summary.guard_refinement_sites
      << ";match-sites=" << summary.match_statement_semantic_sites
      << ";match-defaults=" << summary.match_default_pattern_sites
      << ";match-wildcards=" << summary.match_wildcard_pattern_sites
      << ";match-literals=" << summary.match_literal_pattern_sites
      << ";match-bindings=" << summary.match_binding_scope_sites
      << ";match-result-bindings="
      << summary.match_result_case_scope_sites
      << ";match-exhaustive=" << summary.match_exhaustive_statement_sites
      << ";match-bool-exhaustive=" << summary.match_bool_exhaustive_sites
      << ";match-result-exhaustive="
      << summary.match_result_case_exhaustive_sites
      << ";match-nonexhaustive-diagnostics="
      << summary.match_non_exhaustive_diagnostic_sites
      << ";match-exhaustiveness-deferred="
      << summary.match_exhaustiveness_deferred_sites
      << ";defer-sites=" << summary.defer_statement_semantic_sites
      << ";defer-cleanup-order-sites="
      << summary.defer_scope_cleanup_order_sites
      << ";defer-nonlocal-exit-diagnostics="
      << summary.defer_nonlocal_exit_diagnostic_sites
      << ";break-sites=" << summary.break_statement_sites
      << ";continue-sites=" << summary.continue_statement_sites
      << ";break-diagnostics=" << summary.break_restriction_diagnostic_sites
      << ";continue-diagnostics="
      << summary.continue_restriction_diagnostic_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

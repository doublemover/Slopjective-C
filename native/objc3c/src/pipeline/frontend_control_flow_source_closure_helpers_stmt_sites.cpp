#include "pipeline/frontend_control_flow_source_closure_helpers.h"

namespace objc3c::pipeline::orchestration::detail {

void CollectControlFlowControlFlowSourceClosureStmtSites(
    const Stmt *stmt,
    Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    break;
  case Stmt::Kind::Assign:
  case Stmt::Kind::Return:
  case Stmt::Kind::Expr:
  case Stmt::Kind::CollectionMutation:
    break;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      if (stmt->if_stmt->guard_binding_surface_enabled) {
        ++summary.guard_binding_sites;
        summary.guard_binding_clause_sites +=
            stmt->if_stmt->optional_binding_clause_count;
      }
      if (stmt->if_stmt->guard_condition_list_surface_enabled) {
        summary.guard_boolean_condition_sites +=
            stmt->if_stmt->guard_boolean_condition_clause_count;
      }
      for (const auto &child : stmt->if_stmt->then_body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
      for (const auto &child : stmt->if_stmt->else_body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &child : stmt->do_while_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      for (const auto &child : stmt->for_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      for (const auto &child : stmt->for_in_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      if (stmt->switch_stmt->match_surface_enabled) {
        ++summary.match_statement_sites;
      } else {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          if (case_stmt.is_default) {
            ++summary.switch_default_pattern_sites;
          } else {
            ++summary.switch_case_pattern_sites;
          }
          for (const auto &child : case_stmt.body) {
            CollectControlFlowControlFlowSourceClosureStmtSites(
                child.get(), summary);
          }
        }
        break;
      }
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        if (case_stmt.is_default) {
          ++summary.match_default_sites;
        } else {
          ++summary.match_case_pattern_sites;
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
            ++summary.match_binding_pattern_sites;
            break;
          case MatchPatternKind::ResultCase:
            ++summary.match_result_case_pattern_sites;
            break;
          case MatchPatternKind::None:
            break;
          }
        }
        for (const auto &child : case_stmt.body) {
          CollectControlFlowControlFlowSourceClosureStmtSites(
              child.get(), summary);
        }
      }
    }
    break;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      for (const auto &child : stmt->while_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &child : stmt->block_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    break;
  }
}

}  // namespace objc3c::pipeline::orchestration::detail

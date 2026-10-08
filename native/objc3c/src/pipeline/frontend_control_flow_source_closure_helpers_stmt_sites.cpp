#include "pipeline/frontend_control_flow_source_closure_helpers.h"

namespace objc3c::pipeline::orchestration::detail {

static void CollectControlFlowControlFlowSourceClosureExprSites(
    const Expr *expr,
    Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary);

static void CollectControlFlowControlFlowSourceClosureExprListSites(
    const std::vector<std::unique_ptr<Expr>> &expressions,
    Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  for (const auto &expr : expressions) {
    CollectControlFlowControlFlowSourceClosureExprSites(expr.get(), summary);
  }
}

static void CollectControlFlowControlFlowSourceClosureExprSites(
    const Expr *expr,
    Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Binary:
    CollectControlFlowControlFlowSourceClosureExprSites(expr->left.get(),
                                                        summary);
    CollectControlFlowControlFlowSourceClosureExprSites(expr->right.get(),
                                                        summary);
    return;
  case Expr::Kind::Conditional:
    CollectControlFlowControlFlowSourceClosureExprSites(expr->left.get(),
                                                        summary);
    CollectControlFlowControlFlowSourceClosureExprSites(expr->right.get(),
                                                        summary);
    CollectControlFlowControlFlowSourceClosureExprSites(expr->third.get(),
                                                        summary);
    return;
  case Expr::Kind::CollectionLiteral:
    CollectControlFlowControlFlowSourceClosureExprListSites(
        expr->collection_keys, summary);
    CollectControlFlowControlFlowSourceClosureExprListSites(
        expr->collection_values, summary);
    return;
  case Expr::Kind::IndexAccess:
    CollectControlFlowControlFlowSourceClosureExprSites(expr->left.get(),
                                                        summary);
    CollectControlFlowControlFlowSourceClosureExprSites(expr->right.get(),
                                                        summary);
    return;
  case Expr::Kind::Call:
  case Expr::Kind::Try:
  case Expr::Kind::Throw:
  case Expr::Kind::StringInterpolation:
    CollectControlFlowControlFlowSourceClosureExprListSites(expr->args,
                                                            summary);
    return;
  case Expr::Kind::MessageSend:
    CollectControlFlowControlFlowSourceClosureExprSites(expr->receiver.get(),
                                                        summary);
    CollectControlFlowControlFlowSourceClosureExprListSites(expr->args,
                                                            summary);
    return;
  case Expr::Kind::BlockLiteral:
    for (const auto &child : expr->block_body) {
      CollectControlFlowControlFlowSourceClosureStmtSites(child.get(),
                                                          summary);
    }
    return;
  case Expr::Kind::MatchExpression:
    ++summary.match_expression_sites;
    summary.match_expression_arm_sites += expr->match_expression_arms.size();
    CollectControlFlowControlFlowSourceClosureExprSites(
        expr->match_expression_scrutinee.get(), summary);
    for (const auto &arm : expr->match_expression_arms) {
      if (arm.has_guard) {
        ++summary.match_expression_guard_sites;
      }
      CollectControlFlowControlFlowSourceClosureExprSites(
          arm.guard_condition.get(), summary);
      CollectControlFlowControlFlowSourceClosureExprSites(arm.value.get(),
                                                          summary);
    }
    return;
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::StringLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::KeyPathLiteral:
    return;
  }
}

void CollectControlFlowControlFlowSourceClosureStmtSites(
    const Stmt *stmt,
    Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->let_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->assign_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->return_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->expr_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->collection_mutation_stmt->key_or_index.get(), summary);
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->collection_mutation_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->if_stmt->condition.get(), summary);
      CollectControlFlowControlFlowSourceClosureExprListSites(
          stmt->if_stmt->guard_condition_exprs, summary);
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
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->do_while_stmt->condition.get(), summary);
      for (const auto &child : stmt->do_while_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->for_stmt->init.value.get(), summary);
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->for_stmt->step.value.get(), summary);
      for (const auto &child : stmt->for_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->for_in_stmt->collection.get(), summary);
      for (const auto &child : stmt->for_in_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(
            child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->switch_stmt->condition.get(), summary);
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
          if (case_stmt.has_match_guard) {
            ++summary.guarded_match_pattern_sites;
            CollectControlFlowControlFlowSourceClosureExprSites(
                case_stmt.match_guard_condition.get(), summary);
          }
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
      CollectControlFlowControlFlowSourceClosureExprSites(
          stmt->while_stmt->condition.get(), summary);
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

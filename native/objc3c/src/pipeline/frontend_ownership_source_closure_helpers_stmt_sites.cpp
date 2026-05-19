#include "pipeline/frontend_ownership_source_closure_helpers.h"

namespace objc3c::pipeline::orchestration::detail {

void CollectOwnershipSystemExtensionStmtSites(
    const Stmt *stmt,
    Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      if (stmt->let_stmt->resource_attribute_declared) {
        ++summary.resource_attribute_sites;
        if (!stmt->let_stmt->resource_close_symbol.empty()) {
          ++summary.resource_close_clause_sites;
        }
        if (!stmt->let_stmt->resource_invalid_expression.empty()) {
          ++summary.resource_invalid_clause_sites;
        }
      }
      CollectOwnershipSystemExtensionExprSites(
          stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(
          stmt->assign_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(
          stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(
          stmt->if_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->if_stmt->then_body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
      for (const auto &body_stmt : stmt->if_stmt->else_body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
      CollectOwnershipSystemExtensionExprSites(
          stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(
          stmt->for_stmt->init.value.get(), summary);
      CollectOwnershipSystemExtensionExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectOwnershipSystemExtensionExprSites(
          stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &body_stmt : switch_case.body) {
          CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(
          stmt->expr_stmt->value.get(), summary);
    }
    return;
  default:
    return;
  }
}

}  // namespace objc3c::pipeline::orchestration::detail

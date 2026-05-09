#include "pipeline/frontend_ownership_source_completion_helpers.h"

namespace objc3c::pipeline::orchestration {
namespace detail {

void CollectOwnershipCleanupResourceCaptureStmtSites(
    const Stmt *stmt,
    Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary &summary);

void CollectOwnershipCleanupResourceCaptureExprSites(
    const Expr *expr,
    Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::BlockLiteral:
    if (expr->block_has_explicit_capture_list) {
      ++summary.explicit_capture_list_sites;
      summary.explicit_capture_item_sites += expr->block_explicit_capture_count;
      summary.explicit_capture_weak_sites +=
          expr->block_explicit_capture_weak_count;
      summary.explicit_capture_unowned_sites +=
          expr->block_explicit_capture_unowned_count;
      summary.explicit_capture_move_sites +=
          expr->block_explicit_capture_move_count;
      summary.explicit_capture_plain_sites +=
          expr->block_explicit_capture_plain_count;
    }
    for (const auto &stmt : expr->block_body) {
      if (stmt != nullptr) {
        CollectOwnershipCleanupResourceCaptureStmtSites(stmt.get(), summary);
      }
    }
    return;
  case Expr::Kind::Call:
  case Expr::Kind::MessageSend:
    CollectOwnershipCleanupResourceCaptureExprSites(
        expr->receiver.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->left.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->right.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->third.get(), summary);
    for (const auto &arg : expr->args) {
      CollectOwnershipCleanupResourceCaptureExprSites(arg.get(), summary);
    }
    return;
  case Expr::Kind::Binary:
  case Expr::Kind::Conditional:
    CollectOwnershipCleanupResourceCaptureExprSites(expr->left.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->right.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->third.get(), summary);
    return;
  default:
    return;
  }
}

void CollectOwnershipCleanupResourceCaptureStmtSites(
    const Stmt *stmt,
    Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      if (stmt->let_stmt->cleanup_attribute_declared) {
        ++summary.cleanup_attribute_sites;
        if (stmt->let_stmt->cleanup_sugar_declared) {
          ++summary.cleanup_sugar_sites;
        }
      }
      if (stmt->let_stmt->resource_attribute_declared) {
        ++summary.resource_attribute_sites;
        if (stmt->let_stmt->resource_sugar_declared) {
          ++summary.resource_sugar_sites;
        }
        if (!stmt->let_stmt->resource_close_symbol.empty()) {
          ++summary.resource_close_clause_sites;
        }
        if (!stmt->let_stmt->resource_invalid_expression.empty()) {
          ++summary.resource_invalid_clause_sites;
        }
      }
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->assign_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->if_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->if_stmt->then_body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(
            body_stmt.get(), summary);
      }
      for (const auto &body_stmt : stmt->if_stmt->else_body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(
            body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(
            body_stmt.get(), summary);
      }
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->for_stmt->init.value.get(), summary);
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(
            body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &body_stmt : switch_case.body) {
          CollectOwnershipCleanupResourceCaptureStmtSites(
              body_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(
            body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(
            body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->expr_stmt->value.get(), summary);
    }
    return;
  default:
    return;
  }
}

}  // namespace detail

Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
BuildOwnershipCleanupResourceCaptureSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary summary;

  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      detail::CollectOwnershipCleanupResourceCaptureStmtSites(
          stmt.get(), summary);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        detail::CollectOwnershipCleanupResourceCaptureStmtSites(
            stmt.get(), summary);
      }
    }
  }

  summary.cleanup_attribute_source_supported = true;
  summary.resource_sugar_source_supported = true;
  summary.explicit_capture_list_source_supported = true;
  summary.deterministic_handoff =
      summary.cleanup_sugar_sites <= summary.cleanup_attribute_sites &&
      summary.resource_sugar_sites <= summary.resource_attribute_sites &&
      summary.resource_close_clause_sites <= summary.resource_attribute_sites &&
      summary.resource_invalid_clause_sites <=
          summary.resource_attribute_sites &&
      summary.explicit_capture_weak_sites +
              summary.explicit_capture_unowned_sites +
              summary.explicit_capture_move_sites +
              summary.explicit_capture_plain_sites <=
          summary.explicit_capture_item_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildOwnershipCleanupResourceCaptureSourceCompletionReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration

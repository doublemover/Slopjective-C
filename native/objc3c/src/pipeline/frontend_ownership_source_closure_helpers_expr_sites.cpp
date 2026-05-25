#include "pipeline/frontend_ownership_source_closure_helpers.h"

namespace objc3c::pipeline::orchestration::detail {

void CollectOwnershipSystemExtensionExprSites(
    const Expr *expr,
    Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
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
        CollectOwnershipSystemExtensionStmtSites(stmt.get(), summary);
      }
    }
    return;
  case Expr::Kind::Call:
  case Expr::Kind::MessageSend:
    CollectOwnershipSystemExtensionExprSites(expr->receiver.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->left.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->right.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->third.get(), summary);
    for (const auto &arg : expr->args) {
      CollectOwnershipSystemExtensionExprSites(arg.get(), summary);
    }
    for (const auto &key : expr->collection_keys) {
      CollectOwnershipSystemExtensionExprSites(key.get(), summary);
    }
    for (const auto &value : expr->collection_values) {
      CollectOwnershipSystemExtensionExprSites(value.get(), summary);
    }
    return;
  case Expr::Kind::CollectionLiteral:
    for (const auto &key : expr->collection_keys) {
      CollectOwnershipSystemExtensionExprSites(key.get(), summary);
    }
    for (const auto &value : expr->collection_values) {
      CollectOwnershipSystemExtensionExprSites(value.get(), summary);
    }
    return;
  case Expr::Kind::IndexAccess:
    CollectOwnershipSystemExtensionExprSites(expr->left.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->right.get(), summary);
    return;
  case Expr::Kind::Binary:
  case Expr::Kind::Conditional:
    CollectOwnershipSystemExtensionExprSites(expr->left.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->right.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->third.get(), summary);
    return;
  case Expr::Kind::MatchExpression:
    CollectOwnershipSystemExtensionExprSites(
        expr->match_expression_scrutinee.get(), summary);
    for (const auto &arm : expr->match_expression_arms) {
      CollectOwnershipSystemExtensionExprSites(
          arm.guard_condition.get(), summary);
      CollectOwnershipSystemExtensionExprSites(arm.value.get(), summary);
    }
    return;
  default:
    return;
  }
}

}  // namespace objc3c::pipeline::orchestration::detail

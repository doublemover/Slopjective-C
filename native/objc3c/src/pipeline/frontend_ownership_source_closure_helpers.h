#pragma once

#include <vector>

#include "ast/objc3_ast_declarations.h"
#include "pipeline/frontend_source_closure_replay_keys.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {
namespace detail {

inline void CollectOwnershipSystemExtensionStmtSites(
    const Stmt *stmt,
    Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary);

inline void CollectOwnershipSystemExtensionExprSites(
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
    return;
  case Expr::Kind::Binary:
  case Expr::Kind::Conditional:
    CollectOwnershipSystemExtensionExprSites(expr->left.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->right.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->third.get(), summary);
    return;
  default:
    return;
  }
}

inline void CollectOwnershipSystemExtensionStmtSites(
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

}  // namespace detail

inline Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
BuildOwnershipSystemExtensionSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendOwnershipSystemExtensionSourceClosureSummary summary;

  auto count_borrowed_in_params =
      [&summary](const std::vector<FuncParam> &params) {
        for (const auto &param : params) {
          if (param.borrowed_pointer_qualified) {
            ++summary.borrowed_pointer_sites;
          }
        }
      };

  for (const auto &fn : program.functions) {
    count_borrowed_in_params(fn.params);
    if (fn.return_borrowed_pointer_qualified) {
      ++summary.borrowed_pointer_sites;
    }
    if (fn.objc_returns_borrowed_declared) {
      ++summary.returns_borrowed_attribute_sites;
    }
    for (const auto &stmt : fn.body) {
      detail::CollectOwnershipSystemExtensionStmtSites(stmt.get(), summary);
    }
  }

  auto count_method =
      [&summary, &count_borrowed_in_params](const Objc3MethodDecl &method) {
        count_borrowed_in_params(method.params);
        if (method.return_borrowed_pointer_qualified) {
          ++summary.borrowed_pointer_sites;
        }
        if (method.objc_returns_borrowed_declared) {
          ++summary.returns_borrowed_attribute_sites;
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      count_method(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      count_method(method);
      for (const auto &stmt : method.body) {
        detail::CollectOwnershipSystemExtensionStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.resource_attribute_source_supported = true;
  summary.borrowed_pointer_source_supported = true;
  summary.returns_borrowed_source_supported = true;
  summary.explicit_capture_list_source_supported = true;
  summary.deterministic_handoff =
      summary.resource_close_clause_sites <= summary.resource_attribute_sites &&
      summary.resource_invalid_clause_sites <= summary.resource_attribute_sites &&
      summary.explicit_capture_weak_sites +
              summary.explicit_capture_unowned_sites +
              summary.explicit_capture_move_sites +
              summary.explicit_capture_plain_sites <=
          summary.explicit_capture_item_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildOwnershipSystemExtensionSourceClosureReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration

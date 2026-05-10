#include "pipeline/frontend_concurrency_source_closure_helpers.h"

#include <string>

#include "pipeline/frontend_source_closure_replay_keys.h"
#include "support/objc3_concurrency_symbol_profiles.h"

namespace objc3c::pipeline::orchestration {
namespace {

void CollectConcurrencyTaskGroupCancellationExprSites(
    const Expr *expr,
    Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  const auto collect_symbol = [&summary](const std::string &symbol) {
    if (objc3c::support::IsConcurrencyTaskCreationSymbol(symbol)) {
      ++summary.task_creation_sites;
    }
    if (objc3c::support::IsConcurrencyTaskGroupScopeSymbol(symbol)) {
      ++summary.task_group_scope_sites;
    }
    if (objc3c::support::IsConcurrencyTaskGroupAddTaskSymbol(symbol)) {
      ++summary.task_group_add_task_sites;
    }
    if (objc3c::support::IsConcurrencyTaskGroupWaitNextSymbol(symbol)) {
      ++summary.task_group_wait_next_sites;
    }
    if (objc3c::support::IsConcurrencyTaskGroupCancelAllSymbol(symbol)) {
      ++summary.task_group_cancel_all_sites;
    }
    if (objc3c::support::IsConcurrencyCancellationCheckSymbol(symbol)) {
      ++summary.cancellation_check_sites;
    }
    if (objc3c::support::IsConcurrencyCancellationHandlerSymbol(symbol)) {
      ++summary.cancellation_handler_sites;
    }
  };

  switch (expr->kind) {
  case Expr::Kind::Call:
    collect_symbol(expr->ident);
    break;
  case Expr::Kind::MessageSend:
    collect_symbol(expr->selector);
    break;
  default:
    break;
  }

  CollectConcurrencyTaskGroupCancellationExprSites(expr->receiver.get(),
                                                   summary);
  CollectConcurrencyTaskGroupCancellationExprSites(expr->left.get(), summary);
  CollectConcurrencyTaskGroupCancellationExprSites(expr->right.get(), summary);
  CollectConcurrencyTaskGroupCancellationExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectConcurrencyTaskGroupCancellationExprSites(arg.get(), summary);
  }
}

void CollectConcurrencyTaskGroupCancellationStmtSites(
    const Stmt *stmt,
    Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->if_stmt->condition.get(), summary);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(then_stmt.get(),
                                                         summary);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(else_stmt.get(),
                                                         summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(),
                                                         summary);
      }
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->for_stmt->init.value.get(), summary);
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(),
                                                         summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectConcurrencyTaskGroupCancellationStmtSites(case_stmt.get(),
                                                           summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(),
                                                         summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(),
                                                         summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->expr_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

}  // namespace

Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
BuildConcurrencyTaskGroupCancellationSourceClosureSummary(
    const Objc3Program &program) {
  Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary summary;

  for (const auto &fn : program.functions) {
    if (fn.async_declared) {
      ++summary.async_callable_sites;
    }
    if (fn.executor_affinity_declared) {
      ++summary.executor_attribute_sites;
    }
    for (const auto &stmt : fn.body) {
      CollectConcurrencyTaskGroupCancellationStmtSites(stmt.get(), summary);
    }
  }

  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (method.async_declared) {
        ++summary.async_callable_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.executor_attribute_sites;
      }
      for (const auto &stmt : method.body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.task_creation_source_supported = true;
  summary.task_group_source_supported = true;
  summary.cancellation_source_supported = true;
  summary.deterministic_handoff =
      summary.task_group_add_task_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.task_group_wait_next_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.task_group_cancel_all_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.cancellation_handler_sites <= summary.cancellation_check_sites + 1u;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildConcurrencyTaskGroupCancellationSourceClosureReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration

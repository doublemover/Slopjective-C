#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_declarations.h"
#include "lex/objc3_lexer.h"
#include "pipeline/frontend_source_closure_replay_keys.h"
#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"
#include "sema/model/semantic_symbol.h"
#include "support/objc3_concurrency_symbol_profiles.h"

namespace objc3c::pipeline::orchestration {
namespace detail {

inline void CollectConcurrencyAsyncSourceClosureExprSites(
    const Expr *expr,
    Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  if (expr->await_expression_enabled) {
    ++summary.await_expression_sites;
  }
  CollectConcurrencyAsyncSourceClosureExprSites(expr->receiver.get(), summary);
  CollectConcurrencyAsyncSourceClosureExprSites(expr->left.get(), summary);
  CollectConcurrencyAsyncSourceClosureExprSites(expr->right.get(), summary);
  CollectConcurrencyAsyncSourceClosureExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectConcurrencyAsyncSourceClosureExprSites(arg.get(), summary);
  }
}

inline void CollectConcurrencyAsyncSourceClosureStmtSites(
    const Stmt *stmt,
    Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->if_stmt->condition.get(), summary);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(then_stmt.get(), summary);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(else_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->for_stmt->init.value.get(), summary);
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectConcurrencyAsyncSourceClosureStmtSites(case_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(
          stmt->expr_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

inline void CollectConcurrencyTaskGroupCancellationExprSites(
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

  CollectConcurrencyTaskGroupCancellationExprSites(expr->receiver.get(), summary);
  CollectConcurrencyTaskGroupCancellationExprSites(expr->left.get(), summary);
  CollectConcurrencyTaskGroupCancellationExprSites(expr->right.get(), summary);
  CollectConcurrencyTaskGroupCancellationExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectConcurrencyTaskGroupCancellationExprSites(arg.get(), summary);
  }
}

inline void CollectConcurrencyTaskGroupCancellationStmtSites(
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
        CollectConcurrencyTaskGroupCancellationStmtSites(then_stmt.get(), summary);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(else_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(), summary);
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
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectConcurrencyTaskGroupCancellationStmtSites(case_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(), summary);
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

}  // namespace detail

inline Objc3FrontendConcurrencyAsyncSourceClosureSummary
BuildConcurrencyAsyncSourceClosureSummary(
    const Objc3Program &program,
    const std::vector<Objc3LexToken> &tokens) {
  Objc3FrontendConcurrencyAsyncSourceClosureSummary summary;
  bool async_profiles_normalized = true;
  bool await_profiles_normalized = true;

  for (const auto &token : tokens) {
    if (token.kind == Objc3LexTokenKind::KwAsync) {
      ++summary.async_keyword_sites;
    } else if (token.kind == Objc3LexTokenKind::KwAwait) {
      ++summary.await_keyword_sites;
    }
  }

  for (const auto &fn : program.functions) {
    if (fn.async_declared) {
      ++summary.async_function_sites;
    }
    if (fn.executor_affinity_declared) {
      ++summary.executor_attribute_sites;
      if (fn.executor_affinity_kind == "main") {
        ++summary.executor_main_sites;
      } else if (fn.executor_affinity_kind == "global") {
        ++summary.executor_global_sites;
      } else if (fn.executor_affinity_named) {
        ++summary.executor_named_sites;
      }
    }
    async_profiles_normalized =
        async_profiles_normalized && fn.async_continuation_profile_is_normalized;
    await_profiles_normalized =
        await_profiles_normalized && fn.await_suspension_profile_is_normalized;
    for (const auto &stmt : fn.body) {
      detail::CollectConcurrencyAsyncSourceClosureStmtSites(
          stmt.get(), summary);
    }
  }

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      if (method.async_declared) {
        ++summary.async_method_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.executor_attribute_sites;
        if (method.executor_affinity_kind == "main") {
          ++summary.executor_main_sites;
        } else if (method.executor_affinity_kind == "global") {
          ++summary.executor_global_sites;
        } else if (method.executor_affinity_named) {
          ++summary.executor_named_sites;
        }
      }
      async_profiles_normalized =
          async_profiles_normalized &&
          method.async_continuation_profile_is_normalized;
      await_profiles_normalized =
          await_profiles_normalized &&
          method.await_suspension_profile_is_normalized;
    }
  }

  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (method.async_declared) {
        ++summary.async_method_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.executor_attribute_sites;
        if (method.executor_affinity_kind == "main") {
          ++summary.executor_main_sites;
        } else if (method.executor_affinity_kind == "global") {
          ++summary.executor_global_sites;
        } else if (method.executor_affinity_named) {
          ++summary.executor_named_sites;
        }
      }
      async_profiles_normalized =
          async_profiles_normalized &&
          method.async_continuation_profile_is_normalized;
      await_profiles_normalized =
          await_profiles_normalized &&
          method.await_suspension_profile_is_normalized;
      for (const auto &stmt : method.body) {
        detail::CollectConcurrencyAsyncSourceClosureStmtSites(
            stmt.get(), summary);
      }
    }
  }

  summary.async_function_source_supported = true;
  summary.async_method_source_supported = true;
  summary.await_expression_source_supported = true;
  summary.executor_attribute_source_supported = true;
  summary.deterministic_handoff =
      async_profiles_normalized && await_profiles_normalized &&
      summary.async_function_sites <= summary.async_keyword_sites &&
      summary.await_expression_sites <= summary.await_keyword_sites &&
      summary.executor_main_sites + summary.executor_global_sites +
              summary.executor_named_sites <=
          summary.executor_attribute_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildConcurrencyAsyncSourceClosureReplayKey(summary);
  return summary;
}

inline Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
BuildConcurrencyActorMemberIsolationSourceClosureSummary(
    const Objc3Program &program) {
  Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary summary;

  for (const auto &interface_decl : program.interfaces) {
    if (!interface_decl.is_actor) {
      continue;
    }
    ++summary.actor_interface_sites;
    summary.actor_property_sites += interface_decl.properties.size();
    summary.actor_method_sites += interface_decl.methods.size();
    summary.actor_member_metadata_sites +=
        interface_decl.properties.size() + interface_decl.methods.size();
    for (const auto &method : interface_decl.methods) {
      if (method.objc_nonisolated_declared) {
        ++summary.objc_nonisolated_annotation_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.actor_member_executor_annotation_sites;
      }
      if (method.async_declared) {
        ++summary.actor_async_method_sites;
      }
    }
  }

  summary.actor_declaration_source_supported = true;
  summary.actor_member_source_supported = true;
  summary.isolation_annotation_source_supported = true;
  summary.actor_metadata_surface_supported = true;
  summary.deterministic_handoff =
      summary.objc_nonisolated_annotation_sites <= summary.actor_method_sites &&
      summary.actor_member_executor_annotation_sites <= summary.actor_method_sites &&
      summary.actor_async_method_sites <= summary.actor_method_sites &&
      summary.actor_member_metadata_sites ==
          summary.actor_method_sites + summary.actor_property_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildConcurrencyActorMemberIsolationSourceClosureReplayKey(summary);
  return summary;
}

inline Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
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
      detail::CollectConcurrencyTaskGroupCancellationStmtSites(
          stmt.get(), summary);
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
        detail::CollectConcurrencyTaskGroupCancellationStmtSites(
            stmt.get(), summary);
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

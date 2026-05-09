#include "parse/objc3_parser_task_runtime_cancellation_profiles.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"

namespace objc3c::parse {
namespace {

bool IsTaskRuntimeHookSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("task_runtime") != std::string::npos ||
         lowered.find("task_group") != std::string::npos ||
         lowered.find("group_task") != std::string::npos ||
         lowered.find("task_spawn") != std::string::npos ||
         lowered.find("spawn_task") != std::string::npos ||
         lowered.find("detached_task") != std::string::npos ||
         lowered.find("task_detach") != std::string::npos ||
         lowered.find("runtime_task") != std::string::npos ||
         lowered.find("executor") != std::string::npos;
}

bool IsCancellationCheckSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cancel") != std::string::npos ||
         lowered.find("is_cancelled") != std::string::npos ||
         lowered.find("cancellation") != std::string::npos;
}

bool IsCancellationHandlerSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cancel_handler") != std::string::npos ||
         lowered.find("with_cancellation_handler") != std::string::npos ||
         lowered.find("on_cancel") != std::string::npos;
}

bool IsSuspensionPointSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("await") != std::string::npos ||
         lowered.find("wait_next") != std::string::npos ||
         lowered.find("suspend") != std::string::npos ||
         lowered.find("yield") != std::string::npos;
}

struct Objc3TaskRuntimeCancellationSiteCounts {
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
};

void CollectTaskRuntimeCancellationSitesFromSymbol(
    const std::string &symbol,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (IsTaskRuntimeHookSymbol(symbol)) {
    counts.runtime_hook_sites += 1u;
  }
  if (IsCancellationCheckSymbol(symbol)) {
    counts.cancellation_check_sites += 1u;
  }
  if (IsCancellationHandlerSymbol(symbol)) {
    counts.cancellation_handler_sites += 1u;
  }
  if (IsSuspensionPointSymbol(symbol)) {
    counts.suspension_point_sites += 1u;
  }
}

void CollectTaskRuntimeCancellationExprSites(
    const Expr *expr,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectTaskRuntimeCancellationSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectTaskRuntimeCancellationExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectTaskRuntimeCancellationSitesFromSymbol(expr->selector, counts);
    CollectTaskRuntimeCancellationExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectTaskRuntimeCancellationExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectTaskRuntimeCancellationExprSites(expr->left.get(), counts);
    CollectTaskRuntimeCancellationExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectTaskRuntimeCancellationExprSites(expr->left.get(), counts);
    CollectTaskRuntimeCancellationExprSites(expr->right.get(), counts);
    CollectTaskRuntimeCancellationExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

void CollectTaskRuntimeCancellationForClauseSites(
    const ForClause &clause,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  CollectTaskRuntimeCancellationExprSites(clause.value.get(), counts);
}

void CollectTaskRuntimeCancellationStmtSites(
    const Stmt *stmt,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->let_stmt->value.get(),
                                              counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->assign_stmt->value.get(),
                                              counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->return_stmt->value.get(),
                                              counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationExprSites(stmt->if_stmt->condition.get(),
                                            counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectTaskRuntimeCancellationStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectTaskRuntimeCancellationStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    CollectTaskRuntimeCancellationExprSites(
        stmt->do_while_stmt->condition.get(), counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationForClauseSites(stmt->for_stmt->init, counts);
    CollectTaskRuntimeCancellationExprSites(stmt->for_stmt->condition.get(),
                                            counts);
    CollectTaskRuntimeCancellationForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationExprSites(stmt->switch_stmt->condition.get(),
                                            counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectTaskRuntimeCancellationStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationExprSites(stmt->while_stmt->condition.get(),
                                            counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->expr_stmt->value.get(),
                                              counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

Objc3TaskRuntimeCancellationSiteCounts CountTaskRuntimeCancellationSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3TaskRuntimeCancellationSiteCounts counts;
  for (const auto &stmt : body) {
    CollectTaskRuntimeCancellationStmtSites(stmt.get(), counts);
  }
  return counts;
}

Objc3TaskRuntimeCancellationProfile
BuildTaskRuntimeCancellationProfileFromCounts(
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites) {
  Objc3TaskRuntimeCancellationProfile profile;
  profile.runtime_hook_sites = runtime_hook_sites;
  profile.cancellation_check_sites = cancellation_check_sites;
  profile.cancellation_handler_sites = cancellation_handler_sites;
  profile.suspension_point_sites = suspension_point_sites;
  profile.cancellation_propagation_sites =
      std::min(profile.cancellation_handler_sites,
               profile.cancellation_check_sites);
  profile.task_runtime_interop_sites = profile.runtime_hook_sites;
  if (profile.task_runtime_interop_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.cancellation_check_sites) {
    profile.task_runtime_interop_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.task_runtime_interop_sites += profile.cancellation_check_sites;
  }
  if (profile.task_runtime_interop_sites >
      std::numeric_limits<std::size_t>::max() - profile.suspension_point_sites) {
    profile.task_runtime_interop_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.task_runtime_interop_sites += profile.suspension_point_sites;
  }
  profile.gate_blocked_sites = profile.cancellation_propagation_sites;
  if (profile.gate_blocked_sites > profile.task_runtime_interop_sites) {
    profile.gate_blocked_sites = profile.task_runtime_interop_sites;
    profile.contract_violation_sites += 1u;
  }
  profile.normalized_sites =
      profile.task_runtime_interop_sites - profile.gate_blocked_sites;
  if (profile.runtime_hook_sites > profile.task_runtime_interop_sites ||
      profile.cancellation_check_sites > profile.task_runtime_interop_sites ||
      profile.cancellation_handler_sites > profile.task_runtime_interop_sites ||
      profile.suspension_point_sites > profile.task_runtime_interop_sites ||
      profile.cancellation_propagation_sites >
          profile.cancellation_check_sites ||
      profile.normalized_sites > profile.task_runtime_interop_sites ||
      profile.gate_blocked_sites > profile.task_runtime_interop_sites ||
      profile.contract_violation_sites > profile.task_runtime_interop_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.task_runtime_interop_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.task_runtime_interop_sites) {
    profile.contract_violation_sites = profile.task_runtime_interop_sites;
  }
  profile.deterministic_task_runtime_cancellation_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

std::string BuildTaskRuntimeCancellationProfile(
    std::size_t task_runtime_interop_sites,
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites,
    std::size_t cancellation_propagation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_task_runtime_cancellation_handoff) {
  std::ostringstream out;
  out << "task-runtime-cancellation:task_runtime_interop_sites="
      << task_runtime_interop_sites
      << ";runtime_hook_sites=" << runtime_hook_sites
      << ";cancellation_check_sites=" << cancellation_check_sites
      << ";cancellation_handler_sites=" << cancellation_handler_sites
      << ";suspension_point_sites=" << suspension_point_sites
      << ";cancellation_propagation_sites=" << cancellation_propagation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_task_runtime_cancellation_handoff="
      << (deterministic_task_runtime_cancellation_handoff ? "true" : "false");
  return out.str();
}

bool IsTaskRuntimeCancellationProfileNormalized(
    std::size_t task_runtime_interop_sites,
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites,
    std::size_t cancellation_propagation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (runtime_hook_sites > task_runtime_interop_sites ||
      cancellation_check_sites > task_runtime_interop_sites ||
      cancellation_handler_sites > task_runtime_interop_sites ||
      suspension_point_sites > task_runtime_interop_sites ||
      cancellation_propagation_sites > cancellation_check_sites ||
      normalized_sites > task_runtime_interop_sites ||
      gate_blocked_sites > task_runtime_interop_sites ||
      contract_violation_sites > task_runtime_interop_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != task_runtime_interop_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

Objc3TaskRuntimeCancellationProfile
BuildTaskRuntimeCancellationProfileFromFunction(const FunctionDecl &fn) {
  const Objc3TaskRuntimeCancellationSiteCounts counts =
      CountTaskRuntimeCancellationSitesInBody(fn.body);
  return BuildTaskRuntimeCancellationProfileFromCounts(
      counts.runtime_hook_sites,
      counts.cancellation_check_sites,
      counts.cancellation_handler_sites,
      counts.suspension_point_sites);
}

Objc3TaskRuntimeCancellationProfile
BuildTaskRuntimeCancellationProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3TaskRuntimeCancellationSiteCounts counts;
  if (method.has_body) {
    CollectTaskRuntimeCancellationSitesFromSymbol(method.selector, counts);
  }
  return BuildTaskRuntimeCancellationProfileFromCounts(
      counts.runtime_hook_sites,
      counts.cancellation_check_sites,
      counts.cancellation_handler_sites,
      counts.suspension_point_sites);
}

}  // namespace objc3c::parse

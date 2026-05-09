#include "parse/objc3_parser_await_suspension_profiles.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"

namespace objc3c::parse {
namespace {

bool IsAwaitKeywordSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "await" || lowered.find("await_") != std::string::npos;
}

bool IsAwaitSuspensionPointSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("suspend") != std::string::npos ||
         lowered.find("yield") != std::string::npos;
}

bool IsAwaitResumeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("resume") != std::string::npos ||
         lowered.find("wakeup") != std::string::npos;
}

bool IsAwaitStateMachineSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("state") != std::string::npos ||
         lowered.find("continuation") != std::string::npos ||
         lowered.find("poll") != std::string::npos;
}

bool IsAwaitContinuationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation") != std::string::npos ||
         lowered.find("future") != std::string::npos ||
         lowered.find("promise") != std::string::npos;
}

struct Objc3AwaitSuspensionSiteCounts {
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
};

void CollectAwaitSuspensionSitesFromSymbol(
    const std::string &symbol,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (IsAwaitKeywordSymbol(symbol)) {
    counts.await_keyword_sites += 1u;
  }
  if (IsAwaitSuspensionPointSymbol(symbol)) {
    counts.await_suspension_point_sites += 1u;
  }
  if (IsAwaitResumeSymbol(symbol)) {
    counts.await_resume_sites += 1u;
  }
  if (IsAwaitStateMachineSymbol(symbol)) {
    counts.await_state_machine_sites += 1u;
  }
  if (IsAwaitContinuationSymbol(symbol)) {
    counts.await_continuation_sites += 1u;
  }
}

void CollectAwaitSuspensionExprSites(
    const Expr *expr,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  if (expr->await_expression_enabled) {
    counts.await_keyword_sites += 1u;
    counts.await_suspension_point_sites += 1u;
    counts.await_continuation_sites += 1u;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectAwaitSuspensionSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectAwaitSuspensionSitesFromSymbol(expr->selector, counts);
    CollectAwaitSuspensionExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectAwaitSuspensionExprSites(expr->left.get(), counts);
    CollectAwaitSuspensionExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectAwaitSuspensionExprSites(expr->left.get(), counts);
    CollectAwaitSuspensionExprSites(expr->right.get(), counts);
    CollectAwaitSuspensionExprSites(expr->third.get(), counts);
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

void CollectAwaitSuspensionForClauseSites(
    const ForClause &clause,
    Objc3AwaitSuspensionSiteCounts &counts) {
  CollectAwaitSuspensionExprSites(clause.value.get(), counts);
}

void CollectAwaitSuspensionStmtSites(
    const Stmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectAwaitSuspensionStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectAwaitSuspensionStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    CollectAwaitSuspensionExprSites(stmt->do_while_stmt->condition.get(),
                                    counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionForClauseSites(stmt->for_stmt->init, counts);
    CollectAwaitSuspensionExprSites(stmt->for_stmt->condition.get(), counts);
    CollectAwaitSuspensionForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionExprSites(stmt->switch_stmt->condition.get(),
                                    counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectAwaitSuspensionStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

Objc3AwaitSuspensionSiteCounts CountAwaitSuspensionSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3AwaitSuspensionSiteCounts counts;
  for (const auto &stmt : body) {
    CollectAwaitSuspensionStmtSites(stmt.get(), counts);
  }
  return counts;
}

Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromCounts(
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites) {
  Objc3AwaitSuspensionProfile profile;
  profile.await_keyword_sites = await_keyword_sites;
  profile.await_suspension_point_sites = await_suspension_point_sites;
  profile.await_resume_sites = await_resume_sites;
  profile.await_state_machine_sites = await_state_machine_sites;
  profile.await_continuation_sites = await_continuation_sites;
  profile.await_suspension_sites = profile.await_keyword_sites;
  if (profile.await_suspension_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.await_suspension_point_sites) {
    profile.await_suspension_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.await_suspension_sites += profile.await_suspension_point_sites;
  }
  if (profile.await_suspension_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.await_continuation_sites) {
    profile.await_suspension_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.await_suspension_sites += profile.await_continuation_sites;
  }
  profile.gate_blocked_sites = std::min(
      profile.await_suspension_sites,
      std::min(profile.await_resume_sites, profile.await_state_machine_sites));
  profile.normalized_sites =
      profile.await_suspension_sites - profile.gate_blocked_sites;
  if (profile.await_keyword_sites > profile.await_suspension_sites ||
      profile.await_suspension_point_sites > profile.await_suspension_sites ||
      profile.await_resume_sites > profile.await_suspension_sites ||
      profile.await_state_machine_sites > profile.await_suspension_sites ||
      profile.await_continuation_sites > profile.await_suspension_sites ||
      profile.normalized_sites > profile.await_suspension_sites ||
      profile.gate_blocked_sites > profile.await_suspension_sites ||
      profile.contract_violation_sites > profile.await_suspension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.await_suspension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.await_suspension_sites) {
    profile.contract_violation_sites = profile.await_suspension_sites;
  }
  profile.deterministic_await_suspension_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

std::string BuildAwaitSuspensionProfile(
    std::size_t await_suspension_sites,
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_await_suspension_handoff) {
  std::ostringstream out;
  out << "await-suspension:await_suspension_sites="
      << await_suspension_sites
      << ";await_keyword_sites=" << await_keyword_sites
      << ";await_suspension_point_sites=" << await_suspension_point_sites
      << ";await_resume_sites=" << await_resume_sites
      << ";await_state_machine_sites=" << await_state_machine_sites
      << ";await_continuation_sites=" << await_continuation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_await_suspension_handoff="
      << (deterministic_await_suspension_handoff ? "true" : "false");
  return out.str();
}

bool IsAwaitSuspensionProfileNormalized(
    std::size_t await_suspension_sites,
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (await_keyword_sites > await_suspension_sites ||
      await_suspension_point_sites > await_suspension_sites ||
      await_resume_sites > await_suspension_sites ||
      await_state_machine_sites > await_suspension_sites ||
      await_continuation_sites > await_suspension_sites ||
      normalized_sites > await_suspension_sites ||
      gate_blocked_sites > await_suspension_sites ||
      contract_violation_sites > await_suspension_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != await_suspension_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3AwaitSuspensionSiteCounts counts =
      CountAwaitSuspensionSitesInBody(fn.body);
  return BuildAwaitSuspensionProfileFromCounts(
      counts.await_keyword_sites,
      counts.await_suspension_point_sites,
      counts.await_resume_sites,
      counts.await_state_machine_sites,
      counts.await_continuation_sites);
}

Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3AwaitSuspensionSiteCounts counts;
  if (method.has_body) {
    CollectAwaitSuspensionSitesFromSymbol(method.selector, counts);
  }
  return BuildAwaitSuspensionProfileFromCounts(
      counts.await_keyword_sites,
      counts.await_suspension_point_sites,
      counts.await_resume_sites,
      counts.await_state_machine_sites,
      counts.await_continuation_sites);
}

}  // namespace objc3c::parse

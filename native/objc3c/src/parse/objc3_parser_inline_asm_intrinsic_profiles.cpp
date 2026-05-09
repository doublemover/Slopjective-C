#include "parse/objc3_parser_inline_asm_intrinsic_profiles.h"

#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"

namespace objc3c::parse {
namespace {

bool IsInlineAsmCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "asm" || lowered == "__asm" || lowered == "__asm__" ||
         lowered.rfind("asm_", 0) == 0 || lowered.rfind("__asm_", 0) == 0 ||
         lowered.find("inline_asm") != std::string::npos;
}

bool IsIntrinsicCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.rfind("__builtin_", 0) == 0 ||
         lowered.rfind("llvm.", 0) == 0 ||
         lowered.rfind("llvm_", 0) == 0 ||
         lowered.find("intrinsic") != std::string::npos;
}

bool IsPrivilegedIntrinsicCallSymbol(const std::string &symbol) {
  if (!IsIntrinsicCallSymbol(symbol)) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("privileged") != std::string::npos ||
         lowered.find("unsafe") != std::string::npos ||
         lowered.find("syscall") != std::string::npos ||
         lowered.rfind("__builtin_ia32_", 0) == 0 ||
         lowered.rfind("__builtin_arm_", 0) == 0;
}

struct Objc3InlineAsmIntrinsicSiteCounts {
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
};

void CollectInlineAsmIntrinsicSitesFromSymbol(
    const std::string &symbol,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (IsInlineAsmCallSymbol(symbol)) {
    counts.inline_asm_sites += 1u;
  }
  if (IsIntrinsicCallSymbol(symbol)) {
    counts.intrinsic_sites += 1u;
    counts.governed_intrinsic_sites += 1u;
    if (IsPrivilegedIntrinsicCallSymbol(symbol)) {
      counts.privileged_intrinsic_sites += 1u;
    }
  }
}

void CollectInlineAsmIntrinsicExprSites(
    const Expr *expr,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectInlineAsmIntrinsicSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectInlineAsmIntrinsicExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectInlineAsmIntrinsicSitesFromSymbol(expr->selector, counts);
    CollectInlineAsmIntrinsicExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectInlineAsmIntrinsicExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectInlineAsmIntrinsicExprSites(expr->left.get(), counts);
    CollectInlineAsmIntrinsicExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectInlineAsmIntrinsicExprSites(expr->left.get(), counts);
    CollectInlineAsmIntrinsicExprSites(expr->right.get(), counts);
    CollectInlineAsmIntrinsicExprSites(expr->third.get(), counts);
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

void CollectInlineAsmIntrinsicForClauseSites(
    const ForClause &clause,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  CollectInlineAsmIntrinsicExprSites(clause.value.get(), counts);
}

void CollectInlineAsmIntrinsicStmtSites(
    const Stmt *stmt,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->assign_stmt->value.get(),
                                         counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->return_stmt->value.get(),
                                         counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectInlineAsmIntrinsicStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectInlineAsmIntrinsicStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    CollectInlineAsmIntrinsicExprSites(stmt->do_while_stmt->condition.get(),
                                       counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicForClauseSites(stmt->for_stmt->init, counts);
    CollectInlineAsmIntrinsicExprSites(stmt->for_stmt->condition.get(),
                                       counts);
    CollectInlineAsmIntrinsicForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicExprSites(stmt->switch_stmt->condition.get(),
                                       counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectInlineAsmIntrinsicStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicExprSites(stmt->while_stmt->condition.get(),
                                       counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

Objc3InlineAsmIntrinsicSiteCounts CountInlineAsmIntrinsicSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3InlineAsmIntrinsicSiteCounts counts;
  for (const auto &stmt : body) {
    CollectInlineAsmIntrinsicStmtSites(stmt.get(), counts);
  }
  return counts;
}

Objc3InlineAsmIntrinsicGovernanceProfile
BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites) {
  Objc3InlineAsmIntrinsicGovernanceProfile profile;
  profile.inline_asm_sites = inline_asm_sites;
  profile.intrinsic_sites = intrinsic_sites;
  profile.governed_intrinsic_sites = governed_intrinsic_sites;
  profile.privileged_intrinsic_sites = privileged_intrinsic_sites;
  profile.inline_asm_intrinsic_sites =
      profile.inline_asm_sites + profile.intrinsic_sites;
  profile.gate_blocked_sites = profile.privileged_intrinsic_sites;
  if (profile.gate_blocked_sites > profile.inline_asm_intrinsic_sites) {
    profile.normalized_sites = 0u;
  } else {
    profile.normalized_sites =
        profile.inline_asm_intrinsic_sites - profile.gate_blocked_sites;
  }

  if (profile.inline_asm_sites > profile.inline_asm_intrinsic_sites ||
      profile.intrinsic_sites > profile.inline_asm_intrinsic_sites ||
      profile.governed_intrinsic_sites > profile.intrinsic_sites ||
      profile.privileged_intrinsic_sites > profile.governed_intrinsic_sites ||
      profile.normalized_sites > profile.inline_asm_intrinsic_sites ||
      profile.gate_blocked_sites > profile.inline_asm_intrinsic_sites ||
      profile.contract_violation_sites > profile.inline_asm_intrinsic_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.inline_asm_intrinsic_sites) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_inline_asm_intrinsic_governance_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

std::string BuildInlineAsmIntrinsicGovernanceProfile(
    std::size_t inline_asm_intrinsic_sites,
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_inline_asm_intrinsic_governance_handoff) {
  std::ostringstream out;
  out << "inline-asm-intrinsic-governance:inline_asm_intrinsic_sites="
      << inline_asm_intrinsic_sites
      << ";inline_asm_sites=" << inline_asm_sites
      << ";intrinsic_sites=" << intrinsic_sites
      << ";governed_intrinsic_sites=" << governed_intrinsic_sites
      << ";privileged_intrinsic_sites=" << privileged_intrinsic_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_inline_asm_intrinsic_governance_handoff="
      << (deterministic_inline_asm_intrinsic_governance_handoff ? "true"
                                                                : "false");
  return out.str();
}

bool IsInlineAsmIntrinsicGovernanceProfileNormalized(
    std::size_t inline_asm_intrinsic_sites,
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (inline_asm_sites > inline_asm_intrinsic_sites ||
      intrinsic_sites > inline_asm_intrinsic_sites ||
      governed_intrinsic_sites > intrinsic_sites ||
      privileged_intrinsic_sites > governed_intrinsic_sites ||
      normalized_sites > inline_asm_intrinsic_sites ||
      gate_blocked_sites > inline_asm_intrinsic_sites ||
      contract_violation_sites > inline_asm_intrinsic_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != inline_asm_intrinsic_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

Objc3InlineAsmIntrinsicGovernanceProfile
BuildInlineAsmIntrinsicGovernanceProfileFromFunction(const FunctionDecl &fn) {
  const Objc3InlineAsmIntrinsicSiteCounts counts =
      CountInlineAsmIntrinsicSitesInBody(fn.body);
  return BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
      counts.inline_asm_sites,
      counts.intrinsic_sites,
      counts.governed_intrinsic_sites,
      counts.privileged_intrinsic_sites);
}

Objc3InlineAsmIntrinsicGovernanceProfile
BuildInlineAsmIntrinsicGovernanceProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3InlineAsmIntrinsicSiteCounts counts;
  if (method.has_body) {
    CollectInlineAsmIntrinsicSitesFromSymbol(method.selector, counts);
  }
  return BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
      counts.inline_asm_sites,
      counts.intrinsic_sites,
      counts.governed_intrinsic_sites,
      counts.privileged_intrinsic_sites);
}

}  // namespace objc3c::parse

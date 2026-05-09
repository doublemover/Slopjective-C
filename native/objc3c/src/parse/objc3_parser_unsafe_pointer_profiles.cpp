#include "parse/objc3_parser_unsafe_pointer_profiles.h"

#include <memory>
#include <sstream>
#include <vector>

namespace objc3c::parse {
namespace {

bool IsUnsafeOwnershipQualifierSpelling(const std::string &spelling) {
  return spelling == "__unsafe_unretained";
}

std::size_t CountRawPointerTypeSites(
    const std::vector<FuncParam> &params,
    bool has_return_pointer_declarator) {
  std::size_t sites = has_return_pointer_declarator ? 1u : 0u;
  for (const auto &param : params) {
    if (param.has_pointer_declarator) {
      sites += 1u;
    }
  }
  return sites;
}

std::size_t CountUnsafeKeywordSites(
    const std::vector<FuncParam> &params,
    const std::string &return_ownership_qualifier_spelling) {
  std::size_t sites =
      IsUnsafeOwnershipQualifierSpelling(return_ownership_qualifier_spelling)
          ? 1u
          : 0u;
  for (const auto &param : params) {
    if (IsUnsafeOwnershipQualifierSpelling(
            param.ownership_qualifier_spelling)) {
      sites += 1u;
    }
  }
  return sites;
}

bool IsPointerArithmeticMutationOperator(const std::string &op) {
  return op == "+=" || op == "-=" || op == "++" || op == "--";
}

void CollectPointerArithmeticExprSites(const Expr *expr, std::size_t &sites) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
  case Expr::Kind::Binary:
    if (expr->op == "+" || expr->op == "-") {
      sites += 1u;
    }
    CollectPointerArithmeticExprSites(expr->left.get(), sites);
    CollectPointerArithmeticExprSites(expr->right.get(), sites);
    return;
  case Expr::Kind::Conditional:
    CollectPointerArithmeticExprSites(expr->left.get(), sites);
    CollectPointerArithmeticExprSites(expr->right.get(), sites);
    CollectPointerArithmeticExprSites(expr->third.get(), sites);
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      CollectPointerArithmeticExprSites(arg.get(), sites);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectPointerArithmeticExprSites(expr->receiver.get(), sites);
    for (const auto &arg : expr->args) {
      CollectPointerArithmeticExprSites(arg.get(), sites);
    }
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

void CollectPointerArithmeticForClauseSites(const ForClause &clause,
                                            std::size_t &sites) {
  if (clause.kind == ForClause::Kind::Assign &&
      IsPointerArithmeticMutationOperator(clause.op)) {
    sites += 1u;
  }
  CollectPointerArithmeticExprSites(clause.value.get(), sites);
}

void CollectPointerArithmeticStmtSites(const Stmt *stmt, std::size_t &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->let_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      if (IsPointerArithmeticMutationOperator(stmt->assign_stmt->op)) {
        sites += 1u;
      }
      CollectPointerArithmeticExprSites(stmt->assign_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->return_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->if_stmt->condition.get(), sites);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectPointerArithmeticStmtSites(then_stmt.get(), sites);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectPointerArithmeticStmtSites(else_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    CollectPointerArithmeticExprSites(stmt->do_while_stmt->condition.get(),
                                      sites);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticForClauseSites(stmt->for_stmt->init, sites);
    CollectPointerArithmeticExprSites(stmt->for_stmt->condition.get(), sites);
    CollectPointerArithmeticForClauseSites(stmt->for_stmt->step, sites);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->switch_stmt->condition.get(),
                                      sites);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectPointerArithmeticStmtSites(case_stmt.get(), sites);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->while_stmt->condition.get(),
                                      sites);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->expr_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

std::size_t CountPointerArithmeticSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  std::size_t sites = 0;
  for (const auto &stmt : body) {
    CollectPointerArithmeticStmtSites(stmt.get(), sites);
  }
  return sites;
}

Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromCounts(
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites) {
  Objc3UnsafePointerExtensionProfile profile;
  profile.unsafe_keyword_sites = unsafe_keyword_sites;
  profile.pointer_arithmetic_sites = pointer_arithmetic_sites;
  profile.raw_pointer_type_sites = raw_pointer_type_sites;
  profile.unsafe_operation_sites =
      pointer_arithmetic_sites + raw_pointer_type_sites;
  profile.unsafe_pointer_extension_sites =
      profile.unsafe_keyword_sites + profile.pointer_arithmetic_sites +
      profile.raw_pointer_type_sites;

  const bool gate_open = unsafe_keyword_sites > 0u;
  profile.gate_blocked_sites =
      gate_open ? 0u
                : (profile.pointer_arithmetic_sites +
                   profile.raw_pointer_type_sites);
  profile.normalized_sites =
      profile.unsafe_pointer_extension_sites - profile.gate_blocked_sites;

  if (profile.unsafe_keyword_sites > profile.unsafe_pointer_extension_sites ||
      profile.pointer_arithmetic_sites >
          profile.unsafe_pointer_extension_sites ||
      profile.raw_pointer_type_sites > profile.unsafe_pointer_extension_sites ||
      profile.unsafe_operation_sites >
          profile.unsafe_pointer_extension_sites ||
      profile.normalized_sites > profile.unsafe_pointer_extension_sites ||
      profile.gate_blocked_sites > profile.unsafe_pointer_extension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.unsafe_pointer_extension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (!gate_open && profile.normalized_sites != profile.unsafe_keyword_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (gate_open && profile.gate_blocked_sites != 0u) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_unsafe_pointer_extension_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

std::string BuildUnsafePointerExtensionProfile(
    std::size_t unsafe_pointer_extension_sites,
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites,
    std::size_t unsafe_operation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_unsafe_pointer_extension_handoff) {
  std::ostringstream out;
  out << "unsafe-pointer-extension:unsafe_pointer_extension_sites="
      << unsafe_pointer_extension_sites
      << ";unsafe_keyword_sites=" << unsafe_keyword_sites
      << ";pointer_arithmetic_sites=" << pointer_arithmetic_sites
      << ";raw_pointer_type_sites=" << raw_pointer_type_sites
      << ";unsafe_operation_sites=" << unsafe_operation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_unsafe_pointer_extension_handoff="
      << (deterministic_unsafe_pointer_extension_handoff ? "true" : "false");
  return out.str();
}

bool IsUnsafePointerExtensionProfileNormalized(
    std::size_t unsafe_pointer_extension_sites,
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites,
    std::size_t unsafe_operation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (unsafe_keyword_sites > unsafe_pointer_extension_sites ||
      pointer_arithmetic_sites > unsafe_pointer_extension_sites ||
      raw_pointer_type_sites > unsafe_pointer_extension_sites ||
      unsafe_operation_sites > unsafe_pointer_extension_sites ||
      normalized_sites > unsafe_pointer_extension_sites ||
      gate_blocked_sites > unsafe_pointer_extension_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != unsafe_pointer_extension_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromFunction(
    const FunctionDecl &fn) {
  const std::size_t unsafe_keyword_sites =
      CountUnsafeKeywordSites(fn.params,
                              fn.return_ownership_qualifier_spelling);
  const std::size_t raw_pointer_type_sites =
      CountRawPointerTypeSites(fn.params, fn.has_return_pointer_declarator);
  const std::size_t pointer_arithmetic_sites =
      CountPointerArithmeticSitesInBody(fn.body);
  return BuildUnsafePointerExtensionProfileFromCounts(
      unsafe_keyword_sites, pointer_arithmetic_sites, raw_pointer_type_sites);
}

Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  const std::size_t unsafe_keyword_sites =
      CountUnsafeKeywordSites(method.params,
                              method.return_ownership_qualifier_spelling);
  const std::size_t raw_pointer_type_sites = CountRawPointerTypeSites(
      method.params, method.has_return_pointer_declarator);
  const std::size_t pointer_arithmetic_sites =
      method.has_body && raw_pointer_type_sites > 0u ? 1u : 0u;
  return BuildUnsafePointerExtensionProfileFromCounts(
      unsafe_keyword_sites, pointer_arithmetic_sites, raw_pointer_type_sites);
}

}  // namespace objc3c::parse

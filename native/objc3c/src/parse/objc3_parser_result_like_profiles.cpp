#include "parse/objc3_parser_result_like_profiles.h"

#include <sstream>

namespace objc3c::parse {
namespace {

bool IsResultLikeFailureExpr(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  switch (expr->kind) {
  case Expr::Kind::NilLiteral:
    return true;
  case Expr::Kind::BoolLiteral:
    return !expr->bool_value;
  case Expr::Kind::Number:
    return expr->number == 0;
  case Expr::Kind::Identifier:
    return expr->ident == "err" || expr->ident == "error" ||
           expr->ident == "failure";
  default:
    return false;
  }
}

void CollectResultLikeExprProfile(const Expr *expr,
                                  Objc3ResultLikeProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Binary:
    CollectResultLikeExprProfile(expr->left.get(), profile);
    CollectResultLikeExprProfile(expr->right.get(), profile);
    return;
  case Expr::Kind::Conditional:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    CollectResultLikeExprProfile(expr->left.get(), profile);
    CollectResultLikeExprProfile(expr->right.get(), profile);
    CollectResultLikeExprProfile(expr->third.get(), profile);
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      CollectResultLikeExprProfile(arg.get(), profile);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectResultLikeExprProfile(expr->receiver.get(), profile);
    for (const auto &arg : expr->args) {
      CollectResultLikeExprProfile(arg.get(), profile);
    }
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  default:
    return;
  }
}

void CollectResultLikeForClauseProfile(const ForClause &clause,
                                       Objc3ResultLikeProfile &profile) {
  CollectResultLikeExprProfile(clause.value.get(), profile);
}

void CollectResultLikeStmtProfile(const Stmt *stmt,
                                  Objc3ResultLikeProfile &profile) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->let_stmt->value.get(), profile);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->assign_stmt->value.get(), profile);
    }
    return;
  case Stmt::Kind::Return:
    profile.result_like_sites += 1u;
    profile.normalized_sites += 1u;
    if (stmt->return_stmt != nullptr && stmt->return_stmt->value != nullptr) {
      profile.result_payload_sites += 1u;
      CollectResultLikeExprProfile(stmt->return_stmt->value.get(), profile);
      if (IsResultLikeFailureExpr(stmt->return_stmt->value.get())) {
        profile.result_failure_sites += 1u;
      } else {
        profile.result_success_sites += 1u;
      }
    } else {
      profile.result_success_sites += 1u;
    }
    return;
  case Stmt::Kind::If:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->if_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->if_stmt->condition.get(), profile);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectResultLikeStmtProfile(then_stmt.get(), profile);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectResultLikeStmtProfile(else_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectResultLikeStmtProfile(body_stmt.get(), profile);
      }
      CollectResultLikeExprProfile(stmt->do_while_stmt->condition.get(),
                                   profile);
    }
    return;
  case Stmt::Kind::For:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->for_stmt != nullptr) {
      CollectResultLikeForClauseProfile(stmt->for_stmt->init, profile);
      CollectResultLikeExprProfile(stmt->for_stmt->condition.get(), profile);
      CollectResultLikeForClauseProfile(stmt->for_stmt->step, profile);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectResultLikeStmtProfile(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::Switch:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->switch_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->switch_stmt->condition.get(),
                                   profile);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectResultLikeStmtProfile(case_stmt.get(), profile);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->while_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->while_stmt->condition.get(),
                                   profile);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectResultLikeStmtProfile(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectResultLikeStmtProfile(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->expr_stmt->value.get(), profile);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

}  // namespace

std::string BuildResultLikeProfile(
    std::size_t result_like_sites,
    std::size_t result_success_sites,
    std::size_t result_failure_sites,
    std::size_t result_branch_sites,
    std::size_t result_payload_sites,
    std::size_t normalized_sites,
    std::size_t branch_merge_sites,
    std::size_t contract_violation_sites,
    bool deterministic_result_like_lowering_handoff) {
  std::ostringstream out;
  out << "result-like-lowering:result_like_sites=" << result_like_sites
      << ";result_success_sites=" << result_success_sites
      << ";result_failure_sites=" << result_failure_sites
      << ";result_branch_sites=" << result_branch_sites
      << ";result_payload_sites=" << result_payload_sites
      << ";normalized_sites=" << normalized_sites
      << ";branch_merge_sites=" << branch_merge_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_result_like_lowering_handoff="
      << (deterministic_result_like_lowering_handoff ? "true" : "false");
  return out.str();
}

bool IsResultLikeProfileNormalized(
    std::size_t result_like_sites,
    std::size_t result_success_sites,
    std::size_t result_failure_sites,
    std::size_t result_branch_sites,
    std::size_t result_payload_sites,
    std::size_t normalized_sites,
    std::size_t branch_merge_sites,
    std::size_t contract_violation_sites) {
  if (result_success_sites + result_failure_sites != normalized_sites) {
    return false;
  }
  if (result_success_sites > result_like_sites ||
      result_failure_sites > result_like_sites ||
      result_branch_sites > result_like_sites ||
      result_payload_sites > result_like_sites) {
    return false;
  }
  if (normalized_sites + branch_merge_sites != result_like_sites) {
    return false;
  }
  return contract_violation_sites == 0;
}

Objc3ResultLikeProfile BuildResultLikeProfileFromBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ResultLikeProfile profile;
  for (const auto &stmt : body) {
    CollectResultLikeStmtProfile(stmt.get(), profile);
  }

  if (profile.result_success_sites + profile.result_failure_sites !=
      profile.normalized_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.result_success_sites > profile.result_like_sites ||
      profile.result_failure_sites > profile.result_like_sites ||
      profile.result_branch_sites > profile.result_like_sites ||
      profile.result_payload_sites > profile.result_like_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.branch_merge_sites !=
      profile.result_like_sites) {
    profile.contract_violation_sites += 1u;
  }
  profile.deterministic_result_like_lowering_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

Objc3ResultLikeProfile BuildResultLikeProfileFromOpaqueBody(bool has_body) {
  Objc3ResultLikeProfile profile;
  if (has_body) {
    profile.result_like_sites = 1u;
    profile.result_branch_sites = 1u;
    profile.branch_merge_sites = 1u;
  }
  profile.deterministic_result_like_lowering_handoff = true;
  return profile;
}

}  // namespace objc3c::parse

#include "sema/objc3_semantic_match_exhaustiveness.h"

bool IsMatchCatchAllPattern(const SwitchCase &case_stmt) {
  if (case_stmt.has_match_guard) {
    return false;
  }
  return case_stmt.is_default ||
         case_stmt.match_pattern_kind == MatchPatternKind::Wildcard ||
         case_stmt.match_pattern_kind == MatchPatternKind::Binding;
}

bool IsMatchExpressionCatchAllPattern(
    const Expr::MatchExpressionArm &case_arm) {
  if (case_arm.has_guard) {
    return false;
  }
  return case_arm.is_default ||
         case_arm.pattern_kind ==
             Expr::MatchExpressionPatternKind::Wildcard ||
         case_arm.pattern_kind ==
             Expr::MatchExpressionPatternKind::Binding;
}

bool MatchGuardHasUnsupportedSideEffect(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  switch (expr->kind) {
    case Expr::Kind::Call:
    case Expr::Kind::Try:
    case Expr::Kind::Throw:
    case Expr::Kind::MessageSend:
    case Expr::Kind::BlockLiteral:
      return true;
    case Expr::Kind::Binary:
      return MatchGuardHasUnsupportedSideEffect(expr->left.get()) ||
             MatchGuardHasUnsupportedSideEffect(expr->right.get());
    case Expr::Kind::Conditional:
      return MatchGuardHasUnsupportedSideEffect(expr->left.get()) ||
             MatchGuardHasUnsupportedSideEffect(expr->right.get()) ||
             MatchGuardHasUnsupportedSideEffect(expr->third.get());
    case Expr::Kind::CollectionLiteral:
      for (const auto &key : expr->collection_keys) {
        if (MatchGuardHasUnsupportedSideEffect(key.get())) {
          return true;
        }
      }
      for (const auto &value : expr->collection_values) {
        if (MatchGuardHasUnsupportedSideEffect(value.get())) {
          return true;
        }
      }
      return false;
    case Expr::Kind::IndexAccess:
      return MatchGuardHasUnsupportedSideEffect(expr->left.get()) ||
             MatchGuardHasUnsupportedSideEffect(expr->right.get());
    case Expr::Kind::StringInterpolation:
      for (const auto &payload : expr->args) {
        if (MatchGuardHasUnsupportedSideEffect(payload.get())) {
          return true;
        }
      }
      return false;
    case Expr::Kind::MatchExpression:
      return true;
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::StringLiteral:
    case Expr::Kind::Identifier:
    case Expr::Kind::KeyPathLiteral:
      return false;
  }
  return true;
}

MatchExhaustivenessInfo ClassifyMatchExhaustiveness(
    const SwitchStmt &switch_stmt) {
  MatchExhaustivenessInfo info;
  bool saw_true = false;
  bool saw_false = false;
  bool saw_ok = false;
  bool saw_err = false;
  for (const auto &case_stmt : switch_stmt.cases) {
    if (IsMatchCatchAllPattern(case_stmt)) {
      info.exhaustive = true;
      return info;
    }
    if (case_stmt.match_pattern_kind == MatchPatternKind::LiteralBool) {
      if (case_stmt.value == 0) {
        saw_false = true;
      } else {
        saw_true = true;
      }
    } else if (case_stmt.match_pattern_kind == MatchPatternKind::ResultCase) {
      if (case_stmt.match_result_case_name == "Ok") {
        saw_ok = true;
      } else if (case_stmt.match_result_case_name == "Err") {
        saw_err = true;
      }
    }
  }
  info.bool_exhaustive = saw_true && saw_false;
  info.result_case_exhaustive = saw_ok && saw_err;
  info.exhaustive = info.bool_exhaustive || info.result_case_exhaustive;
  return info;
}

MatchExpressionExhaustivenessInfo ClassifyMatchExpressionExhaustiveness(
    const Expr &expr) {
  MatchExpressionExhaustivenessInfo info;
  bool saw_true = false;
  bool saw_false = false;
  bool saw_ok = false;
  bool saw_err = false;
  for (const auto &case_arm : expr.match_expression_arms) {
    if (IsMatchExpressionCatchAllPattern(case_arm)) {
      info.exhaustive = true;
      info.catch_all_exhaustive = true;
      return info;
    }
    if (case_arm.pattern_kind ==
        Expr::MatchExpressionPatternKind::LiteralBool) {
      if (case_arm.literal_value == 0) {
        saw_false = true;
      } else {
        saw_true = true;
      }
    } else if (case_arm.pattern_kind ==
               Expr::MatchExpressionPatternKind::ResultCase) {
      if (case_arm.result_case_name == "Ok") {
        saw_ok = true;
      } else if (case_arm.result_case_name == "Err") {
        saw_err = true;
      }
    }
  }
  info.bool_exhaustive = saw_true && saw_false;
  info.result_case_exhaustive = saw_ok && saw_err;
  info.exhaustive = info.bool_exhaustive || info.result_case_exhaustive;
  return info;
}

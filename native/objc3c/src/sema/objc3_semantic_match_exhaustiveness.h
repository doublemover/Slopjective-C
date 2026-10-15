#pragma once

#include "ast/objc3_ast_core.h"

struct MatchExhaustivenessInfo {
  bool exhaustive = false;
  bool bool_exhaustive = false;
  bool result_case_exhaustive = false;
};

struct MatchExpressionExhaustivenessInfo {
  bool exhaustive = false;
  bool bool_exhaustive = false;
  bool result_case_exhaustive = false;
  bool catch_all_exhaustive = false;
};

bool IsMatchCatchAllPattern(const SwitchCase &case_stmt);
bool IsMatchExpressionCatchAllPattern(
    const Expr::MatchExpressionArm &case_arm);
bool MatchGuardHasUnsupportedSideEffect(const Expr *expr);

MatchExhaustivenessInfo ClassifyMatchExhaustiveness(
    const SwitchStmt &switch_stmt);
MatchExpressionExhaustivenessInfo ClassifyMatchExpressionExhaustiveness(
    const Expr &expr);

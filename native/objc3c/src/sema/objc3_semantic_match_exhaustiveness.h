#pragma once

#include "ast/objc3_ast_core.h"

struct MatchExhaustivenessInfo {
  bool exhaustive = false;
  bool bool_exhaustive = false;
  bool result_case_exhaustive = false;
};

bool IsMatchCatchAllPattern(const SwitchCase &case_stmt);

MatchExhaustivenessInfo ClassifyMatchExhaustiveness(
    const SwitchStmt &switch_stmt);

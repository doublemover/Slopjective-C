#include "sema/objc3_semantic_match_exhaustiveness.h"

bool IsMatchCatchAllPattern(const SwitchCase &case_stmt) {
  if (case_stmt.has_match_guard) {
    return false;
  }
  return case_stmt.is_default ||
         case_stmt.match_pattern_kind == MatchPatternKind::Wildcard ||
         case_stmt.match_pattern_kind == MatchPatternKind::Binding;
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

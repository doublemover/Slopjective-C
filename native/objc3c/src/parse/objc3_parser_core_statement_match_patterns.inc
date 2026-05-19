#include "parse/objc3_parser_core_statement_match_patterns_pattern_heads.inc"
#include "parse/objc3_parser_core_statement_match_patterns_binding_patterns.inc"
#include "parse/objc3_parser_core_statement_match_patterns_result_case_patterns.inc"
#include "parse/objc3_parser_core_statement_match_patterns_diagnostics_recovery.inc"

  bool ParseMatchPattern(SwitchCase &case_stmt) {
    case_stmt.match_pattern_enabled = true;
    if (ParseWildcardMatchPattern(case_stmt)) {
      return true;
    }
    if (ParseLiteralMatchPattern(case_stmt)) {
      return true;
    }

    bool handled = false;
    if (!ParseBindingMatchPattern(case_stmt, handled)) {
      return false;
    }
    if (handled) {
      return true;
    }

    if (ReportUnsupportedTypeTestMatchPatternIfPresent()) {
      return false;
    }

    if (!ParseResultCaseMatchPattern(case_stmt, handled)) {
      return false;
    }
    if (handled) {
      return true;
    }

    ReportInvalidMatchPattern();
    return false;
  }

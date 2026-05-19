static void RecordDoCatchUnsupportedRuntimeDiagnostic(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  RecordErrorHandlingSemanticDiagnostic(
      summary.contract_violation_sites, line, column, "O3S267",
      "unsupported feature claim: do/catch statements are not yet runnable in Objective-C 3 native mode",
      diagnostics);
}

static void RecordDoCatchEmptyCatchDiagnostic(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  RecordErrorHandlingSemanticDiagnostic(
      summary.contract_violation_sites, line, column, "O3S268",
      "do/catch requires at least one catch clause", diagnostics);
}

static void RecordDoCatchUnreachableCatchDiagnostic(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  RecordErrorHandlingSemanticDiagnostic(
      summary.contract_violation_sites, line, column, "O3S269",
      "catch clauses after a catch-all are unreachable", diagnostics);
}

static void RecordTryUnsupportedRuntimeDiagnostic(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  RecordErrorHandlingSemanticDiagnostic(
      summary.contract_violation_sites, line, column, "O3S270",
      "unsupported feature claim: try expressions are not yet runnable in Objective-C 3 native mode",
      diagnostics);
}

static void RecordTryOperandSurfaceDiagnostic(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  RecordErrorHandlingSemanticDiagnostic(
      summary.contract_violation_sites, line, column, "O3S271",
      "try operand must be a throwing or NSError-bridged call surface",
      diagnostics);
}

static void RecordTryPropagationContextDiagnostic(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  RecordErrorHandlingSemanticDiagnostic(
      summary.contract_violation_sites, line, column, "O3S272",
      "propagating try requires a throws function or an enclosing do/catch",
      diagnostics);
}

static void RecordThrowUnsupportedRuntimeDiagnostic(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  RecordErrorHandlingSemanticDiagnostic(
      summary.contract_violation_sites, line, column, "O3S273",
      "unsupported feature claim: throw statements are not yet runnable in Objective-C 3 native mode",
      diagnostics);
}

static void RecordThrowContextDiagnostic(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    unsigned line,
    unsigned column,
    std::vector<std::string> &diagnostics) {
  RecordErrorHandlingSemanticDiagnostic(
      summary.contract_violation_sites, line, column, "O3S274",
      "throw statements require a throws function or a catch body",
      diagnostics);
}

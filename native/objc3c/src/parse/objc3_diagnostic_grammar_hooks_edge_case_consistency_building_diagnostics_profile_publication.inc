namespace {

void PublishObjc3DiagnosticGrammarHooksEdgeCaseConsistencyFailureReason(
    Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface &surface) {
  if (!surface.language_profile_supported) {
    surface.failure_reason =
        "parser diagnostic grammar hooks require the canonical language profile";
  } else if (!surface.core_feature_expansion_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion is not ready";
  } else if (!surface.parser_snapshot_accounting_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks snapshot accounting is inconsistent";
  } else if (!surface.parser_diagnostic_token_budget_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks token budget is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks language-version pragma coordinate order is inconsistent";
  } else if (!surface.consistency_handoff_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks consistency handoff is inconsistent";
  } else {
    surface.failure_reason =
        "parser diagnostic grammar hooks edge-case consistency is not ready";
  }
}

}  // namespace

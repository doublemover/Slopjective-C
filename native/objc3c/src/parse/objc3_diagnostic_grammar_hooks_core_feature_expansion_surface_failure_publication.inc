void PublishObjc3DiagnosticGrammarHooksCoreFeatureExpansionFailure(
    Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface &surface) {
  if (surface.core_feature_expansion_ready) {
    return;
  }

  if (!surface.core_feature_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks core feature is not ready";
  } else if (!surface.accounting_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion accounting is inconsistent";
  } else if (!surface.source_precision_coverage_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion source precision coverage is inconsistent";
  } else if (!surface.replay_keys_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion replay keys are not ready";
  } else {
    surface.failure_reason = "parser diagnostic grammar hooks core feature expansion is not ready";
  }
}

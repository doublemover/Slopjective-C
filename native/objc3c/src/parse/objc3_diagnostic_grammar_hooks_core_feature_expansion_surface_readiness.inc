bool IsObjc3DiagnosticGrammarHooksCoreFeatureExpansionReady(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface &surface) {
  return surface.core_feature_expansion_ready && !surface.expansion_key.empty();
}

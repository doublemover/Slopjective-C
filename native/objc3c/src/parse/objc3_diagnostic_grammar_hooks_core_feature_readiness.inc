bool IsObjc3DiagnosticGrammarHooksCoreFeatureReady(
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &surface) {
  return surface.core_feature_consistent && !surface.core_feature_key.empty();
}

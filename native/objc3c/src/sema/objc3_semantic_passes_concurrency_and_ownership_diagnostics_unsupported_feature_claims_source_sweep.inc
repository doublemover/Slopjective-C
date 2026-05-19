static Objc3UnsupportedFeatureClaimEnforcementStats
DiagnoseUnsupportedFeatureClaimSources(
    const Objc3Program &ast,
    bool allow_source_only_block_literals,
    bool allow_source_only_defer_statements,
    bool allow_source_only_error_runtime_surface,
    bool arc_mode_enabled,
    std::vector<std::string> &diagnostics) {
  // unsupported-feature enforcement anchor: keep the
  // runnable-core positive path at zero-site readiness and reject accepted
  // unsupported advanced sources deterministically with O3S221.
  Objc3UnsupportedFeatureClaimEnforcementStats stats;
  const Objc3UnsupportedFeatureClaimContext context =
      BuildUnsupportedFeatureClaimContext(ast,
                                          allow_source_only_block_literals,
                                          allow_source_only_defer_statements,
                                          allow_source_only_error_runtime_surface,
                                          arc_mode_enabled);
  for (const auto &fn : ast.functions) {
    DiagnoseUnsupportedFunctionFeatureClaims(fn, diagnostics, stats, context);
  }
  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &method : protocol_decl.methods) {
      DiagnoseUnsupportedMethodFeatureClaims(method, diagnostics, stats, context);
    }
  }
  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method : interface_decl.methods) {
      DiagnoseUnsupportedMethodFeatureClaims(method, diagnostics, stats, context);
    }
  }
  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      DiagnoseUnsupportedMethodFeatureClaims(method, diagnostics, stats, context);
    }
  }
  PublishUnsupportedFeatureClaimFamilyCounts(stats);
  return stats;
}

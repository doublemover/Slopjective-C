Objc3ErrorDiagnosticsRecoveryProfile
BuildErrorDiagnosticsRecoveryProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ErrorDiagnosticsRecoverySiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectErrorDiagnosticsRecoveryProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  return BuildErrorDiagnosticsRecoveryProfileFromCounts(
      counts.diagnostic_emit_sites,
      counts.recovery_anchor_sites,
      counts.recovery_boundary_sites,
      counts.fail_closed_diagnostic_sites);
}

Objc3ErrorDiagnosticsRecoveryProfile
BuildErrorDiagnosticsRecoveryProfileFromFunction(const FunctionDecl &fn) {
  const Objc3ErrorDiagnosticsRecoverySiteCounts counts =
      CountErrorDiagnosticsRecoverySitesInBody(fn.body);
  return BuildErrorDiagnosticsRecoveryProfileFromCounts(
      counts.diagnostic_emit_sites,
      counts.recovery_anchor_sites,
      counts.recovery_boundary_sites,
      counts.fail_closed_diagnostic_sites);
}

void CollectErrorDiagnosticsRecoverySitesFromSymbol(
    const std::string &symbol,
    Objc3ErrorDiagnosticsRecoverySiteCounts &counts) {
  if (IsErrorDiagnosticSymbol(symbol)) {
    counts.diagnostic_emit_sites += 1u;
  }
  if (IsRecoveryAnchorSymbol(symbol)) {
    counts.recovery_anchor_sites += 1u;
  }
  if (IsRecoveryBoundarySymbol(symbol)) {
    counts.recovery_boundary_sites += 1u;
  }
  if (IsFailClosedDiagnosticSymbol(symbol)) {
    counts.fail_closed_diagnostic_sites += 1u;
  }
}

void CollectErrorDiagnosticsRecoveryProfileSymbol(
    const std::string &symbol,
    void *context) {
  auto *counts =
      static_cast<Objc3ErrorDiagnosticsRecoverySiteCounts *>(context);
  CollectErrorDiagnosticsRecoverySitesFromSymbol(symbol, *counts);
}

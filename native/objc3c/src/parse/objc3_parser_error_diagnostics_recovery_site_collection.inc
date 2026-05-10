bool IsErrorDiagnosticSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("error") != std::string::npos ||
         lowered.find("diagnostic") != std::string::npos ||
         lowered.find("invalid") != std::string::npos ||
         lowered.find("missing") != std::string::npos ||
         lowered.find("unsupported") != std::string::npos;
}

bool IsRecoveryAnchorSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("recover") != std::string::npos ||
         lowered.find("recovery") != std::string::npos ||
         lowered.find("resync") != std::string::npos ||
         lowered.find("anchor") != std::string::npos;
}

bool IsRecoveryBoundarySymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("throws") != std::string::npos ||
         lowered.find("throw") != std::string::npos ||
         lowered.find("result") != std::string::npos ||
         lowered.find("nserror") != std::string::npos ||
         lowered.find("catch") != std::string::npos ||
         lowered.find("finally") != std::string::npos ||
         lowered.find("boundary") != std::string::npos ||
         lowered.find("cleanup") != std::string::npos;
}

bool IsFailClosedDiagnosticSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("fatal") != std::string::npos ||
         lowered.find("panic") != std::string::npos ||
         lowered.find("trap") != std::string::npos ||
         lowered.find("abort") != std::string::npos ||
         lowered.find("fail_closed") != std::string::npos ||
         lowered.find("hard_error") != std::string::npos;
}

struct Objc3ErrorDiagnosticsRecoverySiteCounts {
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
};

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

Objc3ErrorDiagnosticsRecoverySiteCounts
CountErrorDiagnosticsRecoverySitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ErrorDiagnosticsRecoverySiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectErrorDiagnosticsRecoveryProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}

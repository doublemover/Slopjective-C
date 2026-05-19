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

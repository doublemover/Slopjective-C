// Parser-owned actor/sendability admission remains symbol profiling on top of
// the current token stream. This does not create dedicated actor/sendable
// grammar forms; it freezes the source contract around actor-isolation
// declarations, hop markers, sendable markers, and non-sendable crossings.
bool IsActorIsolationDeclSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("actor") != std::string::npos ||
         lowered.find("isolated") != std::string::npos ||
         lowered.find("isolation") != std::string::npos;
}

bool IsActorHopSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("hop_to") != std::string::npos ||
         lowered.find("enqueue") != std::string::npos ||
         lowered.find("executor") != std::string::npos;
}

bool IsSendableAnnotationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("sendable") != std::string::npos ||
         lowered.find("sendability") != std::string::npos;
}

bool IsNonSendableCrossingSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("non_sendable") != std::string::npos ||
         lowered.find("unsafe_sendable") != std::string::npos ||
         lowered.find("cross_actor") != std::string::npos;
}

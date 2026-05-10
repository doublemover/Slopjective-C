static PureContractCause FallbackPureContractCause(const FunctionDecl &fn) {
  PureContractCause cause;
  cause.token = "unknown";
  cause.line = fn.line;
  cause.column = fn.column;
  cause.present = true;
  cause.detail_token = cause.token;
  cause.detail_line = cause.line;
  cause.detail_column = cause.column;
  cause.detail_present = true;
  return cause;
}

static PureContractCause ResolvePureContractDiagnosticCause(
    const FunctionDecl &fn,
    const std::unordered_map<std::string, PureContractCause> &impure_causes) {
  PureContractCause cause;
  const auto cause_it = impure_causes.find(fn.name);
  if (cause_it != impure_causes.end()) {
    cause = cause_it->second;
  }
  if (!cause.present) {
    cause = FallbackPureContractCause(fn);
  }
  NormalizePureContractCauseDetail(cause);
  return cause;
}

static void EmitPureContractViolationDiagnostics(
    const std::vector<FunctionDecl> &functions,
    const std::unordered_set<std::string> &impure_functions,
    const std::unordered_map<std::string, PureContractCause> &impure_causes,
    std::vector<std::string> &diagnostics) {
  std::unordered_set<std::string> reported;
  for (const auto &fn : functions) {
    if (fn.is_prototype || !fn.is_pure) {
      continue;
    }
    if (impure_functions.find(fn.name) == impure_functions.end()) {
      continue;
    }
    if (!reported.insert(fn.name).second) {
      continue;
    }
    const PureContractCause cause =
        ResolvePureContractDiagnosticCause(fn, impure_causes);
    diagnostics.push_back(
        MakeDiag(fn.line, fn.column, "O3S215",
                 "pure contract violation: function '" + fn.name +
                     "' declared 'pure' has side effects (cause: " +
                     cause.token + "; cause-site:" +
                     std::to_string(cause.line) + ":" +
                     std::to_string(cause.column) + "; detail:" +
                     cause.detail_token + "@" +
                     std::to_string(cause.detail_line) + ":" +
                     std::to_string(cause.detail_column) + ")"));
  }
}

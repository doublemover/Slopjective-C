template <typename CallableDecl>
static void AccumulateErrorHandlingErrorBridgeLegalityForCallable(
    const CallableDecl &decl, const Objc3Program &program,
    Objc3ErrorHandlingErrorBridgeLegalitySummary &summary,
    bool allow_source_only_error_runtime_surface,
    std::vector<std::string> &diagnostics) {
  const bool has_nserror = decl.objc_nserror_declared;
  const bool has_status_code = decl.objc_status_code_declared;
  const bool has_marker = has_nserror || has_status_code;
  const bool has_profile_bridge_sites =
      decl.ns_error_bridging_profile_is_normalized &&
      decl.ns_error_bridging_sites != 0u;
  const bool has_bridge_surface =
      has_marker || has_profile_bridge_sites;
  if (!has_bridge_surface) {
    return;
  }

  ++summary.bridge_callable_sites;
  if (has_nserror) {
    ++summary.objc_nserror_callable_sites;
  }
  if (has_status_code) {
    ++summary.objc_status_code_callable_sites;
  }

  const bool has_error_out = CallableHasNSErrorOutParameterForErrorHandling(decl);
  const bool valid_nserror_return =
      CallableReturnsSupportedNSErrorBoolForErrorHandling(decl);
  const bool valid_status_return =
      CallableReturnsSupportedStatusCodeForErrorHandling(decl);
  const bool throws_conflict = has_marker && decl.throws_declared;
  const bool marker_conflict = has_nserror && has_status_code;
  const bool valid_error_type =
      !has_status_code ||
      objc3c::support::LowercaseAscii(
          decl.objc_status_code_error_type_spelling) ==
          "nserror";
  const FunctionDecl *mapping =
      has_status_code
          ? FindFunctionDeclByName(program, decl.objc_status_code_mapping_symbol)
          : nullptr;
  const bool mapping_exists = !has_status_code || mapping != nullptr;
  const bool mapping_signature_valid =
      !has_status_code ||
      (mapping != nullptr && mapping->params.size() == 1u &&
       mapping->params.front().type == decl.return_type &&
       CallableReturnsNSErrorObjectForErrorHandling(*mapping));

  if (!allow_source_only_error_runtime_surface && has_marker) {
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S285",
        "unsupported feature claim: NSError/status bridge legality is not yet runnable in Objective-C 3 native mode",
        diagnostics);
    ++summary.unsupported_combination_sites;
  }
  if (has_nserror && !has_error_out) {
    ++summary.missing_error_out_parameter_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S275",
        "objc_nserror requires an NSError out parameter",
        diagnostics);
  }
  if (has_nserror && !valid_nserror_return) {
    ++summary.invalid_nserror_return_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S276",
        "objc_nserror currently requires a BOOL-like success return",
        diagnostics);
  }
  if (throws_conflict) {
    ++summary.throws_bridge_conflict_sites;
    ++summary.unsupported_combination_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S277",
        "NSError/status bridge markers cannot currently be combined with throws",
        diagnostics);
  }
  if (marker_conflict) {
    ++summary.marker_conflict_sites;
    ++summary.unsupported_combination_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S278",
        "objc_nserror and objc_status_code cannot appear on the same callable",
        diagnostics);
  }
  if (has_status_code && !has_error_out) {
    ++summary.missing_error_out_parameter_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S279",
        "objc_status_code requires an NSError out parameter",
        diagnostics);
  }
  if (has_status_code && !valid_error_type) {
    ++summary.invalid_error_type_sites;
    ++summary.unsupported_combination_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S280",
        "objc_status_code currently requires error_type: NSError",
        diagnostics);
  }
  if (has_status_code && !valid_status_return) {
    ++summary.invalid_status_return_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S281",
        "objc_status_code requires a BOOL-like or integer status return",
        diagnostics);
  }
  if (has_status_code && !mapping_exists) {
    ++summary.missing_mapping_symbol_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S282",
        "objc_status_code mapping symbol must resolve to a declared function",
        diagnostics);
  }
  if (has_status_code && mapping_exists && !mapping_signature_valid) {
    ++summary.invalid_mapping_signature_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S283",
        "objc_status_code mapping function must accept one matching status parameter and return NSError",
        diagnostics);
  }

  if (CallableHasSemanticallyValidNSErrorStatusBridge(decl, program)) {
    ++summary.semantically_valid_bridge_callable_sites;
    ++summary.try_eligible_bridge_callable_sites;
  }
}

Objc3ErrorHandlingErrorBridgeLegalitySummary BuildErrorHandlingErrorBridgeLegalitySummary(
    const Objc3Program &program,
    bool allow_source_only_error_runtime_surface,
    std::vector<std::string> &diagnostics) {
  Objc3ErrorHandlingErrorBridgeLegalitySummary summary;
  for (const auto &fn : program.functions) {
    AccumulateErrorHandlingErrorBridgeLegalityForCallable(
        fn, program, summary, allow_source_only_error_runtime_surface,
        diagnostics);
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      AccumulateErrorHandlingErrorBridgeLegalityForCallable(
          method, program, summary, allow_source_only_error_runtime_surface,
          diagnostics);
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      AccumulateErrorHandlingErrorBridgeLegalityForCallable(
          method, program, summary, allow_source_only_error_runtime_surface,
          diagnostics);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      AccumulateErrorHandlingErrorBridgeLegalityForCallable(
          method, program, summary, allow_source_only_error_runtime_surface,
          diagnostics);
    }
  }

  summary.bridge_legality_landed =
      summary.semantically_valid_bridge_callable_sites <=
          summary.bridge_callable_sites &&
      summary.objc_nserror_callable_sites <= summary.bridge_callable_sites &&
      summary.objc_status_code_callable_sites <= summary.bridge_callable_sites;
  summary.try_bridge_filter_landed =
      summary.try_eligible_bridge_callable_sites ==
      summary.semantically_valid_bridge_callable_sites;
  summary.unsupported_combinations_fail_closed =
      summary.unsupported_combination_sites <= summary.contract_violation_sites;
  summary.deterministic =
      summary.bridge_legality_landed && summary.try_bridge_filter_landed &&
      summary.unsupported_combinations_fail_closed &&
      summary.contract_violation_sites <=
          summary.bridge_callable_sites + summary.unsupported_combination_sites;
  summary.ready_for_lowering_and_runtime = false;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";bridges=" << summary.bridge_callable_sites
      << ";valid=" << summary.semantically_valid_bridge_callable_sites
      << ";unsupported=" << summary.unsupported_combination_sites
      << ";violations=" << summary.contract_violation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

static Objc3ErrorDiagnosticsRecoverySummary
BuildErrorDiagnosticsRecoverySummaryFromProgramAst(const Objc3Program &ast) {
  Objc3ErrorDiagnosticsRecoverySummary summary;

  const auto checked_add = [&](std::size_t &total, std::size_t value) {
    if (value > std::numeric_limits<std::size_t>::max() - total) {
      total = std::numeric_limits<std::size_t>::max();
      summary.deterministic = false;
      return;
    }
    total += value;
  };

  const auto is_partitioned = [](std::size_t lhs, std::size_t rhs,
                                 std::size_t total) {
    return lhs <= std::numeric_limits<std::size_t>::max() - rhs &&
           lhs + rhs == total;
  };

  const auto accumulate_error_diagnostics_recovery_profile =
      [&](const auto &declaration) {
        checked_add(summary.error_diagnostics_recovery_sites,
                    declaration.error_diagnostics_recovery_sites);
        checked_add(summary.diagnostic_emit_sites,
                    declaration.diagnostic_emit_sites);
        checked_add(summary.recovery_anchor_sites,
                    declaration.recovery_anchor_sites);
        checked_add(summary.recovery_boundary_sites,
                    declaration.recovery_boundary_sites);
        checked_add(summary.fail_closed_diagnostic_sites,
                    declaration.fail_closed_diagnostic_sites);
        checked_add(summary.normalized_sites,
                    declaration.error_diagnostics_recovery_normalized_sites);
        checked_add(summary.gate_blocked_sites,
                    declaration.error_diagnostics_recovery_gate_blocked_sites);
        checked_add(
            summary.contract_violation_sites,
            declaration.error_diagnostics_recovery_contract_violation_sites);

        const bool profile_contract_normalized =
            declaration.diagnostic_emit_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.recovery_anchor_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.recovery_boundary_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.fail_closed_diagnostic_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.error_diagnostics_recovery_normalized_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.error_diagnostics_recovery_gate_blocked_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.error_diagnostics_recovery_contract_violation_sites <=
                declaration.error_diagnostics_recovery_sites &&
            is_partitioned(
                declaration.error_diagnostics_recovery_normalized_sites,
                declaration.error_diagnostics_recovery_gate_blocked_sites,
                declaration.error_diagnostics_recovery_sites);
        summary.deterministic =
            summary.deterministic &&
            declaration.error_diagnostics_recovery_profile_is_normalized &&
            declaration.deterministic_error_diagnostics_recovery_handoff &&
            profile_contract_normalized;
      };

  for (const auto &function_decl : ast.functions) {
    accumulate_error_diagnostics_recovery_profile(function_decl);
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_error_diagnostics_recovery_profile(method_decl);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_error_diagnostics_recovery_profile(method_decl);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.diagnostic_emit_sites <= summary.error_diagnostics_recovery_sites &&
      summary.recovery_anchor_sites <= summary.error_diagnostics_recovery_sites &&
      summary.recovery_boundary_sites <=
          summary.error_diagnostics_recovery_sites &&
      summary.fail_closed_diagnostic_sites <=
          summary.error_diagnostics_recovery_sites &&
      summary.normalized_sites <= summary.error_diagnostics_recovery_sites &&
      summary.gate_blocked_sites <= summary.error_diagnostics_recovery_sites &&
      summary.contract_violation_sites <=
          summary.error_diagnostics_recovery_sites &&
      is_partitioned(summary.normalized_sites, summary.gate_blocked_sites,
                     summary.error_diagnostics_recovery_sites);
  return summary;
}

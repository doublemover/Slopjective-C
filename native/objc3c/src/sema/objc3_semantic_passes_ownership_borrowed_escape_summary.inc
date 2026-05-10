Objc3OwnershipBorrowedPointerEscapeAnalysisSummary
BuildOwnershipBorrowedPointerEscapeAnalysisSummary(
    const Objc3Program &program,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3OwnershipBorrowedPointerEscapeAnalysisSummary summary;
  OwnershipBorrowedEscapeSiteProfile profile;
  const auto callable_contracts =
      BuildOwnershipBorrowedCallableContracts(program);

  for (const auto &fn : program.functions) {
    for (const auto &param : fn.params) {
      if (param.borrowed_pointer_qualified) {
        ++summary.borrowed_parameter_sites;
      }
    }
    for (const auto &stmt : fn.body) {
      CollectOwnershipBorrowedEscapeStmtSites(stmt.get(), callable_contracts,
                                              profile);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      for (const auto &param : method.params) {
        if (param.borrowed_pointer_qualified) {
          ++summary.borrowed_parameter_sites;
        }
      }
      if (!method.has_body) {
        continue;
      }
      for (const auto &stmt : method.body) {
        CollectOwnershipBorrowedEscapeStmtSites(stmt.get(), callable_contracts,
                                                profile);
      }
    }
  }

  const auto count_diagnostic_code = [&diagnostics](std::string_view code) {
    return std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code](const std::string &entry) {
          return entry.find(code) != std::string::npos;
        });
  };

  summary.borrowed_return_callable_sites =
      profile.borrowed_return_callable_sites;
  summary.illegal_unproven_call_escape_sites =
      count_diagnostic_code("O3S298");
  summary.illegal_escaping_block_capture_sites =
      count_diagnostic_code("O3S299");
  summary.illegal_borrowed_return_sites = count_diagnostic_code("O3S300");
  summary.borrowed_escape_candidate_sites =
      summary.illegal_unproven_call_escape_sites +
      summary.illegal_escaping_block_capture_sites +
      summary.illegal_borrowed_return_sites;

  const bool dependency_surface_present =
      !dependency_summary.contract_id.empty() &&
      !dependency_summary.surface_path.empty();
  summary.dependency_required = true;
  summary.borrowed_call_boundary_enforced =
      dependency_surface_present && summary.borrowed_parameter_sites > 0u;
  summary.escaping_block_capture_fail_closed = dependency_surface_present;
  summary.borrowed_return_contract_enforced = dependency_surface_present;
  summary.retainable_family_legality_deferred = true;
  summary.lowering_runtime_deferred = true;
  summary.deterministic =
      dependency_surface_present && summary.borrowed_call_boundary_enforced &&
      summary.escaping_block_capture_fail_closed &&
      summary.borrowed_return_contract_enforced;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "borrowed pointer escape analysis must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";borrowed-params=" << summary.borrowed_parameter_sites
      << ";borrowed-return-callables="
      << summary.borrowed_return_callable_sites
      << ";illegal-sites=" << summary.illegal_unproven_call_escape_sites
      << ":" << summary.illegal_escaping_block_capture_sites << ":"
      << summary.illegal_borrowed_return_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

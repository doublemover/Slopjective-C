Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
BuildOwnershipResourceMoveUseAfterMoveSemanticsSummary(
    const Objc3Program &program,
    const Objc3OwnershipSystemExtensionSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary summary;
  OwnershipResourceMoveSiteProfile profile;
  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      CollectOwnershipResourceMoveStmtSites(stmt.get(), profile);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      if (!method.has_body) {
        continue;
      }
      for (const auto &stmt : method.body) {
        CollectOwnershipResourceMoveStmtSites(stmt.get(), profile);
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

  summary.cleanup_owned_local_sites = profile.cleanup_owned_local_sites;
  summary.resource_move_capture_sites = profile.resource_move_capture_sites;
  summary.illegal_non_resource_move_sites = count_diagnostic_code("O3S295");
  summary.illegal_use_after_move_sites = count_diagnostic_code("O3S296");
  summary.illegal_duplicate_move_sites = count_diagnostic_code("O3S297");

  const bool dependency_surface_present =
      !dependency_summary.contract_id.empty() &&
      !dependency_summary.surface_path.empty();
  summary.dependency_required = true;
  summary.cleanup_ownership_transfer_enforced =
      dependency_surface_present &&
      summary.resource_move_capture_sites >=
          summary.illegal_duplicate_move_sites;
  summary.use_after_move_fail_closed =
      summary.illegal_use_after_move_sites <=
      summary.cleanup_owned_local_sites + summary.resource_move_capture_sites;
  summary.duplicate_move_fail_closed =
      summary.illegal_duplicate_move_sites <= summary.resource_move_capture_sites;
  summary.borrowed_escape_semantics_deferred = true;
  summary.retainable_family_legality_deferred = true;
  summary.lowering_runtime_deferred = true;
  summary.deterministic =
      dependency_surface_present &&
      summary.cleanup_ownership_transfer_enforced &&
      summary.use_after_move_fail_closed && summary.duplicate_move_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "resource move and use-after-move semantics must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";cleanup-owned-locals=" << summary.cleanup_owned_local_sites
      << ";resource-move-captures=" << summary.resource_move_capture_sites
      << ";illegal-sites=" << summary.illegal_non_resource_move_sites << ":"
      << summary.illegal_use_after_move_sites << ":"
      << summary.illegal_duplicate_move_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

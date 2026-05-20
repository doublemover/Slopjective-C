Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
BuildConcurrencyActorIsolationSendabilityEnforcementSummary(
    const Objc3Program &ast,
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(), [&](const std::string &diag) {
          return diag.find(code) != std::string::npos;
        }));
  };

  summary.actor_interface_sites = dependency_summary.actor_interface_sites;
  summary.actor_method_sites = dependency_summary.actor_method_sites;
  summary.objc_nonisolated_annotation_sites =
      dependency_summary.objc_nonisolated_annotation_sites;
  summary.actor_member_executor_annotation_sites =
      dependency_summary.actor_member_executor_annotation_sites;
  summary.actor_async_method_sites = dependency_summary.actor_async_method_sites;

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method : interface_decl.methods) {
      if (method.objc_nonisolated_declared) {
        ++summary.total_nonisolated_method_sites;
      }
    }
  }
  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      if (method.objc_nonisolated_declared) {
        ++summary.total_nonisolated_method_sites;
      }
      const Objc3ConcurrencyActorMethodBodyProfile body_profile =
          BuildConcurrencyActorMethodBodyProfile(method);
      summary.actor_isolation_sites += body_profile.actor_isolation_sites;
      summary.actor_hop_sites += body_profile.actor_hop_sites;
      summary.non_sendable_crossing_sites +=
          body_profile.non_sendable_crossing_sites;
    }
  }

  summary.illegal_non_actor_nonisolated_sites =
      count_diagnostic_code("O3S286");
  summary.illegal_nonisolated_async_sites = count_diagnostic_code("O3S287");
  summary.illegal_nonisolated_executor_sites = count_diagnostic_code("O3S288");
  summary.illegal_actor_hop_without_async_sites =
      count_diagnostic_code("O3S289");
  summary.illegal_non_sendable_crossing_sites =
      count_diagnostic_code("O3S290");
  summary.illegal_non_actor_actor_hop_sites =
      count_diagnostic_code("O3S342");
  summary.illegal_non_actor_non_sendable_crossing_sites =
      count_diagnostic_code("O3S343");
  summary.illegal_non_actor_actor_isolation_sites =
      count_diagnostic_code("O3S344");
  summary.illegal_nonisolated_actor_isolation_sites =
      count_diagnostic_code("O3S345");

  summary.dependency_required = true;
  summary.non_actor_nonisolated_fail_closed =
      summary.illegal_non_actor_nonisolated_sites <=
      summary.total_nonisolated_method_sites;
  summary.nonisolated_combination_fail_closed =
      summary.illegal_nonisolated_async_sites <=
          summary.actor_async_method_sites &&
      summary.illegal_nonisolated_executor_sites <=
          summary.actor_member_executor_annotation_sites;
  summary.actor_hop_async_boundary_enforced =
      summary.illegal_actor_hop_without_async_sites <= summary.actor_hop_sites;
  summary.non_sendable_crossing_fail_closed =
      summary.illegal_non_sendable_crossing_sites <=
      summary.non_sendable_crossing_sites;
  summary.non_actor_actor_semantics_fail_closed =
      summary.illegal_non_actor_actor_hop_sites <= summary.actor_hop_sites &&
      summary.illegal_non_actor_non_sendable_crossing_sites <=
          summary.non_sendable_crossing_sites &&
      summary.illegal_non_actor_actor_isolation_sites <=
          summary.actor_isolation_sites;
  summary.nonisolated_actor_isolation_fail_closed =
      summary.illegal_nonisolated_actor_isolation_sites <=
      summary.total_nonisolated_method_sites;
  summary.runnable_lowering_deferred = true;
  summary.actor_runtime_deferred = true;
  summary.executor_runtime_deferred = true;
  summary.deterministic =
      dependency_summary.deterministic &&
      summary.non_actor_nonisolated_fail_closed &&
      summary.nonisolated_combination_fail_closed &&
      summary.actor_hop_async_boundary_enforced &&
      summary.non_sendable_crossing_fail_closed &&
      summary.non_actor_actor_semantics_fail_closed &&
      summary.nonisolated_actor_isolation_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "actor isolation/sendability enforcement diagnostics and dependency packet must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";actor-methods=" << summary.actor_method_sites
      << ";actor-isolation=" << summary.actor_isolation_sites
      << ";nonisolated-sites=" << summary.total_nonisolated_method_sites
      << ";illegal-sites=" << summary.illegal_non_actor_nonisolated_sites << ":"
      << summary.illegal_nonisolated_async_sites << ":"
      << summary.illegal_nonisolated_executor_sites << ":"
      << summary.illegal_actor_hop_without_async_sites << ":"
      << summary.illegal_non_sendable_crossing_sites << ":"
      << summary.illegal_non_actor_actor_hop_sites << ":"
      << summary.illegal_non_actor_non_sendable_crossing_sites << ":"
      << summary.illegal_non_actor_actor_isolation_sites << ":"
      << summary.illegal_nonisolated_actor_isolation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary
BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummary(
    const Objc3Program &ast,
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(), [&](const std::string &diag) {
          return diag.find(code) != std::string::npos;
        }));
  };

  summary.actor_method_sites = dependency_summary.actor_method_sites;
  for (const auto &implementation_decl : ast.implementations) {
    const bool owner_is_actor = std::any_of(
        ast.interfaces.begin(), ast.interfaces.end(),
        [&](const Objc3InterfaceDecl &interface_decl) {
          return interface_decl.is_actor &&
                 interface_decl.name == implementation_decl.name;
        });
    if (!owner_is_actor) {
      continue;
    }
    for (const auto &method : implementation_decl.methods) {
      const Objc3ConcurrencyActorMethodBodyProfile body_profile =
          BuildConcurrencyActorMethodBodyProfile(method);
      summary.replay_proof_sites += body_profile.replay_proof_sites;
      summary.race_guard_sites += body_profile.race_guard_sites;
      summary.task_handoff_sites += body_profile.task_handoff_sites;
      summary.actor_isolation_sites += body_profile.actor_isolation_sites;
      summary.escaping_block_literal_sites +=
          body_profile.escaping_block_literal_sites;
    }
  }

  summary.illegal_missing_race_guard_sites = count_diagnostic_code("O3S291");
  summary.illegal_missing_replay_proof_sites = count_diagnostic_code("O3S292");
  summary.illegal_missing_actor_isolation_sites =
      count_diagnostic_code("O3S293");
  summary.illegal_escaping_block_literal_sites =
      count_diagnostic_code("O3S294");

  summary.dependency_required = true;
  summary.race_guard_fail_closed =
      summary.illegal_missing_race_guard_sites <= summary.task_handoff_sites;
  summary.replay_proof_fail_closed =
      summary.illegal_missing_replay_proof_sites <= summary.task_handoff_sites;
  summary.actor_isolation_boundary_fail_closed =
      summary.illegal_missing_actor_isolation_sites <=
      summary.task_handoff_sites;
  summary.escaping_block_fail_closed =
      summary.illegal_escaping_block_literal_sites <=
      summary.escaping_block_literal_sites;
  summary.runnable_lowering_deferred = true;
  summary.actor_runtime_deferred = true;
  summary.deterministic =
      dependency_summary.deterministic && summary.race_guard_fail_closed &&
      summary.replay_proof_fail_closed &&
      summary.actor_isolation_boundary_fail_closed &&
      summary.escaping_block_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "actor race-hazard and escape diagnostics must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";task-handoff=" << summary.task_handoff_sites
      << ";race-guard=" << summary.race_guard_sites
      << ";replay-proof=" << summary.replay_proof_sites
      << ";actor-isolation=" << summary.actor_isolation_sites
      << ";escaping-blocks=" << summary.escaping_block_literal_sites
      << ";illegal-sites=" << summary.illegal_missing_race_guard_sites << ":"
      << summary.illegal_missing_replay_proof_sites << ":"
      << summary.illegal_missing_actor_isolation_sites << ":"
      << summary.illegal_escaping_block_literal_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

  summary.macro_execution_deferred = true;
  summary.property_behavior_runtime_deferred = true;
  summary.deterministic =
      summary.derive_macro_source_supported &&
      summary.macro_package_provenance_surface_reused &&
      summary.property_behavior_source_supported &&
      summary.synthesized_visibility_surface_reused;
  summary.ready_for_core_implementation = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "metaprogramming metaprogramming source and property-behavior semantic surfaces must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";source=" << source_summary.contract_id
      << ";macro-source=" << macro_summary.contract_id
      << ";property-source=" << property_summary.contract_id
      << ";derive-markers=" << summary.derive_marker_sites
      << ";macro-sites=" << summary.macro_marker_sites << ":"
      << summary.macro_package_sites << ":" << summary.macro_provenance_sites
      << ":" << summary.expansion_visible_macro_sites
      << ";property-sites=" << summary.property_behavior_sites << ":"
      << summary.interface_property_behavior_sites << ":"
      << summary.implementation_property_behavior_sites << ":"
      << summary.protocol_property_behavior_sites
      << ";synthesized-visible=" << summary.synthesized_binding_visible_sites
      << ":" << summary.synthesized_getter_visible_sites << ":"
      << summary.synthesized_setter_visible_sites
      << ";property-behavior-contract-violations="
      << summary.property_behavior_contract_violation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

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
        [code](const std::string &entry) { return entry.find(code) != std::string::npos; });
  };

  summary.cleanup_owned_local_sites = profile.cleanup_owned_local_sites;
  summary.resource_move_capture_sites = profile.resource_move_capture_sites;
  summary.illegal_non_resource_move_sites =
      count_diagnostic_code("O3S295");
  summary.illegal_use_after_move_sites =
      count_diagnostic_code("O3S296");
  summary.illegal_duplicate_move_sites =
      count_diagnostic_code("O3S297");

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
      summary.illegal_duplicate_move_sites <=
      summary.resource_move_capture_sites;
  summary.borrowed_escape_semantics_deferred = true;
  summary.retainable_family_legality_deferred = true;
  summary.lowering_runtime_deferred = true;
  summary.deterministic =
      dependency_surface_present &&
      summary.cleanup_ownership_transfer_enforced &&
      summary.use_after_move_fail_closed &&
      summary.duplicate_move_fail_closed;
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

Objc3OwnershipBorrowedPointerEscapeAnalysisSummary
BuildOwnershipBorrowedPointerEscapeAnalysisSummary(
    const Objc3Program &program,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3OwnershipBorrowedPointerEscapeAnalysisSummary summary;
  OwnershipBorrowedEscapeSiteProfile profile;
  const auto callable_contracts = BuildOwnershipBorrowedCallableContracts(program);

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
        [code](const std::string &entry) { return entry.find(code) != std::string::npos; });
  };

  summary.borrowed_return_callable_sites = profile.borrowed_return_callable_sites;
  summary.illegal_unproven_call_escape_sites =
      count_diagnostic_code("O3S298");
  summary.illegal_escaping_block_capture_sites =
      count_diagnostic_code("O3S299");
  summary.illegal_borrowed_return_sites =
      count_diagnostic_code("O3S300");
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
      dependency_surface_present &&
      summary.borrowed_call_boundary_enforced &&
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
      << ";illegal-sites=" << summary.illegal_unproven_call_escape_sites << ":"
      << summary.illegal_escaping_block_capture_sites << ":"
      << summary.illegal_borrowed_return_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummary(
    const Objc3Program &program,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary summary;
  OwnershipCaptureListRetainableFamilySiteProfile profile;

  for (const auto &fn : program.functions) {
    CollectOwnershipRetainableFamilyCallableSites(fn, profile);
    for (const auto &stmt : fn.body) {
      CollectOwnershipCaptureListRetainableFamilyStmtSites(stmt.get(), profile);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      CollectOwnershipRetainableFamilyCallableSites(method, profile);
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      CollectOwnershipRetainableFamilyCallableSites(method, profile);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      CollectOwnershipRetainableFamilyCallableSites(method, profile);
      if (!method.has_body) {
        continue;
      }
      for (const auto &stmt : method.body) {
        CollectOwnershipCaptureListRetainableFamilyStmtSites(stmt.get(), profile);
      }
    }
  }

  const auto count_diagnostic_code = [&diagnostics](std::string_view code) {
    return std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code](const std::string &entry) { return entry.find(code) != std::string::npos; });
  };

  summary.explicit_capture_list_sites = profile.explicit_capture_list_sites;
  summary.explicit_capture_item_sites = profile.explicit_capture_item_sites;
  summary.explicit_capture_ownership_mode_sites =
      profile.explicit_capture_ownership_mode_sites;
  summary.retainable_family_callable_sites =
      profile.retainable_family_callable_sites;
  summary.retainable_family_operation_callable_sites =
      profile.retainable_family_operation_callable_sites;
  summary.retainable_family_alias_callable_sites =
      profile.retainable_family_alias_callable_sites;
  summary.illegal_duplicate_explicit_capture_sites =
      count_diagnostic_code("O3S301");
  summary.illegal_non_object_capture_mode_sites =
      count_diagnostic_code("O3S302");
  summary.illegal_unused_explicit_capture_sites =
      count_diagnostic_code("O3S303");
  summary.illegal_conflicting_retainable_family_sites =
      count_diagnostic_code("O3S304");
  summary.illegal_invalid_family_operation_shape_sites =
      count_diagnostic_code("O3S305");
  summary.illegal_invalid_family_alias_shape_sites =
      count_diagnostic_code("O3S306");

  const bool dependency_surface_present =
      !dependency_summary.contract_id.empty() &&
      !dependency_summary.surface_path.empty();
  summary.dependency_required = true;
  summary.explicit_capture_duplicate_fail_closed =
      dependency_surface_present &&
      summary.illegal_duplicate_explicit_capture_sites <=
          summary.explicit_capture_item_sites;
  summary.explicit_capture_ownership_mode_enforced =
      dependency_surface_present &&
      summary.illegal_non_object_capture_mode_sites <=
          summary.explicit_capture_ownership_mode_sites;
  summary.explicit_capture_inventory_enforced =
      dependency_surface_present &&
      summary.illegal_unused_explicit_capture_sites <=
          summary.explicit_capture_item_sites;
  summary.retainable_family_conflict_enforced =
      dependency_surface_present &&
      summary.illegal_conflicting_retainable_family_sites <=
          summary.retainable_family_callable_sites;
  summary.retainable_family_operation_shape_enforced =
      dependency_surface_present &&
      summary.illegal_invalid_family_operation_shape_sites <=
          summary.retainable_family_operation_callable_sites;
  summary.retainable_family_alias_shape_enforced =
      dependency_surface_present &&
      summary.illegal_invalid_family_alias_shape_sites <=
          summary.retainable_family_alias_callable_sites;
  summary.lowering_runtime_deferred = true;
  summary.deterministic =
      dependency_surface_present &&
      summary.explicit_capture_duplicate_fail_closed &&
      summary.explicit_capture_ownership_mode_enforced &&
      summary.explicit_capture_inventory_enforced &&
      summary.retainable_family_conflict_enforced &&
      summary.retainable_family_operation_shape_enforced &&
      summary.retainable_family_alias_shape_enforced;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "capture-list and retainable-family legality must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";captures=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites << ":"
      << summary.explicit_capture_ownership_mode_sites
      << ";retainable-callables=" << summary.retainable_family_callable_sites
      << ":" << summary.retainable_family_operation_callable_sites << ":"
      << summary.retainable_family_alias_callable_sites
      << ";illegal-sites=" << summary.illegal_duplicate_explicit_capture_sites
      << ":" << summary.illegal_non_object_capture_mode_sites << ":"
      << summary.illegal_unused_explicit_capture_sites << ":"
      << summary.illegal_conflicting_retainable_family_sites << ":"
      << summary.illegal_invalid_family_operation_shape_sites << ":"
      << summary.illegal_invalid_family_alias_shape_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
BuildConcurrencyStructuredTaskCancellationSemanticSummary(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyStructuredTaskCancellationSemanticSummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(), [&](const std::string &diag) {
          return diag.find(code) != std::string::npos;
        }));
  };

  const std::size_t task_related_sites =
      dependency_summary.task_creation_sites +
      dependency_summary.task_group_scope_sites +
      dependency_summary.task_group_add_task_sites +
      dependency_summary.task_group_wait_next_sites +
      dependency_summary.task_group_cancel_all_sites +
      dependency_summary.cancellation_check_sites +
      dependency_summary.cancellation_handler_sites;

  summary.async_callable_sites = dependency_summary.async_callable_sites;
  summary.task_creation_sites = dependency_summary.task_creation_sites;
  summary.task_group_scope_sites = dependency_summary.task_group_scope_sites;
  summary.task_group_add_task_sites =
      dependency_summary.task_group_add_task_sites;
  summary.task_group_wait_next_sites =
      dependency_summary.task_group_wait_next_sites;
  summary.task_group_cancel_all_sites =
      dependency_summary.task_group_cancel_all_sites;
  summary.cancellation_check_sites =
      dependency_summary.cancellation_check_sites;
  summary.cancellation_handler_sites =
      dependency_summary.cancellation_handler_sites;
  summary.illegal_non_async_task_sites = count_diagnostic_code("O3S227");
  summary.illegal_task_group_scope_sites = count_diagnostic_code("O3S228");
  summary.illegal_task_hierarchy_sites = count_diagnostic_code("O3S229");
  summary.illegal_cancellation_usage_sites =
      count_diagnostic_code("O3S230");

  summary.source_dependency_required = true;
  summary.async_task_boundary_enforced =
      dependency_summary.ready_for_lowering_and_runtime &&
      summary.illegal_non_async_task_sites <= task_related_sites;
  summary.structured_task_scope_enforced =
      dependency_summary.structured_task_legality_semantics_landed &&
      summary.illegal_task_group_scope_sites <=
          summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites;
  summary.task_hierarchy_enforced =
      dependency_summary.task_lifetime_semantics_landed &&
      summary.illegal_task_hierarchy_sites <= summary.task_creation_sites;
  summary.cancellation_usage_enforced =
      dependency_summary.cancellation_observation_semantics_landed &&
      summary.illegal_cancellation_usage_sites <=
          summary.cancellation_handler_sites +
              summary.task_group_cancel_all_sites;
  summary.runnable_lowering_deferred = true;
  summary.executor_runtime_deferred = true;
  summary.scheduler_runtime_deferred = true;
  summary.deterministic =
      dependency_summary.deterministic && summary.async_task_boundary_enforced &&
      summary.structured_task_scope_enforced &&
      summary.task_hierarchy_enforced &&
      summary.cancellation_usage_enforced;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";task-sites=" << summary.task_creation_sites << ":"
      << summary.task_group_scope_sites << ":"
      << summary.task_group_add_task_sites << ":"
      << summary.task_group_wait_next_sites << ":"
      << summary.task_group_cancel_all_sites
      << ";cancellation-sites=" << summary.cancellation_check_sites << ":"
      << summary.cancellation_handler_sites
      << ";illegal-sites=" << summary.illegal_non_async_task_sites << ":"
      << summary.illegal_task_group_scope_sites << ":"
      << summary.illegal_task_hierarchy_sites << ":"
      << summary.illegal_cancellation_usage_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
BuildConcurrencyExecutorHopAffinityCompatibilitySummary(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &dependency_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(), [&](const std::string &diag) {
          return diag.find(code) != std::string::npos;
        }));
  };

  const std::size_t executor_affinity_sites =
      source_summary.executor_main_sites + source_summary.executor_global_sites +
      source_summary.executor_named_sites;

  summary.async_callable_sites = source_summary.async_function_sites +
                                 source_summary.async_method_sites;
  summary.executor_affinity_sites = executor_affinity_sites;
  summary.executor_main_sites = source_summary.executor_main_sites;
  summary.executor_global_sites = source_summary.executor_global_sites;
  summary.executor_named_sites = source_summary.executor_named_sites;
  summary.task_creation_sites = dependency_summary.task_creation_sites;
  summary.detached_task_creation_sites =
      dependency_summary.task_creation_sites > 0u &&
              source_summary.executor_named_sites > 0u
          ? 1u
          : 0u;
  summary.illegal_missing_executor_affinity_sites =
      count_diagnostic_code("O3S231");
  summary.illegal_main_executor_detached_sites =
      count_diagnostic_code("O3S232");

  summary.dependency_required = true;
  summary.executor_affinity_required_for_task_callables_enforced =
      summary.illegal_missing_executor_affinity_sites <=
          dependency_summary.async_callable_sites;
  summary.detached_task_hop_boundary_enforced =
      summary.illegal_main_executor_detached_sites <=
          summary.executor_main_sites + summary.executor_affinity_sites;
  summary.runnable_lowering_deferred = true;
  summary.executor_runtime_deferred = true;
  summary.scheduler_runtime_deferred = true;
  summary.deterministic =
      summary.executor_affinity_required_for_task_callables_enforced &&
      summary.detached_task_hop_boundary_enforced;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";async-callables=" << summary.async_callable_sites
      << ";executor-sites=" << summary.executor_affinity_sites << ":"
      << summary.executor_main_sites << ":" << summary.executor_global_sites
      << ":" << summary.executor_named_sites
      << ";task-creation-sites=" << summary.task_creation_sites
      << ";illegal-affinity=" << summary.illegal_missing_executor_affinity_sites
      << ";illegal-main-detached="
      << summary.illegal_main_executor_detached_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary
BuildConcurrencyAwaitSuspensionResumeSemanticSummary(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [&](const std::string &diag) { return diag.find(code) != std::string::npos; }));
  };

  summary.async_callable_sites =
      dependency_summary.async_function_sites +
      dependency_summary.async_method_sites;
  summary.await_expression_sites = dependency_summary.await_expression_sites;
  summary.illegal_await_sites = count_diagnostic_code("O3S223");
  summary.await_in_async_callable_sites =
      summary.await_expression_sites >= summary.illegal_await_sites
          ? summary.await_expression_sites - summary.illegal_await_sites
          : 0u;
  summary.await_suspension_point_sites =
      dependency_summary.await_suspension_point_sites;
  summary.await_resume_sites = dependency_summary.await_resume_sites;
  summary.continuation_resume_sites =
      dependency_summary.continuation_resume_sites;
  summary.continuation_suspend_sites =
      dependency_summary.continuation_suspend_sites;

  summary.source_dependency_required = true;
  summary.await_placement_enforced =
      dependency_summary.async_declaration_semantics_landed &&
      dependency_summary.await_legality_semantics_landed &&
      summary.illegal_await_sites <= summary.await_expression_sites &&
      summary.await_in_async_callable_sites + summary.illegal_await_sites ==
          summary.await_expression_sites;
  summary.suspension_profile_enforced =
      dependency_summary.await_suspension_profile_semantics_landed &&
      summary.await_suspension_point_sites <= summary.await_expression_sites;
  summary.resume_profile_enforced =
      dependency_summary.continuation_profile_semantics_landed &&
      summary.continuation_resume_sites <=
          dependency_summary.async_continuation_sites &&
      summary.continuation_suspend_sites <=
          dependency_summary.async_continuation_sites &&
      summary.await_resume_sites <= dependency_summary.await_suspension_sites;
  summary.non_async_await_fail_closed = true;
  summary.deterministic =
      dependency_summary.deterministic && summary.await_placement_enforced &&
      summary.suspension_profile_enforced &&
      summary.resume_profile_enforced;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";async-callables=" << summary.async_callable_sites
      << ";await-expressions=" << summary.await_expression_sites
      << ";await-in-async=" << summary.await_in_async_callable_sites
      << ";illegal-await=" << summary.illegal_await_sites
      << ";suspension-sites=" << summary.await_suspension_point_sites << ":"
      << summary.await_resume_sites
      << ";continuation-sites=" << summary.continuation_resume_sites << ":"
      << summary.continuation_suspend_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary
BuildConcurrencyAsyncDiagnosticsCompatibilitySummary(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &dependency_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const Objc3Program &ast,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [&](const std::string &diag) { return diag.find(code) != std::string::npos; }));
  };

  const auto count_async_function_prototypes = [&]() {
    return static_cast<std::size_t>(std::count_if(
        ast.functions.begin(), ast.functions.end(),
        [](const FunctionDecl &fn) { return fn.async_declared && fn.is_prototype; }));
  };

  summary.async_callable_sites = dependency_summary.async_callable_sites;
  summary.executor_affinity_sites = source_summary.executor_attribute_sites;
  summary.illegal_non_async_executor_sites = count_diagnostic_code("O3S224");
  summary.illegal_async_function_prototype_sites =
      count_diagnostic_code("O3S225");
  summary.illegal_async_throws_sites = count_diagnostic_code("O3S226");
  summary.compatibility_diagnostic_sites =
      summary.illegal_non_async_executor_sites +
      summary.illegal_async_function_prototype_sites +
      summary.illegal_async_throws_sites;
  summary.supported_async_callable_sites =
      summary.async_callable_sites >=
              summary.illegal_async_function_prototype_sites +
                  summary.illegal_async_throws_sites
          ? summary.async_callable_sites -
                (summary.illegal_async_function_prototype_sites +
                 summary.illegal_async_throws_sites)
          : 0u;
  summary.dependency_required = true;
  summary.executor_affinity_requires_async_enforced =
      summary.illegal_non_async_executor_sites <=
      source_summary.executor_attribute_sites;
  summary.async_function_prototypes_fail_closed =
      summary.illegal_async_function_prototype_sites ==
      count_async_function_prototypes();
  summary.async_throws_fail_closed =
      summary.illegal_async_throws_sites <= summary.async_callable_sites;
  summary.unsupported_topology_fail_closed =
      dependency_summary.ready_for_lowering_and_runtime &&
      summary.executor_affinity_requires_async_enforced &&
      summary.async_function_prototypes_fail_closed &&
      summary.async_throws_fail_closed;
  summary.deterministic =
      dependency_summary.deterministic &&
      source_summary.deterministic_handoff &&
      summary.unsupported_topology_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";async-callables=" << summary.async_callable_sites
      << ";executor-sites=" << summary.executor_affinity_sites
      << ";illegal-executor=" << summary.illegal_non_async_executor_sites
      << ";illegal-prototypes=" << summary.illegal_async_function_prototype_sites
      << ";illegal-throws=" << summary.illegal_async_throws_sites
      << ";supported-async=" << summary.supported_async_callable_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

namespace {

struct Objc3ErrorHandlingSemanticWalkContext {
  bool in_throws_callable = false;
  bool local_handler_active = false;
  bool in_catch_body = false;
};

static void RecordErrorHandlingSemanticDiagnostic(
    std::size_t &contract_violation_sites,
    unsigned line,
    unsigned column,
    const char *code,
    const std::string &message,
    std::vector<std::string> &diagnostics) {
  ++contract_violation_sites;
  diagnostics.push_back(MakeDiag(line, column, code, message));
}

static const FunctionDecl *FindFunctionDeclByName(const Objc3Program &program,
                                                  const std::string &name) {
  for (const auto &fn : program.functions) {
    if (fn.name == name) {
      return &fn;
    }
  }
  return nullptr;
}

static bool IsNSErrorTypeSpellingForErrorHandling(const FuncParam &param) {
  if (!param.object_pointer_type_spelling) {
    return false;
  }
  return objc3c::support::LowercaseAscii(param.object_pointer_type_name) ==
         "nserror";
}

static bool IsNSErrorOutParameterSiteForErrorHandling(const FuncParam &param) {
  if (!IsNSErrorTypeSpellingForErrorHandling(param)) {
    return false;
  }
  const std::string lowered_name = objc3c::support::LowercaseAscii(param.name);
  return param.has_pointer_declarator ||
         lowered_name.find("error") != std::string::npos;
}

template <typename CallableDecl>
static bool CallableHasNSErrorOutParameterForErrorHandling(const CallableDecl &decl) {
  for (const auto &param : decl.params) {
    if (IsNSErrorOutParameterSiteForErrorHandling(param)) {
      return true;
    }
  }
  return false;
}

template <typename CallableDecl>
static bool CallableReturnsNSErrorObjectForErrorHandling(const CallableDecl &decl) {
  return decl.return_object_pointer_type_spelling &&
         objc3c::support::LowercaseAscii(decl.return_object_pointer_type_name) ==
             "nserror";
}

template <typename CallableDecl>
static bool CallableReturnsSupportedNSErrorBoolForErrorHandling(
    const CallableDecl &decl) {
  return decl.return_type == ValueType::Bool;
}

template <typename CallableDecl>
static bool CallableReturnsSupportedStatusCodeForErrorHandling(
    const CallableDecl &decl) {
  return decl.return_type == ValueType::Bool ||
         decl.return_type == ValueType::I32;
}

template <typename CallableDecl>
static bool CallableHasSemanticallyValidNSErrorStatusBridge(
    const CallableDecl &decl, const Objc3Program &program) {
  const bool has_marker =
      decl.objc_nserror_attribute_sites != 0u ||
      decl.objc_status_code_attribute_sites != 0u;
  const bool has_profile_bridge_sites =
      decl.ns_error_bridging_profile_is_normalized &&
      decl.ns_error_bridging_sites != 0u;
  if (!has_marker) {
    return has_profile_bridge_sites;
  }
  if (decl.throws_declared ||
      (decl.objc_nserror_declared && decl.objc_status_code_declared)) {
    return false;
  }
  if (decl.objc_nserror_declared) {
    return CallableHasNSErrorOutParameterForErrorHandling(decl) &&
           CallableReturnsSupportedNSErrorBoolForErrorHandling(decl);
  }
  if (decl.objc_status_code_declared) {
    if (!CallableHasNSErrorOutParameterForErrorHandling(decl) ||
        !CallableReturnsSupportedStatusCodeForErrorHandling(decl) ||
        objc3c::support::LowercaseAscii(
            decl.objc_status_code_error_type_spelling) !=
            "nserror") {
      return false;
    }
    const FunctionDecl *mapping =
        FindFunctionDeclByName(program, decl.objc_status_code_mapping_symbol);
    return mapping != nullptr && mapping->params.size() == 1u &&
           mapping->params.front().type == decl.return_type &&
           CallableReturnsNSErrorObjectForErrorHandling(*mapping);
  }
  return has_profile_bridge_sites;
}

static void ResolveMessageSendErrorSurface(const Objc3Program &program,
                                           const std::string &selector,
                                           bool &throwing,
                                           bool &bridged) {
  const auto inspect_method = [&](const Objc3MethodDecl &method) {
    if (method.selector != selector) {
      return;
    }
    throwing = throwing || method.throws_declared;
    bridged = bridged ||
              CallableHasSemanticallyValidNSErrorStatusBridge(method, program);
  };
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      inspect_method(method);
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      inspect_method(method);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      inspect_method(method);
    }
  }
}

static void ResolveTryOperandSurface(const Expr *operand,
                                     const Objc3Program &program,
                                     bool &throwing_callable,
                                     bool &bridged_callable) {
  if (operand == nullptr) {
    return;
  }
  if (operand->kind == Expr::Kind::Call) {
    if (const FunctionDecl *fn = FindFunctionDeclByName(program, operand->ident)) {
      throwing_callable = fn->throws_declared;
      bridged_callable =
          CallableHasSemanticallyValidNSErrorStatusBridge(*fn, program);
    }
    return;
  }
  if (operand->kind == Expr::Kind::MessageSend) {
    ResolveMessageSendErrorSurface(program, operand->selector, throwing_callable,
                                   bridged_callable);
  }
}

static void WalkErrorHandlingTryDoCatchExpr(
    const Expr *expr,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context);

static void WalkErrorHandlingTryDoCatchStmt(
    const Stmt *stmt,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    WalkErrorHandlingTryDoCatchExpr(stmt->let_stmt != nullptr ? stmt->let_stmt->value.get() : nullptr,
                            program, summary, diagnostics,
                            allow_source_only_error_runtime_surface, context);
    return;
  case Stmt::Kind::Assign:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->assign_stmt != nullptr ? stmt->assign_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::Return:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->return_stmt != nullptr ? stmt->return_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(stmt->if_stmt->condition.get(), program, summary,
                            diagnostics, allow_source_only_error_runtime_surface,
                            context);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      WalkErrorHandlingTryDoCatchStmt(then_stmt.get(), program, summary, diagnostics,
                              allow_source_only_error_runtime_surface, context);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      WalkErrorHandlingTryDoCatchStmt(else_stmt.get(), program, summary, diagnostics,
                              allow_source_only_error_runtime_surface, context);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary, diagnostics,
                              allow_source_only_error_runtime_surface, context);
    }
    WalkErrorHandlingTryDoCatchExpr(stmt->do_while_stmt->condition.get(), program,
                            summary, diagnostics,
                            allow_source_only_error_runtime_surface, context);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(stmt->for_stmt->init.value.get(), program, summary,
                            diagnostics, allow_source_only_error_runtime_surface,
                            context);
    WalkErrorHandlingTryDoCatchExpr(stmt->for_stmt->condition.get(), program, summary,
                            diagnostics, allow_source_only_error_runtime_surface,
                            context);
    WalkErrorHandlingTryDoCatchExpr(stmt->for_stmt->step.value.get(), program, summary,
                            diagnostics, allow_source_only_error_runtime_surface,
                            context);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary, diagnostics,
                              allow_source_only_error_runtime_surface, context);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(stmt->switch_stmt->condition.get(), program, summary,
                            diagnostics, allow_source_only_error_runtime_surface,
                            context);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        WalkErrorHandlingTryDoCatchStmt(case_stmt.get(), program, summary, diagnostics,
                                allow_source_only_error_runtime_surface, context);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(stmt->while_stmt->condition.get(), program, summary,
                            diagnostics, allow_source_only_error_runtime_surface,
                            context);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary, diagnostics,
                              allow_source_only_error_runtime_surface, context);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    if (stmt->block_stmt->is_do_catch_scope) {
      ++summary.do_catch_sites;
      summary.catch_clause_sites += stmt->block_stmt->catch_clauses.size();
      for (const auto &clause : stmt->block_stmt->catch_clauses) {
        if (clause.catch_all) {
          ++summary.catch_all_sites;
        }
        if (clause.has_binding) {
          ++summary.catch_binding_sites;
        }
      }
      if (!allow_source_only_error_runtime_surface) {
        RecordErrorHandlingSemanticDiagnostic(
            summary.contract_violation_sites, stmt->line, stmt->column,
            "O3S267",
            "unsupported feature claim: do/catch statements are not yet runnable in Objective-C 3 native mode",
            diagnostics);
      }
      if (stmt->block_stmt->catch_clauses.empty()) {
        RecordErrorHandlingSemanticDiagnostic(
            summary.contract_violation_sites, stmt->line, stmt->column,
            "O3S268", "do/catch requires at least one catch clause",
            diagnostics);
      }
      bool saw_catch_all = false;
      for (std::size_t index = 0; index < stmt->block_stmt->catch_clauses.size();
           ++index) {
        const auto &clause = stmt->block_stmt->catch_clauses[index];
        if (saw_catch_all) {
          RecordErrorHandlingSemanticDiagnostic(
              summary.contract_violation_sites, clause.line, clause.column,
              "O3S269",
              "catch clauses after a catch-all are unreachable",
              diagnostics);
        }
        if (clause.catch_all) {
          saw_catch_all = true;
        }
      }

      Objc3ErrorHandlingSemanticWalkContext body_context = context;
      body_context.local_handler_active = true;
      for (const auto &body_stmt : stmt->block_stmt->body) {
        WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary, diagnostics,
                                allow_source_only_error_runtime_surface,
                                body_context);
      }
      for (const auto &clause : stmt->block_stmt->catch_clauses) {
        Objc3ErrorHandlingSemanticWalkContext catch_context = context;
        catch_context.local_handler_active = false;
        catch_context.in_catch_body = true;
        for (const auto &body_stmt : clause.body) {
          WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary, diagnostics,
                                  allow_source_only_error_runtime_surface,
                                  catch_context);
        }
      }
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary, diagnostics,
                              allow_source_only_error_runtime_surface, context);
    }
    return;
  case Stmt::Kind::Expr:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->expr_stmt != nullptr ? stmt->expr_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static void WalkErrorHandlingTryDoCatchExpr(
    const Expr *expr,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  if (expr == nullptr) {
    return;
  }

  if (expr->try_expression_enabled) {
    ++summary.try_expression_sites;
    switch (expr->try_operator_kind) {
    case Expr::TryOperatorKind::Propagate:
      ++summary.try_propagating_sites;
      break;
    case Expr::TryOperatorKind::Optional:
      ++summary.try_optional_sites;
      break;
    case Expr::TryOperatorKind::Forced:
      ++summary.try_forced_sites;
      break;
    case Expr::TryOperatorKind::None:
      break;
    }

    if (!allow_source_only_error_runtime_surface) {
      RecordErrorHandlingSemanticDiagnostic(
          summary.contract_violation_sites, expr->line, expr->column, "O3S270",
          "unsupported feature claim: try expressions are not yet runnable in Objective-C 3 native mode",
          diagnostics);
    }

    const Expr *operand =
        !expr->args.empty() ? expr->args.front().get() : expr->left.get();
    bool throwing_callable = false;
    bool bridged_callable = false;
    ResolveTryOperandSurface(operand, program, throwing_callable,
                             bridged_callable);
    if (throwing_callable) {
      ++summary.throwing_callable_try_sites;
    }
    if (bridged_callable) {
      ++summary.bridged_callable_try_sites;
    }
    if (!throwing_callable && !bridged_callable) {
      RecordErrorHandlingSemanticDiagnostic(
          summary.contract_violation_sites, expr->line, expr->column, "O3S271",
          "try operand must be a throwing or NSError-bridged call surface",
          diagnostics);
    }
    if (expr->try_expression_requires_throwing_context) {
      if (context.local_handler_active) {
        ++summary.local_handler_sites;
      } else if (context.in_throws_callable) {
        ++summary.caller_propagation_sites;
      } else {
        RecordErrorHandlingSemanticDiagnostic(
            summary.contract_violation_sites, expr->line, expr->column,
            "O3S272",
            "propagating try requires a throws function or an enclosing do/catch",
            diagnostics);
      }
    }
  }

  if (expr->throw_statement_enabled) {
    ++summary.throw_statement_sites;
    if (context.in_catch_body) {
      ++summary.rethrow_sites;
    }
    if (!allow_source_only_error_runtime_surface) {
      RecordErrorHandlingSemanticDiagnostic(
          summary.contract_violation_sites, expr->line, expr->column, "O3S273",
          "unsupported feature claim: throw statements are not yet runnable in Objective-C 3 native mode",
          diagnostics);
    }
    if (!(context.in_throws_callable || context.in_catch_body)) {
      RecordErrorHandlingSemanticDiagnostic(
          summary.contract_violation_sites, expr->line, expr->column,
          "O3S274",
          "throw statements require a throws function or a catch body",
          diagnostics);
    }
  }

  WalkErrorHandlingTryDoCatchExpr(expr->receiver.get(), program, summary, diagnostics,
                          allow_source_only_error_runtime_surface, context);
  WalkErrorHandlingTryDoCatchExpr(expr->left.get(), program, summary, diagnostics,
                          allow_source_only_error_runtime_surface, context);
  WalkErrorHandlingTryDoCatchExpr(expr->right.get(), program, summary, diagnostics,
                          allow_source_only_error_runtime_surface, context);
  WalkErrorHandlingTryDoCatchExpr(expr->third.get(), program, summary, diagnostics,
                          allow_source_only_error_runtime_surface, context);
  for (const auto &arg : expr->args) {
    WalkErrorHandlingTryDoCatchExpr(arg.get(), program, summary, diagnostics,
                            allow_source_only_error_runtime_surface, context);
  }
}

} // namespace

Objc3ErrorHandlingTryDoCatchSemanticSummary BuildErrorHandlingTryDoCatchSemanticSummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &surface,
    bool allow_source_only_error_runtime_surface,
    std::vector<std::string> &diagnostics) {
  (void)surface;
  Objc3ErrorHandlingTryDoCatchSemanticSummary summary;
  for (const auto &fn : program.functions) {
    Objc3ErrorHandlingSemanticWalkContext context;
    context.in_throws_callable = fn.throws_declared;
    for (const auto &stmt : fn.body) {
      WalkErrorHandlingTryDoCatchStmt(stmt.get(), program, summary, diagnostics,
                              allow_source_only_error_runtime_surface, context);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      Objc3ErrorHandlingSemanticWalkContext context;
      context.in_throws_callable = method.throws_declared;
      for (const auto &stmt : method.body) {
        WalkErrorHandlingTryDoCatchStmt(stmt.get(), program, summary, diagnostics,
                                allow_source_only_error_runtime_surface,
                                context);
      }
    }
  }

  summary.try_surface_landed =
      summary.try_expression_sites ==
          summary.try_propagating_sites + summary.try_optional_sites +
              summary.try_forced_sites &&
      summary.throwing_callable_try_sites <= summary.try_expression_sites &&
      summary.bridged_callable_try_sites <= summary.try_expression_sites;
  summary.throw_surface_landed =
      summary.rethrow_sites <= summary.throw_statement_sites;
  summary.do_catch_surface_landed =
      summary.catch_binding_sites <= summary.catch_clause_sites &&
      summary.catch_all_sites <= summary.catch_clause_sites;
  summary.throwing_context_legality_enforced = true;
  summary.deterministic =
      summary.try_surface_landed && summary.throw_surface_landed &&
      summary.do_catch_surface_landed &&
      summary.contract_violation_sites <=
          summary.try_expression_sites + summary.throw_statement_sites +
              summary.do_catch_sites + summary.catch_clause_sites;
  summary.ready_for_lowering_and_runtime = false;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";try=" << summary.try_expression_sites
      << ";throw=" << summary.throw_statement_sites
      << ";do-catch=" << summary.do_catch_sites
      << ";violations=" << summary.contract_violation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3MetaprogrammingDeriveExpansionInventorySummary
BuildMetaprogrammingDeriveExpansionInventorySummary(
    const Objc3Program &program,
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3MetaprogrammingDeriveExpansionInventorySummary summary;

  const auto count_diagnostic = [&diagnostics](std::string_view code,
                                               std::string_view needle) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code, needle](const std::string &entry) {
          return entry.find(code) != std::string::npos &&
                 entry.find(needle) != std::string::npos;
        }));
  };

  for (const auto &interface_decl : program.interfaces) {
    if (!interface_decl.objc_derive_declared) {
      continue;
    }
    ++summary.derive_request_sites;

    const std::string canonical_derive_name =
        NormalizeMetaprogrammingSupportedDeriveName(interface_decl.objc_derive_name);
    if (interface_decl.has_category) {
      ++summary.unsupported_topology_sites;
      continue;
    }
    if (canonical_derive_name.empty()) {
      ++summary.unsupported_derive_request_sites;
      continue;
    }

    ++summary.supported_derive_request_sites;
    const std::string lowered =
        objc3c::support::LowercaseAscii(
            NormalizeMetaprogrammingRawDeriveToken(interface_decl.objc_derive_name));
    if (lowered == "equatable") {
      ++summary.equatable_alias_sites;
    }
    if (canonical_derive_name == "Equality") {
      ++summary.equality_derive_sites;
    } else if (canonical_derive_name == "Hash") {
      ++summary.hash_derive_sites;
    } else if (canonical_derive_name == "DebugDescription") {
      ++summary.debug_description_derive_sites;
    }

    const std::string derived_selector =
        BuildMetaprogrammingDerivedSelectorForInventoryRow(canonical_derive_name);
    const bool selector_conflict =
        !derived_selector.empty() &&
        std::any_of(interface_decl.methods.begin(), interface_decl.methods.end(),
                    [&derived_selector](const Objc3MethodDecl &method_decl) {
                      return BuildMethodSelectorNormalizationContractInfo(
                                 method_decl)
                                 .normalized_selector == derived_selector;
                    });
    if (selector_conflict) {
      ++summary.selector_conflict_sites;
      continue;
    }

    summary.expansion_inventory_rows_lexicographic.push_back(
        interface_decl.name + ":" + canonical_derive_name + ":" +
        derived_selector);
    ++summary.generated_method_entry_count;
  }

  std::sort(summary.expansion_inventory_rows_lexicographic.begin(),
            summary.expansion_inventory_rows_lexicographic.end());


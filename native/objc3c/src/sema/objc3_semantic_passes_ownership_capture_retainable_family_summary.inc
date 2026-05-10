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
      CollectOwnershipCaptureListRetainableFamilyStmtSites(stmt.get(),
                                                           profile);
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
        CollectOwnershipCaptureListRetainableFamilyStmtSites(stmt.get(),
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

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


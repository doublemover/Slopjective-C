Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary
BuildMetaprogrammingMacroSafetySandboxDeterminismSummary(
    const Objc3Program &program,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary summary;

  const auto count_diagnostic = [&diagnostics](std::string_view code,
                                               std::string_view needle) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code, needle](const std::string &entry) {
          return entry.find(code) != std::string::npos &&
                 entry.find(needle) != std::string::npos;
        }));
  };

  const auto accumulate_function = [&summary](const FunctionDecl &fn) {
    const bool has_macro_marker = fn.objc_macro_declared;
    const bool has_macro_package = fn.objc_macro_package_declared;
    const bool has_macro_provenance = fn.objc_macro_provenance_declared;
    if (has_macro_marker) {
      ++summary.macro_marker_sites;
    }
    if (has_macro_package) {
      ++summary.macro_package_sites;
    }
    if (has_macro_provenance) {
      ++summary.macro_provenance_sites;
    }
    if (has_macro_marker && has_macro_package && has_macro_provenance) {
      ++summary.expansion_visible_macro_sites;
    }

    if (!has_macro_marker && !(has_macro_package || has_macro_provenance)) {
      return;
    }
    if (!has_macro_marker && (has_macro_package || has_macro_provenance)) {
      ++summary.orphan_macro_metadata_sites;
      return;
    }
    if (!has_macro_package || !has_macro_provenance) {
      ++summary.incomplete_macro_metadata_sites;
      return;
    }
    if (!IsSupportedMetaprogrammingMacroPackageName(fn.objc_macro_package_name)) {
      ++summary.invalid_package_sites;
      return;
    }
    if (!IsSupportedMetaprogrammingMacroProvenanceToken(fn.objc_macro_provenance_name)) {
      ++summary.invalid_provenance_sites;
      return;
    }
    if (!fn.is_pure || fn.is_prototype || fn.async_declared ||
        fn.throws_declared) {
      ++summary.nondeterministic_callable_sites;
      return;
    }
    ++summary.safe_macro_callable_sites;
  };

  const auto accumulate_method = [&summary](const Objc3MethodDecl &method) {
    if (method.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
    if (method.objc_macro_package_declared) {
      ++summary.macro_package_sites;
    }
    if (method.objc_macro_provenance_declared) {
      ++summary.macro_provenance_sites;
    }
    if (method.objc_macro_declared && method.objc_macro_package_declared &&
        method.objc_macro_provenance_declared) {
      ++summary.expansion_visible_macro_sites;
    }
    if (method.objc_macro_declared || method.objc_macro_package_declared ||
        method.objc_macro_provenance_declared) {
      ++summary.unsupported_callable_topology_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_function(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_method(method_decl);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      accumulate_method(method_decl);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_method(method_decl);
    }
  }

  summary.semantic_dependency_required =
      IsReadyObjc3MetaprogrammingDeriveExpansionInventorySummary(dependency_summary);
  summary.metadata_completeness_enforced =
      summary.incomplete_macro_metadata_sites ==
          count_diagnostic("O3S320", "requires both objc_macro_package and objc_macro_provenance") &&
      summary.orphan_macro_metadata_sites ==
          count_diagnostic("O3S321", "macro package/provenance markers require objc_macro");
  summary.sandbox_namespace_enforced =
      summary.invalid_package_sites ==
      count_diagnostic("O3S322", "macro sandbox rejected package");
  summary.provenance_determinism_enforced =
      summary.invalid_provenance_sites ==
      count_diagnostic("O3S323", "macro provenance must be a lowercase sha256 digest");
  summary.callable_determinism_enforced =
      summary.nondeterministic_callable_sites ==
          count_diagnostic("O3S324", "must be pure, body-backed, non-async, and non-throws") &&
      summary.unsupported_callable_topology_sites ==
          count_diagnostic("O3S325", "not sandbox-admitted");
  summary.macro_execution_deferred = true;
  summary.deterministic =
      summary.semantic_dependency_required &&
      summary.metadata_completeness_enforced &&
      summary.sandbox_namespace_enforced &&
      summary.provenance_determinism_enforced &&
      summary.callable_determinism_enforced &&
      summary.safe_macro_callable_sites <= summary.expansion_visible_macro_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "metaprogramming macro safety/sandbox/determinism semantics must remain deterministic and fail closed";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << dependency_summary.contract_id
      << ";macro-sites=" << summary.macro_marker_sites << ":"
      << summary.macro_package_sites << ":" << summary.macro_provenance_sites
      << ":" << summary.expansion_visible_macro_sites
      << ";safe=" << summary.safe_macro_callable_sites
      << ";incomplete=" << summary.incomplete_macro_metadata_sites
      << ";orphan=" << summary.orphan_macro_metadata_sites
      << ";invalid-package=" << summary.invalid_package_sites
      << ";invalid-provenance=" << summary.invalid_provenance_sites
      << ";nondeterministic=" << summary.nondeterministic_callable_sites
      << ";unsupported-topology=" << summary.unsupported_callable_topology_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummary(
    const Objc3Program &program,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary summary;

  const auto count_diagnostic = [&diagnostics](std::string_view code,
                                               std::string_view needle) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code, needle](const std::string &entry) {
          return entry.find(code) != std::string::npos &&
                 entry.find(needle) != std::string::npos;
        }));
  };
  const auto property_is_object_like =
      [](const Objc3PropertyDecl &property_decl) {
        return property_decl.id_spelling || property_decl.instancetype_spelling ||
               property_decl.object_pointer_type_spelling ||
               property_decl.type == ValueType::ObjCId ||
               property_decl.type == ValueType::ObjCClass ||
               property_decl.type == ValueType::ObjCInstancetype ||
               property_decl.type == ValueType::ObjCObjectPtr;
      };
  const auto accumulate_property =
      [&](const Objc3PropertyDecl &property_decl, std::string_view owner_kind) {
        if (!property_decl.property_behavior_declared) {
          return;
        }
        ++summary.property_behavior_sites;
        const std::string canonical_behavior =
            NormalizeMetaprogrammingSupportedPropertyBehaviorName(
                property_decl.property_behavior_name);
        if (canonical_behavior.empty()) {
          ++summary.unsupported_behavior_sites;
          return;
        }
        ++summary.supported_behavior_sites;
        if (canonical_behavior == "Observed") {
          ++summary.observed_behavior_sites;
          if (owner_kind == "protocol") {
            ++summary.observed_on_protocol_sites;
          }
          if (property_decl.is_readonly ||
              !property_decl.effective_setter_available) {
            ++summary.observed_readonly_conflict_sites;
          }
        } else if (canonical_behavior == "Projected") {
          ++summary.projected_behavior_sites;
          if (!property_decl.is_readonly || property_decl.has_setter ||
              property_decl.effective_setter_available) {
            ++summary.projected_writable_conflict_sites;
          }
        }
        if (!property_is_object_like(property_decl)) {
          ++summary.non_object_behavior_sites;
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &property_decl : interface_decl.properties) {
      accumulate_property(property_decl, interface_decl.has_category
                                             ? "category-interface"
                                             : "interface");
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property_decl : protocol_decl.properties) {
      accumulate_property(property_decl, "protocol");
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &property_decl : implementation_decl.properties) {
      accumulate_property(property_decl, "implementation");
    }
  }

  summary.semantic_dependency_required =
      IsReadyObjc3MetaprogrammingMacroSafetySandboxDeterminismSummary(
          dependency_summary);
  summary.supported_behavior_inventory_landed =
      summary.supported_behavior_sites ==
          summary.observed_behavior_sites + summary.projected_behavior_sites &&
      summary.supported_behavior_sites + summary.unsupported_behavior_sites ==
          summary.property_behavior_sites;
  summary.unsupported_behavior_fail_closed =
      summary.unsupported_behavior_sites ==
      count_diagnostic("O3S326", "unsupported property behavior");
  summary.owner_topology_fail_closed =
      summary.observed_on_protocol_sites ==
      count_diagnostic("O3S328",
                       "observed property behavior requires a concrete interface or implementation property");
  summary.interaction_legality_fail_closed =
      summary.observed_readonly_conflict_sites ==
          count_diagnostic("O3S329",
                           "observed property behavior requires writable setter-visible storage") &&
      summary.projected_writable_conflict_sites ==
          count_diagnostic("O3S330",
                           "projected property behavior requires a readonly getter-only property");
  summary.storage_legality_fail_closed =
      summary.non_object_behavior_sites ==
      count_diagnostic("O3S327",
                       "requires an Objective-C object property");
  summary.runtime_materialization_deferred = true;
  summary.deterministic =
      summary.semantic_dependency_required &&
      summary.supported_behavior_inventory_landed &&
      summary.unsupported_behavior_fail_closed &&
      summary.owner_topology_fail_closed &&
      summary.interaction_legality_fail_closed &&
      summary.storage_legality_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "metaprogramming property behavior legality and interaction semantics must remain deterministic and fail closed";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << dependency_summary.contract_id
      << ";behavior-sites=" << summary.property_behavior_sites << ":"
      << summary.supported_behavior_sites << ":"
      << summary.unsupported_behavior_sites
      << ";supported-behaviors=" << summary.observed_behavior_sites << ":"
      << summary.projected_behavior_sites
      << ";compatibility-conflicts=" << summary.observed_on_protocol_sites
      << ":" << summary.observed_readonly_conflict_sites << ":"
      << summary.projected_writable_conflict_sites << ":"
      << summary.non_object_behavior_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

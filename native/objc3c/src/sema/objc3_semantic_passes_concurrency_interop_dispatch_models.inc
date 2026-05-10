  summary.objc_status_code_attribute_sites =
      source_summary.objc_status_code_attribute_sites;
  summary.status_code_success_clause_sites =
      source_summary.status_code_success_clause_sites;
  summary.status_code_error_type_clause_sites =
      source_summary.status_code_error_type_clause_sites;
  summary.status_code_mapping_clause_sites =
      source_summary.status_code_mapping_clause_sites;
  summary.placeholder_throws_propagation_sites =
      surface.throws_propagation_summary.throws_propagation_sites;
  summary.placeholder_unwind_cleanup_sites =
      surface.unwind_cleanup_summary.unwind_cleanup_sites;

  summary.source_dependency_required = true;
  summary.throws_declaration_semantics_landed =
      source_summary.throws_declaration_source_supported &&
      summary.throws_declaration_sites ==
          source_summary.function_throws_declaration_sites +
              source_summary.method_throws_declaration_sites;
  summary.result_carrier_profile_semantics_landed =
      source_summary.result_carrier_source_supported &&
      surface.result_like_lowering_summary.deterministic &&
      summary.result_like_sites == source_summary.result_like_sites &&
      summary.result_success_sites == source_summary.result_success_sites &&
      summary.result_failure_sites == source_summary.result_failure_sites &&
      summary.result_branch_sites == source_summary.result_branch_sites &&
      summary.result_payload_sites == source_summary.result_payload_sites;
  summary.ns_error_bridging_profile_semantics_landed =
      source_summary.ns_error_bridging_source_supported &&
      surface.ns_error_bridging_summary.deterministic &&
      summary.ns_error_bridging_sites >= source_summary.ns_error_bridging_sites &&
      summary.ns_error_out_parameter_sites >=
          source_summary.ns_error_out_parameter_sites &&
      summary.ns_error_bridge_path_sites >=
          source_summary.ns_error_bridge_path_sites &&
      summary.ns_error_out_parameter_sites <= summary.ns_error_bridging_sites &&
      summary.ns_error_bridge_path_sites <= summary.ns_error_bridging_sites;
  summary.bridge_marker_semantics_landed =
      source_summary.error_bridge_marker_source_supported &&
      summary.objc_nserror_attribute_sites ==
          source_summary.objc_nserror_attribute_sites &&
      summary.objc_status_code_attribute_sites ==
          source_summary.objc_status_code_attribute_sites &&
      summary.status_code_success_clause_sites ==
          source_summary.status_code_success_clause_sites &&
      summary.status_code_error_type_clause_sites ==
          source_summary.status_code_error_type_clause_sites &&
      summary.status_code_mapping_clause_sites ==
          source_summary.status_code_mapping_clause_sites;
  summary.parser_fail_closed_boundary_required = true;
  summary.parser_fail_closed_boundary_preserved =
      source_summary.try_keyword_reserved &&
      source_summary.throw_keyword_reserved &&
      source_summary.catch_keyword_reserved && source_summary.try_fail_closed &&
      source_summary.throw_fail_closed && source_summary.do_catch_fail_closed;
  summary.propagation_runtime_deferred = true;
  summary.status_to_error_runtime_deferred = true;
  summary.native_error_abi_deferred = true;
  summary.placeholder_throws_summary_carried =
      surface.throws_propagation_summary.deterministic &&
      surface.unwind_cleanup_summary.deterministic;
  summary.deterministic =
      source_summary.deterministic_handoff &&
      surface.result_like_lowering_summary.deterministic &&
      surface.ns_error_bridging_summary.deterministic &&
      surface.throws_propagation_summary.deterministic &&
      surface.unwind_cleanup_summary.deterministic &&
      summary.throws_declaration_semantics_landed &&
      summary.result_carrier_profile_semantics_landed &&
      summary.ns_error_bridging_profile_semantics_landed &&
      summary.bridge_marker_semantics_landed &&
      summary.parser_fail_closed_boundary_preserved;
  summary.ready_for_lowering_and_runtime = false;

  std::ostringstream out;
  out << summary.contract_id
      << ";source-dependency=" << summary.frontend_dependency_contract_id
      << ";throws=" << summary.throws_declaration_sites
      << ";result-like=" << summary.result_like_sites
      << ";nserror=" << summary.ns_error_bridging_sites
      << ";bridge-markers=" << summary.objc_nserror_attribute_sites << ":"
      << summary.objc_status_code_attribute_sites
      << ";placeholder-throws=" << summary.placeholder_throws_propagation_sites
      << ";placeholder-unwind=" << summary.placeholder_unwind_cleanup_sites
      << ";parser-fail-closed="
      << (summary.parser_fail_closed_boundary_preserved ? "true" : "false")
      << ";propagation-runtime-deferred="
      << (summary.propagation_runtime_deferred ? "true" : "false")
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

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
  summary.runnable_lowering_deferred = true;
  summary.actor_runtime_deferred = true;
  summary.executor_runtime_deferred = true;
  summary.deterministic =
      dependency_summary.deterministic &&
      summary.non_actor_nonisolated_fail_closed &&
      summary.nonisolated_combination_fail_closed &&
      summary.actor_hop_async_boundary_enforced &&
      summary.non_sendable_crossing_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "actor isolation/sendability enforcement diagnostics and dependency packet must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";actor-methods=" << summary.actor_method_sites
      << ";nonisolated-sites=" << summary.total_nonisolated_method_sites
      << ";illegal-sites=" << summary.illegal_non_actor_nonisolated_sites << ":"
      << summary.illegal_nonisolated_async_sites << ":"
      << summary.illegal_nonisolated_executor_sites << ":"
      << summary.illegal_actor_hop_without_async_sites << ":"
      << summary.illegal_non_sendable_crossing_sites
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

Objc3OwnershipSystemExtensionSemanticModelSummary
BuildOwnershipSystemExtensionSemanticModelSummary(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
        &source_summary,
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &completion_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary) {
  Objc3OwnershipSystemExtensionSemanticModelSummary summary;
  summary.cleanup_attribute_sites = completion_summary.cleanup_attribute_sites;
  summary.cleanup_sugar_sites = completion_summary.cleanup_sugar_sites;
  summary.resource_attribute_sites = completion_summary.resource_attribute_sites;
  summary.resource_sugar_sites = completion_summary.resource_sugar_sites;
  summary.borrowed_pointer_sites = source_summary.borrowed_pointer_sites;
  summary.returns_borrowed_attribute_sites =
      source_summary.returns_borrowed_attribute_sites;
  summary.explicit_capture_list_sites =
      completion_summary.explicit_capture_list_sites;
  summary.explicit_capture_item_sites =
      completion_summary.explicit_capture_item_sites;
  summary.retainable_family_annotation_sites =
      retainable_summary.family_retain_sites +
      retainable_summary.family_release_sites +
      retainable_summary.family_autorelease_sites;
  summary.retainable_family_compatibility_alias_sites =
      retainable_summary.compatibility_returns_retained_sites +
      retainable_summary.compatibility_returns_not_retained_sites +
      retainable_summary.compatibility_consumed_sites;

  summary.source_dependency_required = true;
  summary.cleanup_resource_semantic_model_frozen =
      completion_summary.cleanup_attribute_source_supported &&
      completion_summary.resource_sugar_source_supported &&
      summary.resource_attribute_sites >= summary.resource_sugar_sites;
  summary.borrowed_pointer_semantic_model_frozen =
      source_summary.borrowed_pointer_source_supported &&
      source_summary.returns_borrowed_source_supported &&
      summary.borrowed_pointer_sites >= summary.returns_borrowed_attribute_sites;
  summary.capture_legality_semantic_model_frozen =
      completion_summary.explicit_capture_list_source_supported &&
      summary.explicit_capture_item_sites >= summary.explicit_capture_list_sites;
  summary.retainable_family_semantic_model_frozen =
      retainable_summary.callable_annotation_source_supported &&
      retainable_summary.compatibility_alias_source_supported &&
      summary.retainable_family_annotation_sites > 0u;
  summary.resource_move_semantics_deferred = true;
  summary.borrowed_escape_semantics_deferred = true;
  summary.retainable_family_legality_deferred = true;
  summary.deterministic =
      source_summary.deterministic_handoff &&
      completion_summary.deterministic_handoff &&
      retainable_summary.deterministic_handoff &&
      summary.cleanup_resource_semantic_model_frozen &&
      summary.borrowed_pointer_semantic_model_frozen &&
      summary.capture_legality_semantic_model_frozen &&
      summary.retainable_family_semantic_model_frozen;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";source=" << source_summary.contract_id
      << ";completion=" << completion_summary.contract_id
      << ";retainable=" << retainable_summary.contract_id
      << ";cleanup=" << summary.cleanup_attribute_sites << ":"
      << summary.cleanup_sugar_sites
      << ";resource=" << summary.resource_attribute_sites << ":"
      << summary.resource_sugar_sites
      << ";borrowed=" << summary.borrowed_pointer_sites << ":"
      << summary.returns_borrowed_attribute_sites
      << ";capture=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites
      << ";retainable=" << summary.retainable_family_annotation_sites << ":"
      << summary.retainable_family_compatibility_alias_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3InteropInteropSemanticModelSummary BuildInteropInteropSemanticModelSummary(
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &foreign_source_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_source_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &ownership_summary,
    const Objc3ErrorHandlingErrorBridgeLegalitySummary &error_summary,
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &async_summary,
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &actor_summary) {
  // semantic freeze anchor: Part 11 interop legality is modeled as a
  // single summary over already-landed Part 6/7/8 semantic surfaces plus the
  // new foreign/import and Swift/C++ annotation source state, while ABI
  // lowering and runnable bridge generation remain later work.
  Objc3InteropInteropSemanticModelSummary summary;
  summary.foreign_callable_sites = foreign_source_summary.foreign_callable_sites;
  summary.import_module_annotation_sites =
      foreign_source_summary.import_module_annotation_sites;
  summary.imported_module_name_sites =
      foreign_source_summary.imported_module_name_sites;
  summary.export_header_annotation_sites =
      foreign_source_summary.export_header_annotation_sites;
  summary.export_header_name_sites =
      foreign_source_summary.export_header_name_sites;
  summary.mixed_image_annotation_sites =
      foreign_source_summary.mixed_image_annotation_sites;
  summary.mixed_image_name_sites =
      foreign_source_summary.mixed_image_name_sites;
  summary.package_entry_annotation_sites =
      foreign_source_summary.package_entry_annotation_sites;
  summary.package_entry_name_sites =
      foreign_source_summary.package_entry_name_sites;
  summary.swift_name_annotation_sites =
      interop_source_summary.swift_name_annotation_sites;
  summary.swift_private_annotation_sites =
      interop_source_summary.swift_private_annotation_sites;
  summary.cpp_name_annotation_sites =
      interop_source_summary.cpp_name_annotation_sites;
  summary.header_name_annotation_sites =
      interop_source_summary.header_name_annotation_sites;
  summary.abi_alignment_annotation_sites =
      interop_source_summary.abi_alignment_annotation_sites;
  summary.foreign_type_annotation_sites =
      interop_source_summary.foreign_type_annotation_sites;
  summary.named_annotation_payload_sites =
      interop_source_summary.named_annotation_payload_sites;
  summary.retainable_family_callable_sites =
      ownership_summary.retainable_family_callable_sites;
  summary.bridge_callable_sites = error_summary.bridge_callable_sites;
  summary.async_executor_affinity_sites = async_summary.executor_affinity_sites;
  summary.actor_hazard_sites = actor_summary.task_handoff_sites;
  summary.interop_metadata_annotation_sites =
      foreign_source_summary.interop_annotation_sites +
      interop_source_summary.interop_metadata_annotation_sites;

  summary.source_dependency_required = true;
  summary.foreign_annotation_source_supported =
      foreign_source_summary.foreign_declaration_source_supported &&
      foreign_source_summary.imported_surface_source_supported &&
      foreign_source_summary.interop_annotation_source_supported &&
      interop_source_summary.swift_annotation_source_supported &&
      interop_source_summary.cpp_annotation_source_supported &&
      interop_source_summary.interop_metadata_source_supported &&
      foreign_source_summary.deterministic_handoff &&
      interop_source_summary.deterministic_handoff;
  summary.ownership_interaction_profile_frozen =
      IsReadyObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary(
          ownership_summary) &&
      summary.retainable_family_callable_sites >=
          ownership_summary.retainable_family_operation_callable_sites;
  summary.error_bridge_profile_reused =
      error_summary.source_dependency_required &&
      error_summary.bridge_legality_landed &&
      error_summary.try_bridge_filter_landed &&
      error_summary.unsupported_combinations_fail_closed &&
      error_summary.native_emit_remains_fail_closed &&
      error_summary.deterministic &&
      error_summary.failure_reason.empty() &&
      summary.bridge_callable_sites >=
          error_summary.try_eligible_bridge_callable_sites;
  summary.async_affinity_profile_reused =
      IsReadyObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummary(async_summary) &&
      summary.async_executor_affinity_sites >=
          async_summary.supported_async_callable_sites;
  summary.actor_hazard_profile_reused =
      IsReadyObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary(actor_summary) &&
      summary.actor_hazard_sites >=
          actor_summary.illegal_missing_actor_isolation_sites;
  summary.metadata_payload_profile_frozen =
      summary.imported_module_name_sites <=
          summary.import_module_annotation_sites &&
      summary.export_header_name_sites <=
          summary.export_header_annotation_sites &&
      summary.mixed_image_name_sites <=
          summary.mixed_image_annotation_sites &&
      summary.package_entry_name_sites <=
          summary.package_entry_annotation_sites &&
      summary.named_annotation_payload_sites ==
          summary.swift_name_annotation_sites +
              summary.cpp_name_annotation_sites +
              summary.header_name_annotation_sites +
              summary.foreign_type_annotation_sites;
  summary.ffi_abi_lowering_deferred = true;
  summary.runtime_bridge_generation_deferred = true;
  summary.deterministic =
      summary.foreign_annotation_source_supported &&
      summary.ownership_interaction_profile_frozen &&
      summary.error_bridge_profile_reused &&
      summary.async_affinity_profile_reused &&
      summary.actor_hazard_profile_reused &&
      summary.metadata_payload_profile_frozen;
  summary.ready_for_semantic_expansion = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "interop interop source, ownership, error, async, actor, and metadata profiles must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";foreign=" << summary.foreign_callable_sites
      << ";import-module=" << summary.import_module_annotation_sites << ":"
      << summary.imported_module_name_sites
      << ";export-header=" << summary.export_header_annotation_sites << ":"
      << summary.export_header_name_sites
      << ";mixed-image=" << summary.mixed_image_annotation_sites << ":"
      << summary.mixed_image_name_sites
      << ";package-entry=" << summary.package_entry_annotation_sites << ":"
      << summary.package_entry_name_sites
      << ";swift=" << summary.swift_name_annotation_sites << ":"
      << summary.swift_private_annotation_sites
      << ";cpp=" << summary.cpp_name_annotation_sites << ":"
      << summary.header_name_annotation_sites
      << ";abi-align=" << summary.abi_alignment_annotation_sites
      << ";foreign-type=" << summary.foreign_type_annotation_sites
      << ";named-payload=" << summary.named_annotation_payload_sites
      << ";retainable-family=" << summary.retainable_family_callable_sites
      << ";bridge=" << summary.bridge_callable_sites
      << ";async-affinity=" << summary.async_executor_affinity_sites
      << ";actor-hazard=" << summary.actor_hazard_sites
      << ";interop-metadata=" << summary.interop_metadata_annotation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

template <typename CallableDeclT>
static bool HasInteropForeignOrImportAnnotations(const CallableDeclT &decl) {
  return decl.objc_foreign_declared || decl.objc_import_module_declared ||
         decl.objc_export_header_declared || decl.objc_mixed_image_declared ||
         decl.objc_package_entry_declared;
}

template <typename CallableDeclT>
static bool HasInteropCppInteropAnnotations(const CallableDeclT &decl) {
  return decl.objc_cxx_name_declared || decl.objc_header_name_declared ||
         decl.objc_abi_align_declared || decl.objc_foreign_type_declared;
}

template <typename CallableDeclT>
static bool HasInteropSwiftInteropAnnotations(const CallableDeclT &decl) {
  return decl.objc_swift_name_declared || decl.objc_swift_private_declared;
}

template <typename CallableDeclT>
static bool HasInteropOwnershipInteractionSurface(const CallableDeclT &decl) {
  if (decl.has_return_ownership_qualifier ||
      decl.objc_returns_borrowed_declared ||
      decl.return_borrowed_pointer_qualified ||
      !decl.retainable_c_family_callable_attributes.empty()) {
    return true;
  }
  return std::any_of(
      decl.params.begin(), decl.params.end(),
      [](const FuncParam &param) { return param.has_ownership_qualifier; });
}

template <typename CallableDeclT>
static bool HasInteropAsyncInteractionSurface(const CallableDeclT &decl) {
  return decl.async_declared || decl.executor_affinity_declared;
}

Objc3InteropCppInteropInteractionSummary BuildInteropCppInteropInteractionSummary(
    const Objc3Program &program,
    const Objc3InteropInteropRuntimeParitySummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3InteropCppInteropInteractionSummary summary;

  const auto count_diagnostic_code = [&diagnostics](std::string_view code) {
    return std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code](const std::string &entry) {
          return entry.find(code) != std::string::npos;
        });
  };

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (!HasInteropCppInteropAnnotations(decl)) {
      return;
    }
    ++summary.cpp_interop_callable_sites;
    if (decl.objc_cxx_name_declared) {
      ++summary.cpp_named_callable_sites;
    }
    if (decl.objc_header_name_declared) {
      ++summary.header_named_callable_sites;
    }
    if (HasInteropOwnershipInteractionSurface(decl)) {
      ++summary.ownership_interaction_sites;
    }
    if (decl.throws_declared) {
      ++summary.throws_interaction_sites;
    }
    if (HasInteropAsyncInteractionSurface(decl)) {
      ++summary.async_interaction_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_callable(method_decl);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      accumulate_callable(method_decl);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_callable(method_decl);
    }
  }

  summary.ownership_rejection_sites = count_diagnostic_code("O3S334");
  summary.throws_rejection_sites = count_diagnostic_code("O3S335");
  summary.async_rejection_sites = count_diagnostic_code("O3S336");

  const bool dependency_surface_present =
      IsReadyObjc3InteropInteropRuntimeParitySummary(dependency_summary);
  summary.dependency_required = true;
  summary.cpp_annotation_profile_reused =
      dependency_surface_present &&
      summary.cpp_named_callable_sites <= summary.cpp_interop_callable_sites &&
      summary.header_named_callable_sites <=
          summary.cpp_interop_callable_sites;
  summary.ownership_interactions_fail_closed =
      dependency_surface_present &&
      summary.ownership_rejection_sites == summary.ownership_interaction_sites;
  summary.throws_interactions_fail_closed =
      dependency_surface_present &&
      summary.throws_rejection_sites == summary.throws_interaction_sites;
  summary.async_interactions_fail_closed =
      dependency_surface_present &&
      summary.async_rejection_sites == summary.async_interaction_sites;
  summary.ffi_abi_lowering_deferred = true;
  summary.runtime_bridge_generation_deferred = true;
  summary.deterministic =
      summary.cpp_annotation_profile_reused &&
      summary.ownership_interactions_fail_closed &&
      summary.throws_interactions_fail_closed &&
      summary.async_interactions_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "interop cxx-facing ownership, throws, and async diagnostics must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";cpp-callables=" << summary.cpp_interop_callable_sites << ":"
      << summary.cpp_named_callable_sites << ":"
      << summary.header_named_callable_sites
      << ";interactions=" << summary.ownership_interaction_sites << ":"
      << summary.throws_interaction_sites << ":"
      << summary.async_interaction_sites
      << ";rejections=" << summary.ownership_rejection_sites << ":"
      << summary.throws_rejection_sites << ":"
      << summary.async_rejection_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3InteropSwiftInteropIsolationSummary
BuildInteropSwiftInteropIsolationSummary(
    const Objc3Program &program,
    const Objc3InteropCppInteropInteractionSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3InteropSwiftInteropIsolationSummary summary;

  const auto count_diagnostic_code = [&diagnostics](std::string_view code) {
    return std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code](const std::string &entry) {
          return entry.find(code) != std::string::npos;
        });
  };
  const auto is_actor_interface =
      [&program](const std::string &name) -> bool {
    const auto interface_it =
        std::find_if(program.interfaces.begin(), program.interfaces.end(),
                     [&name](const Objc3InterfaceDecl &interface_decl) {
                       return !interface_decl.has_category &&
                              interface_decl.name == name &&
                              interface_decl.is_actor;
                     });
    return interface_it != program.interfaces.end();
  };
  const auto accumulate_callable =
      [&summary](const auto &decl, bool actor_owned,
                 bool implementation_surface) {
        if (!HasInteropSwiftInteropAnnotations(decl)) {
          return;
        }
        ++summary.swift_interop_callable_sites;
        if (decl.objc_swift_name_declared) {
          ++summary.swift_named_callable_sites;
        }
        if (decl.objc_swift_private_declared) {
          ++summary.swift_private_callable_sites;
        }
        if (decl.objc_swift_private_declared && !decl.objc_swift_name_declared) {
          ++summary.swift_private_without_name_sites;
        }
        if (actor_owned) {
          ++summary.actor_owned_swift_callable_sites;
        }
        if (decl.objc_nonisolated_declared) {
          ++summary.nonisolated_swift_callable_sites;
        }
        if (implementation_surface) {
          ++summary.implementation_swift_callable_sites;
        }
      };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn, false, false);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_callable(method_decl, interface_decl.is_actor, false);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      accumulate_callable(method_decl, false, false);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    const bool actor_owned =
        !implementation_decl.has_category &&
        is_actor_interface(implementation_decl.name);
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_callable(method_decl, actor_owned, true);
    }
  }

  summary.swift_private_without_name_rejection_sites =
      count_diagnostic_code("O3S337");
  summary.actor_isolation_mapping_rejection_sites =
      count_diagnostic_code("O3S338");
  summary.nonisolated_mapping_rejection_sites =
      count_diagnostic_code("O3S339");
  summary.implementation_surface_rejection_sites =
      count_diagnostic_code("O3S340");

  const bool dependency_surface_present =
      IsReadyObjc3InteropCppInteropInteractionSummary(dependency_summary);
  summary.dependency_required = true;
  summary.swift_metadata_profile_reused =
      dependency_surface_present &&
      summary.swift_named_callable_sites <= summary.swift_interop_callable_sites &&
      summary.swift_private_callable_sites <=
          summary.swift_interop_callable_sites;
  summary.swift_private_requires_name_enforced =
      dependency_surface_present &&
      summary.swift_private_without_name_rejection_sites ==
          summary.swift_private_without_name_sites;
  summary.actor_isolation_mapping_fail_closed =
      dependency_surface_present &&
      summary.actor_isolation_mapping_rejection_sites ==
          summary.actor_owned_swift_callable_sites;
  summary.nonisolated_mapping_fail_closed =
      dependency_surface_present &&
      summary.nonisolated_mapping_rejection_sites ==
          summary.nonisolated_swift_callable_sites;
  summary.implementation_surface_fail_closed =
      dependency_surface_present &&
      summary.implementation_surface_rejection_sites ==
          summary.implementation_swift_callable_sites;
  summary.ffi_abi_lowering_deferred = true;
  summary.runtime_bridge_generation_deferred = true;
  summary.deterministic =
      summary.swift_metadata_profile_reused &&
      summary.swift_private_requires_name_enforced &&
      summary.actor_isolation_mapping_fail_closed &&
      summary.nonisolated_mapping_fail_closed &&
      summary.implementation_surface_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "interop swift metadata and isolation diagnostics must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";swift-callables=" << summary.swift_interop_callable_sites << ":"
      << summary.swift_named_callable_sites << ":"
      << summary.swift_private_callable_sites
      << ";unsupported=" << summary.swift_private_without_name_sites << ":"
      << summary.actor_owned_swift_callable_sites << ":"
      << summary.nonisolated_swift_callable_sites << ":"
      << summary.implementation_swift_callable_sites
      << ";rejections=" << summary.swift_private_without_name_rejection_sites
      << ":" << summary.actor_isolation_mapping_rejection_sites << ":"
      << summary.nonisolated_mapping_rejection_sites << ":"
      << summary.implementation_surface_rejection_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

template <typename CallableDeclT>
static bool HasDispatchCallableDispatchIntentAttributes(
    const CallableDeclT &decl);
static bool HasDispatchContainerDispatchIntentAttributes(
    const Objc3InterfaceDecl &decl);

Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary
BuildMetaprogrammingExpansionBehaviorSemanticModelSummary(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &source_summary,
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &macro_summary,
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary summary;

  const auto count_diagnostic = [&diagnostics](std::string_view code,
                                               std::string_view needle) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code, needle](const std::string &entry) {
          return entry.find(code) != std::string::npos &&
                 entry.find(needle) != std::string::npos;
        }));
  };

  summary.derive_marker_sites = source_summary.derive_marker_sites;
  summary.macro_marker_sites = source_summary.macro_marker_sites;
  summary.macro_package_sites = macro_summary.macro_package_sites;
  summary.macro_provenance_sites = macro_summary.macro_provenance_sites;
  summary.expansion_visible_macro_sites =
      macro_summary.expansion_visible_macro_sites;
  summary.property_behavior_sites = property_summary.property_behavior_sites;
  summary.interface_property_behavior_sites =
      property_summary.interface_property_behavior_sites;
  summary.implementation_property_behavior_sites =
      property_summary.implementation_property_behavior_sites;
  summary.protocol_property_behavior_sites =
      property_summary.protocol_property_behavior_sites;
  summary.synthesized_binding_visible_sites =
      property_summary.synthesized_binding_visible_sites;
  summary.synthesized_getter_visible_sites =
      property_summary.synthesized_getter_visible_sites;
  summary.synthesized_setter_visible_sites =
      property_summary.synthesized_setter_visible_sites;
  summary.property_behavior_contract_violation_sites =
      count_diagnostic("O3S206", "behavior attribute");

  summary.source_dependency_required = true;
  summary.derive_macro_source_supported =
      source_summary.derive_marker_source_supported &&
      source_summary.macro_marker_source_supported &&
      source_summary.property_behavior_source_supported &&
      source_summary.deterministic_handoff &&
      source_summary.ready_for_semantic_expansion;
  summary.macro_package_provenance_surface_reused =
      macro_summary.macro_package_source_supported &&
      macro_summary.macro_provenance_source_supported &&
      macro_summary.expansion_visible_source_supported &&
      macro_summary.deterministic_handoff &&
      macro_summary.ready_for_semantic_expansion &&
      summary.expansion_visible_macro_sites <= summary.macro_marker_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_package_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_provenance_sites;
  summary.property_behavior_source_supported =
      property_summary.property_behavior_source_supported &&
      property_summary.deterministic_handoff &&
      property_summary.ready_for_semantic_expansion;
  summary.synthesized_visibility_surface_reused =
      property_summary.synthesized_declaration_visibility_supported &&
      summary.synthesized_binding_visible_sites <=
          summary.synthesized_getter_visible_sites &&
      summary.synthesized_setter_visible_sites <=
          summary.synthesized_getter_visible_sites &&
      summary.interface_property_behavior_sites +
              summary.implementation_property_behavior_sites +
              summary.protocol_property_behavior_sites ==
          summary.property_behavior_sites &&
      summary.property_behavior_contract_violation_sites <=
          summary.property_behavior_sites;
  summary.derive_synthesis_deferred = true;

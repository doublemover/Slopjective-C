                      protocol_decl.semantic_link_symbol,
                      protocol_decl.semantic_link_symbol);
  }
  for (const auto &implementation_decl : program.implementations) {
    append_properties(
        implementation_decl.properties,
        implementation_decl.has_category ? "category-implementation"
                                         : "class-implementation",
        implementation_decl.has_category
            ? BuildMetaprogrammingCategoryOwnerName(implementation_decl.name,
                                           implementation_decl.category_name)
            : implementation_decl.name,
        implementation_decl.semantic_link_symbol,
        implementation_decl.has_category
            ? BuildMetaprogrammingRuntimeCategoryOwnerIdentity(
                  implementation_decl.name, implementation_decl.category_name)
            : BuildMetaprogrammingRuntimeClassOwnerIdentity(implementation_decl.name));
  }

  std::sort(bundles.begin(), bundles.end(),
            [](const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &lhs,
               const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &rhs) {
              return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name,
                              lhs.behavior_name) <
                     std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name,
                              rhs.behavior_name);
            });
  return bundles;
}

Objc3MetaprogrammingSynthesizedArtifactEmissionContract
BuildMetaprogrammingSynthesizedArtifactEmissionContract(
    const Objc3MetaprogrammingExpansionLoweringContract &dependency_contract,
    const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle> &derive_bundles,
    const std::vector<Objc3IRMetaprogrammingMacroArtifactBundle> &macro_bundles,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &property_behavior_bundles) {
  Objc3MetaprogrammingSynthesizedArtifactEmissionContract contract;
  contract.derive_inventory_sites = dependency_contract.derive_inventory_sites;
  contract.emitted_derive_method_sites = derive_bundles.size();
  contract.emitted_macro_artifact_sites = macro_bundles.size();
  contract.emitted_property_behavior_artifact_sites =
      property_behavior_bundles.size();
  contract.emitted_global_artifact_sites =
      contract.emitted_derive_method_sites +
      contract.emitted_macro_artifact_sites +
      contract.emitted_property_behavior_artifact_sites;
  contract.emitted_runtime_method_list_sites =
      contract.emitted_derive_method_sites;
  contract.guard_blocked_sites = dependency_contract.guard_blocked_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      dependency_contract.deterministic &&
      contract.emitted_derive_method_sites <=
          dependency_contract.derived_selector_artifact_sites &&
      contract.emitted_macro_artifact_sites <=
          dependency_contract.macro_replay_visible_sites &&
      contract.emitted_property_behavior_artifact_sites <=
          dependency_contract.property_behavior_sites;
  return contract;
}

std::string BuildConcurrencyActorLoweringMetadataContractJson(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &source_summary,
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &enforcement_summary,
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &hazard_summary,
    const Objc3ActorLoweringMetadataContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3ActorLoweringMetadataContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataSurfacePath)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(source_summary.contract_id)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(enforcement_summary.contract_id)
      << "\",\"hazard_contract_id\":\""
      << EscapeJsonString(hazard_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataLaneContract)
      << "\",\"metadata_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"actor_interface_sites\":" << contract.actor_interface_sites
      << ",\"actor_method_sites\":" << contract.actor_method_sites
      << ",\"actor_metadata_record_sites\":"
      << contract.actor_metadata_record_sites
      << ",\"nonisolated_entry_sites\":" << contract.nonisolated_entry_sites
      << ",\"executor_affinity_sites\":" << contract.executor_affinity_sites
      << ",\"actor_hop_artifact_sites\":"
      << contract.actor_hop_artifact_sites
      << ",\"actor_isolation_thunk_sites\":"
      << contract.actor_isolation_thunk_sites
      << ",\"replay_proof_dependency_sites\":"
      << contract.replay_proof_dependency_sites
      << ",\"race_guard_dependency_sites\":"
      << contract.race_guard_dependency_sites
      << ",\"task_handoff_sites\":" << contract.task_handoff_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildDispatchDispatchControlLoweringContractJson(
    const Objc3DispatchDispatchIntentSemanticModelSummary &semantic_summary,
    const Objc3DispatchDispatchIntentLegalitySummary &legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary &compatibility_summary,
    const Objc3DispatchDispatchControlLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3DispatchDispatchControlLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"legality_contract_id\":\""
      << EscapeJsonString(legality_summary.contract_id)
      << "\",\"compatibility_contract_id\":\""
      << EscapeJsonString(compatibility_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3DispatchDispatchControlLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"direct_call_candidate_sites\":"
      << contract.direct_call_candidate_sites
      << ",\"direct_members_defaulted_sites\":"
      << contract.direct_members_defaulted_sites
      << ",\"dynamic_opt_out_sites\":" << contract.dynamic_opt_out_sites
      << ",\"final_container_sites\":" << contract.final_container_sites
      << ",\"sealed_container_sites\":" << contract.sealed_container_sites
      << ",\"override_legality_sites\":" << contract.override_legality_sites
      << ",\"metadata_preserved_callable_sites\":"
      << contract.metadata_preserved_callable_sites
      << ",\"metadata_preserved_container_sites\":"
      << contract.metadata_preserved_container_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildMetaprogrammingExpansionLoweringContractJson(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_source_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &derive_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &macro_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &property_legality_summary,
    const Objc3MetaprogrammingExpansionLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3MetaprogrammingExpansionLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringSurfacePath)
      << "\",\"property_source_contract_id\":\""
      << EscapeJsonString(property_source_summary.contract_id)
      << "\",\"derive_contract_id\":\""
      << EscapeJsonString(derive_summary.contract_id)
      << "\",\"macro_contract_id\":\""
      << EscapeJsonString(macro_summary.contract_id)
      << "\",\"property_legality_contract_id\":\""
      << EscapeJsonString(property_legality_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"derive_inventory_sites\":" << contract.derive_inventory_sites
      << ",\"derived_selector_artifact_sites\":"
      << contract.derived_selector_artifact_sites
      << ",\"macro_replay_visible_sites\":"
      << contract.macro_replay_visible_sites
      << ",\"property_behavior_sites\":" << contract.property_behavior_sites
      << ",\"synthesized_binding_sites\":"
      << contract.synthesized_binding_sites
      << ",\"synthesized_getter_sites\":"
      << contract.synthesized_getter_sites
      << ",\"synthesized_setter_sites\":"
      << contract.synthesized_setter_sites
      << ",\"replay_visible_metadata_sites\":"
      << contract.replay_visible_metadata_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildInteropInteropLoweringContractJson(
    const Objc3InteropInteropSemanticModelSummary &semantic_summary,
    const Objc3InteropInteropRuntimeParitySummary &runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropSwiftInteropIsolationSummary &swift_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropInteropLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3InteropInteropLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"runtime_parity_contract_id\":\""
      << EscapeJsonString(runtime_parity_summary.contract_id)
      << "\",\"cpp_interaction_contract_id\":\""
      << EscapeJsonString(cpp_summary.contract_id)
      << "\",\"swift_isolation_contract_id\":\""
      << EscapeJsonString(swift_summary.contract_id)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(preservation_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"foreign_callable_sites\":" << contract.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << contract.c_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << contract.objc_runtime_parity_callable_sites
      << ",\"ownership_bridge_callable_sites\":"
      << contract.ownership_bridge_callable_sites
      << ",\"error_surface_sites\":" << contract.error_surface_sites
      << ",\"async_boundary_sites\":" << contract.async_boundary_sites
      << ",\"swift_concurrency_metadata_sites\":"
      << contract.swift_concurrency_metadata_sites
      << ",\"interface_preserved_foreign_callable_sites\":"
      << contract.interface_preserved_foreign_callable_sites
      << ",\"interface_preserved_metadata_annotation_sites\":"
      << contract.interface_preserved_metadata_annotation_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildInteropForeignCallLifetimeLoweringContractJson(
    const Objc3InteropInteropLoweringContract &dependency_contract,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropForeignCallLifetimeLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3InteropForeignCallLifetimeLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringSurfacePath)
      << "\",\"interop_contract_id\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringContractId)
      << "\",\"bridge_dependency_contract_id\":\""
      << EscapeJsonString(kObjc3InteropCppInteropInteractionSummaryContractId)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(preservation_summary.contract_id)
      << "\",\"dependency_replay_key\":\""
      << EscapeJsonString(
             Objc3InteropInteropLoweringReplayKey(dependency_contract))
      << "\",\"cpp_dependency_replay_key\":\""
      << EscapeJsonString(cpp_summary.replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringDeferredModel)
      << "\",\"foreign_callable_sites\":" << contract.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << contract.c_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << contract.objc_runtime_parity_callable_sites
      << ",\"ownership_bridge_sites\":" << contract.ownership_bridge_sites
      << ",\"lifetime_bridge_sites\":" << contract.lifetime_bridge_sites
      << ",\"metadata_preservation_sites\":"
      << contract.metadata_preservation_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
    const Objc3MetaprogrammingExpansionLoweringContract &dependency_contract,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3MetaprogrammingSynthesizedArtifactEmissionSurfacePath)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringContractId)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingSynthesizedArtifactEmissionLaneContract)
      << "\",\"emission_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingSynthesizedArtifactEmissionModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingSynthesizedArtifactEmissionDeferredModel)
      << "\",\"dependency_replay_key\":\""
      << EscapeJsonString(Objc3MetaprogrammingExpansionLoweringReplayKey(
             dependency_contract))
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"derive_inventory_sites\":" << contract.derive_inventory_sites
      << ",\"emitted_derive_method_sites\":"
      << contract.emitted_derive_method_sites
      << ",\"emitted_macro_artifact_sites\":"
      << contract.emitted_macro_artifact_sites
      << ",\"emitted_property_behavior_artifact_sites\":"
      << contract.emitted_property_behavior_artifact_sites
      << ",\"emitted_global_artifact_sites\":"
      << contract.emitted_global_artifact_sites
      << ",\"emitted_runtime_method_list_sites\":"
      << contract.emitted_runtime_method_list_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::vector<std::string> CollectInteropLocalImportModuleNames(
    const Objc3Program &program) {
  std::vector<std::string> names;
  const auto accumulate_callable = [&names](const auto &decl) {
    if (decl.objc_import_module_declared &&
        !decl.objc_import_module_name.empty()) {
      names.push_back(decl.objc_import_module_name);
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  std::sort(names.begin(), names.end());
  names.erase(std::unique(names.begin(), names.end()), names.end());
  return names;
}

Objc3InteropForeignSurfaceInterfacePreservationSummary
BuildInteropForeignSurfaceInterfacePreservationSummary(
    const Objc3Program &program,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &foreign_import_source_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &cpp_swift_source_summary,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  // preservation anchor: keep Part 11 truthful at the frontend
  // manifest/runtime-import-surface layer by preserving provider counts,
  // module-name payloads, and imported provider inventories before lane-B
  // semantic expansion or later ABI/runtime interop work.
  Objc3InteropForeignSurfaceInterfacePreservationSummary summary;
  summary.foreign_import_source_replay_key =
      foreign_import_source_summary.replay_key;
  summary.cpp_swift_source_replay_key = cpp_swift_source_summary.replay_key;
  summary.local_import_module_names_lexicographic =
      CollectInteropLocalImportModuleNames(program);
  summary.local_foreign_callable_count =
      foreign_import_source_summary.foreign_callable_sites;
  summary.local_import_module_annotation_count =
      foreign_import_source_summary.import_module_annotation_sites;
  summary.local_imported_module_name_count =
      foreign_import_source_summary.imported_module_name_sites;
  summary.local_swift_name_annotation_count =
      cpp_swift_source_summary.swift_name_annotation_sites;
  summary.local_swift_private_annotation_count =
      cpp_swift_source_summary.swift_private_annotation_sites;
  summary.local_cpp_name_annotation_count =
      cpp_swift_source_summary.cpp_name_annotation_sites;
  summary.local_header_name_annotation_count =
      cpp_swift_source_summary.header_name_annotation_sites;
  summary.local_named_annotation_payload_count =
      cpp_swift_source_summary.named_annotation_payload_sites;
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready &&
      foreign_import_source_summary.ready_for_semantic_expansion &&
      cpp_swift_source_summary.ready_for_semantic_expansion;
  summary.deterministic =
      foreign_import_source_summary.deterministic_handoff &&
      cpp_swift_source_summary.deterministic_handoff &&
      summary.local_import_module_names_lexicographic.size() <=
          summary.local_imported_module_name_count;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.interop_foreign_surface_interface_preservation_present) {
      continue;
    }
    ++summary.imported_module_count;
    if (!surface.frontend_closure_summary.module_name.empty()) {
      summary.imported_provider_module_names_lexicographic.push_back(
          surface.frontend_closure_summary.module_name);
    }
    summary.imported_foreign_callable_count +=
        surface.interop_local_foreign_callable_count;
    summary.imported_import_module_annotation_count +=
        surface.interop_local_import_module_annotation_count;
    summary.imported_imported_module_name_count +=
        surface.interop_local_imported_module_name_count;
    summary.imported_swift_name_annotation_count +=
        surface.interop_local_swift_name_annotation_count;
    summary.imported_swift_private_annotation_count +=
        surface.interop_local_swift_private_annotation_count;
    summary.imported_cpp_name_annotation_count +=
        surface.interop_local_cpp_name_annotation_count;
    summary.imported_header_name_annotation_count +=
        surface.interop_local_header_name_annotation_count;
    summary.imported_named_annotation_payload_count +=
        surface.interop_local_named_annotation_payload_count;
    summary.deterministic = summary.deterministic && surface.interop_deterministic;
  }
  std::sort(summary.imported_provider_module_names_lexicographic.begin(),
            summary.imported_provider_module_names_lexicographic.end());
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready &&
      summary.imported_provider_module_names_lexicographic.size() ==
          summary.imported_module_count;
  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_preservation_ready="
             << (summary.separate_compilation_preservation_ready ? "true"
                                                                 : "false")
             << ";imported_module_count=" << summary.imported_module_count
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";foreign_import_source_replay_key="
             << summary.foreign_import_source_replay_key
             << ";cpp_swift_source_replay_key="
             << summary.cpp_swift_source_replay_key
             << ";local_foreign_callable_count="
             << summary.local_foreign_callable_count
             << ";local_import_module_annotation_count="
             << summary.local_import_module_annotation_count
             << ";local_imported_module_name_count="
             << summary.local_imported_module_name_count
             << ";local_swift_name_annotation_count="
             << summary.local_swift_name_annotation_count
             << ";local_swift_private_annotation_count="
             << summary.local_swift_private_annotation_count
             << ";local_cpp_name_annotation_count="
             << summary.local_cpp_name_annotation_count
             << ";local_header_name_annotation_count="
             << summary.local_header_name_annotation_count
             << ";local_named_annotation_payload_count="
             << summary.local_named_annotation_payload_count
             << ";imported_foreign_callable_count="
             << summary.imported_foreign_callable_count
             << ";imported_import_module_annotation_count="
             << summary.imported_import_module_annotation_count
             << ";imported_imported_module_name_count="
             << summary.imported_imported_module_name_count
             << ";imported_swift_name_annotation_count="
             << summary.imported_swift_name_annotation_count
             << ";imported_swift_private_annotation_count="
             << summary.imported_swift_private_annotation_count
             << ";imported_cpp_name_annotation_count="
             << summary.imported_cpp_name_annotation_count
             << ";imported_header_name_annotation_count="
             << summary.imported_header_name_annotation_count
             << ";imported_named_annotation_payload_count="
             << summary.imported_named_annotation_payload_count;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
    const Objc3InteropForeignSurfaceInterfacePreservationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"foreign_import_source_contract_id\":\""
      << EscapeJsonString(summary.foreign_import_source_contract_id)
      << "\",\"cpp_swift_source_contract_id\":\""
      << EscapeJsonString(summary.cpp_swift_source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"foreign_import_source_replay_key\":\""
      << EscapeJsonString(summary.foreign_import_source_replay_key)
      << "\",\"cpp_swift_source_replay_key\":\""
      << EscapeJsonString(summary.cpp_swift_source_replay_key)
      << "\",\"local_import_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.local_import_module_names_lexicographic)
      << ",\"imported_provider_module_names_lexicographic\":"
      << BuildStringArrayJson(
             summary.imported_provider_module_names_lexicographic)
      << ",\"local_foreign_callable_count\":"
      << summary.local_foreign_callable_count
      << ",\"local_import_module_annotation_count\":"
      << summary.local_import_module_annotation_count
      << ",\"local_imported_module_name_count\":"
      << summary.local_imported_module_name_count
      << ",\"local_swift_name_annotation_count\":"
      << summary.local_swift_name_annotation_count
      << ",\"local_swift_private_annotation_count\":"
      << summary.local_swift_private_annotation_count
      << ",\"local_cpp_name_annotation_count\":"
      << summary.local_cpp_name_annotation_count
      << ",\"local_header_name_annotation_count\":"
      << summary.local_header_name_annotation_count
      << ",\"local_named_annotation_payload_count\":"
      << summary.local_named_annotation_payload_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"imported_foreign_callable_count\":"
      << summary.imported_foreign_callable_count
      << ",\"imported_import_module_annotation_count\":"
      << summary.imported_import_module_annotation_count
      << ",\"imported_imported_module_name_count\":"
      << summary.imported_imported_module_name_count
      << ",\"imported_swift_name_annotation_count\":"
      << summary.imported_swift_name_annotation_count
      << ",\"imported_swift_private_annotation_count\":"
      << summary.imported_swift_private_annotation_count
      << ",\"imported_cpp_name_annotation_count\":"
      << summary.imported_cpp_name_annotation_count
      << ",\"imported_header_name_annotation_count\":"
      << summary.imported_header_name_annotation_count
      << ",\"imported_named_annotation_payload_count\":"
      << summary.imported_named_annotation_payload_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

Objc3InteropHeaderModuleBridgeGenerationSummary
BuildInteropHeaderModuleBridgeGenerationSummary(
    const Objc3Program &program,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &ffi_preservation_contract,
    const std::string &ffi_preservation_replay_key,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3InteropHeaderModuleBridgeGenerationSummary summary;
  summary.local_import_module_names_lexicographic =
      preservation_summary.local_import_module_names_lexicographic;
  summary.local_foreign_callable_count =
      ffi_preservation_contract.local_foreign_callable_count;
  summary.local_import_module_name_count =
      preservation_summary.local_imported_module_name_count;
  summary.local_cpp_name_annotation_count =
      preservation_summary.local_cpp_name_annotation_count;
  summary.local_header_name_annotation_count =
      preservation_summary.local_header_name_annotation_count;
  summary.local_swift_name_annotation_count =
      preservation_summary.local_swift_name_annotation_count;
  summary.runtime_generation_ready =
      ffi_preservation_contract.runtime_import_artifact_ready &&
      ffi_preservation_contract.separate_compilation_preservation_ready &&
      ffi_preservation_contract.deterministic &&
      summary.local_foreign_callable_count > 0;
  summary.deterministic =
      preservation_summary.deterministic &&
      ffi_preservation_contract.deterministic &&
      summary.local_import_module_names_lexicographic.size() <=
          summary.local_import_module_name_count;
  summary.preservation_replay_key = ffi_preservation_replay_key;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.interop_header_module_bridge_generation_present) {
      continue;
    }
    ++summary.imported_module_count;
    if (!surface.frontend_closure_summary.module_name.empty()) {
      summary.imported_provider_module_names_lexicographic.push_back(
          surface.frontend_closure_summary.module_name);
    }
    summary.deterministic =
        summary.deterministic &&
        surface.interop_header_module_bridge_deterministic;
  }
  std::sort(summary.imported_provider_module_names_lexicographic.begin(),
            summary.imported_provider_module_names_lexicographic.end());
  summary.cross_module_packaging_ready =
      summary.runtime_generation_ready &&
      summary.imported_provider_module_names_lexicographic.size() ==
          summary.imported_module_count;

  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << ";source_contract_id=" << summary.source_contract_id
             << ";preservation_contract_id="
             << summary.preservation_contract_id
             << ";header_artifact_relative_path="
             << summary.header_artifact_relative_path
             << ";module_artifact_relative_path="
             << summary.module_artifact_relative_path
             << ";bridge_artifact_relative_path="
             << summary.bridge_artifact_relative_path
             << ";local_foreign_callable_count="
             << summary.local_foreign_callable_count
             << ";local_import_module_name_count="
             << summary.local_import_module_name_count
             << ";local_cpp_name_annotation_count="
             << summary.local_cpp_name_annotation_count
             << ";local_header_name_annotation_count="
             << summary.local_header_name_annotation_count
             << ";local_swift_name_annotation_count="
             << summary.local_swift_name_annotation_count
             << ";imported_module_count=" << summary.imported_module_count
             << ";runtime_generation_ready="
             << (summary.runtime_generation_ready ? "true" : "false")
             << ";cross_module_packaging_ready="
             << (summary.cross_module_packaging_ready ? "true" : "false")
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";preservation_replay_key="
             << summary.preservation_replay_key;
  summary.replay_key = replay_key.str();
  (void)program;
  return summary;
}

std::string BuildInteropHeaderModuleBridgeGenerationSummaryJson(
    const Objc3InteropHeaderModuleBridgeGenerationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(summary.preservation_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"generation_model\":\""
      << EscapeJsonString(summary.generation_model)
      << "\",\"packaging_model\":\""
      << EscapeJsonString(summary.packaging_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"header_artifact_relative_path\":\""
      << EscapeJsonString(summary.header_artifact_relative_path)
      << "\",\"module_artifact_relative_path\":\""
      << EscapeJsonString(summary.module_artifact_relative_path)
      << "\",\"bridge_artifact_relative_path\":\""
      << EscapeJsonString(summary.bridge_artifact_relative_path)
      << "\",\"local_import_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.local_import_module_names_lexicographic)
      << ",\"imported_provider_module_names_lexicographic\":"
      << BuildStringArrayJson(
             summary.imported_provider_module_names_lexicographic)
      << ",\"local_foreign_callable_count\":"
      << summary.local_foreign_callable_count
      << ",\"local_import_module_name_count\":"
      << summary.local_import_module_name_count
      << ",\"local_cpp_name_annotation_count\":"
      << summary.local_cpp_name_annotation_count
      << ",\"local_header_name_annotation_count\":"
      << summary.local_header_name_annotation_count
      << ",\"local_swift_name_annotation_count\":"
      << summary.local_swift_name_annotation_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"runtime_generation_ready\":"
      << (summary.runtime_generation_ready ? "true" : "false")
      << ",\"cross_module_packaging_ready\":"
      << (summary.cross_module_packaging_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"preservation_replay_key\":\""
      << EscapeJsonString(summary.preservation_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::size_t CountInteropInterfaceAnnotationSites(
    const Objc3InteropForeignSurfaceInterfacePreservationSummary &summary,
    bool imported) {
  if (imported) {
    return summary.imported_import_module_annotation_count +
           summary.imported_imported_module_name_count +
           summary.imported_swift_name_annotation_count +
           summary.imported_swift_private_annotation_count +
           summary.imported_cpp_name_annotation_count +
           summary.imported_header_name_annotation_count +
           summary.imported_named_annotation_payload_count;
  }
  return summary.local_import_module_annotation_count +
         summary.local_imported_module_name_count +
         summary.local_swift_name_annotation_count +
         summary.local_swift_private_annotation_count +
         summary.local_cpp_name_annotation_count +
         summary.local_header_name_annotation_count +
         summary.local_named_annotation_payload_count;
}

Objc3InteropFfiMetadataInterfacePreservationContract
BuildInteropFfiMetadataInterfacePreservationContract(
    const Objc3InteropForeignCallLifetimeLoweringContract &lowering_contract,
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    bool runtime_import_artifact_ready,
    std::string &replay_key_out) {
  Objc3InteropFfiMetadataInterfacePreservationContract contract;
  contract.local_foreign_callable_count =
      lowering_contract.foreign_callable_sites;
  contract.local_metadata_preservation_sites =
      lowering_contract.metadata_preservation_sites;
  contract.local_interface_annotation_sites =
      CountInteropInterfaceAnnotationSites(preservation_summary, false);
  contract.runtime_import_artifact_ready =
      runtime_import_artifact_ready && lowering_contract.deterministic &&
      preservation_summary.runtime_import_artifact_ready;
  contract.deterministic =
      lowering_contract.deterministic && preservation_summary.deterministic;

  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.interop_ffi_metadata_interface_preservation_present) {
      continue;
    }
    ++contract.imported_module_count;
    contract.imported_foreign_callable_count +=
        surface.interop_ffi_local_foreign_callable_count;
    contract.imported_metadata_preservation_sites +=
        surface.interop_ffi_local_metadata_preservation_sites;
    contract.imported_interface_annotation_sites +=
        surface.interop_ffi_local_interface_annotation_sites;
    contract.deterministic =
        contract.deterministic && surface.interop_ffi_deterministic;
  }
  contract.separate_compilation_preservation_ready =
      contract.runtime_import_artifact_ready;

  std::ostringstream replay_key;
  replay_key
      << Objc3InteropFfiMetadataInterfacePreservationReplayKey(contract)
      << ";lowering_replay_key=" << lowering_replay_key
      << ";preservation_replay_key=" << preservation_summary.replay_key;
  replay_key_out = replay_key.str();
  return contract;
}

std::string BuildInteropFfiMetadataInterfacePreservationContractJson(
    const Objc3InteropForeignCallLifetimeLoweringContract &lowering_contract,
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract,
    const std::string &replay_key) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3InteropFfiMetadataInterfacePreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationSourceContractId)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(
             kObjc3InteropForeignSurfaceInterfacePreservationContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationSurfacePath)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName)
      << "\",\"source_model\":\""
      << EscapeJsonString(kObjc3InteropFfiMetadataInterfacePreservationSourceModel)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationPreservationModel)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationFailClosedModel)
      << "\",\"lowering_replay_key\":\""
      << EscapeJsonString(lowering_replay_key)
      << "\",\"preservation_replay_key\":\""
      << EscapeJsonString(preservation_summary.replay_key)
      << "\",\"local_foreign_callable_count\":"
      << contract.local_foreign_callable_count
      << ",\"local_metadata_preservation_sites\":"
      << contract.local_metadata_preservation_sites
      << ",\"local_interface_annotation_sites\":"
      << contract.local_interface_annotation_sites
      << ",\"imported_module_count\":" << contract.imported_module_count
      << ",\"imported_foreign_callable_count\":"
      << contract.imported_foreign_callable_count
      << ",\"imported_metadata_preservation_sites\":"
      << contract.imported_metadata_preservation_sites
      << ",\"imported_interface_annotation_sites\":"
      << contract.imported_interface_annotation_sites
      << ",\"runtime_import_artifact_ready\":"
      << (contract.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (contract.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}

struct Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary {
  std::string contract_id =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId;
  std::string source_contract_id =
      kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId;
  std::string surface_path =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationSourceModel;
  std::string preservation_model =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationModel;
  std::string fail_closed_model =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationFailClosedModel;
  std::string replay_key;
  std::string expansion_lowering_replay_key;
  std::string synthesized_emission_replay_key;
  std::vector<std::string> imported_module_names_lexicographic;
  std::size_t local_derive_method_count = 0;
  std::size_t local_macro_artifact_count = 0;
  std::size_t local_interface_property_behavior_artifact_count = 0;
  std::size_t local_implementation_property_behavior_artifact_count = 0;
  std::size_t local_runtime_method_list_count = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_derive_method_count = 0;
  std::size_t imported_macro_artifact_count = 0;
  std::size_t imported_interface_property_behavior_artifact_count = 0;
  std::size_t imported_implementation_property_behavior_artifact_count = 0;
  std::size_t imported_runtime_method_list_count = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
};

struct Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary {
  std::string contract_id =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId;
  std::string source_contract_id =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId;
  std::string surface_path =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName;
  std::string host_executable_relative_path =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath;
  std::string cache_root_relative_path =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath;
  std::string host_model =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel;
  std::string toolchain_model =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel;
  std::string cache_model =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel;
  std::string fail_closed_model =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel;
  std::string replay_key;
  std::string metaprogramming_replay_key;
  std::size_t local_macro_artifact_count = 0;
  std::size_t local_property_behavior_artifact_count = 0;
  std::size_t imported_module_count = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_ready = false;
  bool deterministic = false;
};

std::size_t CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle> &bundles,
    std::string_view owner_kind) {
  return static_cast<std::size_t>(std::count_if(
      bundles.begin(), bundles.end(),
      [owner_kind](const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &bundle) {
        return bundle.owner_kind == owner_kind;
      }));
}

Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
BuildMetaprogrammingModuleInterfaceReplayPreservationSummary(
    const Objc3MetaprogrammingExpansionLoweringContract &expansion_contract,
    const std::string &expansion_replay_key,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
        &synthesized_contract,
    const std::string &synthesized_replay_key,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &property_behavior_bundles,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary summary;
  summary.expansion_lowering_replay_key = expansion_replay_key;
  summary.synthesized_emission_replay_key = synthesized_replay_key;
  summary.local_derive_method_count =
      synthesized_contract.emitted_derive_method_sites;
  summary.local_macro_artifact_count =
      synthesized_contract.emitted_macro_artifact_sites;
  summary.local_interface_property_behavior_artifact_count =
      CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
          property_behavior_bundles, "class-interface");
  summary.local_implementation_property_behavior_artifact_count =
      CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
          property_behavior_bundles, "class-implementation");
  summary.local_runtime_method_list_count =
      synthesized_contract.emitted_runtime_method_list_sites;
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready &&
      IsValidObjc3MetaprogrammingExpansionLoweringContract(expansion_contract) &&
      IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
          synthesized_contract);
  summary.deterministic =
      expansion_contract.deterministic && synthesized_contract.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.metaprogramming_module_interface_replay_preservation_present) {
      continue;
    }
    ++summary.imported_module_count;
    if (!surface.frontend_closure_summary.module_name.empty()) {
      summary.imported_module_names_lexicographic.push_back(
          surface.frontend_closure_summary.module_name);
    }
    summary.imported_derive_method_count +=
        surface.metaprogramming_local_derive_method_count;
    summary.imported_macro_artifact_count +=
        surface.metaprogramming_local_macro_artifact_count;
    summary.imported_interface_property_behavior_artifact_count +=
        surface.metaprogramming_local_interface_property_behavior_artifact_count;
    summary.imported_implementation_property_behavior_artifact_count +=
        surface.metaprogramming_local_implementation_property_behavior_artifact_count;
    summary.imported_runtime_method_list_count +=
        surface.metaprogramming_local_runtime_method_list_count;
    summary.deterministic =
        summary.deterministic && surface.metaprogramming_deterministic;
  }
  std::sort(summary.imported_module_names_lexicographic.begin(),
            summary.imported_module_names_lexicographic.end());
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready &&
      summary.imported_module_names_lexicographic.size() ==
          summary.imported_module_count;
  std::ostringstream replay_key;
  replay_key << Objc3MetaprogrammingModuleInterfaceReplayPreservationSummary()
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_preservation_ready="
             << (summary.separate_compilation_preservation_ready ? "true"
                                                                 : "false")
             << ";imported_module_count=" << summary.imported_module_count
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";expansion_lowering_replay_key=" << expansion_replay_key
             << ";synthesized_emission_replay_key=" << synthesized_replay_key
             << ";local_derive_method_count="
             << summary.local_derive_method_count
             << ";local_macro_artifact_count="
             << summary.local_macro_artifact_count
             << ";local_interface_property_behavior_artifact_count="
             << summary.local_interface_property_behavior_artifact_count
             << ";local_implementation_property_behavior_artifact_count="
             << summary.local_implementation_property_behavior_artifact_count
             << ";local_runtime_method_list_count="
             << summary.local_runtime_method_list_count
             << ";imported_derive_method_count="
             << summary.imported_derive_method_count
             << ";imported_macro_artifact_count="
             << summary.imported_macro_artifact_count
             << ";imported_interface_property_behavior_artifact_count="
             << summary.imported_interface_property_behavior_artifact_count
             << ";imported_implementation_property_behavior_artifact_count="
             << summary.imported_implementation_property_behavior_artifact_count
             << ";imported_runtime_method_list_count="
             << summary.imported_runtime_method_list_count;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"expansion_lowering_replay_key\":\""
      << EscapeJsonString(summary.expansion_lowering_replay_key)
      << "\",\"synthesized_emission_replay_key\":\""
      << EscapeJsonString(summary.synthesized_emission_replay_key)
      << "\",\"imported_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.imported_module_names_lexicographic)
      << ",\"local_derive_method_count\":"
      << summary.local_derive_method_count
      << ",\"local_macro_artifact_count\":"
      << summary.local_macro_artifact_count
      << ",\"local_interface_property_behavior_artifact_count\":"
      << summary.local_interface_property_behavior_artifact_count
      << ",\"local_implementation_property_behavior_artifact_count\":"
      << summary.local_implementation_property_behavior_artifact_count
      << ",\"local_runtime_method_list_count\":"
      << summary.local_runtime_method_list_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"imported_derive_method_count\":"
      << summary.imported_derive_method_count
      << ",\"imported_macro_artifact_count\":"
      << summary.imported_macro_artifact_count
      << ",\"imported_interface_property_behavior_artifact_count\":"
      << summary.imported_interface_property_behavior_artifact_count
      << ",\"imported_implementation_property_behavior_artifact_count\":"
      << summary.imported_implementation_property_behavior_artifact_count
      << ",\"imported_runtime_method_list_count\":"
      << summary.imported_runtime_method_list_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary
BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
        &module_interface_summary,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    const Objc3FrontendOptions &options) {
  Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary summary;
  if (!options.metaprogramming_cache_root_relative_path.empty()) {
    summary.cache_root_relative_path =
        options.metaprogramming_cache_root_relative_path;
  }
  summary.metaprogramming_replay_key = module_interface_summary.replay_key;
  summary.local_macro_artifact_count =
      module_interface_summary.local_macro_artifact_count;
  summary.local_property_behavior_artifact_count =
      module_interface_summary.local_interface_property_behavior_artifact_count +
      module_interface_summary
          .local_implementation_property_behavior_artifact_count;
  summary.runtime_import_artifact_ready =
      module_interface_summary.runtime_import_artifact_ready;
  summary.deterministic = module_interface_summary.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.metaprogramming_macro_host_process_cache_runtime_integration_present) {
      continue;
    }
    ++summary.imported_module_count;
    summary.deterministic =
        summary.deterministic &&
        surface.metaprogramming_macro_host_process_cache_deterministic;
  }
  summary.separate_compilation_ready =
      summary.runtime_import_artifact_ready &&
      module_interface_summary.separate_compilation_preservation_ready;
  std::ostringstream replay_key;
  replay_key << Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary()
             << ";metaprogramming_replay_key=" << module_interface_summary.replay_key
             << ";local_macro_artifact_count="
             << summary.local_macro_artifact_count
             << ";local_property_behavior_artifact_count="
             << summary.local_property_behavior_artifact_count
             << ";imported_module_count=" << summary.imported_module_count
             << ";cache_root_relative_path=" << summary.cache_root_relative_path
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_ready="
             << (summary.separate_compilation_ready ? "true" : "false")
             << ";deterministic="
             << (summary.deterministic ? "true" : "false");
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson(
    const Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"host_executable_relative_path\":\""
      << EscapeJsonString(summary.host_executable_relative_path)
      << "\",\"cache_root_relative_path\":\""
      << EscapeJsonString(summary.cache_root_relative_path)
      << "\",\"host_model\":\"" << EscapeJsonString(summary.host_model)
      << "\",\"toolchain_model\":\""
      << EscapeJsonString(summary.toolchain_model)
      << "\",\"cache_model\":\"" << EscapeJsonString(summary.cache_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"metaprogramming_replay_key\":\""
      << EscapeJsonString(summary.metaprogramming_replay_key)
      << "\",\"local_macro_artifact_count\":"
      << summary.local_macro_artifact_count
      << ",\"local_property_behavior_artifact_count\":"
      << summary.local_property_behavior_artifact_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_ready\":"
      << (summary.separate_compilation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipSystemExtensionLoweringContractJson(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &semantic_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &resource_summary,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &borrowed_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &family_summary,
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3OwnershipSystemExtensionLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"resource_semantic_contract_id\":\""
      << EscapeJsonString(resource_summary.contract_id)

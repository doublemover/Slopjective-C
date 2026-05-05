      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"manifest_authority_model\":\""
      << EscapeJsonString(summary.manifest_authority_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(summary) ? "true"
                                                                       : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"duplicate_registration_semantics_frozen\":"
      << (summary.duplicate_registration_semantics_frozen ? "true" : "false")
      << ",\"realization_order_semantics_frozen\":"
      << (summary.realization_order_semantics_frozen ? "true" : "false")
      << ",\"failure_mode_semantics_frozen\":"
      << (summary.failure_mode_semantics_frozen ? "true" : "false")
      << ",\"image_local_initialization_scope_frozen\":"
      << (summary.image_local_initialization_scope_frozen ? "true" : "false")
      << ",\"constructor_root_uniqueness_frozen\":"
      << (summary.constructor_root_uniqueness_frozen ? "true" : "false")
      << ",\"startup_execution_not_yet_landed\":"
      << (summary.startup_execution_not_yet_landed ? "true" : "false")
      << ",\"live_duplicate_registration_enforcement_not_yet_landed\":"
      << (summary.live_duplicate_registration_enforcement_not_yet_landed ? "true"
                                                                         : "false")
      << ",\"image_local_realization_not_yet_landed\":"
      << (summary.image_local_realization_not_yet_landed ? "true" : "false")
      << ",\"ready_for_bootstrap_implementation\":"
      << (summary.ready_for_bootstrap_implementation ? "true" : "false")
      << ",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeBootstrapSemanticsReplayKey(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_invariant_contract_id="
      << summary.bootstrap_invariant_contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";realization_order_policy=" << summary.realization_order_policy
      << ";failure_mode=" << summary.failure_mode
      << ";image_local_initialization_scope="
      << summary.image_local_initialization_scope
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";runtime_library_archive_relative_path="
      << summary.runtime_library_archive_relative_path
      << ";registration_result_model=" << summary.registration_result_model
      << ";registration_order_ordinal_model="
      << summary.registration_order_ordinal_model
      << ";runtime_state_snapshot_symbol="
      << summary.runtime_state_snapshot_symbol
      << ";success_status_code=" << summary.success_status_code
      << ";invalid_descriptor_status_code="
      << summary.invalid_descriptor_status_code
      << ";duplicate_registration_status_code="
      << summary.duplicate_registration_status_code
      << ";out_of_order_status_code="
      << summary.out_of_order_status_code
      << ";invalid_registration_roots_status_code="
      << summary.invalid_registration_roots_status_code
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";bootstrap_invariant_replay_key="
      << summary.bootstrap_invariant_replay_key
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapSemanticsSummary BuildRuntimeBootstrapSemanticsSummary(
    const Objc3RuntimeStartupBootstrapInvariantSummary &bootstrap_invariants,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  Objc3RuntimeBootstrapSemanticsSummary summary;
  summary.fail_closed = true;
  summary.bootstrap_invariant_contract_ready =
      IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(
          bootstrap_invariants);
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.registration_manifest_bootstrap_semantics_published =
      summary.registration_manifest_contract_ready;
  summary.live_runtime_enforcement_landed = true;
  summary.runtime_probe_required = true;
  summary.no_partial_commit_on_failure = true;
  summary.ready_for_constructor_root_implementation =
      summary.bootstrap_invariant_contract_ready &&
      summary.registration_manifest_contract_ready;
  summary.translation_unit_registration_order_ordinal =
      registration_manifest.translation_unit_registration_order_ordinal;
  if (summary.bootstrap_invariant_contract_ready) {
    summary.bootstrap_invariant_replay_key = bootstrap_invariants.replay_key;
  }
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.ready_for_constructor_root_implementation) {
    summary.replay_key = BuildRuntimeBootstrapSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapSemanticsSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap semantics summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapSemanticsSummaryJson(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"bootstrap_invariant_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_invariant_contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"duplicate_registration_policy\":\""
      << EscapeJsonString(summary.duplicate_registration_policy)
      << "\",\"realization_order_policy\":\""
      << EscapeJsonString(summary.realization_order_policy)
      << "\",\"failure_mode\":\""
      << EscapeJsonString(summary.failure_mode)
      << "\",\"image_local_initialization_scope\":\""
      << EscapeJsonString(summary.image_local_initialization_scope)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"manifest_authority_model\":\""
      << EscapeJsonString(summary.manifest_authority_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"runtime_library_archive_relative_path\":\""
      << EscapeJsonString(summary.runtime_library_archive_relative_path)
      << "\",\"registration_result_model\":\""
      << EscapeJsonString(summary.registration_result_model)
      << "\",\"registration_order_ordinal_model\":\""
      << EscapeJsonString(summary.registration_order_ordinal_model)
      << "\",\"runtime_state_snapshot_symbol\":\""
      << EscapeJsonString(summary.runtime_state_snapshot_symbol)
      << "\",\"success_status_code\":" << summary.success_status_code
      << ",\"invalid_descriptor_status_code\":"
      << summary.invalid_descriptor_status_code
      << ",\"duplicate_registration_status_code\":"
      << summary.duplicate_registration_status_code
      << ",\"out_of_order_status_code\":"
      << summary.out_of_order_status_code
      << ",\"invalid_registration_roots_status_code\":"
      << summary.invalid_registration_roots_status_code
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapSemanticsSummary(summary) ? "true"
                                                                : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"bootstrap_invariant_contract_ready\":"
      << (summary.bootstrap_invariant_contract_ready ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"live_runtime_enforcement_landed\":"
      << (summary.live_runtime_enforcement_landed ? "true" : "false")
      << ",\"registration_manifest_bootstrap_semantics_published\":"
      << (summary.registration_manifest_bootstrap_semantics_published ? "true"
                                                                      : "false")
      << ",\"runtime_probe_required\":"
      << (summary.runtime_probe_required ? "true" : "false")
      << ",\"no_partial_commit_on_failure\":"
      << (summary.no_partial_commit_on_failure ? "true" : "false")
      << ",\"ready_for_constructor_root_implementation\":"
      << (summary.ready_for_constructor_root_implementation ? "true" : "false")
      << ",\"bootstrap_invariant_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_invariant_replay_key)
      << "\",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeBootstrapLoweringReplayKey(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";registration_descriptor_frontend_closure_contract_id="
      << summary.registration_descriptor_frontend_closure_contract_id
      << ";registration_descriptor_artifact="
      << summary.registration_descriptor_artifact
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";lowering_boundary_model=" << summary.lowering_boundary_model
      << ";registration_descriptor_handoff_model="
      << summary.registration_descriptor_handoff_model
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_init_stub_symbol_prefix="
      << summary.constructor_init_stub_symbol_prefix
      << ";registration_table_symbol_prefix="
      << summary.registration_table_symbol_prefix
      << ";image_local_init_state_symbol_prefix="
      << summary.image_local_init_state_symbol_prefix
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";global_ctor_list_model=" << summary.global_ctor_list_model
      << ";registration_table_layout_model="
      << summary.registration_table_layout_model
      << ";image_local_initialization_model="
      << summary.image_local_initialization_model
      << ";registration_table_abi_version="
      << summary.registration_table_abi_version
      << ";registration_table_pointer_field_count="
      << summary.registration_table_pointer_field_count
      << ";constructor_root_emission_state="
      << summary.constructor_root_emission_state
      << ";init_stub_emission_state=" << summary.init_stub_emission_state
      << ";registration_table_emission_state="
      << summary.registration_table_emission_state
      << ";bootstrap_ir_materialization_landed="
      << (summary.bootstrap_ir_materialization_landed ? "true" : "false")
      << ";image_local_initialization_landed="
      << (summary.image_local_initialization_landed ? "true" : "false")
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key
      << ";registration_descriptor_frontend_closure_replay_key="
      << summary.registration_descriptor_frontend_closure_replay_key;
  return out.str();
}

// constructor-root/init-array lowering anchor: the canonical
// lowering summary now freezes the live ctor-root/global_ctors materialization
// path and requires the emitted registration-descriptor frontend closure to be
// ready before the handoff is considered valid.
Objc3RuntimeBootstrapLoweringSummary BuildRuntimeBootstrapLoweringSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &registration_descriptor_frontend_closure) {
  Objc3RuntimeBootstrapLoweringSummary summary;
  summary.fail_closed = true;
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.registration_descriptor_frontend_closure_contract_ready =
      IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          registration_descriptor_frontend_closure);
  summary.lowering_contract_published = true;
  summary.manifest_authority_preserved =
      summary.registration_manifest_contract_ready &&
      registration_manifest.constructor_root_manifest_authoritative;
  summary.no_bootstrap_ir_materialization_yet = false;
  summary.bootstrap_ir_materialization_landed =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready;
  summary.image_local_initialization_landed =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready;
  summary.ready_for_bootstrap_materialization =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready;
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.bootstrap_semantics_contract_ready) {
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }
  if (summary.registration_descriptor_frontend_closure_contract_ready) {
    summary.registration_descriptor_frontend_closure_replay_key =
        registration_descriptor_frontend_closure.replay_key;
  }
  if (summary.ready_for_bootstrap_materialization) {
    summary.replay_key = BuildRuntimeBootstrapLoweringReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapLoweringSummary(summary)) {
    summary.failure_reason = "runtime bootstrap lowering summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapLoweringSummaryJson(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_semantics_contract_id)
      << "\",\"registration_descriptor_frontend_closure_contract_id\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_contract_id)
      << "\",\"registration_descriptor_artifact\":\""
      << EscapeJsonString(summary.registration_descriptor_artifact)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"lowering_boundary_model\":\""
      << EscapeJsonString(summary.lowering_boundary_model)
      << "\",\"registration_descriptor_handoff_model\":\""
      << EscapeJsonString(summary.registration_descriptor_handoff_model)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_init_stub_symbol_prefix\":\""
      << EscapeJsonString(summary.constructor_init_stub_symbol_prefix)
      << "\",\"registration_table_symbol_prefix\":\""
      << EscapeJsonString(summary.registration_table_symbol_prefix)
      << "\",\"image_local_init_state_symbol_prefix\":\""
      << EscapeJsonString(summary.image_local_init_state_symbol_prefix)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"global_ctor_list_model\":\""
      << EscapeJsonString(summary.global_ctor_list_model)
      << "\",\"registration_table_layout_model\":\""
      << EscapeJsonString(summary.registration_table_layout_model)
      << "\",\"image_local_initialization_model\":\""
      << EscapeJsonString(summary.image_local_initialization_model)
      << "\",\"registration_table_abi_version\":"
      << summary.registration_table_abi_version
      << ",\"registration_table_pointer_field_count\":"
      << summary.registration_table_pointer_field_count
      << ",\"constructor_root_emission_state\":\""
      << EscapeJsonString(summary.constructor_root_emission_state)
      << "\",\"init_stub_emission_state\":\""
      << EscapeJsonString(summary.init_stub_emission_state)
      << "\",\"registration_table_emission_state\":\""
      << EscapeJsonString(summary.registration_table_emission_state)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapLoweringSummary(summary) ? "true"
                                                               : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"registration_descriptor_frontend_closure_contract_ready\":"
      << (summary.registration_descriptor_frontend_closure_contract_ready
              ? "true"
              : "false")
      << ",\"lowering_contract_published\":"
      << (summary.lowering_contract_published ? "true" : "false")
      << ",\"manifest_authority_preserved\":"
      << (summary.manifest_authority_preserved ? "true" : "false")
      << ",\"no_bootstrap_ir_materialization_yet\":"
      << (summary.no_bootstrap_ir_materialization_yet ? "true" : "false")
      << ",\"bootstrap_ir_materialization_landed\":"
      << (summary.bootstrap_ir_materialization_landed ? "true" : "false")
      << ",\"image_local_initialization_landed\":"
      << (summary.image_local_initialization_landed ? "true" : "false")
      << ",\"ready_for_bootstrap_materialization\":"
      << (summary.ready_for_bootstrap_materialization ? "true" : "false")
      << ",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceReplayKey(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";source_surface_path=" << summary.source_surface_path
      << ";registration_descriptor_pragma_name="
      << summary.registration_descriptor_pragma_name
      << ";image_root_pragma_name=" << summary.image_root_pragma_name
      << ";module_identity_source=" << summary.module_identity_source
      << ";module_name=" << summary.module_name
      << ";registration_descriptor_identifier="
      << summary.registration_descriptor_identifier
      << ";registration_descriptor_identity_source="
      << summary.registration_descriptor_identity_source
      << ";image_root_identifier=" << summary.image_root_identifier
      << ";image_root_identity_source=" << summary.image_root_identity_source
      << ";bootstrap_visible_metadata_ownership_model="
      << summary.bootstrap_visible_metadata_ownership_model
      << ";registration_descriptor_pragma_seen="
      << (summary.registration_descriptor_pragma_seen ? "true" : "false")
      << ";registration_descriptor_pragma_duplicate="
      << (summary.registration_descriptor_pragma_duplicate ? "true" : "false")
      << ";registration_descriptor_pragma_non_leading="
      << (summary.registration_descriptor_pragma_non_leading ? "true"
                                                             : "false")
      << ";registration_descriptor_pragma_directive_count="
      << summary.registration_descriptor_pragma_directive_count
      << ";image_root_pragma_seen="
      << (summary.image_root_pragma_seen ? "true" : "false")
      << ";image_root_pragma_duplicate="
      << (summary.image_root_pragma_duplicate ? "true" : "false")
      << ";image_root_pragma_non_leading="
      << (summary.image_root_pragma_non_leading ? "true" : "false")
      << ";image_root_pragma_directive_count="
      << summary.image_root_pragma_directive_count
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
    const Objc3Program &program,
    const Objc3FrontendBootstrapRegistrationSourcePragmaContract
        &bootstrap_registration_source_pragma_contract,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary summary;
  summary.fail_closed = true;
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.source_surface_frozen = true;
  summary.prelude_pragma_contract_published = true;
  summary.bootstrap_visible_metadata_ownership_published = true;
  summary.module_name =
      program.module_name.empty() ? "objc3_module" : program.module_name;
  summary.registration_descriptor_pragma_seen =
      bootstrap_registration_source_pragma_contract.registration_descriptor.seen;
  summary.registration_descriptor_pragma_duplicate =
      bootstrap_registration_source_pragma_contract.registration_descriptor
          .duplicate;
  summary.registration_descriptor_pragma_non_leading =
      bootstrap_registration_source_pragma_contract.registration_descriptor
          .non_leading;
  summary.registration_descriptor_pragma_directive_count =
      bootstrap_registration_source_pragma_contract.registration_descriptor
          .directive_count;
  summary.image_root_pragma_seen =
      bootstrap_registration_source_pragma_contract.image_root.seen;
  summary.image_root_pragma_duplicate =
      bootstrap_registration_source_pragma_contract.image_root.duplicate;
  summary.image_root_pragma_non_leading =
      bootstrap_registration_source_pragma_contract.image_root.non_leading;
  summary.image_root_pragma_directive_count =
      bootstrap_registration_source_pragma_contract.image_root.directive_count;

  const std::string safe_module_name =
      objc3c::support::MakeIdentifierSafeSuffix(summary.module_name, "objc3_module");
  if (summary.registration_descriptor_pragma_seen &&
      !bootstrap_registration_source_pragma_contract.registration_descriptor
           .identifier.empty()) {
    summary.registration_descriptor_identifier =
        bootstrap_registration_source_pragma_contract.registration_descriptor
            .identifier;
    summary.registration_descriptor_identity_source =
        kObjc3RuntimeBootstrapDerivedIdentitySourcePragma;
  } else {
    summary.registration_descriptor_identifier =
        safe_module_name +
        kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  }
  if (summary.image_root_pragma_seen &&
      !bootstrap_registration_source_pragma_contract.image_root.identifier
           .empty()) {
    summary.image_root_identifier =
        bootstrap_registration_source_pragma_contract.image_root.identifier;
    summary.image_root_identity_source =
        kObjc3RuntimeBootstrapDerivedIdentitySourcePragma;
  } else {
    summary.image_root_identifier =
        safe_module_name + kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  }

  summary.registration_descriptor_identifier_resolved =
      !summary.registration_descriptor_identifier.empty();
  summary.image_root_identifier_resolved =
      !summary.image_root_identifier.empty();
  summary.ready_for_descriptor_frontend_closure =
      summary.registration_manifest_contract_ready &&
      summary.registration_descriptor_identifier_resolved &&
      summary.image_root_identifier_resolved;
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.ready_for_descriptor_frontend_closure) {
    summary.replay_key =
        BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceReplayKey(
            summary);
  }
  if (!IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
          summary)) {
    summary.failure_reason =
        "registration descriptor/image-root source surface summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummaryJson(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"source_surface_path\":\""
      << EscapeJsonString(summary.source_surface_path)
      << "\",\"registration_descriptor_pragma_name\":\""
      << EscapeJsonString(summary.registration_descriptor_pragma_name)
      << "\",\"image_root_pragma_name\":\""
      << EscapeJsonString(summary.image_root_pragma_name)
      << "\",\"module_identity_source\":\""
      << EscapeJsonString(summary.module_identity_source)
      << "\",\"registration_descriptor_identity_source\":\""
      << EscapeJsonString(summary.registration_descriptor_identity_source)
      << "\",\"image_root_identity_source\":\""
      << EscapeJsonString(summary.image_root_identity_source)
      << "\",\"bootstrap_visible_metadata_ownership_model\":\""
      << EscapeJsonString(summary.bootstrap_visible_metadata_ownership_model)
      << "\",\"module_name\":\"" << EscapeJsonString(summary.module_name)
      << "\",\"registration_descriptor_identifier\":\""
      << EscapeJsonString(summary.registration_descriptor_identifier)
      << "\",\"image_root_identifier\":\""
      << EscapeJsonString(summary.image_root_identifier)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
              summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"source_surface_frozen\":"
      << (summary.source_surface_frozen ? "true" : "false")
      << ",\"prelude_pragma_contract_published\":"
      << (summary.prelude_pragma_contract_published ? "true" : "false")
      << ",\"registration_descriptor_identifier_resolved\":"
      << (summary.registration_descriptor_identifier_resolved ? "true"
                                                              : "false")
      << ",\"image_root_identifier_resolved\":"
      << (summary.image_root_identifier_resolved ? "true" : "false")
      << ",\"bootstrap_visible_metadata_ownership_published\":"
      << (summary.bootstrap_visible_metadata_ownership_published ? "true"
                                                                 : "false")
      << ",\"ready_for_descriptor_frontend_closure\":"
      << (summary.ready_for_descriptor_frontend_closure ? "true" : "false")
      << ",\"registration_descriptor_pragma_seen\":"
      << (summary.registration_descriptor_pragma_seen ? "true" : "false")
      << ",\"registration_descriptor_pragma_duplicate\":"
      << (summary.registration_descriptor_pragma_duplicate ? "true" : "false")
      << ",\"registration_descriptor_pragma_non_leading\":"
      << (summary.registration_descriptor_pragma_non_leading ? "true"
                                                             : "false")
      << ",\"registration_descriptor_pragma_directive_count\":"
      << summary.registration_descriptor_pragma_directive_count
      << ",\"image_root_pragma_seen\":"
      << (summary.image_root_pragma_seen ? "true" : "false")
      << ",\"image_root_pragma_duplicate\":"
      << (summary.image_root_pragma_duplicate ? "true" : "false")
      << ",\"image_root_pragma_non_leading\":"
      << (summary.image_root_pragma_non_leading ? "true" : "false")
      << ",\"image_root_pragma_directive_count\":"
      << summary.image_root_pragma_directive_count
      << ",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeRegistrationDescriptorFrontendClosureReplayKey(
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_surface_contract_id=" << summary.source_surface_contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";payload_model=" << summary.payload_model
      << ";artifact_relative_path=" << summary.artifact_relative_path
      << ";authority_model=" << summary.authority_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";payload_ownership_model=" << summary.payload_ownership_model
      << ";registration_descriptor_identifier="
      << summary.registration_descriptor_identifier
      << ";image_root_identifier=" << summary.image_root_identifier
      << ";registration_descriptor_identity_source="
      << summary.registration_descriptor_identity_source
      << ";image_root_identity_source="
      << summary.image_root_identity_source
      << ";bootstrap_visible_metadata_ownership_model="
      << summary.bootstrap_visible_metadata_ownership_model
      << ";class_descriptor_count=" << summary.class_descriptor_count
      << ";protocol_descriptor_count=" << summary.protocol_descriptor_count
      << ";category_descriptor_count=" << summary.category_descriptor_count
      << ";property_descriptor_count=" << summary.property_descriptor_count
      << ";ivar_descriptor_count=" << summary.ivar_descriptor_count
      << ";total_descriptor_count=" << summary.total_descriptor_count
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";source_surface_replay_key=" << summary.source_surface_replay_key
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
BuildRuntimeRegistrationDescriptorFrontendClosureSummary(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &source_surface_summary,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest_summary) {
  Objc3RuntimeRegistrationDescriptorFrontendClosureSummary summary;
  summary.fail_closed = true;
  summary.source_surface_contract_ready =
      IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
          source_surface_summary);
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest_summary);
  summary.descriptor_frontend_surface_published = true;
  summary.descriptor_artifact_template_published = true;
  summary.registration_descriptor_identifier =
      source_surface_summary.registration_descriptor_identifier;
  summary.image_root_identifier = source_surface_summary.image_root_identifier;
  summary.registration_descriptor_identity_source =
      source_surface_summary.registration_descriptor_identity_source;
  summary.image_root_identity_source =
      source_surface_summary.image_root_identity_source;
  summary.bootstrap_visible_metadata_ownership_model =
      source_surface_summary.bootstrap_visible_metadata_ownership_model;
  summary.class_descriptor_count =
      registration_manifest_summary.class_descriptor_count;
  summary.protocol_descriptor_count =
      registration_manifest_summary.protocol_descriptor_count;
  summary.category_descriptor_count =
      registration_manifest_summary.category_descriptor_count;
  summary.property_descriptor_count =
      registration_manifest_summary.property_descriptor_count;
  summary.ivar_descriptor_count =
      registration_manifest_summary.ivar_descriptor_count;
  summary.total_descriptor_count =
      registration_manifest_summary.total_descriptor_count;
  summary.translation_unit_registration_order_ordinal =
      registration_manifest_summary.translation_unit_registration_order_ordinal;
  summary.descriptor_fields_resolved =
      !summary.registration_descriptor_identifier.empty() &&
      !summary.image_root_identifier.empty();
  summary.ready_for_descriptor_artifact_emission =
      summary.source_surface_contract_ready &&
      summary.registration_manifest_contract_ready &&
      summary.descriptor_fields_resolved;
  summary.ready_for_registration_descriptor_lowering =
      summary.ready_for_descriptor_artifact_emission;
  if (summary.source_surface_contract_ready) {
    summary.source_surface_replay_key = source_surface_summary.replay_key;
  }
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key =
        registration_manifest_summary.replay_key;
  }
  if (summary.ready_for_registration_descriptor_lowering) {
    summary.replay_key =
        BuildRuntimeRegistrationDescriptorFrontendClosureReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          summary)) {
    summary.failure_reason =
        "registration descriptor frontend closure summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeRegistrationDescriptorFrontendClosureSummaryJson(
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"source_surface_contract_id\":\""
      << EscapeJsonString(summary.source_surface_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\""
      << EscapeJsonString(summary.payload_model)
      << "\",\"artifact_relative_path\":\""
      << EscapeJsonString(summary.artifact_relative_path)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"payload_ownership_model\":\""
      << EscapeJsonString(summary.payload_ownership_model)
      << "\",\"registration_descriptor_identifier\":\""
      << EscapeJsonString(summary.registration_descriptor_identifier)
      << "\",\"image_root_identifier\":\""
      << EscapeJsonString(summary.image_root_identifier)
      << "\",\"registration_descriptor_identity_source\":\""
      << EscapeJsonString(summary.registration_descriptor_identity_source)
      << "\",\"image_root_identity_source\":\""
      << EscapeJsonString(summary.image_root_identity_source)
      << "\",\"bootstrap_visible_metadata_ownership_model\":\""
      << EscapeJsonString(summary.bootstrap_visible_metadata_ownership_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"source_surface_contract_ready\":"
      << (summary.source_surface_contract_ready ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"descriptor_frontend_surface_published\":"
      << (summary.descriptor_frontend_surface_published ? "true" : "false")
      << ",\"descriptor_artifact_template_published\":"
      << (summary.descriptor_artifact_template_published ? "true" : "false")
      << ",\"descriptor_fields_resolved\":"
      << (summary.descriptor_fields_resolved ? "true" : "false")
      << ",\"ready_for_descriptor_artifact_emission\":"
      << (summary.ready_for_descriptor_artifact_emission ? "true" : "false")
      << ",\"ready_for_registration_descriptor_lowering\":"
      << (summary.ready_for_registration_descriptor_lowering ? "true" : "false")
      << ",\"class_descriptor_count\":" << summary.class_descriptor_count
      << ",\"protocol_descriptor_count\":"
      << summary.protocol_descriptor_count
      << ",\"category_descriptor_count\":"
      << summary.category_descriptor_count
      << ",\"property_descriptor_count\":"
      << summary.property_descriptor_count
      << ",\"ivar_descriptor_count\":" << summary.ivar_descriptor_count
      << ",\"total_descriptor_count\":" << summary.total_descriptor_count
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"source_surface_replay_key\":\""
      << EscapeJsonString(summary.source_surface_replay_key)
      << "\",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\""
      << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeBootstrapLegalityFailureContractReplayKey(
    const Objc3RuntimeBootstrapLegalityFailureContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_descriptor_frontend_closure_contract_id="
      << summary.registration_descriptor_frontend_closure_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";runtime_state_snapshot_symbol="
      << summary.runtime_state_snapshot_symbol
      << ";registration_descriptor_identifier="
      << summary.registration_descriptor_identifier
      << ";image_root_identifier=" << summary.image_root_identifier
      << ";registration_descriptor_identity_source="
      << summary.registration_descriptor_identity_source
      << ";image_root_identity_source="
      << summary.image_root_identity_source
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";registration_descriptor_frontend_closure_replay_key="
      << summary.registration_descriptor_frontend_closure_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key
      << ";semantic_boundary_replay_key="
      << summary.semantic_boundary_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapLegalityFailureContractSummary
BuildRuntimeBootstrapLegalityFailureContractSummary(
    const Objc3BootstrapLegalityFailureContractSummary &semantic_boundary,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics) {
  Objc3RuntimeBootstrapLegalityFailureContractSummary summary;
  summary.fail_closed = true;
  summary.registration_descriptor_frontend_closure_contract_ready =
      IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          registration_descriptor_frontend_closure);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.semantic_boundary_ready =
      IsReadyObjc3BootstrapLegalityFailureContractSummary(semantic_boundary);

  if (summary.semantic_boundary_ready) {
    summary.duplicate_registration_policy =
        semantic_boundary.duplicate_registration_policy;
    summary.image_registration_order_invariant =
        semantic_boundary.image_registration_order_invariant;
    summary.failure_mode = semantic_boundary.failure_mode;
    summary.restart_lifecycle_model =
        semantic_boundary.restart_lifecycle_model;
    summary.replay_order_model = semantic_boundary.replay_order_model;
    summary.image_local_init_reset_model =
        semantic_boundary.image_local_init_reset_model;
    summary.catalog_retention_model =
        semantic_boundary.catalog_retention_model;
    summary.runtime_state_snapshot_symbol =
        semantic_boundary.runtime_state_snapshot_symbol;
    summary.duplicate_registration_policy_frozen =
        semantic_boundary.duplicate_registration_policy_frozen;
    summary.image_order_invariant_frozen =
        semantic_boundary.image_order_invariant_frozen;
    summary.bootstrap_rejection_frozen =
        semantic_boundary.bootstrap_rejection_frozen;
    summary.restart_boundary_frozen =
        semantic_boundary.restart_boundary_frozen;
    summary.semantic_diagnostics_required =
        semantic_boundary.semantic_diagnostics_required;
    summary.semantic_boundary_replay_key = semantic_boundary.replay_key;
  }

  if (summary.registration_descriptor_frontend_closure_contract_ready) {
    summary.registration_descriptor_identifier =
        registration_descriptor_frontend_closure
            .registration_descriptor_identifier;
    summary.image_root_identifier =
        registration_descriptor_frontend_closure.image_root_identifier;
    summary.registration_descriptor_identity_source =
        registration_descriptor_frontend_closure
            .registration_descriptor_identity_source;
    summary.image_root_identity_source =
        registration_descriptor_frontend_closure.image_root_identity_source;
    summary.translation_unit_registration_order_ordinal =
        registration_descriptor_frontend_closure
            .translation_unit_registration_order_ordinal;
    summary.registration_descriptor_frontend_closure_replay_key =
        registration_descriptor_frontend_closure.replay_key;
  }

  if (summary.bootstrap_semantics_contract_ready) {
    summary.translation_unit_registration_order_ordinal =
        bootstrap_semantics.translation_unit_registration_order_ordinal;
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }

  summary.ready_for_lowering_and_runtime =
      summary.registration_descriptor_frontend_closure_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.semantic_boundary_ready &&
      summary.duplicate_registration_policy ==
          bootstrap_semantics.duplicate_registration_policy &&
      summary.image_registration_order_invariant ==
          bootstrap_semantics.registration_order_ordinal_model &&
      summary.failure_mode == bootstrap_semantics.failure_mode &&
      summary.runtime_state_snapshot_symbol ==
          bootstrap_semantics.runtime_state_snapshot_symbol &&
      summary.translation_unit_registration_order_ordinal ==
          bootstrap_semantics.translation_unit_registration_order_ordinal;

  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildRuntimeBootstrapLegalityFailureContractReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap legality duplicate/order/failure contract summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapLegalityFailureContractSummaryJson(
    const Objc3RuntimeBootstrapLegalityFailureContractSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_descriptor_frontend_closure_contract_id\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_contract_id)
      << "\",\"bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"duplicate_registration_policy\":\""
      << EscapeJsonString(summary.duplicate_registration_policy)
      << "\",\"image_registration_order_invariant\":\""
      << EscapeJsonString(summary.image_registration_order_invariant)
      << "\",\"failure_mode\":\""
      << EscapeJsonString(summary.failure_mode)
      << "\",\"restart_lifecycle_model\":\""
      << EscapeJsonString(summary.restart_lifecycle_model)
      << "\",\"replay_order_model\":\""
      << EscapeJsonString(summary.replay_order_model)
      << "\",\"image_local_init_reset_model\":\""
      << EscapeJsonString(summary.image_local_init_reset_model)
      << "\",\"catalog_retention_model\":\""
      << EscapeJsonString(summary.catalog_retention_model)
      << "\",\"runtime_state_snapshot_symbol\":\""
      << EscapeJsonString(summary.runtime_state_snapshot_symbol)
      << "\",\"registration_descriptor_identifier\":\""
      << EscapeJsonString(summary.registration_descriptor_identifier)
      << "\",\"image_root_identifier\":\""
      << EscapeJsonString(summary.image_root_identifier)
      << "\",\"registration_descriptor_identity_source\":\""
      << EscapeJsonString(summary.registration_descriptor_identity_source)
      << "\",\"image_root_identity_source\":\""
      << EscapeJsonString(summary.image_root_identity_source)
      << "\",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_descriptor_frontend_closure_contract_ready\":"
      << (summary.registration_descriptor_frontend_closure_contract_ready
              ? "true"
              : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"semantic_boundary_ready\":"
      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"duplicate_registration_policy_frozen\":"
      << (summary.duplicate_registration_policy_frozen ? "true" : "false")
      << ",\"image_order_invariant_frozen\":"
      << (summary.image_order_invariant_frozen ? "true" : "false")
      << ",\"bootstrap_rejection_frozen\":"
      << (summary.bootstrap_rejection_frozen ? "true" : "false")
      << ",\"restart_boundary_frozen\":"
      << (summary.restart_boundary_frozen ? "true" : "false")
      << ",\"semantic_diagnostics_required\":"
      << (summary.semantic_diagnostics_required ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeBootstrapLegalitySemanticsReplayKey(
    const Objc3RuntimeBootstrapLegalitySemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_failure_contract_id="
      << summary.bootstrap_legality_failure_contract_id
      << ";registration_descriptor_frontend_closure_contract_id="
      << summary.registration_descriptor_frontend_closure_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";cross_image_legality_model=" << summary.cross_image_legality_model
      << ";semantic_diagnostic_model=" << summary.semantic_diagnostic_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";translation_unit_identity_key=" << summary.translation_unit_identity_key
      << ";registration_descriptor_identifier="
      << summary.registration_descriptor_identifier
      << ";image_root_identifier=" << summary.image_root_identifier
      << ";registration_descriptor_identity_source="
      << summary.registration_descriptor_identity_source
      << ";image_root_identity_source=" << summary.image_root_identity_source
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";semantic_boundary_replay_key=" << summary.semantic_boundary_replay_key
      << ";bootstrap_legality_failure_contract_replay_key="
      << summary.bootstrap_legality_failure_contract_replay_key
      << ";registration_descriptor_frontend_closure_replay_key="
      << summary.registration_descriptor_frontend_closure_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapLegalitySemanticsSummary
BuildRuntimeBootstrapLegalitySemanticsSummary(
    const Objc3BootstrapLegalitySemanticsSummary &semantic_boundary,
    const Objc3RuntimeBootstrapLegalityFailureContractSummary
        &bootstrap_legality_failure_contract,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics,
    const std::string &translation_unit_identity_key) {
  Objc3RuntimeBootstrapLegalitySemanticsSummary summary;
  summary.fail_closed = true;
  summary.semantic_boundary_ready =
      IsReadyObjc3BootstrapLegalitySemanticsSummary(semantic_boundary);
  summary.bootstrap_legality_failure_contract_ready =
      IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(
          bootstrap_legality_failure_contract);
  summary.registration_descriptor_frontend_closure_contract_ready =
      IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          registration_descriptor_frontend_closure);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.translation_unit_identity_key = translation_unit_identity_key;

  if (summary.semantic_boundary_ready) {
    summary.duplicate_registration_policy =
        semantic_boundary.duplicate_registration_policy;
    summary.image_registration_order_invariant =
        semantic_boundary.image_registration_order_invariant;
    summary.cross_image_legality_model =
        semantic_boundary.cross_image_legality_model;
    summary.semantic_diagnostic_model =
        semantic_boundary.semantic_diagnostic_model;
    summary.duplicate_registration_semantics_landed =
        semantic_boundary.duplicate_registration_semantics_landed;
    summary.image_order_semantics_landed =
        semantic_boundary.image_order_semantics_landed;
    summary.cross_image_legality_semantics_landed =
        semantic_boundary.cross_image_legality_semantics_landed;
    summary.semantic_diagnostics_landed =
        semantic_boundary.semantic_diagnostics_landed;
    summary.semantic_boundary_replay_key = semantic_boundary.replay_key;
  }

  if (summary.bootstrap_legality_failure_contract_ready) {
    summary.registration_descriptor_identifier =
        bootstrap_legality_failure_contract.registration_descriptor_identifier;
    summary.image_root_identifier =
        bootstrap_legality_failure_contract.image_root_identifier;
    summary.registration_descriptor_identity_source =
        bootstrap_legality_failure_contract
            .registration_descriptor_identity_source;
    summary.image_root_identity_source =
        bootstrap_legality_failure_contract.image_root_identity_source;
    summary.translation_unit_registration_order_ordinal =
        bootstrap_legality_failure_contract
            .translation_unit_registration_order_ordinal;
    summary.bootstrap_legality_failure_contract_replay_key =
        bootstrap_legality_failure_contract.replay_key;
  }

  if (summary.registration_descriptor_frontend_closure_contract_ready) {
    summary.translation_unit_identity_model =
        registration_descriptor_frontend_closure.translation_unit_identity_model;
    summary.registration_descriptor_frontend_closure_replay_key =
        registration_descriptor_frontend_closure.replay_key;
  }

  if (summary.bootstrap_semantics_contract_ready) {
    summary.translation_unit_identity_model =
        bootstrap_semantics.translation_unit_identity_model;
    summary.translation_unit_registration_order_ordinal =
        bootstrap_semantics.translation_unit_registration_order_ordinal;
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }

  summary.ready_for_lowering_and_runtime =
      summary.semantic_boundary_ready &&
      summary.bootstrap_legality_failure_contract_ready &&
      summary.registration_descriptor_frontend_closure_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.duplicate_registration_semantics_landed &&
      summary.image_order_semantics_landed &&
      summary.cross_image_legality_semantics_landed &&
      summary.semantic_diagnostics_landed &&
      summary.duplicate_registration_policy ==
          bootstrap_legality_failure_contract.duplicate_registration_policy &&
      summary.duplicate_registration_policy ==
          bootstrap_semantics.duplicate_registration_policy &&
      summary.image_registration_order_invariant ==
          bootstrap_legality_failure_contract.image_registration_order_invariant &&
      summary.image_registration_order_invariant ==
          bootstrap_semantics.registration_order_ordinal_model &&
      summary.translation_unit_identity_model ==
          registration_descriptor_frontend_closure.translation_unit_identity_model &&
      summary.translation_unit_identity_model ==
          bootstrap_semantics.translation_unit_identity_model &&
      summary.registration_descriptor_identifier ==
          registration_descriptor_frontend_closure
              .registration_descriptor_identifier &&
      summary.image_root_identifier ==
          registration_descriptor_frontend_closure.image_root_identifier &&
      summary.registration_descriptor_identity_source ==
          registration_descriptor_frontend_closure
              .registration_descriptor_identity_source &&
      summary.image_root_identity_source ==
          registration_descriptor_frontend_closure.image_root_identity_source &&
      summary.translation_unit_registration_order_ordinal ==
          registration_descriptor_frontend_closure
              .translation_unit_registration_order_ordinal &&
      summary.translation_unit_registration_order_ordinal ==
          bootstrap_semantics.translation_unit_registration_order_ordinal &&
      !summary.translation_unit_identity_key.empty();

  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key = BuildRuntimeBootstrapLegalitySemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapLegalitySemanticsSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap legality duplicate/order semantics summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapLegalitySemanticsSummaryJson(
    const Objc3RuntimeBootstrapLegalitySemanticsSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"bootstrap_legality_failure_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_legality_failure_contract_id)
      << "\",\"registration_descriptor_frontend_closure_contract_id\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_contract_id)
      << "\",\"bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"duplicate_registration_policy\":\""
      << EscapeJsonString(summary.duplicate_registration_policy)
      << "\",\"image_registration_order_invariant\":\""
      << EscapeJsonString(summary.image_registration_order_invariant)
      << "\",\"cross_image_legality_model\":\""
      << EscapeJsonString(summary.cross_image_legality_model)
      << "\",\"semantic_diagnostic_model\":\""
      << EscapeJsonString(summary.semantic_diagnostic_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"translation_unit_identity_key\":\""
      << EscapeJsonString(summary.translation_unit_identity_key)
      << "\",\"registration_descriptor_identifier\":\""
      << EscapeJsonString(summary.registration_descriptor_identifier)
      << "\",\"image_root_identifier\":\""
      << EscapeJsonString(summary.image_root_identifier)
      << "\",\"registration_descriptor_identity_source\":\""
      << EscapeJsonString(summary.registration_descriptor_identity_source)
      << "\",\"image_root_identity_source\":\""
      << EscapeJsonString(summary.image_root_identity_source)
      << "\",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal << ",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapLegalitySemanticsSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"semantic_boundary_ready\":"
      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"bootstrap_legality_failure_contract_ready\":"
      << (summary.bootstrap_legality_failure_contract_ready ? "true" : "false")
      << ",\"registration_descriptor_frontend_closure_contract_ready\":"
      << (summary.registration_descriptor_frontend_closure_contract_ready
              ? "true"
              : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"duplicate_registration_semantics_landed\":"
      << (summary.duplicate_registration_semantics_landed ? "true" : "false")
      << ",\"image_order_semantics_landed\":"
      << (summary.image_order_semantics_landed ? "true" : "false")
      << ",\"cross_image_legality_semantics_landed\":"
      << (summary.cross_image_legality_semantics_landed ? "true" : "false")
      << ",\"semantic_diagnostics_landed\":"
      << (summary.semantic_diagnostics_landed ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"bootstrap_legality_failure_contract_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_legality_failure_contract_replay_key)
      << "\",\"registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeBootstrapFailureRestartSemanticsReplayKey(
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_semantics_contract_id="
      << summary.bootstrap_legality_semantics_contract_id
      << ";bootstrap_reset_contract_id=" << summary.bootstrap_reset_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";unsupported_topology_model=" << summary.unsupported_topology_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";translation_unit_identity_key=" << summary.translation_unit_identity_key
      << ";runtime_state_snapshot_symbol=" << summary.runtime_state_snapshot_symbol
      << ";replay_registered_images_symbol="
      << summary.replay_registered_images_symbol
      << ";reset_replay_state_snapshot_symbol="
      << summary.reset_replay_state_snapshot_symbol
      << ";invalid_descriptor_status_code="
      << summary.invalid_descriptor_status_code
      << ";invalid_registration_roots_status_code="
      << summary.invalid_registration_roots_status_code
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";semantic_boundary_replay_key=" << summary.semantic_boundary_replay_key
      << ";bootstrap_legality_semantics_replay_key="
      << summary.bootstrap_legality_semantics_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapFailureRestartSemanticsSummary
BuildRuntimeBootstrapFailureRestartSemanticsSummary(
    const Objc3BootstrapFailureRestartSemanticsSummary &semantic_boundary,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics,
    const Objc3RuntimeBootstrapApiSummary &bootstrap_api,
    const Objc3RuntimeBootstrapLoweringSummary &bootstrap_lowering,
    const std::string &translation_unit_identity_key) {
  Objc3RuntimeBootstrapFailureRestartSemanticsSummary summary;
  summary.fail_closed = true;
  summary.semantic_boundary_ready =
      IsReadyObjc3BootstrapFailureRestartSemanticsSummary(semantic_boundary);
  summary.bootstrap_legality_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapLegalitySemanticsSummary(
          bootstrap_legality_semantics);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.bootstrap_reset_contract_ready =
      IsReadyObjc3RuntimeBootstrapApiSummary(bootstrap_api) &&
      IsReadyObjc3RuntimeBootstrapLoweringSummary(bootstrap_lowering);
  summary.translation_unit_identity_key = translation_unit_identity_key;

  if (summary.semantic_boundary_ready) {
    summary.failure_mode = semantic_boundary.failure_mode;
    summary.restart_lifecycle_model = semantic_boundary.restart_lifecycle_model;
    summary.replay_order_model = semantic_boundary.replay_order_model;
    summary.image_local_init_reset_model =
        semantic_boundary.image_local_init_reset_model;
    summary.catalog_retention_model = semantic_boundary.catalog_retention_model;
    summary.unsupported_topology_model =
        semantic_boundary.unsupported_topology_model;
    summary.failure_mode_semantics_landed =
        semantic_boundary.failure_mode_semantics_landed;
    summary.restart_semantics_landed =
        semantic_boundary.restart_semantics_landed;
    summary.replay_semantics_landed = semantic_boundary.replay_semantics_landed;
    summary.unsupported_topology_semantics_landed =
        semantic_boundary.unsupported_topology_semantics_landed;
    summary.deterministic_recovery_semantics_landed =
        semantic_boundary.deterministic_recovery_semantics_landed;
    summary.semantic_boundary_replay_key = semantic_boundary.replay_key;
  }

  if (summary.bootstrap_legality_semantics_contract_ready) {
    summary.translation_unit_identity_model =
        bootstrap_legality_semantics.translation_unit_identity_model;
    summary.translation_unit_registration_order_ordinal =
        bootstrap_legality_semantics.translation_unit_registration_order_ordinal;
    summary.bootstrap_legality_semantics_replay_key =
        bootstrap_legality_semantics.replay_key;
  }

  if (summary.bootstrap_semantics_contract_ready) {
    summary.translation_unit_identity_model =
        bootstrap_semantics.translation_unit_identity_model;
    summary.runtime_state_snapshot_symbol =
        bootstrap_semantics.runtime_state_snapshot_symbol;
    summary.invalid_descriptor_status_code =
        bootstrap_semantics.invalid_descriptor_status_code;
    summary.invalid_registration_roots_status_code =
        bootstrap_semantics.invalid_registration_roots_status_code;
    summary.translation_unit_registration_order_ordinal =
        bootstrap_semantics.translation_unit_registration_order_ordinal;
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }

  summary.runtime_restart_probe_required = true;
  summary.ready_for_lowering_and_runtime =
      summary.semantic_boundary_ready &&
      summary.bootstrap_legality_semantics_contract_ready &&
      summary.bootstrap_semantics_contract_ready &&
      summary.bootstrap_reset_contract_ready &&
      summary.failure_mode_semantics_landed &&
      summary.restart_semantics_landed &&
      summary.replay_semantics_landed &&
      summary.unsupported_topology_semantics_landed &&
      summary.deterministic_recovery_semantics_landed &&
      summary.failure_mode == bootstrap_semantics.failure_mode &&
      summary.restart_lifecycle_model ==
          kObjc3RuntimeBootstrapResetLifecycleModel &&
      summary.replay_order_model == kObjc3RuntimeBootstrapReplayOrderModel &&
      summary.image_local_init_reset_model ==
          kObjc3RuntimeBootstrapImageLocalInitStateResetModel &&
      summary.catalog_retention_model ==
          kObjc3RuntimeBootstrapCatalogRetentionModel &&
      summary.translation_unit_identity_model ==
          bootstrap_legality_semantics.translation_unit_identity_model &&
      summary.translation_unit_identity_model ==
          bootstrap_semantics.translation_unit_identity_model &&
      summary.translation_unit_registration_order_ordinal ==
          bootstrap_legality_semantics.translation_unit_registration_order_ordinal &&
      summary.translation_unit_registration_order_ordinal ==
          bootstrap_semantics.translation_unit_registration_order_ordinal &&
      !summary.translation_unit_identity_key.empty();

  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildRuntimeBootstrapFailureRestartSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapFailureRestartSemanticsSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap failure-mode and restart semantics summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapFailureRestartSemanticsSummaryJson(
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"bootstrap_legality_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_legality_semantics_contract_id)
      << "\",\"bootstrap_reset_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_reset_contract_id)
      << "\",\"bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"failure_mode\":\""
      << EscapeJsonString(summary.failure_mode)
      << "\",\"restart_lifecycle_model\":\""
      << EscapeJsonString(summary.restart_lifecycle_model)
      << "\",\"replay_order_model\":\""
      << EscapeJsonString(summary.replay_order_model)
      << "\",\"image_local_init_reset_model\":\""
      << EscapeJsonString(summary.image_local_init_reset_model)
      << "\",\"catalog_retention_model\":\""
      << EscapeJsonString(summary.catalog_retention_model)
      << "\",\"unsupported_topology_model\":\""
      << EscapeJsonString(summary.unsupported_topology_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"translation_unit_identity_key\":\""
      << EscapeJsonString(summary.translation_unit_identity_key)
      << "\",\"runtime_state_snapshot_symbol\":\""
      << EscapeJsonString(summary.runtime_state_snapshot_symbol)
      << "\",\"replay_registered_images_symbol\":\""
      << EscapeJsonString(summary.replay_registered_images_symbol)
      << "\",\"reset_replay_state_snapshot_symbol\":\""
      << EscapeJsonString(summary.reset_replay_state_snapshot_symbol)
      << "\",\"invalid_descriptor_status_code\":"
      << summary.invalid_descriptor_status_code
      << ",\"invalid_registration_roots_status_code\":"
      << summary.invalid_registration_roots_status_code
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapFailureRestartSemanticsSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"semantic_boundary_ready\":"

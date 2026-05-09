      << "  \"bootstrap_live_discovery_tracking_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveDiscoveryTrackingModel)
      << "\",\n"
      << "  \"bootstrap_live_replay_tracking_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveReplayTrackingModel) << "\",\n"
      << "  \"bootstrap_live_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_live_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_live_restart_hardening_contract_id\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRestartHardeningContractId)
      << "\",\n"
      << "  \"bootstrap_live_idempotence_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveIdempotenceModel) << "\",\n"
      << "  \"bootstrap_live_teardown_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveTeardownModel) << "\",\n"
      << "  \"bootstrap_live_restart_evidence_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRestartEvidenceModel)
      << "\",\n"
      << "  \"bootstrap_live_restart_reset_for_testing_symbol\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_reset_for_testing_symbol)
      << "\",\n"
      << "  \"bootstrap_live_restart_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_live_restart_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_reset_contract_id) << "\",\n"
      << "  \"bootstrap_reset_internal_header_path\": \""
      << EscapeJsonString(inputs.bootstrap_reset_internal_header_path)
      << "\",\n"
      << "  \"bootstrap_reset_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_lifecycle_model\": \""
      << EscapeJsonString(inputs.bootstrap_reset_lifecycle_model) << "\",\n"
      << "  \"bootstrap_reset_replay_order_model\": \""
      << EscapeJsonString(inputs.bootstrap_reset_replay_order_model)
      << "\",\n"
      << "  \"bootstrap_reset_image_local_init_state_reset_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_image_local_init_state_reset_model)
      << "\",\n"
      << "  \"bootstrap_reset_bootstrap_catalog_retention_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_bootstrap_catalog_retention_model)
      << "\",\n"
      << "  \"bootstrap_lowering_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_lowering_contract_id) << "\",\n"
      << "  \"bootstrap_lowering_boundary_model\": \""
      << EscapeJsonString(inputs.bootstrap_lowering_boundary_model)
      << "\",\n"
      << "  \"bootstrap_global_ctor_list_model\": \""
      << EscapeJsonString(inputs.bootstrap_global_ctor_list_model)
      << "\",\n"
      << "  \"bootstrap_registration_table_layout_model\": \""
      << EscapeJsonString(inputs.bootstrap_registration_table_layout_model)
      << "\",\n"
      << "  \"bootstrap_image_local_initialization_model\": \""
      << EscapeJsonString(inputs.bootstrap_image_local_initialization_model)
      << "\",\n"
      << "  \"bootstrap_constructor_root_emission_state\": \""
      << EscapeJsonString(inputs.bootstrap_constructor_root_emission_state)
      << "\",\n"
      << "  \"bootstrap_init_stub_emission_state\": \""
      << EscapeJsonString(inputs.bootstrap_init_stub_emission_state)
      << "\",\n"
      << "  \"bootstrap_registration_table_emission_state\": \""
      << EscapeJsonString(
             inputs.bootstrap_registration_table_emission_state)
      << "\",\n"
      << "  \"bootstrap_registration_table_symbol_prefix\": \""
      << EscapeJsonString(inputs.bootstrap_registration_table_symbol_prefix)
      << "\",\n"
      << "  \"bootstrap_image_local_init_state_symbol_prefix\": \""
      << EscapeJsonString(
             inputs.bootstrap_image_local_init_state_symbol_prefix)
      << "\",\n"
      << "  \"bootstrap_registration_table_symbol\": \""
      << EscapeJsonString(bootstrap_registration_table_symbol) << "\",\n"
      << "  \"bootstrap_image_local_init_state_symbol\": \""
      << EscapeJsonString(bootstrap_image_local_init_state_symbol)
      << "\",\n"
      << "  \"bootstrap_registration_table_abi_version\": "
      << inputs.bootstrap_registration_table_abi_version << ",\n"
      << "  \"bootstrap_registration_table_pointer_field_count\": "
      << inputs.bootstrap_registration_table_pointer_field_count << ",\n"
      << "  \"success_status_code\": " << inputs.success_status_code << ",\n"
      << "  \"invalid_descriptor_status_code\": "
      << inputs.invalid_descriptor_status_code << ",\n"
      << "  \"duplicate_registration_status_code\": "
      << inputs.duplicate_registration_status_code << ",\n"
      << "  \"out_of_order_status_code\": "
      << inputs.out_of_order_status_code << ",\n"
      << "  \"translation_unit_registration_order_ordinal\": "
      << inputs.translation_unit_registration_order_ordinal << ",\n"
      << "  \"translation_unit_identity_key\": \""
      << EscapeJsonString(
             linker_retention_artifacts.translation_unit_identity_key)
      << "\",\n"
      << "  \"object_format\": \""
      << EscapeJsonString(linker_retention_artifacts.object_format)
      << "\",\n"
      << "  \"linker_anchor_symbol\": \""
      << EscapeJsonString(linker_retention_artifacts.linker_anchor_symbol)
      << "\",\n"
      << "  \"discovery_root_symbol\": \""
      << EscapeJsonString(linker_retention_artifacts.discovery_root_symbol)
      << "\",\n"
      << "  \"driver_linker_flags\": [\n"
      << "    \""
      << EscapeJsonString(linker_retention_artifacts.driver_linker_flag)
      << "\"\n"
      << "  ],\n"
      << "  \"launch_integration_ready\": true,\n"
      << "  \"ready_for_lowering_init_stub_emission\": true,\n"
      << "  \"ready_for_bootstrap_lowering_materialization\": true,\n"
      << "  \"ready_for_runtime_bootstrap_enforcement\": true,\n"
      << "  \"ready_for_runtime_bootstrap_table_consumption\": true,\n"
      << "  \"ready_for_live_registration_discovery_replay\": true,\n"
      << "  \"ready_for_live_restart_hardening\": true\n"
      << "}\n";
  manifest_json = out.str();
  return true;
}

bool TryBuildObjc3RuntimeRegistrationDescriptorArtifact(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    std::string &descriptor_json,
    std::string &error) {
  descriptor_json.clear();
  error.clear();
  if (inputs.contract_id.empty() ||
      inputs.registration_manifest_contract_id.empty() ||
      inputs.source_surface_contract_id.empty() ||
      inputs.bootstrap_lowering_contract_id.empty() ||
      inputs.payload_model.empty() ||
      inputs.artifact_relative_path.empty() || inputs.authority_model.empty() ||
      inputs.translation_unit_identity_model.empty() ||
      inputs.payload_ownership_model.empty() ||
      inputs.runtime_support_library_archive_relative_path.empty() ||
      inputs.registration_entrypoint_symbol.empty() ||
      inputs.registration_descriptor_pragma_name.empty() ||
      inputs.image_root_pragma_name.empty() || inputs.module_identity_source.empty() ||
      inputs.registration_descriptor_identifier.empty() ||
      inputs.registration_descriptor_identity_source.empty() ||
      inputs.image_root_identifier.empty() ||
      inputs.image_root_identity_source.empty() ||
      inputs.bootstrap_visible_metadata_ownership_model.empty() ||
      inputs.constructor_root_symbol.empty() ||
      inputs.constructor_init_stub_symbol_prefix.empty() ||
      inputs.bootstrap_registration_table_symbol_prefix.empty() ||
      inputs.bootstrap_image_local_init_state_symbol_prefix.empty() ||
      inputs.bootstrap_registration_table_layout_model.empty() ||
      inputs.bootstrap_image_local_initialization_model.empty() ||
      inputs.bootstrap_constructor_root_emission_state.empty() ||
      inputs.bootstrap_init_stub_emission_state.empty() ||
      inputs.bootstrap_registration_table_emission_state.empty() ||
      inputs.bootstrap_registration_table_abi_version == 0 ||
      inputs.bootstrap_registration_table_pointer_field_count == 0 ||
      inputs.total_descriptor_count !=
          inputs.class_descriptor_count + inputs.protocol_descriptor_count +
              inputs.category_descriptor_count +
              inputs.property_descriptor_count + inputs.ivar_descriptor_count ||
      inputs.translation_unit_registration_order_ordinal == 0 ||
      inputs.object_artifact_relative_path.empty() ||
      inputs.backend_artifact_relative_path.empty() ||
      linker_retention_artifacts.translation_unit_identity_key.empty() ||
      linker_retention_artifacts.object_format.empty() ||
      linker_retention_artifacts.linker_anchor_symbol.empty() ||
      linker_retention_artifacts.discovery_root_symbol.empty()) {
    error =
        "runtime registration descriptor artifact inputs incomplete for " +
        inputs.artifact_relative_path;
    return false;
  }

  const std::string safe_identity_suffix =
      objc3c::support::MakeIdentifierSafeSuffix(linker_retention_artifacts.translation_unit_identity_key,
                                                "translation_unit");
  const std::string constructor_init_stub_symbol =
      inputs.constructor_init_stub_symbol_prefix + safe_identity_suffix;
  const std::string bootstrap_registration_table_symbol =
      inputs.bootstrap_registration_table_symbol_prefix + safe_identity_suffix;
  const std::string bootstrap_image_local_init_state_symbol =
      inputs.bootstrap_image_local_init_state_symbol_prefix + safe_identity_suffix;

  std::ostringstream out;
  JsonObjectWriter descriptor(out);
  descriptor.StringField("contract_id", inputs.contract_id);
  descriptor.StringField("registration_manifest_contract_id",
                         inputs.registration_manifest_contract_id);
  descriptor.StringField("source_surface_contract_id",
                         inputs.source_surface_contract_id);
  descriptor.StringField("bootstrap_lowering_contract_id",
                         inputs.bootstrap_lowering_contract_id);
  descriptor.StringField("payload_model", inputs.payload_model);
  descriptor.StringField("artifact", inputs.artifact_relative_path);
  descriptor.StringField("authority_model", inputs.authority_model);
  descriptor.StringField("translation_unit_identity_model",
                         inputs.translation_unit_identity_model);
  descriptor.StringField(
      "translation_unit_identity_key",
      linker_retention_artifacts.translation_unit_identity_key);
  descriptor.StringField("payload_ownership_model",
                         inputs.payload_ownership_model);
  descriptor.StringField("runtime_support_library_archive_relative_path",
                         inputs.runtime_support_library_archive_relative_path);
  descriptor.StringField("registration_entrypoint_symbol",
                         inputs.registration_entrypoint_symbol);
  descriptor.StringField("registration_descriptor_pragma_name",
                         inputs.registration_descriptor_pragma_name);
  descriptor.StringField("image_root_pragma_name",
                         inputs.image_root_pragma_name);
  descriptor.StringField("module_identity_source",
                         inputs.module_identity_source);
  descriptor.StringField("registration_descriptor_identifier",
                         inputs.registration_descriptor_identifier);
  descriptor.StringField("registration_descriptor_identity_source",
                         inputs.registration_descriptor_identity_source);
  descriptor.StringField("image_root_identifier", inputs.image_root_identifier);
  descriptor.StringField("image_root_identity_source",
                         inputs.image_root_identity_source);
  descriptor.StringField("bootstrap_visible_metadata_ownership_model",
                         inputs.bootstrap_visible_metadata_ownership_model);
  descriptor.StringField("constructor_root_symbol",
                         inputs.constructor_root_symbol);
  descriptor.StringField("constructor_init_stub_symbol",
                         constructor_init_stub_symbol);
  descriptor.StringField("bootstrap_registration_table_symbol",
                         bootstrap_registration_table_symbol);
  descriptor.StringField("bootstrap_image_local_init_state_symbol",
                         bootstrap_image_local_init_state_symbol);
  descriptor.StringField("bootstrap_registration_table_layout_model",
                         inputs.bootstrap_registration_table_layout_model);
  descriptor.StringField("bootstrap_image_local_initialization_model",
                         inputs.bootstrap_image_local_initialization_model);
  descriptor.StringField("bootstrap_constructor_root_emission_state",
                         inputs.bootstrap_constructor_root_emission_state);
  descriptor.StringField("bootstrap_init_stub_emission_state",
                         inputs.bootstrap_init_stub_emission_state);
  descriptor.StringField("bootstrap_registration_table_emission_state",
                         inputs.bootstrap_registration_table_emission_state);
  descriptor.UnsignedField("bootstrap_registration_table_abi_version",
                           inputs.bootstrap_registration_table_abi_version);
  descriptor.UnsignedField(
      "bootstrap_registration_table_pointer_field_count",
      inputs.bootstrap_registration_table_pointer_field_count);
  descriptor.UnsignedField("class_descriptor_count",
                           inputs.class_descriptor_count);
  descriptor.UnsignedField("protocol_descriptor_count",
                           inputs.protocol_descriptor_count);
  descriptor.UnsignedField("category_descriptor_count",
                           inputs.category_descriptor_count);
  descriptor.UnsignedField("property_descriptor_count",
                           inputs.property_descriptor_count);
  descriptor.UnsignedField("ivar_descriptor_count",
                           inputs.ivar_descriptor_count);
  descriptor.UnsignedField("total_descriptor_count",
                           inputs.total_descriptor_count);
  descriptor.UnsignedField("translation_unit_registration_order_ordinal",
                           inputs.translation_unit_registration_order_ordinal);
  descriptor.StringField("object_artifact", inputs.object_artifact_relative_path);
  descriptor.StringField("backend_artifact",
                         inputs.backend_artifact_relative_path);
  descriptor.StringField("object_format", linker_retention_artifacts.object_format);
  descriptor.StringField("linker_anchor_symbol",
                         linker_retention_artifacts.linker_anchor_symbol);
  descriptor.StringField("discovery_root_symbol",
                         linker_retention_artifacts.discovery_root_symbol);
  descriptor.BoolField("ready_for_descriptor_artifact_emission", true);
  descriptor.BoolField("ready_for_registration_descriptor_lowering", true);
  descriptor.BoolField("ready_for_loader_table_lowering", true);
  descriptor_json = FinishJsonObject(descriptor, out);
  return true;
}

bool TryBuildObjc3MetaprogrammingMacroHostProcessCacheArtifact(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.contract_id.empty() || inputs.source_contract_id.empty() ||
      inputs.surface_path.empty() || inputs.artifact_relative_path.empty() ||
      inputs.host_executable_relative_path.empty() ||
      inputs.cache_root_relative_path.empty() || inputs.host_model.empty() ||
      inputs.toolchain_model.empty() || inputs.cache_model.empty() ||
      inputs.fail_closed_model.empty() || inputs.replay_key.empty()) {
    error = "metaprogramming macro host process/cache artifact inputs are incomplete";
    return false;
  }
  if (!std::filesystem::exists(source_input_path)) {
    error = "metaprogramming macro host process/cache source input not found: " +
            source_input_path.generic_string();
    return false;
  }

  const std::filesystem::path host_executable =
      std::filesystem::path(inputs.host_executable_relative_path);
  if (!std::filesystem::exists(host_executable)) {
    error = "metaprogramming macro host process/cache host executable not found: " +
            host_executable.generic_string();
    return false;
  }

  const std::string cache_key = ComputeFnv1a64Hex(inputs.replay_key);
  const std::filesystem::path cache_root =
      std::filesystem::path(inputs.cache_root_relative_path);
  const std::filesystem::path cache_entry = cache_root / cache_key;
  const std::filesystem::path cache_summary_path =
      cache_entry / "module.host-summary.json";
  const std::filesystem::path cache_runtime_import_path =
      BuildRuntimeAwareImportModuleArtifactPath(cache_entry, "module");
  const std::filesystem::path cache_manifest_path =
      BuildManifestArtifactPath(cache_entry, "module");

  bool cache_hit = std::filesystem::exists(cache_summary_path) &&
                   std::filesystem::exists(cache_runtime_import_path) &&
                   std::filesystem::exists(cache_manifest_path);
  bool launch_attempted = false;
  int host_process_exit_code = 0;

  if (!cache_hit) {
    launch_attempted = true;
    std::error_code create_error;
    std::filesystem::create_directories(cache_entry, create_error);
    if (create_error) {
      error = "failed to create metaprogramming host/cache entry directory: " +
              cache_entry.generic_string() + ": " + create_error.message();
      return false;
    }
    const std::vector<std::string> args = {
        source_input_path.generic_string(),
        "--out-dir",
        cache_entry.generic_string(),
        "--emit-prefix",
        "module",
        "--summary-out",
        cache_summary_path.generic_string(),
        "--no-emit-ir",
        "--no-emit-object"};
    host_process_exit_code = RunProcess(host_executable.generic_string(), args);
    if (host_process_exit_code != 0) {
      error = "metaprogramming macro host process launch failed with exit code " +
              std::to_string(host_process_exit_code);
      return false;
    }
    cache_hit = std::filesystem::exists(cache_summary_path) &&
                std::filesystem::exists(cache_runtime_import_path) &&
                std::filesystem::exists(cache_manifest_path);
    if (!cache_hit) {
      error =
          "metaprogramming macro host process launch completed but cache artifacts are incomplete";
      return false;
    }
  }

  std::ostringstream out;
  JsonObjectWriter artifact(out);
  artifact.StringField("contract_id", inputs.contract_id);
  artifact.StringField("source_contract_id", inputs.source_contract_id);
  artifact.StringField("surface_path", inputs.surface_path);
  artifact.StringField("artifact", inputs.artifact_relative_path);
  artifact.StringField("host_executable_relative_path",
                       inputs.host_executable_relative_path);
  artifact.StringField("cache_root_relative_path",
                       inputs.cache_root_relative_path);
  artifact.StringField("cache_key", cache_key);
  artifact.StringField("cache_entry_relative_path",
                       cache_entry.generic_string());
  artifact.StringField("cache_summary_relative_path",
                       cache_summary_path.generic_string());
  artifact.StringField("cache_runtime_import_surface_relative_path",
                       cache_runtime_import_path.generic_string());
  artifact.StringField("cache_manifest_relative_path",
                       cache_manifest_path.generic_string());
  artifact.StringField("host_model", inputs.host_model);
  artifact.StringField("toolchain_model", inputs.toolchain_model);
  artifact.StringField("cache_model", inputs.cache_model);
  artifact.StringField("fail_closed_model", inputs.fail_closed_model);
  artifact.StringField("source_input_path", source_input_path.generic_string());
  artifact.BoolField("cache_ready", true);
  artifact.BoolField("launch_attempted", launch_attempted);
  artifact.BoolField("cache_hit", cache_hit);
  artifact.BoolField("cache_summary_present",
                     std::filesystem::exists(cache_summary_path));
  artifact.BoolField("cache_runtime_import_surface_present",
                     std::filesystem::exists(cache_runtime_import_path));
  artifact.BoolField("cache_manifest_present",
                     std::filesystem::exists(cache_manifest_path));
  artifact.StringField("cache_materialization_state",
                       launch_attempted ? "materialized" : "cache-hit");
  artifact.NumberField("host_process_exit_code",
                       static_cast<double>(host_process_exit_code));
  artifact.BoolField("deterministic", inputs.deterministic);
  artifact.StringField("replay_key", inputs.replay_key);
  artifact_json = FinishJsonObject(artifact, out);
  return true;
}

bool TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &plan_json,
    std::string &linker_response_payload,
    std::string &error) {
  plan_json.clear();
  linker_response_payload.clear();
  error.clear();

  if (inputs.contract_id.empty() ||
      inputs.source_orchestration_contract_id.empty() ||
      inputs.import_surface_contract_id.empty() ||
      inputs.registration_manifest_contract_id.empty() ||
      inputs.payload_model.empty() ||
      inputs.artifact_relative_path.empty() ||
      inputs.linker_response_artifact_relative_path.empty() ||
      inputs.authority_model.empty() ||
      inputs.packaging_model.empty() ||
      inputs.registration_scope_model.empty() ||
      inputs.link_object_order_model.empty() ||
      inputs.local_module_name.empty() ||
      inputs.local_import_surface_artifact_relative_path.empty() ||
      inputs.local_registration_manifest_artifact_relative_path.empty() ||
      inputs.local_object_artifact_relative_path.empty() ||
      inputs.runtime_support_library_archive_relative_path.empty() ||
      inputs.object_format.empty() ||
      inputs.local_translation_unit_identity_model.empty() ||
      inputs.local_translation_unit_identity_key.empty() ||
      inputs.local_translation_unit_registration_order_ordinal == 0 ||
      inputs.local_total_descriptor_count !=
          inputs.local_class_descriptor_count +
              inputs.local_protocol_descriptor_count +
              inputs.local_category_descriptor_count +
              inputs.local_property_descriptor_count +
              inputs.local_ivar_descriptor_count ||
      inputs.expected_error_handling_contract_id.empty() ||
      inputs.expected_error_handling_source_contract_id.empty() ||
      inputs.expected_concurrency_actor_contract_id.empty() ||
      inputs.expected_concurrency_actor_source_contract_id.empty() ||
      inputs.expected_interop_ffi_contract_id.empty() ||
      inputs.expected_interop_ffi_source_contract_id.empty() ||
      inputs.expected_interop_ffi_preservation_contract_id.empty() ||
      inputs.expected_interop_header_module_bridge_contract_id.empty() ||
      inputs.expected_interop_header_module_bridge_source_contract_id.empty() ||
      inputs.expected_interop_header_module_bridge_preservation_contract_id
          .empty() ||
      inputs.expected_interop_bridge_header_artifact_relative_path.empty() ||
      inputs.expected_interop_bridge_module_artifact_relative_path.empty() ||
      inputs.expected_interop_bridge_artifact_relative_path.empty() ||
      inputs.expected_metaprogramming_host_cache_contract_id.empty() ||
      inputs.expected_metaprogramming_host_cache_source_contract_id.empty() ||
      inputs.expected_metaprogramming_host_cache_executable_relative_path.empty() ||
      inputs.expected_metaprogramming_host_cache_root_relative_path.empty() ||
      inputs.expected_block_ownership_contract_id.empty() ||
      inputs.expected_block_ownership_source_contract_id.empty() ||
      inputs
          .expected_block_ownership_object_invoke_thunk_lowering_contract_id
          .empty() ||
      inputs.expected_block_ownership_byref_helper_lowering_contract_id
          .empty() ||
      inputs.expected_block_ownership_escape_runtime_hook_lowering_contract_id
          .empty() ||
      inputs
          .expected_block_ownership_runtime_support_library_link_wiring_contract_id
          .empty() ||
      inputs.expected_storage_reflection_contract_id.empty() ||
      inputs.expected_storage_reflection_source_contract_id.empty() ||
      inputs
          .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id
          .empty() ||
      inputs
          .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id
          .empty() ||
      inputs
          .expected_storage_reflection_executable_ivar_layout_emission_contract_id
          .empty() ||
      inputs
          .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id
          .empty() ||
      inputs.expected_bootstrap_live_registration_contract_id.empty() ||
      inputs.expected_bootstrap_live_restart_hardening_contract_id.empty() ||
      inputs.expected_bootstrap_replay_registered_images_symbol.empty() ||
      inputs.expected_bootstrap_reset_replay_state_snapshot_symbol.empty() ||
      inputs.expected_bootstrap_reset_for_testing_symbol.empty() ||
      inputs.local_driver_linker_flags.empty() ||
      inputs.direct_import_surface_artifact_paths.empty() ||
      inputs.imported_inputs.empty()) {
    error = "cross-module runtime link-plan artifact inputs are incomplete";
    return false;
  }
  // cleanup-unwind integration anchor: runnable cleanup/unwind
  // proofs stay toolchain-visible through the linker-response sidecar plus the
  // emitted runtime-support archive path that native executable probes consume.
  // runtime-fast-path-integration anchor: Part 9 keeps imported
  // direct-surface artifact paths visible in the cross-module link plan so the
  // runtime/cache lane can prove exactly which imported modules participate in
  // dispatch-boundary preservation before D002 widens the live fast path.
  // live-dispatch-fast-path anchor: imported direct-surface artifact paths feed the runtime cache seeding model once live direct/final/sealed fast paths are materialized after registration.
  if (inputs.direct_import_surface_artifact_paths.size() !=
      inputs.imported_inputs.size()) {
    error =
        "cross-module runtime link-plan direct import surface count does not "
        "match imported input count";
    return false;
  }
  if (inputs.local_storage_reflection_synthesized_accessor_entries !=
          inputs.local_storage_reflection_synthesized_getter_entries +
              inputs.local_storage_reflection_synthesized_setter_entries ||
      inputs.local_storage_reflection_ivar_layout_entries !=
          inputs.local_ivar_descriptor_count) {
    error =
        "cross-module runtime link-plan local storage/reflection preservation summary drifted from descriptor counts";
    return false;
  }
  if (inputs.local_block_ownership_invoke_trampoline_symbolized_sites >
          inputs.local_block_ownership_block_literal_sites ||
      inputs.local_block_ownership_copy_helper_symbolized_sites >
          inputs.local_block_ownership_copy_helper_required_sites ||
      inputs.local_block_ownership_dispose_helper_symbolized_sites >
          inputs.local_block_ownership_dispose_helper_required_sites ||
      inputs.local_block_ownership_escape_to_heap_sites >
          inputs.local_block_ownership_block_literal_sites) {
    error =
        "cross-module runtime link-plan local block-ownership preservation summary drifted from lowering counts";
    return false;
  }

  std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput> imported_inputs =
      inputs.imported_inputs;
  std::sort(imported_inputs.begin(), imported_inputs.end(),
            [](const auto &lhs, const auto &rhs) {
              if (lhs.translation_unit_registration_order_ordinal !=
                  rhs.translation_unit_registration_order_ordinal) {
                return lhs.translation_unit_registration_order_ordinal <
                       rhs.translation_unit_registration_order_ordinal;
              }
              if (lhs.translation_unit_identity_key !=
                  rhs.translation_unit_identity_key) {
                return lhs.translation_unit_identity_key <
                       rhs.translation_unit_identity_key;
              }
              return lhs.module_name < rhs.module_name;
            });

  std::unordered_set<std::string> seen_translation_unit_identity_keys;
  std::unordered_set<std::uint64_t> seen_registration_ordinals;
  std::unordered_set<std::string> seen_driver_linker_flags;
  std::unordered_set<std::string> seen_direct_import_surface_paths;
  std::unordered_set<std::string> seen_error_handling_replay_keys;
  std::unordered_set<std::string> seen_concurrency_actor_replay_keys;
  std::unordered_set<std::string> seen_interop_ffi_replay_keys;
  std::unordered_set<std::string> seen_interop_header_module_bridge_replay_keys;
  std::unordered_set<std::string> seen_metaprogramming_host_cache_replay_keys;
  std::vector<std::string> ordered_link_object_artifacts;
  std::vector<std::string> merged_driver_linker_flags;
  std::vector<std::string> imported_error_handling_module_names;
  std::vector<std::string> imported_concurrency_actor_module_names;
  std::vector<std::string> imported_interop_ffi_module_names;
  std::vector<std::string> imported_interop_header_module_bridge_module_names;
  std::vector<std::string> imported_metaprogramming_host_cache_module_names;
  std::size_t imported_class_descriptor_count = 0;
  std::size_t imported_protocol_descriptor_count = 0;
  std::size_t imported_category_descriptor_count = 0;
  std::size_t imported_property_descriptor_count = 0;
  std::size_t imported_ivar_descriptor_count = 0;
  std::size_t imported_total_descriptor_count = 0;
  std::size_t imported_block_ownership_block_literal_sites = 0;
  std::size_t imported_block_ownership_invoke_trampoline_symbolized_sites = 0;
  std::size_t imported_block_ownership_copy_helper_required_sites = 0;
  std::size_t imported_block_ownership_dispose_helper_required_sites = 0;
  std::size_t imported_block_ownership_copy_helper_symbolized_sites = 0;
  std::size_t imported_block_ownership_dispose_helper_symbolized_sites = 0;
  std::size_t imported_block_ownership_escape_to_heap_sites = 0;
  std::size_t imported_block_ownership_byref_layout_symbolized_sites = 0;
  std::size_t imported_storage_reflection_implementation_owned_property_entries =
      0;
  std::size_t imported_storage_reflection_synthesized_accessor_owner_entries =
      0;
  std::size_t imported_storage_reflection_synthesized_getter_entries = 0;
  std::size_t imported_storage_reflection_synthesized_setter_entries = 0;
  std::size_t imported_storage_reflection_synthesized_accessor_entries = 0;
  std::size_t imported_storage_reflection_current_property_read_entries = 0;
  std::size_t imported_storage_reflection_current_property_write_entries = 0;
  std::size_t imported_storage_reflection_current_property_exchange_entries = 0;
  std::size_t imported_storage_reflection_weak_current_property_load_entries =
      0;
  std::size_t imported_storage_reflection_weak_current_property_store_entries =
      0;
  std::size_t imported_storage_reflection_ivar_layout_entries = 0;
  std::size_t imported_storage_reflection_ivar_layout_owner_entries = 0;
  std::vector<std::string> direct_import_surface_artifact_paths =
      inputs.direct_import_surface_artifact_paths;
  std::sort(direct_import_surface_artifact_paths.begin(),
            direct_import_surface_artifact_paths.end());

  seen_translation_unit_identity_keys.insert(
      inputs.local_translation_unit_identity_key);
  seen_registration_ordinals.insert(
      inputs.local_translation_unit_registration_order_ordinal);
  for (const auto &surface_path : direct_import_surface_artifact_paths) {
    if (surface_path.empty()) {
      error =
          "cross-module runtime link-plan direct import surface path missing";
      return false;
    }
    if (!seen_direct_import_surface_paths.insert(surface_path).second) {
      error =
          "cross-module runtime link-plan duplicate direct import surface "
          "path: " +
          surface_path;
      return false;
    }
  }
  for (const auto &local_flag : inputs.local_driver_linker_flags) {
    if (local_flag.empty()) {
      error =
          "cross-module runtime link-plan local driver-linker flag input missing";
      return false;
    }
  }

  struct OrderedLinkInput {
    std::uint64_t registration_order_ordinal = 0;
    std::string translation_unit_identity_key;
    std::string module_name;
    std::string object_artifact_path;
    std::vector<std::string> driver_linker_flags;
  };
  std::vector<OrderedLinkInput> ordered_link_inputs;
  ordered_link_inputs.reserve(imported_inputs.size() + 1u);

  for (const auto &imported_input : imported_inputs) {
    if (imported_input.module_name.empty() ||
        imported_input.import_surface_artifact_path.empty() ||
        imported_input.registration_manifest_artifact_path.empty() ||
        imported_input.object_artifact_path.empty() ||
        imported_input.discovery_artifact_path.empty() ||
        imported_input.linker_response_artifact_path.empty() ||
        imported_input.translation_unit_identity_model.empty() ||
        imported_input.translation_unit_identity_key.empty() ||
        imported_input.object_format.empty() ||
        imported_input.runtime_support_library_archive_relative_path.empty() ||
        imported_input.translation_unit_registration_order_ordinal == 0 ||
        imported_input.total_descriptor_count !=
            imported_input.class_descriptor_count +
                imported_input.protocol_descriptor_count +
                imported_input.category_descriptor_count +
                imported_input.property_descriptor_count +
                imported_input.ivar_descriptor_count ||
        imported_input.driver_linker_flags.empty() ||
        imported_input.bootstrap_live_registration_contract_id.empty() ||
        imported_input.bootstrap_live_restart_hardening_contract_id.empty() ||
        imported_input.bootstrap_live_replay_registered_images_symbol.empty() ||
        imported_input.bootstrap_live_reset_replay_state_snapshot_symbol
            .empty() ||
        imported_input.bootstrap_live_restart_reset_for_testing_symbol.empty() ||
        imported_input.bootstrap_live_restart_replay_registered_images_symbol
            .empty() ||
        imported_input
            .bootstrap_live_restart_reset_replay_state_snapshot_symbol.empty()) {
      error =
          "cross-module runtime link-plan imported input is incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.object_format != inputs.object_format) {
      error = "cross-module runtime link-plan object-format mismatch for " +
              imported_input.module_name;
      return false;
    }
    // error-runtime/bridge-helper anchor: Part 6 lane-D still rides
    // the same packaged runtime-support archive path as the rest of the native
    // runtime helper cluster, so imported modules must agree on that archive
    // before later cross-module Part 6 execution claims can become truthful.
    // continuation/runtime-helper anchor: the first private Part 7
    // continuation helper cluster likewise stays inside the same packaged
    // runtime-support archive path. Mixed-module async/runtime-helper claims
    // therefore may not diverge on the runtime archive even before live
    // suspension integration lands.
    // live continuation/runtime integration anchor: the supported
    // direct-call await slice now links and executes through that same archive,
    // so runnable Part 7 helper traffic still depends on this runtime archive
    // equality across mixed-module link plans.
    // actor-runtime/executor-binding anchor: the private actor
    // runtime helper cluster also rides that same packaged runtime archive, so
    // actor-state/executor-binding claims cannot diverge on archive identity
    // across mixed-module link plans.
    // actor-mailbox/isolation-runtime anchor: live mailbox helper
    // traffic still links through that same packaged runtime archive, so mixed
    // actor-runtime link plans must keep one archive identity even after the
    // mailbox helpers become runnable.
    // system-helper/runtime-contract anchor: Part 8 cleanup/resource
    // and retainable-family runtime proof also stays on that same packaged
    // runtime archive path. Mixed-module link plans may not diverge on archive
    // identity while lane-D still reuses the existing private helper cluster.
    // expansion-host/runtime-boundary anchor: Part 10 does not launch
    // a macro host from the driver yet; the packaged objc3_runtime.lib archive
    // remains the only host-facing runtime handoff while macro execution and
    // runtime package loading stay fail-closed.
    if (imported_input.runtime_support_library_archive_relative_path !=
        inputs.runtime_support_library_archive_relative_path) {
      error =
          "cross-module runtime link-plan runtime library path mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.ready_for_live_registration_discovery_replay ||
        imported_input.bootstrap_live_registration_contract_id !=
            inputs.expected_bootstrap_live_registration_contract_id ||
        imported_input.bootstrap_live_replay_registered_images_symbol !=
            inputs.expected_bootstrap_replay_registered_images_symbol ||
        imported_input.bootstrap_live_reset_replay_state_snapshot_symbol !=
            inputs.expected_bootstrap_reset_replay_state_snapshot_symbol) {
      error =
          "cross-module runtime link-plan live registration replay preservation mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.ready_for_live_restart_hardening ||
        imported_input.bootstrap_live_restart_hardening_contract_id !=
            inputs.expected_bootstrap_live_restart_hardening_contract_id ||
        imported_input.bootstrap_live_restart_reset_for_testing_symbol !=
            inputs.expected_bootstrap_reset_for_testing_symbol ||
        imported_input.bootstrap_live_restart_replay_registered_images_symbol !=
            inputs.expected_bootstrap_replay_registered_images_symbol ||
        imported_input
                .bootstrap_live_restart_reset_replay_state_snapshot_symbol !=
            inputs.expected_bootstrap_reset_replay_state_snapshot_symbol) {
      error =
          "cross-module runtime link-plan live restart hardening preservation mismatch for " +
          imported_input.module_name;
      return false;
    }
    for (const auto &flag : imported_input.driver_linker_flags) {
      if (flag.empty()) {
        error =
            "cross-module runtime link-plan imported linker flag is empty for " +
            imported_input.module_name;
        return false;
      }
    }
    if (!seen_translation_unit_identity_keys
             .insert(imported_input.translation_unit_identity_key)
             .second) {
      error =
          "cross-module runtime link-plan duplicate translation-unit identity key: " +
          imported_input.translation_unit_identity_key;
      return false;
    }
    if (!seen_registration_ordinals
             .insert(imported_input.translation_unit_registration_order_ordinal)
             .second) {
      error =
          "cross-module runtime link-plan duplicate registration order ordinal: " +
          std::to_string(
              imported_input.translation_unit_registration_order_ordinal);
      return false;
    }
    if (imported_input.error_handling_result_and_bridging_artifact_replay_present) {
      if (imported_input.error_handling_contract_id != inputs.expected_error_handling_contract_id) {
        error =
            "cross-module runtime link-plan Part 6 contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.error_handling_source_contract_id !=
          inputs.expected_error_handling_source_contract_id) {
        error =
            "cross-module runtime link-plan Part 6 source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.error_handling_binary_artifact_replay_ready ||
          !imported_input.error_handling_runtime_import_artifact_ready ||
          !imported_input.error_handling_separate_compilation_replay_ready ||
          imported_input.error_handling_result_and_bridging_artifact_replay_key.empty() ||
          imported_input.error_handling_error_handling_replay_key.empty() ||
          imported_input.error_handling_throws_replay_key.empty() ||
          imported_input.error_handling_result_like_replay_key.empty() ||
          imported_input.error_handling_ns_error_replay_key.empty() ||
          imported_input.error_handling_unwind_replay_key.empty()) {
        error =
            "cross-module runtime link-plan Part 6 replay surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_error_handling_replay_keys.insert(imported_input.error_handling_error_handling_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 6 replay key: " +
            imported_input.error_handling_error_handling_replay_key;
        return false;
      }
      imported_error_handling_module_names.push_back(imported_input.module_name);
    }
    if (imported_input.concurrency_actor_mailbox_runtime_import_present) {
      // actor cross-module isolation-metadata hardening anchor:
      // imported actor-runtime surfaces must preserve one canonical private
      // mailbox/isolation replay contract and may not drift on contract ids or
      // replay keys across mixed-module link plans.
      if (imported_input.concurrency_actor_contract_id !=
          inputs.expected_concurrency_actor_contract_id) {
        error =
            "cross-module runtime link-plan Part 7 actor contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.concurrency_actor_source_contract_id !=
          inputs.expected_concurrency_actor_source_contract_id) {
        error =
            "cross-module runtime link-plan Part 7 actor source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.concurrency_actor_mailbox_runtime_ready ||
          !imported_input.concurrency_actor_mailbox_runtime_deterministic ||
          imported_input.concurrency_actor_mailbox_runtime_replay_key.empty() ||
          imported_input.concurrency_actor_lowering_replay_key.empty() ||
          imported_input.concurrency_actor_isolation_lowering_replay_key.empty()) {
        error =
            "cross-module runtime link-plan Part 7 actor replay surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_concurrency_actor_replay_keys
               .insert(imported_input.concurrency_actor_mailbox_runtime_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 7 actor replay key: " +
            imported_input.concurrency_actor_mailbox_runtime_replay_key;
        return false;
      }
      imported_concurrency_actor_module_names.push_back(imported_input.module_name);
    }
    if (imported_input.interop_ffi_metadata_interface_preservation_present) {
      // bridge-packaging/toolchain anchor: imported Part 11 runtime-
      // import surfaces must preserve one canonical metadata/interface packet
      // across mixed-module link plans before D002 claims live header/module/
      // bridge generation from that packaging topology.
      if (imported_input.interop_ffi_contract_id !=
          inputs.expected_interop_ffi_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 ffi contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.interop_ffi_source_contract_id !=
          inputs.expected_interop_ffi_source_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 ffi source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.interop_ffi_preservation_contract_id !=
          inputs.expected_interop_ffi_preservation_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 ffi preservation contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.interop_ffi_runtime_import_artifact_ready ||
          !imported_input.interop_ffi_separate_compilation_preservation_ready ||
          !imported_input.interop_ffi_deterministic ||
          imported_input.interop_ffi_replay_key.empty() ||
          imported_input.interop_ffi_lowering_replay_key.empty() ||
          imported_input.interop_ffi_preservation_replay_key.empty()) {
        error =
            "cross-module runtime link-plan Part 11 ffi preservation surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_interop_ffi_replay_keys
               .insert(imported_input.interop_ffi_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 11 ffi replay key: " +
            imported_input.interop_ffi_replay_key;
        return false;
      }
      imported_interop_ffi_module_names.push_back(imported_input.module_name);
    }
    if (imported_input.interop_header_module_bridge_generation_present) {
      if (imported_input.interop_header_module_bridge_contract_id !=
          inputs.expected_interop_header_module_bridge_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.interop_header_module_bridge_source_contract_id !=
          inputs.expected_interop_header_module_bridge_source_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.interop_header_module_bridge_preservation_contract_id !=
          inputs.expected_interop_header_module_bridge_preservation_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation preservation contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.interop_header_module_bridge_runtime_generation_ready ||
          !imported_input
               .interop_header_module_bridge_cross_module_packaging_ready ||
          !imported_input.interop_header_module_bridge_deterministic ||
          imported_input.interop_header_module_bridge_replay_key.empty() ||
          imported_input.interop_header_module_bridge_preservation_replay_key
              .empty() ||
          imported_input.interop_bridge_header_artifact_relative_path.empty() ||
          imported_input.interop_bridge_module_artifact_relative_path.empty() ||
          imported_input.interop_bridge_artifact_relative_path.empty()) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.interop_bridge_header_artifact_relative_path !=
              inputs.expected_interop_bridge_header_artifact_relative_path ||
          imported_input.interop_bridge_module_artifact_relative_path !=
              inputs.expected_interop_bridge_module_artifact_relative_path ||
          imported_input.interop_bridge_artifact_relative_path !=
              inputs.expected_interop_bridge_artifact_relative_path) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation artifact path mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_interop_header_module_bridge_replay_keys
               .insert(imported_input.interop_header_module_bridge_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 11 bridge-generation replay key: " +
            imported_input.interop_header_module_bridge_replay_key;
        return false;
      }
      imported_interop_header_module_bridge_module_names.push_back(
          imported_input.module_name);
    }
    if (imported_input.metaprogramming_macro_host_process_cache_runtime_integration_present) {
      if (imported_input.metaprogramming_macro_host_process_cache_contract_id !=
          inputs.expected_metaprogramming_host_cache_contract_id) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.metaprogramming_macro_host_process_cache_source_contract_id !=
          inputs.expected_metaprogramming_host_cache_source_contract_id) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.metaprogramming_macro_host_process_cache_runtime_ready ||
          !imported_input.metaprogramming_macro_host_process_cache_separate_compilation_ready ||
          !imported_input.metaprogramming_macro_host_process_cache_deterministic ||
          imported_input.metaprogramming_macro_host_process_cache_replay_key.empty() ||
          imported_input.metaprogramming_macro_host_process_cache_host_executable_relative_path.empty() ||
          imported_input.metaprogramming_macro_host_process_cache_root_relative_path.empty()) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.metaprogramming_macro_host_process_cache_host_executable_relative_path !=
          inputs.expected_metaprogramming_host_cache_executable_relative_path) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache executable path mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.metaprogramming_macro_host_process_cache_root_relative_path !=
          inputs.expected_metaprogramming_host_cache_root_relative_path) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache root path mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_metaprogramming_host_cache_replay_keys
               .insert(imported_input.metaprogramming_macro_host_process_cache_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported metaprogramming host/cache replay key: " +
            imported_input.metaprogramming_macro_host_process_cache_replay_key;
        return false;
      }
      imported_metaprogramming_host_cache_module_names.push_back(imported_input.module_name);
    }
    if (!imported_input.block_ownership_artifact_preservation_present ||
        imported_input.block_ownership_contract_id !=
            inputs.expected_block_ownership_contract_id ||
        imported_input.block_ownership_source_contract_id !=
            inputs.expected_block_ownership_source_contract_id ||
        imported_input
                .block_ownership_object_invoke_thunk_lowering_contract_id !=
            inputs
                .expected_block_ownership_object_invoke_thunk_lowering_contract_id ||
        imported_input.block_ownership_byref_helper_lowering_contract_id !=
            inputs.expected_block_ownership_byref_helper_lowering_contract_id ||
        imported_input
                .block_ownership_escape_runtime_hook_lowering_contract_id !=
            inputs
                .expected_block_ownership_escape_runtime_hook_lowering_contract_id ||
        imported_input
                .block_ownership_runtime_support_library_link_wiring_contract_id !=
            inputs
                .expected_block_ownership_runtime_support_library_link_wiring_contract_id) {
      error =
          "cross-module runtime link-plan block-ownership preservation contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.block_ownership_runtime_import_artifact_ready ||
        !imported_input
             .block_ownership_separate_compilation_preservation_ready ||
        !imported_input
             .block_ownership_runtime_support_library_link_wiring_ready ||
        !imported_input.block_ownership_deterministic ||
        imported_input.block_ownership_replay_key.empty() ||
        imported_input.block_ownership_local_invoke_trampoline_symbolized_sites >
            imported_input.block_ownership_local_block_literal_sites ||
        imported_input.block_ownership_local_copy_helper_symbolized_sites >
            imported_input.block_ownership_local_copy_helper_required_sites ||
        imported_input.block_ownership_local_dispose_helper_symbolized_sites >
            imported_input.block_ownership_local_dispose_helper_required_sites ||
        imported_input.block_ownership_local_escape_to_heap_sites >
            imported_input.block_ownership_local_block_literal_sites) {
      error =
          "cross-module runtime link-plan block-ownership preservation surface incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.storage_reflection_artifact_preservation_present ||
        imported_input.storage_reflection_contract_id !=
            inputs.expected_storage_reflection_contract_id ||
        imported_input.storage_reflection_source_contract_id !=
            inputs.expected_storage_reflection_source_contract_id ||
        imported_input
                .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id !=
            inputs
                .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id ||
        imported_input
                .storage_reflection_executable_property_accessor_layout_lowering_contract_id !=
            inputs
                .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id ||
        imported_input
                .storage_reflection_executable_ivar_layout_emission_contract_id !=
            inputs
                .expected_storage_reflection_executable_ivar_layout_emission_contract_id ||
        imported_input
                .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id !=
            inputs
                .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id) {
      error =
          "cross-module runtime link-plan storage/reflection preservation contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.storage_reflection_runtime_import_artifact_ready ||
        !imported_input
             .storage_reflection_separate_compilation_preservation_ready ||
        !imported_input.storage_reflection_deterministic ||
        imported_input.storage_reflection_replay_key.empty() ||
        imported_input.storage_reflection_local_property_descriptor_count !=
            imported_input.property_descriptor_count ||
        imported_input.storage_reflection_local_ivar_descriptor_count !=
            imported_input.ivar_descriptor_count ||
        imported_input.storage_reflection_synthesized_accessor_entries !=
            imported_input.storage_reflection_synthesized_getter_entries +
                imported_input.storage_reflection_synthesized_setter_entries ||
        imported_input.storage_reflection_ivar_layout_entries !=
            imported_input.ivar_descriptor_count) {
      error =
          "cross-module runtime link-plan storage/reflection preservation surface incomplete for " +
          imported_input.module_name;
      return false;
    }
    ordered_link_inputs.push_back(
        {imported_input.translation_unit_registration_order_ordinal,
         imported_input.translation_unit_identity_key,
         imported_input.module_name,
         imported_input.object_artifact_path,
         imported_input.driver_linker_flags});
    imported_class_descriptor_count += imported_input.class_descriptor_count;
    imported_protocol_descriptor_count +=
        imported_input.protocol_descriptor_count;
    imported_category_descriptor_count +=
        imported_input.category_descriptor_count;
    imported_property_descriptor_count +=
        imported_input.property_descriptor_count;
    imported_ivar_descriptor_count += imported_input.ivar_descriptor_count;
    imported_total_descriptor_count += imported_input.total_descriptor_count;
    imported_block_ownership_block_literal_sites +=
        imported_input.block_ownership_local_block_literal_sites;
    imported_block_ownership_invoke_trampoline_symbolized_sites +=
        imported_input.block_ownership_local_invoke_trampoline_symbolized_sites;
    imported_block_ownership_copy_helper_required_sites +=
        imported_input.block_ownership_local_copy_helper_required_sites;
    imported_block_ownership_dispose_helper_required_sites +=
        imported_input.block_ownership_local_dispose_helper_required_sites;
    imported_block_ownership_copy_helper_symbolized_sites +=
        imported_input.block_ownership_local_copy_helper_symbolized_sites;
    imported_block_ownership_dispose_helper_symbolized_sites +=
        imported_input.block_ownership_local_dispose_helper_symbolized_sites;
    imported_block_ownership_escape_to_heap_sites +=
        imported_input.block_ownership_local_escape_to_heap_sites;
    imported_block_ownership_byref_layout_symbolized_sites +=
        imported_input.block_ownership_local_byref_layout_symbolized_sites;
    imported_storage_reflection_implementation_owned_property_entries +=
        imported_input.storage_reflection_implementation_owned_property_entries;
    imported_storage_reflection_synthesized_accessor_owner_entries +=
        imported_input.storage_reflection_synthesized_accessor_owner_entries;
    imported_storage_reflection_synthesized_getter_entries +=
        imported_input.storage_reflection_synthesized_getter_entries;
    imported_storage_reflection_synthesized_setter_entries +=
        imported_input.storage_reflection_synthesized_setter_entries;
    imported_storage_reflection_synthesized_accessor_entries +=
        imported_input.storage_reflection_synthesized_accessor_entries;
    imported_storage_reflection_current_property_read_entries +=
        imported_input.storage_reflection_current_property_read_entries;
    imported_storage_reflection_current_property_write_entries +=
        imported_input.storage_reflection_current_property_write_entries;
    imported_storage_reflection_current_property_exchange_entries +=
        imported_input.storage_reflection_current_property_exchange_entries;
    imported_storage_reflection_weak_current_property_load_entries +=
        imported_input.storage_reflection_weak_current_property_load_entries;
    imported_storage_reflection_weak_current_property_store_entries +=
        imported_input.storage_reflection_weak_current_property_store_entries;
    imported_storage_reflection_ivar_layout_entries +=
        imported_input.storage_reflection_ivar_layout_entries;
    imported_storage_reflection_ivar_layout_owner_entries +=
        imported_input.storage_reflection_ivar_layout_owner_entries;
  }

  ordered_link_inputs.push_back(
      {inputs.local_translation_unit_registration_order_ordinal,
       inputs.local_translation_unit_identity_key,
       inputs.local_module_name,
       inputs.local_object_artifact_relative_path,
       inputs.local_driver_linker_flags});
  std::sort(ordered_link_inputs.begin(), ordered_link_inputs.end(),
            [](const auto &lhs, const auto &rhs) {
              if (lhs.registration_order_ordinal != rhs.registration_order_ordinal) {
                return lhs.registration_order_ordinal <
                       rhs.registration_order_ordinal;
              }
              return lhs.translation_unit_identity_key <
                     rhs.translation_unit_identity_key;
            });
  for (const auto &ordered_input : ordered_link_inputs) {
    ordered_link_object_artifacts.push_back(ordered_input.object_artifact_path);
    for (const auto &flag : ordered_input.driver_linker_flags) {
      if (seen_driver_linker_flags.insert(flag).second) {
        merged_driver_linker_flags.push_back(flag);
      }
    }
  }
  if (merged_driver_linker_flags.empty()) {
    error =
        "cross-module runtime link-plan merged driver-linker flag list is empty";
    return false;
  }

  std::vector<std::string> module_names_lexicographic;
  module_names_lexicographic.reserve(imported_inputs.size() + 1u);
  module_names_lexicographic.push_back(inputs.local_module_name);
  for (const auto &imported_input : imported_inputs) {
    module_names_lexicographic.push_back(imported_input.module_name);
  }
  std::sort(module_names_lexicographic.begin(), module_names_lexicographic.end());

  std::ostringstream imported_modules_json;
  imported_modules_json << "[\n";
  for (std::size_t index = 0; index < imported_inputs.size(); ++index) {
    const auto &imported_input = imported_inputs[index];
    imported_modules_json
        << "    {\n"
        << "      \"module_name\": \""
        << EscapeJsonString(imported_input.module_name) << "\",\n"
        << "      \"import_surface_artifact_path\": \""
        << EscapeJsonString(imported_input.import_surface_artifact_path)
        << "\",\n"
        << "      \"registration_manifest_artifact_path\": \""
        << EscapeJsonString(imported_input.registration_manifest_artifact_path)
        << "\",\n"
        << "      \"object_artifact_path\": \""
        << EscapeJsonString(imported_input.object_artifact_path) << "\",\n"
        << "      \"discovery_artifact_path\": \""
        << EscapeJsonString(imported_input.discovery_artifact_path) << "\",\n"
        << "      \"linker_response_artifact_path\": \""
        << EscapeJsonString(imported_input.linker_response_artifact_path)
        << "\",\n"
        << "      \"translation_unit_identity_model\": \""
        << EscapeJsonString(imported_input.translation_unit_identity_model)
        << "\",\n"
        << "      \"translation_unit_identity_key\": \""
        << EscapeJsonString(imported_input.translation_unit_identity_key)
        << "\",\n"
        << "      \"translation_unit_registration_order_ordinal\": "
        << imported_input.translation_unit_registration_order_ordinal << ",\n"
        << "      \"class_descriptor_count\": "
        << imported_input.class_descriptor_count << ",\n"
        << "      \"protocol_descriptor_count\": "
        << imported_input.protocol_descriptor_count << ",\n"
        << "      \"category_descriptor_count\": "
        << imported_input.category_descriptor_count << ",\n"
        << "      \"property_descriptor_count\": "
        << imported_input.property_descriptor_count << ",\n"
        << "      \"ivar_descriptor_count\": "
        << imported_input.ivar_descriptor_count << ",\n"
        << "      \"total_descriptor_count\": "
        << imported_input.total_descriptor_count << ",\n"
        << "      \"object_format\": \""
        << EscapeJsonString(imported_input.object_format) << "\",\n"
        << "      \"runtime_support_library_archive_relative_path\": \""
        << EscapeJsonString(
               imported_input.runtime_support_library_archive_relative_path)
        << "\",\n"
        << "      \"ready_for_live_registration_discovery_replay\": "
        << (imported_input.ready_for_live_registration_discovery_replay
                ? "true"
                : "false")
        << ",\n"
        << "      \"ready_for_live_restart_hardening\": "
        << (imported_input.ready_for_live_restart_hardening ? "true"
                                                            : "false")
        << ",\n"
        << "      \"bootstrap_live_registration_contract_id\": \""
        << EscapeJsonString(
               imported_input.bootstrap_live_registration_contract_id)
        << "\",\n"
        << "      \"bootstrap_live_restart_hardening_contract_id\": \""
        << EscapeJsonString(
               imported_input.bootstrap_live_restart_hardening_contract_id)
        << "\",\n"
        << "      \"bootstrap_live_replay_registered_images_symbol\": \""
        << EscapeJsonString(
               imported_input.bootstrap_live_replay_registered_images_symbol)
        << "\",\n"
        << "      \"bootstrap_live_reset_replay_state_snapshot_symbol\": \""
        << EscapeJsonString(
               imported_input
                   .bootstrap_live_reset_replay_state_snapshot_symbol)
        << "\",\n"
        << "      \"bootstrap_live_restart_reset_for_testing_symbol\": \""
        << EscapeJsonString(
               imported_input
                   .bootstrap_live_restart_reset_for_testing_symbol)
        << "\",\n"
        << "      \"bootstrap_live_restart_replay_registered_images_symbol\": \""
        << EscapeJsonString(
               imported_input
                   .bootstrap_live_restart_replay_registered_images_symbol)
        << "\",\n"
        << "      \"bootstrap_live_restart_reset_replay_state_snapshot_symbol\": \""
        << EscapeJsonString(
               imported_input
                   .bootstrap_live_restart_reset_replay_state_snapshot_symbol)
        << "\",\n"
        << "      \"error_handling_result_and_bridging_artifact_replay_present\": "
        << (imported_input.error_handling_result_and_bridging_artifact_replay_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"error_handling_binary_artifact_replay_ready\": "
        << (imported_input.error_handling_binary_artifact_replay_ready ? "true"
                                                              : "false")
        << ",\n"
        << "      \"error_handling_runtime_import_artifact_ready\": "
        << (imported_input.error_handling_runtime_import_artifact_ready ? "true"
                                                               : "false")
        << ",\n"
        << "      \"error_handling_separate_compilation_replay_ready\": "
        << (imported_input.error_handling_separate_compilation_replay_ready ? "true"
                                                                   : "false")
        << ",\n"
        << "      \"error_handling_deterministic\": "
        << (imported_input.error_handling_deterministic ? "true" : "false")
        << ",\n"
        << "      \"error_handling_contract_id\": \""
        << EscapeJsonString(imported_input.error_handling_contract_id) << "\",\n"
        << "      \"error_handling_source_contract_id\": \""
        << EscapeJsonString(imported_input.error_handling_source_contract_id)
        << "\",\n"
        << "      \"error_handling_result_and_bridging_artifact_replay_key\": \""
        << EscapeJsonString(
               imported_input.error_handling_result_and_bridging_artifact_replay_key)
        << "\",\n"
        << "      \"error_handling_replay_key\": \""
        << EscapeJsonString(imported_input.error_handling_error_handling_replay_key) << "\",\n"
        << "      \"throws_replay_key\": \""
        << EscapeJsonString(imported_input.error_handling_throws_replay_key)
        << "\",\n"
        << "      \"result_like_replay_key\": \""
        << EscapeJsonString(imported_input.error_handling_result_like_replay_key)
        << "\",\n"
        << "      \"ns_error_replay_key\": \""
        << EscapeJsonString(imported_input.error_handling_ns_error_replay_key)
        << "\",\n"
        << "      \"unwind_replay_key\": \""
        << EscapeJsonString(imported_input.error_handling_unwind_replay_key)
        << "\",\n"
        << "      \"concurrency_actor_mailbox_runtime_import_present\": "
        << (imported_input.concurrency_actor_mailbox_runtime_import_present ? "true"
                                                                      : "false")
        << ",\n"
        << "      \"concurrency_actor_mailbox_runtime_ready\": "
        << (imported_input.concurrency_actor_mailbox_runtime_ready ? "true"
                                                             : "false")
        << ",\n"
        << "      \"concurrency_actor_mailbox_runtime_deterministic\": "
        << (imported_input.concurrency_actor_mailbox_runtime_deterministic ? "true"
                                                                     : "false")
        << ",\n"
        << "      \"concurrency_actor_contract_id\": \""
        << EscapeJsonString(imported_input.concurrency_actor_contract_id)
        << "\",\n"
        << "      \"concurrency_actor_source_contract_id\": \""
        << EscapeJsonString(imported_input.concurrency_actor_source_contract_id)
        << "\",\n"
        << "      \"concurrency_actor_mailbox_runtime_replay_key\": \""
        << EscapeJsonString(imported_input.concurrency_actor_mailbox_runtime_replay_key)
        << "\",\n"
        << "      \"concurrency_actor_lowering_replay_key\": \""
        << EscapeJsonString(imported_input.concurrency_actor_lowering_replay_key)
        << "\",\n"
        << "      \"concurrency_actor_isolation_lowering_replay_key\": \""
        << EscapeJsonString(
               imported_input.concurrency_actor_isolation_lowering_replay_key)
        << "\",\n"
        << "      \"interop_ffi_metadata_interface_preservation_present\": "
        << (imported_input.interop_ffi_metadata_interface_preservation_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"interop_ffi_runtime_import_artifact_ready\": "
        << (imported_input.interop_ffi_runtime_import_artifact_ready ? "true"
                                                                    : "false")
        << ",\n"
        << "      \"interop_ffi_separate_compilation_preservation_ready\": "
        << (imported_input.interop_ffi_separate_compilation_preservation_ready
                ? "true"
                : "false")

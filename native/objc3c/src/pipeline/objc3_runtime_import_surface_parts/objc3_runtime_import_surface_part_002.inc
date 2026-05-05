                      "local_import_module_annotation_count",
                      surface.interop_local_import_module_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_imported_module_name_count",
                      surface.interop_local_imported_module_name_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_swift_name_annotation_count",
                      surface.interop_local_swift_name_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_swift_private_annotation_count",
                      surface.interop_local_swift_private_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_cpp_name_annotation_count",
                      surface.interop_local_cpp_name_annotation_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_header_name_annotation_count",
                      surface.interop_local_header_name_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_named_annotation_payload_count",
                      surface.interop_local_named_annotation_payload_count,
                      error)) {
    return false;
  }

  if (contract_id != kObjc3InteropForeignSurfaceInterfacePreservationContractId) {
    error =
        "unexpected Part 11 foreign surface interface preservation contract id in import surface";
    return false;
  }
  if (surface.interop_foreign_import_source_contract_id !=
      kObjc3InteropForeignImportSourceClosureContractId) {
    error =
        "unexpected Part 11 foreign/import source contract id in import surface";
    return false;
  }
  if (surface.interop_cpp_swift_source_contract_id !=
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionContractId) {
    error =
        "unexpected Part 11 C++/Swift annotation source contract id in import surface";
    return false;
  }

  surface.interop_foreign_surface_interface_preservation_present = true;
  surface.interop_contract_id = std::move(contract_id);
  return true;
}

bool PopulateImportedMetaprogrammingMacroHostProcessCacheRuntimeIntegration(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *integration_value = FindMember(
      root,
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName);
  if (integration_value == nullptr) {
    return true;
  }

  const JsonValue::Object *integration_object = AsObject(*integration_value);
  if (integration_object == nullptr) {
    error =
        "metaprogramming macro host process/cache runtime integration surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*integration_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*integration_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*integration_object, "runtime_import_artifact_ready",
                      surface.metaprogramming_macro_host_process_cache_runtime_ready,
                      error) ||
      !ReadBoolMember(*integration_object, "separate_compilation_ready",
                      surface.metaprogramming_macro_host_process_cache_separate_compilation_ready,
                      error) ||
      !ReadBoolMember(*integration_object, "deterministic",
                      surface.metaprogramming_macro_host_process_cache_deterministic,
                      error) ||
      !ReadStringMember(
          *integration_object, "replay_key",
          surface.metaprogramming_macro_host_process_cache_replay_key, error) ||
      !ReadStringMember(*integration_object, "host_executable_relative_path",
                        surface
                            .metaprogramming_macro_host_process_cache_host_executable_relative_path,
                        error) ||
      !ReadStringMember(*integration_object, "cache_root_relative_path",
                        surface.metaprogramming_macro_host_process_cache_root_relative_path,
                        error)) {
    return false;
  }

  if (contract_id !=
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId) {
    error =
        "unexpected metaprogramming macro host process/cache runtime integration contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId) {
    error =
        "unexpected metaprogramming macro host process/cache runtime integration source contract id in import surface";
    return false;
  }

  surface.metaprogramming_macro_host_process_cache_runtime_integration_present = true;
  surface.metaprogramming_macro_host_process_cache_contract_id = std::move(contract_id);
  surface.metaprogramming_macro_host_process_cache_source_contract_id =
      std::move(source_contract_id);
  return true;
}

bool PopulateImportedInteropFfiMetadataInterfacePreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "interop ffi metadata/interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string preservation_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(*preservation_object, "preservation_contract_id",
                        preservation_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.interop_ffi_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.interop_ffi_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.interop_ffi_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.interop_ffi_replay_key, error) ||
      !ReadStringMember(*preservation_object, "lowering_replay_key",
                        surface.interop_ffi_lowering_replay_key, error) ||
      !ReadStringMember(*preservation_object, "preservation_replay_key",
                        surface.interop_ffi_preservation_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_foreign_callable_count",
                      surface.interop_ffi_local_foreign_callable_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_metadata_preservation_sites",
                      surface.interop_ffi_local_metadata_preservation_sites,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_interface_annotation_sites",
                      surface.interop_ffi_local_interface_annotation_sites,
                      error)) {
    return false;
  }

  if (contract_id != kObjc3InteropFfiMetadataInterfacePreservationContractId) {
    error =
        "unexpected Part 11 ffi metadata/interface preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3InteropFfiMetadataInterfacePreservationSourceContractId) {
    error =
        "unexpected Part 11 ffi metadata/interface preservation source contract id in import surface";
    return false;
  }
  if (preservation_contract_id !=
      kObjc3InteropForeignSurfaceInterfacePreservationContractId) {
    error =
        "unexpected Part 11 ffi metadata/interface preservation dependency contract id in import surface";
    return false;
  }

  surface.interop_ffi_metadata_interface_preservation_present = true;
  surface.interop_ffi_contract_id = std::move(contract_id);
  surface.interop_ffi_source_contract_id = std::move(source_contract_id);
  surface.interop_ffi_preservation_contract_id =
      std::move(preservation_contract_id);
  return true;
}

bool PopulateImportedInteropHeaderModuleBridgeGeneration(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *generation_value = FindMember(
      root, kObjc3InteropHeaderModuleBridgeGenerationImportArtifactMemberName);
  if (generation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *generation_object = AsObject(*generation_value);
  if (generation_object == nullptr) {
    error =
        "interop header/module/bridge generation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string preservation_contract_id;
  if (!ReadStringMember(*generation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*generation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(*generation_object, "preservation_contract_id",
                        preservation_contract_id, error) ||
      !ReadStringMember(*generation_object, "header_artifact_relative_path",
                        surface.interop_bridge_header_artifact_relative_path,
                        error) ||
      !ReadStringMember(*generation_object, "module_artifact_relative_path",
                        surface.interop_bridge_module_artifact_relative_path,
                        error) ||
      !ReadStringMember(*generation_object, "bridge_artifact_relative_path",
                        surface.interop_bridge_artifact_relative_path, error) ||
      !ReadBoolMember(*generation_object, "runtime_generation_ready",
                      surface.interop_header_module_bridge_runtime_generation_ready,
                      error) ||
      !ReadBoolMember(
          *generation_object, "cross_module_packaging_ready",
          surface.interop_header_module_bridge_cross_module_packaging_ready,
          error) ||
      !ReadBoolMember(*generation_object, "deterministic",
                      surface.interop_header_module_bridge_deterministic,
                      error) ||
      !ReadStringMember(*generation_object, "replay_key",
                        surface.interop_header_module_bridge_replay_key, error) ||
      !ReadStringMember(*generation_object, "preservation_replay_key",
                        surface.interop_header_module_bridge_preservation_replay_key,
                        error) ||
      !ReadSizeMember(*generation_object, "local_foreign_callable_count",
                      surface.interop_header_module_bridge_local_foreign_callable_count,
                      error)) {
    return false;
  }

  if (contract_id != kObjc3InteropHeaderModuleBridgeGenerationContractId) {
    error =
        "unexpected Part 11 header/module/bridge generation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3InteropHeaderModuleBridgeGenerationSourceContractId) {
    error =
        "unexpected Part 11 header/module/bridge generation source contract id in import surface";
    return false;
  }
  if (preservation_contract_id !=
      kObjc3InteropHeaderModuleBridgeGenerationPreservationContractId) {
    error =
        "unexpected Part 11 header/module/bridge generation preservation contract id in import surface";
    return false;
  }

  // Deferred Part 11 bridge packets are still emitted into the import surface
  // for documentation/provenance, but cross-module runtime link planning should
  // only treat them as active when the bridge path itself is live.
  if (!surface.interop_header_module_bridge_runtime_generation_ready ||
      !surface.interop_header_module_bridge_cross_module_packaging_ready) {
    return true;
  }

  surface.interop_header_module_bridge_generation_present = true;
  surface.interop_header_module_bridge_contract_id = std::move(contract_id);
  surface.interop_header_module_bridge_source_contract_id =
      std::move(source_contract_id);
  surface.interop_header_module_bridge_preservation_contract_id =
      std::move(preservation_contract_id);
  return true;
}

bool PopulateImportedDispatchDispatchMetadataInterfacePreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, "objc_dispatch_dispatch_metadata_and_interface_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "dispatch dispatch metadata/interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.dispatch_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.dispatch_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.dispatch_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.dispatch_replay_key, error) ||
      !ReadStringMember(*preservation_object, "lowering_replay_key",
                        surface.dispatch_lowering_replay_key, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_direct_callable_record_count",
                      surface.dispatch_local_direct_callable_record_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_final_callable_record_count",
                      surface.dispatch_local_final_callable_record_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_final_container_record_count",
                      surface.dispatch_local_final_container_record_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_sealed_container_record_count",
                      surface.dispatch_local_sealed_container_record_count,
                      error)) {
    return false;
  }

  if (contract_id !=
      "objc3c.dispatch.dispatch.metadata.interface.preservation.v1") {
    error =
        "unexpected Part 9 dispatch metadata/interface preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      "objc3c.dispatch.dispatch.control.lowering.contract.v1") {
    error =
        "unexpected Part 9 dispatch metadata/interface preservation source contract id in import surface";
    return false;
  }

  surface.dispatch_dispatch_metadata_interface_preservation_present = true;
  surface.dispatch_contract_id = std::move(contract_id);
  surface.dispatch_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedRuntimeStorageReflectionArtifactPreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root,
      kObjc3RuntimeStorageReflectionArtifactPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "runtime storage/reflection artifact preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string dispatch_and_synthesized_accessor_lowering_surface_contract_id;
  std::string executable_property_accessor_layout_lowering_contract_id;
  std::string executable_ivar_layout_emission_contract_id;
  std::string executable_synthesized_accessor_property_lowering_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(
          *preservation_object,
          "dispatch_and_synthesized_accessor_lowering_surface_contract_id",
          dispatch_and_synthesized_accessor_lowering_surface_contract_id,
          error) ||
      !ReadStringMember(*preservation_object,
                        "executable_property_accessor_layout_lowering_contract_id",
                        executable_property_accessor_layout_lowering_contract_id,
                        error) ||
      !ReadStringMember(*preservation_object,
                        "executable_ivar_layout_emission_contract_id",
                        executable_ivar_layout_emission_contract_id, error) ||
      !ReadStringMember(
          *preservation_object,
          "executable_synthesized_accessor_property_lowering_contract_id",
          executable_synthesized_accessor_property_lowering_contract_id,
          error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.storage_reflection_runtime_import_artifact_ready,
                      error) ||
      !ReadBoolMember(
          *preservation_object, "separate_compilation_preservation_ready",
          surface.storage_reflection_separate_compilation_preservation_ready,
          error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.storage_reflection_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.storage_reflection_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_property_descriptor_count",
                      surface.storage_reflection_local_property_descriptor_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_ivar_descriptor_count",
                      surface.storage_reflection_local_ivar_descriptor_count,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "implementation_owned_property_entries",
          surface.storage_reflection_implementation_owned_property_entries,
          error) ||
      !ReadSizeMember(
          *preservation_object, "synthesized_accessor_owner_entries",
          surface.storage_reflection_synthesized_accessor_owner_entries,
          error) ||
      !ReadSizeMember(*preservation_object, "synthesized_getter_entries",
                      surface.storage_reflection_synthesized_getter_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "synthesized_setter_entries",
                      surface.storage_reflection_synthesized_setter_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "synthesized_accessor_entries",
                      surface.storage_reflection_synthesized_accessor_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "current_property_read_entries",
                      surface.storage_reflection_current_property_read_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "current_property_write_entries",
                      surface.storage_reflection_current_property_write_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "current_property_exchange_entries",
                      surface.storage_reflection_current_property_exchange_entries,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "weak_current_property_load_entries",
          surface.storage_reflection_weak_current_property_load_entries,
          error) ||
      !ReadSizeMember(
          *preservation_object, "weak_current_property_store_entries",
          surface.storage_reflection_weak_current_property_store_entries,
          error) ||
      !ReadSizeMember(*preservation_object, "ivar_layout_entries",
                      surface.storage_reflection_ivar_layout_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "ivar_layout_owner_entries",
                      surface.storage_reflection_ivar_layout_owner_entries,
                      error)) {
    return false;
  }

  if (contract_id !=
      kObjc3RuntimeStorageReflectionArtifactPreservationContractId) {
    error =
        "unexpected runtime storage/reflection artifact preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId) {
    error =
        "unexpected runtime storage/reflection source contract id in import surface";
    return false;
  }
  if (dispatch_and_synthesized_accessor_lowering_surface_contract_id !=
      kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId) {
    error =
        "unexpected runtime storage/reflection lowering contract id in import surface";
    return false;
  }
  if (executable_property_accessor_layout_lowering_contract_id !=
      kObjc3ExecutablePropertyAccessorLayoutLoweringContractId) {
    error =
        "unexpected runtime storage/reflection accessor-layout contract id in import surface";
    return false;
  }
  if (executable_ivar_layout_emission_contract_id !=
      kObjc3ExecutableIvarLayoutEmissionContractId) {
    error =
        "unexpected runtime storage/reflection ivar-layout contract id in import surface";
    return false;
  }
  if (executable_synthesized_accessor_property_lowering_contract_id !=
      kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId) {
    error =
        "unexpected runtime storage/reflection synthesized-accessor contract id in import surface";
    return false;
  }

  surface.storage_reflection_artifact_preservation_present = true;
  surface.storage_reflection_contract_id = std::move(contract_id);
  surface.storage_reflection_source_contract_id =
      std::move(source_contract_id);
  surface
      .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id =
      std::move(
          dispatch_and_synthesized_accessor_lowering_surface_contract_id);
  surface
      .storage_reflection_executable_property_accessor_layout_lowering_contract_id =
      std::move(executable_property_accessor_layout_lowering_contract_id);
  surface.storage_reflection_executable_ivar_layout_emission_contract_id =
      std::move(executable_ivar_layout_emission_contract_id);
  surface
      .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id =
      std::move(
          executable_synthesized_accessor_property_lowering_contract_id);
  return true;
}

bool PopulateImportedRuntimeBlockOwnershipArtifactPreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, kObjc3RuntimeBlockOwnershipArtifactPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "runtime block-ownership artifact preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string block_object_invoke_thunk_lowering_contract_id;
  std::string block_byref_helper_lowering_contract_id;
  std::string block_escape_runtime_hook_lowering_contract_id;
  std::string runtime_support_library_link_wiring_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(*preservation_object,
                        "block_object_invoke_thunk_lowering_contract_id",
                        block_object_invoke_thunk_lowering_contract_id,
                        error) ||
      !ReadStringMember(*preservation_object,
                        "block_byref_helper_lowering_contract_id",
                        block_byref_helper_lowering_contract_id, error) ||
      !ReadStringMember(*preservation_object,
                        "block_escape_runtime_hook_lowering_contract_id",
                        block_escape_runtime_hook_lowering_contract_id,
                        error) ||
      !ReadStringMember(*preservation_object,
                        "runtime_support_library_link_wiring_contract_id",
                        runtime_support_library_link_wiring_contract_id,
                        error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.block_ownership_runtime_import_artifact_ready,
                      error) ||
      !ReadBoolMember(
          *preservation_object, "separate_compilation_preservation_ready",
          surface.block_ownership_separate_compilation_preservation_ready,
          error) ||
      !ReadBoolMember(
          *preservation_object, "runtime_support_library_link_wiring_ready",
          surface.block_ownership_runtime_support_library_link_wiring_ready,
          error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.block_ownership_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.block_ownership_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_block_literal_sites",
                      surface.block_ownership_local_block_literal_sites,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "local_invoke_trampoline_symbolized_sites",
          surface.block_ownership_local_invoke_trampoline_symbolized_sites,
          error) ||
      !ReadSizeMember(*preservation_object, "local_copy_helper_required_sites",
                      surface.block_ownership_local_copy_helper_required_sites,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "local_dispose_helper_required_sites",
          surface.block_ownership_local_dispose_helper_required_sites,
          error) ||
      !ReadSizeMember(*preservation_object, "local_copy_helper_symbolized_sites",
                      surface.block_ownership_local_copy_helper_symbolized_sites,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "local_dispose_helper_symbolized_sites",
          surface.block_ownership_local_dispose_helper_symbolized_sites,
          error) ||
      !ReadSizeMember(*preservation_object, "local_escape_to_heap_sites",
                      surface.block_ownership_local_escape_to_heap_sites,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "local_byref_layout_symbolized_sites",
          surface.block_ownership_local_byref_layout_symbolized_sites,
          error)) {
    return false;
  }

  if (contract_id !=
      kObjc3RuntimeBlockOwnershipArtifactPreservationContractId) {
    error =
        "unexpected runtime block-ownership artifact preservation contract id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId) {
    error =
        "unexpected runtime block-ownership source contract id in import surface";
    return false;
  }
  if (block_object_invoke_thunk_lowering_contract_id !=
      Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId) {
    error =
        "unexpected runtime block-ownership invoke-thunk contract id in import surface";
    return false;
  }
  if (block_byref_helper_lowering_contract_id !=
      Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId) {
    error =
        "unexpected runtime block-ownership byref-helper contract id in import surface";
    return false;
  }
  if (block_escape_runtime_hook_lowering_contract_id !=
      Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId) {
    error =
        "unexpected runtime block-ownership escape-runtime-hook contract id in import surface";
    return false;
  }
  if (runtime_support_library_link_wiring_contract_id !=
      kObjc3RuntimeSupportLibraryLinkWiringContractId) {
    error =
        "unexpected runtime block-ownership runtime-link contract id in import surface";
    return false;
  }

  surface.block_ownership_artifact_preservation_present = true;
  surface.block_ownership_contract_id = std::move(contract_id);
  surface.block_ownership_source_contract_id = std::move(source_contract_id);
  surface.block_ownership_object_invoke_thunk_lowering_contract_id =
      std::move(block_object_invoke_thunk_lowering_contract_id);
  surface.block_ownership_byref_helper_lowering_contract_id =
      std::move(block_byref_helper_lowering_contract_id);
  surface.block_ownership_escape_runtime_hook_lowering_contract_id =
      std::move(block_escape_runtime_hook_lowering_contract_id);
  surface.block_ownership_runtime_support_library_link_wiring_contract_id =
      std::move(runtime_support_library_link_wiring_contract_id);
  return true;
}

bool ParseRuntimeMetadataSourceRecordSet(
    const JsonValue::Object &root, const std::string &declarations_name,
    Objc3RuntimeMetadataSourceRecordSet &record_set, std::string &error) {
  if (!ParseRecordArray(root, declarations_name, "classes",
                        record_set.classes_lexicographic, ParseClassRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "protocols",
                        record_set.protocols_lexicographic,
                        ParseProtocolRecord, error) ||
      !ParseRecordArray(root, declarations_name, "categories",
                        record_set.categories_lexicographic, ParseCategoryRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "properties",
                        record_set.properties_lexicographic, ParsePropertyRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "methods",
                        record_set.methods_lexicographic, ParseMethodRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "ivars",
                        record_set.ivars_lexicographic, ParseIvarRecord,
                        error)) {
    return false;
  }
  record_set.deterministic = true;
  return true;
}

bool ParseSerializedRuntimeMetadataReusePayload(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *payload_value =
      FindMember(root, kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName);
  if (payload_value == nullptr) {
    surface.reused_module_names_lexicographic = {
        surface.frontend_closure_summary.module_name};
    surface.uses_serialized_runtime_metadata_payload = false;
    return true;
  }
  const JsonValue::Object *payload_object = AsObject(*payload_value);
  if (payload_object == nullptr) {
    error = "serialized runtime metadata reuse payload must be an object";
    return false;
  }

  std::string contract_id;
  std::string module_name;
  std::string replay_key;
  std::vector<std::string> reused_module_names;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t metadata_reference_count = 0;
  bool ready = false;
  if (!ReadStringMember(*payload_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*payload_object, "module_name", module_name, error) ||
      !ReadStringArrayMember(*payload_object, "reused_module_names_lexicographic",
                             reused_module_names, error) ||
      !ReadSizeMember(*payload_object, "runtime_owned_declaration_count",
                      runtime_owned_declaration_count, error) ||
      !ReadSizeMember(*payload_object, "metadata_reference_count",
                      metadata_reference_count, error) ||
      !ReadBoolMember(*payload_object, "ready", ready, error) ||
      !ReadStringMember(*payload_object, "replay_key", replay_key, error)) {
    return false;
  }
  if (contract_id != kObjc3SerializedRuntimeMetadataArtifactReuseContractId) {
    error = "unexpected serialized runtime metadata reuse payload contract id";
    return false;
  }
  if (!ready) {
    error = "serialized runtime metadata reuse payload is not ready";
    return false;
  }
  if (reused_module_names.empty()) {
    error = "serialized runtime metadata reuse payload must list reused modules";
    return false;
  }

  Objc3RuntimeMetadataSourceRecordSet payload_record_set;
  if (!ParseRuntimeMetadataSourceRecordSet(*payload_object,
                                           "runtime_owned_declarations",
                                           payload_record_set, error)) {
    return false;
  }
  const JsonValue *references_value =
      FindMember(*payload_object, "metadata_references");
  if (references_value == nullptr) {
    error = "serialized runtime metadata reuse payload is missing metadata_references";
    return false;
  }
  const JsonValue::Array *references_array = AsArray(*references_value);
  if (references_array == nullptr) {
    error =
        "serialized runtime metadata reuse payload metadata_references must be an array";
    return false;
  }
  if (CountRuntimeMetadataSourceRecordSetDeclarations(payload_record_set) !=
      runtime_owned_declaration_count) {
    error =
        "serialized runtime metadata reuse payload declaration count does not match payload inventory";
    return false;
  }
  if (references_array->size() != metadata_reference_count ||
      CountRuntimeMetadataSourceRecordSetReferences(payload_record_set) !=
          metadata_reference_count) {
    error =
        "serialized runtime metadata reuse payload reference count does not match payload inventory";
    return false;
  }

  surface.runtime_metadata_source_records = std::move(payload_record_set);
  surface.reused_module_names_lexicographic = std::move(reused_module_names);
  surface.uses_serialized_runtime_metadata_payload = true;
  (void)module_name;
  (void)replay_key;
  return true;
}

bool ParseImportedRuntimeModuleSurface(const JsonValue::Object &root,
                                       Objc3ImportedRuntimeModuleSurface &surface,
                                       std::string &error) {
  if (!PopulateFrontendClosureSummary(root,
                                      surface.frontend_closure_summary,
                                      error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemOptionalKeypathSurface(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemGenericContractPreservation(root, surface,
                                                            error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemNullabilityContractPreservation(root, surface,
                                                                error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemProtocolContractPreservation(root, surface,
                                                             error)) {
    return false;
  }
  if (!PopulateImportedErrorHandlingResultAndBridgingArtifactReplay(root, surface,
                                                            error)) {
    return false;
  }
  if (!PopulateImportedConcurrencyActorMailboxRuntimeImport(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedInteropForeignSurfaceInterfacePreservation(root, surface,
                                                                 error)) {
    return false;
  }
  if (!PopulateImportedInteropFfiMetadataInterfacePreservation(root, surface,
                                                              error)) {
    return false;
  }
  if (!PopulateImportedInteropHeaderModuleBridgeGeneration(root, surface,
                                                          error)) {
    return false;
  }
  if (!PopulateImportedMetaprogrammingModuleInterfaceReplayPreservation(root, surface,
                                                               error)) {
    return false;
  }
  if (!PopulateImportedMetaprogrammingMacroHostProcessCacheRuntimeIntegration(root,
                                                                     surface,
                                                                     error)) {
    return false;
  }
  if (!PopulateImportedDispatchDispatchMetadataInterfacePreservation(root, surface,
                                                                  error)) {
    return false;
  }
  if (!PopulateImportedRuntimeBlockOwnershipArtifactPreservation(root, surface,
                                                                 error)) {
    return false;
  }
  if (!PopulateImportedRuntimeStorageReflectionArtifactPreservation(root,
                                                                    surface,
                                                                    error)) {
    return false;
  }
  Objc3RuntimeMetadataSourceRecordSet local_runtime_metadata_source_records;
  if (!ParseRuntimeMetadataSourceRecordSet(root, "runtime_owned_declarations",
                                           local_runtime_metadata_source_records,
                                           error)) {
    return false;
  }
  surface.frontend_closure_summary.runtime_metadata_source_records_ready = true;

  const JsonValue *references_value = FindMember(root, "metadata_references");
  if (references_value == nullptr) {
    error = "missing JSON array member 'metadata_references'";
    return false;
  }
  const JsonValue::Array *references_array = AsArray(*references_value);
  if (references_array == nullptr) {
    error = "JSON member 'metadata_references' must be an array";
    return false;
  }

  surface.frontend_closure_summary.class_record_count =
      local_runtime_metadata_source_records.classes_lexicographic.size();
  surface.frontend_closure_summary.protocol_record_count =
      local_runtime_metadata_source_records.protocols_lexicographic.size();
  surface.frontend_closure_summary.category_record_count =
      local_runtime_metadata_source_records.categories_lexicographic.size();
  surface.frontend_closure_summary.property_record_count =
      local_runtime_metadata_source_records.properties_lexicographic.size();
  surface.frontend_closure_summary.method_record_count =
      local_runtime_metadata_source_records.methods_lexicographic.size();
  surface.frontend_closure_summary.ivar_record_count =
      local_runtime_metadata_source_records.ivars_lexicographic.size();

  surface.frontend_closure_summary.superclass_reference_count = 0;
  surface.frontend_closure_summary.protocol_reference_count = 0;
  surface.frontend_closure_summary.property_accessor_reference_count = 0;
  surface.frontend_closure_summary.property_ivar_binding_reference_count = 0;
  surface.frontend_closure_summary.method_selector_reference_count = 0;
  for (const JsonValue &reference_value : *references_array) {
    const JsonValue::Object *reference_object = AsObject(reference_value);
    if (reference_object == nullptr) {
      error = "metadata_references must contain objects";
      return false;
    }
    std::string reference_kind;
    if (!ReadStringMember(*reference_object, "reference_kind", reference_kind,
                          error)) {
      return false;
    }
    if (reference_kind == "class-superclass") {
      ++surface.frontend_closure_summary.superclass_reference_count;
    } else if (reference_kind == "class-adopted-protocol" ||
               reference_kind == "protocol-inherited-protocol" ||
               reference_kind == "category-adopted-protocol") {
      ++surface.frontend_closure_summary.protocol_reference_count;
    } else if (reference_kind == "property-getter-selector" ||
               reference_kind == "property-setter-selector") {
      ++surface.frontend_closure_summary.property_accessor_reference_count;
    } else if (reference_kind == "property-ivar-binding") {
      ++surface.frontend_closure_summary.property_ivar_binding_reference_count;
    } else if (reference_kind == "method-selector") {
      ++surface.frontend_closure_summary.method_selector_reference_count;
    }
  }

  const std::size_t declaration_count =
      CountRuntimeMetadataSourceRecordSetDeclarations(
          local_runtime_metadata_source_records);
  if (surface.frontend_closure_summary.runtime_owned_declaration_count !=
      declaration_count) {
    error = "runtime-owned declaration count does not match imported record inventory";
    return false;
  }
  if (surface.frontend_closure_summary.metadata_reference_count !=
      references_array->size()) {
    error = "metadata reference count does not match imported reference inventory";
    return false;
  }
  if (surface.frontend_closure_summary.contract_id !=
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId) {
    error = "unexpected import-surface contract id";
    return false;
  }
  if (surface.frontend_closure_summary.source_surface_contract_id !=
      kObjc3RuntimeAwareImportModuleSurfaceContractId) {
    error = "unexpected import-surface source contract id";
    return false;
  }
  if (surface.frontend_closure_summary.frontend_surface_path !=
      kObjc3RuntimeAwareImportModuleFrontendClosureSurfacePath) {
    error = "unexpected import-surface frontend surface path";
    return false;
  }
  if (surface.frontend_closure_summary.payload_model !=
      kObjc3RuntimeAwareImportModuleFrontendClosurePayloadModel) {
    error = "unexpected import-surface payload model";
    return false;
  }
  if (surface.frontend_closure_summary.artifact_relative_path !=
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath) {
    error = "unexpected import-surface artifact name";
    return false;
  }
  if (surface.frontend_closure_summary.authority_model !=
      kObjc3RuntimeAwareImportModuleFrontendAuthorityModel) {
    error = "unexpected import-surface authority model";
    return false;
  }
  if (surface.frontend_closure_summary.payload_ownership_model !=
      kObjc3RuntimeAwareImportModuleFrontendPayloadOwnershipModel) {
    error = "unexpected import-surface ownership model";
    return false;
  }
  if (!surface.frontend_closure_summary.ready_for_frontend_module_consumption) {
    error = "import-surface artifact is not ready for frontend module consumption";
    return false;
  }
  if (!IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          surface.frontend_closure_summary)) {
    error = "import-surface closure summary is incomplete";
    return false;
  }
  if (!ParseSerializedRuntimeMetadataReusePayload(root, surface, error)) {
    return false;
  }
  if (!surface.uses_serialized_runtime_metadata_payload) {
    surface.runtime_metadata_source_records =
        std::move(local_runtime_metadata_source_records);
  }
  return true;
}

bool PopulateImportedRuntimeRegistrationManifestPeerArtifacts(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  std::string contract_id;
  bool ready_for_runtime_bootstrap_enforcement = false;
  bool ready_for_live_registration_discovery_replay = false;
  bool ready_for_live_restart_hardening = false;
  if (!ReadStringMember(root, "contract_id", contract_id, error) ||
      !ReadStringMember(root, "runtime_support_library_archive_relative_path",
                        artifacts.runtime_support_library_archive_relative_path,
                        error) ||
      !ReadStringMember(root, "translation_unit_identity_model",
                        artifacts.translation_unit_identity_model, error) ||
      !ReadStringMember(root, "translation_unit_identity_key",
                        artifacts.translation_unit_identity_key, error) ||
      !ReadStringMember(root, "object_format", artifacts.object_format, error) ||
      !ReadUnsignedMember(root, "translation_unit_registration_order_ordinal",
                          artifacts.translation_unit_registration_order_ordinal,
                          error) ||
      !ReadUnsignedMember(root, "class_descriptor_count",
                          artifacts.class_descriptor_count, error) ||
      !ReadUnsignedMember(root, "protocol_descriptor_count",
                          artifacts.protocol_descriptor_count, error) ||
      !ReadUnsignedMember(root, "category_descriptor_count",
                          artifacts.category_descriptor_count, error) ||
      !ReadUnsignedMember(root, "property_descriptor_count",
                          artifacts.property_descriptor_count, error) ||
      !ReadUnsignedMember(root, "ivar_descriptor_count",
                          artifacts.ivar_descriptor_count, error) ||
      !ReadUnsignedMember(root, "total_descriptor_count",
                          artifacts.total_descriptor_count, error) ||
      !ReadStringArrayMember(root, "driver_linker_flags",
                             artifacts.driver_linker_flags, error) ||
      !ReadStringMember(root, "bootstrap_live_registration_contract_id",
                        artifacts.bootstrap_live_registration_contract_id,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_restart_hardening_contract_id",
                        artifacts.bootstrap_live_restart_hardening_contract_id,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_replay_registered_images_symbol",
                        artifacts.bootstrap_live_replay_registered_images_symbol,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_reset_replay_state_snapshot_symbol",
                        artifacts.bootstrap_live_reset_replay_state_snapshot_symbol,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_restart_reset_for_testing_symbol",
                        artifacts.bootstrap_live_restart_reset_for_testing_symbol,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_restart_replay_registered_images_symbol",
                        artifacts.bootstrap_live_restart_replay_registered_images_symbol,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_restart_reset_replay_state_snapshot_symbol",
                        artifacts.bootstrap_live_restart_reset_replay_state_snapshot_symbol,
                        error) ||
      !ReadBoolMember(root, "ready_for_runtime_bootstrap_enforcement",
                      ready_for_runtime_bootstrap_enforcement, error) ||
      !ReadBoolMember(root, "ready_for_live_registration_discovery_replay",
                      ready_for_live_registration_discovery_replay, error) ||
      !ReadBoolMember(root, "ready_for_live_restart_hardening",
                      ready_for_live_restart_hardening, error)) {
    return false;
  }
  if (contract_id !=
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId) {
    error = "unexpected runtime registration manifest contract id";
    return false;
  }
  if (!ready_for_runtime_bootstrap_enforcement) {
    error =
        "runtime registration manifest is not ready for runtime bootstrap enforcement";
    return false;
  }
  if (!ready_for_live_registration_discovery_replay) {
    error =
        "runtime registration manifest is not ready for live registration discovery replay";
    return false;
  }
  if (!ready_for_live_restart_hardening) {
    error =
        "runtime registration manifest is not ready for live restart hardening";
    return false;
  }
  if (artifacts.total_descriptor_count !=
      artifacts.class_descriptor_count + artifacts.protocol_descriptor_count +
          artifacts.category_descriptor_count +
          artifacts.property_descriptor_count + artifacts.ivar_descriptor_count) {
    error =
        "runtime registration manifest descriptor counts are internally inconsistent";
    return false;
  }
  for (const auto &flag : artifacts.driver_linker_flags) {
    if (flag.empty()) {
      error = "runtime registration manifest contains an empty driver linker flag";
      return false;
    }
  }
  artifacts.ready_for_live_registration_discovery_replay =
      ready_for_live_registration_discovery_replay;
  artifacts.ready_for_live_restart_hardening =
      ready_for_live_restart_hardening;
  return true;
}

bool ValidateImportedRuntimeDiscoveryPeerArtifacts(
    const JsonValue::Object &root,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &object_artifact_relative_path, std::string &error) {
  std::string contract_id;
  std::string translation_unit_identity_model;
  std::string translation_unit_identity_key;
  std::string object_format;
  std::vector<std::string> driver_linker_flags;
  if (!ReadStringMember(root, "contract_id", contract_id, error) ||
      !ReadStringMember(root, "object_artifact",
                        object_artifact_relative_path, error) ||
      !ReadStringMember(root, "translation_unit_identity_model",
                        translation_unit_identity_model, error) ||
      !ReadStringMember(root, "translation_unit_identity_key",
                        translation_unit_identity_key, error) ||
      !ReadStringMember(root, "object_format", object_format, error) ||
      !ReadStringArrayMember(root, "driver_linker_flags", driver_linker_flags,
                             error)) {
    return false;
  }
  if (contract_id != kObjc3RuntimeLinkerRetentionContractId) {
    error = "unexpected runtime metadata discovery contract id";
    return false;
  }
  if (translation_unit_identity_model !=
      artifacts.translation_unit_identity_model) {
    error =
        "runtime metadata discovery translation-unit identity model mismatch";
    return false;
  }
  if (translation_unit_identity_key != artifacts.translation_unit_identity_key) {
    error = "runtime metadata discovery translation-unit identity key mismatch";
    return false;
  }
  if (object_format != artifacts.object_format) {
    error = "runtime metadata discovery object format mismatch";
    return false;
  }
  if (driver_linker_flags != artifacts.driver_linker_flags) {
    error = "runtime metadata discovery driver linker flags mismatch";
    return false;
  }
  return true;
}

}  // namespace

bool TryLoadObjc3ImportedRuntimeModuleSurface(
    const std::filesystem::path &path,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  std::string io_error;
  std::string payload;
  if (!objc3c::support::TryReadTextFile(path, payload, io_error, "unable to open file", "failed to read file")) {
    error = path.generic_string() + ": " + io_error;
    return false;
  }

  JsonParser parser(payload);
  JsonValue root_value;
  std::string parse_error;
  if (!parser.Parse(root_value, parse_error)) {
    error = path.generic_string() + ": " + parse_error;
    return false;
  }
  const JsonValue::Object *root_object = AsObject(root_value);
  if (root_object == nullptr) {
    error = path.generic_string() + ": import surface payload must be a JSON object";
    return false;
  }

  Objc3ImportedRuntimeModuleSurface parsed_surface;
  parsed_surface.source_path = path;
  if (!ParseImportedRuntimeModuleSurface(*root_object, parsed_surface,
                                         parse_error)) {
    error = path.generic_string() + ": " + parse_error;
    return false;
  }
  surface = std::move(parsed_surface);
  return true;
}

bool TryLoadObjc3ImportedRuntimeModulePackagingPeerArtifacts(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  artifacts = Objc3ImportedRuntimeModulePackagingPeerArtifacts{};
  error.clear();

  std::string emit_prefix;
  if (!TryResolveEmitPrefixFromImportSurfacePath(surface.source_path, emit_prefix,
                                                 error)) {
    error = surface.source_path.generic_string() + ": " + error;
    return false;
  }

  const std::filesystem::path parent = surface.source_path.parent_path();
  const std::filesystem::path registration_manifest_path =
      BuildRuntimeRegistrationManifestArtifactPath(parent, emit_prefix)
          .lexically_normal();
  const std::filesystem::path discovery_artifact_path =
      BuildRuntimeMetadataDiscoveryArtifactPath(parent, emit_prefix)
          .lexically_normal();
  const std::filesystem::path linker_response_artifact_path =
      BuildRuntimeMetadataLinkerResponseArtifactPath(parent, emit_prefix)
          .lexically_normal();

  std::string manifest_io_error;
  std::string manifest_payload;
  if (!objc3c::support::TryReadTextFile(registration_manifest_path,
                                        manifest_payload,
                                        manifest_io_error,
                                        "unable to open file",
                                        "failed to read file")) {
    error = registration_manifest_path.generic_string() + ": " + manifest_io_error;
    return false;
  }
  JsonParser manifest_parser(manifest_payload);
  JsonValue manifest_root_value;
  std::string manifest_parse_error;
  if (!manifest_parser.Parse(manifest_root_value, manifest_parse_error)) {
    error = registration_manifest_path.generic_string() + ": " +
            manifest_parse_error;
    return false;
  }
  const JsonValue::Object *manifest_root_object = AsObject(manifest_root_value);
  if (manifest_root_object == nullptr) {
    error = registration_manifest_path.generic_string() +
            ": runtime registration manifest payload must be a JSON object";
    return false;
  }

  Objc3ImportedRuntimeModulePackagingPeerArtifacts parsed_artifacts;
  if (!PopulateImportedRuntimeRegistrationManifestPeerArtifacts(
          *manifest_root_object, parsed_artifacts, manifest_parse_error)) {
    error = registration_manifest_path.generic_string() + ": " +
            manifest_parse_error;
    return false;
  }

  std::string discovery_io_error;
  std::string discovery_payload;
  if (!objc3c::support::TryReadTextFile(discovery_artifact_path,
                                        discovery_payload,
                                        discovery_io_error,
                                        "unable to open file",
                                        "failed to read file")) {
    error = discovery_artifact_path.generic_string() + ": " + discovery_io_error;
    return false;
  }
  JsonParser discovery_parser(discovery_payload);
  JsonValue discovery_root_value;
  std::string discovery_parse_error;
  if (!discovery_parser.Parse(discovery_root_value, discovery_parse_error)) {
    error = discovery_artifact_path.generic_string() + ": " +
            discovery_parse_error;
    return false;
  }
  const JsonValue::Object *discovery_root_object = AsObject(discovery_root_value);
  if (discovery_root_object == nullptr) {
    error = discovery_artifact_path.generic_string() +
            ": runtime metadata discovery payload must be a JSON object";
    return false;
  }

  std::string object_artifact_relative_path;
  if (!ValidateImportedRuntimeDiscoveryPeerArtifacts(*discovery_root_object,
                                                     parsed_artifacts,
                                                     object_artifact_relative_path,
                                                     discovery_parse_error)) {
    error = discovery_artifact_path.generic_string() + ": " +
            discovery_parse_error;
    return false;
  }

  std::string response_io_error;
  std::string linker_response_payload;
  if (!objc3c::support::TryReadTextFile(linker_response_artifact_path,
                                        linker_response_payload,
                                        response_io_error,
                                        "unable to open file",
                                        "failed to read file")) {
    error = linker_response_artifact_path.generic_string() + ": " +
            response_io_error;
    return false;
  }
  const std::vector<std::string> response_flags =
      SplitNonEmptyLines(linker_response_payload);
  if (response_flags != parsed_artifacts.driver_linker_flags) {
    error = linker_response_artifact_path.generic_string() +
            ": linker response payload drifted from imported driver linker flags";
    return false;
  }

  const std::filesystem::path object_artifact_path =
      (parent / object_artifact_relative_path).lexically_normal();
  if (!std::filesystem::exists(object_artifact_path)) {
    error = object_artifact_path.generic_string() +
            ": imported object artifact is missing";
    return false;
  }

  parsed_artifacts.registration_manifest_path = registration_manifest_path;
  parsed_artifacts.discovery_artifact_path = discovery_artifact_path;
  parsed_artifacts.linker_response_artifact_path = linker_response_artifact_path;
  parsed_artifacts.object_artifact_path = object_artifact_path;
  artifacts = std::move(parsed_artifacts);
  return true;
}

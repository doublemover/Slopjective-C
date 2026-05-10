void EmitObjc3IRRuntimeBootstrapMetadataComments(
    const Objc3IRFrontendMetadata &frontend_metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    const Objc3IRRuntimeBootstrapMetadataCommentOptions &options,
    std::ostringstream &out) {
  if (Objc3IRRuntimeBootstrapLoweringReady(frontend_metadata)) {
    out << "; runtime_bootstrap_ctor_init_emission = "
        << "contract=objc3c.runtime.constructor.init.stub.emission.v1"
        << ";constructor_root_symbol="
        << frontend_metadata.runtime_bootstrap_lowering_constructor_root_symbol
        << ";constructor_init_stub_symbol="
        << runtime_metadata_symbols.init_stub_symbol
        << ";registration_table_symbol="
        << runtime_metadata_symbols.registration_table_symbol
        << ";image_descriptor_symbol="
        << runtime_metadata_symbols.image_descriptor_symbol
        << ";registration_entrypoint_symbol="
        << frontend_metadata
               .runtime_bootstrap_lowering_registration_entrypoint_symbol
        << ";global_ctor_list_model="
        << frontend_metadata.runtime_bootstrap_lowering_global_ctor_list_model
        << ";happy_path=register-before-user-main\n";
    out << "; runtime_registration_table_image_local_initialization = "
        << "contract=objc3c.runtime.registration.table.image.local.initialization.v1"
        << ";registration_table_symbol="
        << runtime_metadata_symbols.registration_table_symbol
        << ";registration_table_layout_model="
        << frontend_metadata
               .runtime_bootstrap_lowering_registration_table_layout_model
        << ";registration_table_abi_version="
        << frontend_metadata
               .runtime_bootstrap_lowering_registration_table_abi_version
        << ";registration_table_pointer_field_count="
        << frontend_metadata
               .runtime_bootstrap_lowering_registration_table_pointer_field_count
        << ";class_section_root_symbol=__objc3_sec_class_descriptors"
        << ";protocol_section_root_symbol=__objc3_sec_protocol_descriptors"
        << ";category_section_root_symbol=__objc3_sec_category_descriptors"
        << ";property_section_root_symbol=__objc3_sec_property_descriptors"
        << ";ivar_section_root_symbol=__objc3_sec_ivar_descriptors"
        << ";selector_pool_symbol="
        << (options.selector_pool_globals_empty ? "null"
                                                : "@__objc3_sec_selector_pool")
        << ";string_pool_symbol="
        << (options.runtime_string_pool_globals_empty
                ? "null"
                : "@__objc3_sec_string_pool")
        << ";image_local_init_state_symbol="
        << runtime_metadata_symbols.image_local_init_state_symbol
        << ";image_local_initialization_model="
        << frontend_metadata
               .runtime_bootstrap_lowering_image_local_initialization_model
        << ";happy_path=guarded-once-before-runtime-registration\n";
    out << "; runtime_bootstrap_registrar_image_walk = "
        << "contract=" << kObjc3RuntimeBootstrapRegistrarContractId
        << ";stage_registration_table_symbol="
        << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
        << ";image_walk_snapshot_symbol="
        << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol
        << ";image_walk_model=" << kObjc3RuntimeBootstrapImageWalkModel
        << ";selector_pool_interning_model="
        << kObjc3RuntimeBootstrapSelectorPoolInterningModel
        << ";realization_staging_model="
        << kObjc3RuntimeBootstrapRealizationStagingModel << "\n";
  }

  if (Objc3IRRuntimeBootstrapRegistrationDescriptorImageRootLoweringReady(
          frontend_metadata)) {
    out << "; runtime_registration_descriptor_image_root_lowering = "
        << Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary()
        << ";registration_descriptor_identifier="
        << frontend_metadata.runtime_bootstrap_registration_descriptor_identifier
        << ";image_root_identifier="
        << frontend_metadata.runtime_bootstrap_image_root_identifier
        << ";registration_descriptor_symbol="
        << runtime_metadata_symbols.registration_descriptor_symbol
        << ";image_root_symbol=" << runtime_metadata_symbols.image_root_symbol
        << "\n";
    if (frontend_metadata.runtime_metadata_archive_static_link_discovery_ready) {
      out << "; runtime_bootstrap_archive_static_link_replay_corpus = "
          << Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary()
          << ";translation_unit_identity_key="
          << frontend_metadata
                 .runtime_metadata_archive_static_link_translation_unit_identity_key
          << ";registration_descriptor_identifier="
          << frontend_metadata
                 .runtime_bootstrap_registration_descriptor_identifier
          << ";image_root_identifier="
          << frontend_metadata.runtime_bootstrap_image_root_identifier
          << ";replay_registered_images_symbol="
          << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
          << ";reset_replay_state_snapshot_symbol="
          << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol << "\n";
    }
  }
}

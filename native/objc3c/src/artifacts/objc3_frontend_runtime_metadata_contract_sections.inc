void ApplyRuntimeMetadataSectionAbiContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi) {
  ir_frontend_metadata.runtime_metadata_section_abi_contract_id =
      runtime_metadata_section_abi.contract_id;
  ir_frontend_metadata.runtime_metadata_section_boundary_frozen =
      runtime_metadata_section_abi.boundary_frozen;
  ir_frontend_metadata.runtime_metadata_section_fail_closed =
      runtime_metadata_section_abi.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_object_file_inventory_frozen =
      runtime_metadata_section_abi.object_file_section_inventory_frozen;
  ir_frontend_metadata.runtime_metadata_section_symbol_policy_frozen =
      runtime_metadata_section_abi.symbol_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_visibility_model_frozen =
      runtime_metadata_section_abi.visibility_model_frozen;
  ir_frontend_metadata.runtime_metadata_section_retention_policy_frozen =
      runtime_metadata_section_abi.retention_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_ready_for_scaffold =
      runtime_metadata_section_abi.ready_for_section_scaffold;
  ir_frontend_metadata.runtime_metadata_section_logical_image_info_section =
      runtime_metadata_section_abi.logical_image_info_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_class_descriptor_section =
      runtime_metadata_section_abi.logical_class_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_protocol_descriptor_section =
      runtime_metadata_section_abi.logical_protocol_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_category_descriptor_section =
      runtime_metadata_section_abi.logical_category_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_property_descriptor_section =
      runtime_metadata_section_abi.logical_property_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_logical_ivar_descriptor_section =
      runtime_metadata_section_abi.logical_ivar_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_descriptor_symbol_prefix =
      runtime_metadata_section_abi.descriptor_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_aggregate_symbol_prefix =
      runtime_metadata_section_abi.aggregate_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_image_info_symbol =
      runtime_metadata_section_abi.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_descriptor_linkage =
      runtime_metadata_section_abi.descriptor_linkage;
  ir_frontend_metadata.runtime_metadata_section_aggregate_linkage =
      runtime_metadata_section_abi.aggregate_linkage;
  ir_frontend_metadata.runtime_metadata_section_visibility =
      runtime_metadata_section_abi.metadata_visibility;
  ir_frontend_metadata.runtime_metadata_section_retention_root =
      runtime_metadata_section_abi.retention_root;
}

void ApplyRuntimeMetadataSectionPublicationContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  ir_frontend_metadata.runtime_metadata_section_publication_contract_id =
      runtime_metadata_section_publication.contract_id;
  ir_frontend_metadata.runtime_metadata_section_publication_abi_contract_id =
      runtime_metadata_section_publication.abi_contract_id;
  ir_frontend_metadata.runtime_metadata_section_publication_emitted =
      runtime_metadata_section_publication.publication_emitted;
  ir_frontend_metadata.runtime_metadata_section_publication_fail_closed =
      runtime_metadata_section_publication.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_publication_uses_llvm_used =
      runtime_metadata_section_publication.uses_llvm_used;
  ir_frontend_metadata.runtime_metadata_section_publication_image_info_emitted =
      runtime_metadata_section_publication.image_info_emitted;
  ir_frontend_metadata
      .runtime_metadata_section_publication_class_descriptor_count =
      runtime_metadata_section_publication.class_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_protocol_descriptor_count =
      runtime_metadata_section_publication.protocol_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_category_descriptor_count =
      runtime_metadata_section_publication.category_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_property_descriptor_count =
      runtime_metadata_section_publication.property_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_ivar_descriptor_count =
      runtime_metadata_section_publication.ivar_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_total_descriptor_count =
      runtime_metadata_section_publication.total_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_total_retained_global_count =
      runtime_metadata_section_publication.total_retained_global_count;
  ir_frontend_metadata.runtime_metadata_section_publication_image_info_symbol =
      runtime_metadata_section_publication.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_publication_class_aggregate_symbol =
      runtime_metadata_section_publication.class_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_protocol_aggregate_symbol =
      runtime_metadata_section_publication.protocol_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_category_aggregate_symbol =
      runtime_metadata_section_publication.category_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_property_aggregate_symbol =
      runtime_metadata_section_publication.property_aggregate_symbol;
  ir_frontend_metadata.runtime_metadata_section_publication_ivar_aggregate_symbol =
      runtime_metadata_section_publication.ivar_aggregate_symbol;
}

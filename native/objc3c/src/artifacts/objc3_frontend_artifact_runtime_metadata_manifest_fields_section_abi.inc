  manifest << ",\"runtime_metadata_section_abi_contract_id\":\""
           << runtime_metadata_section_abi.contract_id
           << "\",\"runtime_metadata_section_boundary_frozen\":"
           << (runtime_metadata_section_abi.boundary_frozen ? "true" : "false")
           << ",\"runtime_metadata_section_fail_closed\":"
           << (runtime_metadata_section_abi.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_section_object_file_inventory_frozen\":"
           << (runtime_metadata_section_abi.object_file_section_inventory_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_metadata_section_symbol_policy_frozen\":"
           << (runtime_metadata_section_abi.symbol_policy_frozen ? "true"
                                                                 : "false")
           << ",\"runtime_metadata_section_visibility_model_frozen\":"
           << (runtime_metadata_section_abi.visibility_model_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_retention_policy_frozen\":"
           << (runtime_metadata_section_abi.retention_policy_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_ready_for_scaffold\":"
           << (runtime_metadata_section_abi.ready_for_section_scaffold ? "true"
                                                                       : "false")
           << ",\"runtime_metadata_section_logical_image_info_section\":\""
           << runtime_metadata_section_abi.logical_image_info_section
           << "\",\"runtime_metadata_section_logical_class_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_class_descriptor_section
           << "\",\"runtime_metadata_section_logical_protocol_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_protocol_descriptor_section
           << "\",\"runtime_metadata_section_logical_category_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_category_descriptor_section
           << "\",\"runtime_metadata_section_logical_property_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_property_descriptor_section
           << "\",\"runtime_metadata_section_logical_ivar_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_ivar_descriptor_section
           << "\",\"runtime_metadata_section_descriptor_symbol_prefix\":\""
           << runtime_metadata_section_abi.descriptor_symbol_prefix
           << "\",\"runtime_metadata_section_aggregate_symbol_prefix\":\""
           << runtime_metadata_section_abi.aggregate_symbol_prefix
           << "\",\"runtime_metadata_section_image_info_symbol\":\""
           << runtime_metadata_section_abi.image_info_symbol
           << "\",\"runtime_metadata_section_descriptor_linkage\":\""
           << runtime_metadata_section_abi.descriptor_linkage
           << "\",\"runtime_metadata_section_aggregate_linkage\":\""
           << runtime_metadata_section_abi.aggregate_linkage
           << "\",\"runtime_metadata_section_visibility\":\""
           << runtime_metadata_section_abi.metadata_visibility
           << "\",\"runtime_metadata_section_retention_root\":\""
           << runtime_metadata_section_abi.retention_root
           << "\",\"runtime_metadata_section_failure_reason\":\""
           << runtime_metadata_section_abi.failure_reason
           << "\"";

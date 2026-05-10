  manifest << ",\"runtime_metadata_section_publication_contract_id\":\""
           << runtime_metadata_section_publication.contract_id
           << "\",\"runtime_metadata_section_publication_abi_contract_id\":\""
           << runtime_metadata_section_publication.abi_contract_id
           << "\",\"runtime_metadata_section_publication_emitted\":"
           << (runtime_metadata_section_publication.publication_emitted ? "true"
                                                                        : "false")
           << ",\"runtime_metadata_section_publication_fail_closed\":"
           << (runtime_metadata_section_publication.fail_closed ? "true"
                                                               : "false")
           << ",\"runtime_metadata_section_publication_uses_llvm_used\":"
           << (runtime_metadata_section_publication.uses_llvm_used ? "true"
                                                                   : "false")
           << ",\"runtime_metadata_section_publication_image_info_emitted\":"
           << (runtime_metadata_section_publication.image_info_emitted ? "true"
                                                                       : "false")
           << ",\"runtime_metadata_section_publication_class_descriptor_count\":"
           << runtime_metadata_section_publication.class_descriptor_count
           << ",\"runtime_metadata_section_publication_protocol_descriptor_count\":"
           << runtime_metadata_section_publication.protocol_descriptor_count
           << ",\"runtime_metadata_section_publication_category_descriptor_count\":"
           << runtime_metadata_section_publication.category_descriptor_count
           << ",\"runtime_metadata_section_publication_property_descriptor_count\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"runtime_metadata_section_publication_ivar_descriptor_count\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"runtime_metadata_section_publication_total_descriptor_count\":"
           << runtime_metadata_section_publication.total_descriptor_count
           << ",\"runtime_metadata_section_publication_total_retained_global_count\":"
           << runtime_metadata_section_publication.total_retained_global_count
           << ",\"runtime_metadata_section_publication_image_info_symbol\":\""
           << runtime_metadata_section_publication.image_info_symbol
           << "\",\"runtime_metadata_section_publication_class_aggregate_symbol\":\""
           << runtime_metadata_section_publication.class_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_protocol_aggregate_symbol\":\""
           << runtime_metadata_section_publication.protocol_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_category_aggregate_symbol\":\""
           << runtime_metadata_section_publication.category_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_property_aggregate_symbol\":\""
           << runtime_metadata_section_publication.property_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_ivar_aggregate_symbol\":\""
           << runtime_metadata_section_publication.ivar_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_failure_reason\":\""
           << runtime_metadata_section_publication.failure_reason
           << "\"";

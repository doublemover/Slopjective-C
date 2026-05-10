  manifest << ",\"runtime_metadata_object_inspection_contract_id\":\""
           << runtime_metadata_object_inspection.contract_id
           << "\",\"runtime_metadata_object_inspection_publication_contract_id\":\""
           << runtime_metadata_object_inspection.publication_contract_id
           << "\",\"runtime_metadata_object_inspection_matrix_published\":"
           << (runtime_metadata_object_inspection.matrix_published ? "true"
                                                                   : "false")
           << ",\"runtime_metadata_object_inspection_fail_closed\":"
           << (runtime_metadata_object_inspection.fail_closed ? "true"
                                                              : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_readobj\":"
           << (runtime_metadata_object_inspection.uses_llvm_readobj ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_objdump\":"
           << (runtime_metadata_object_inspection.uses_llvm_objdump ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_matrix_row_count\":"
           << runtime_metadata_object_inspection.matrix_row_count
           << ",\"runtime_metadata_object_inspection_fixture_path\":\""
           << runtime_metadata_object_inspection.fixture_path
           << "\",\"runtime_metadata_object_inspection_emit_prefix\":\""
           << runtime_metadata_object_inspection.emit_prefix
           << "\",\"runtime_metadata_object_inspection_object_relative_path\":\""
           << runtime_metadata_object_inspection.object_relative_path
           << "\",\"runtime_metadata_object_inspection_section_inventory_row_key\":\""
           << runtime_metadata_object_inspection.section_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_section_inventory_command\":\""
           << runtime_metadata_object_inspection.section_inventory_command
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_row_key\":\""
           << runtime_metadata_object_inspection.symbol_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_command\":\""
           << runtime_metadata_object_inspection.symbol_inventory_command
           << "\",\"runtime_metadata_object_inspection_failure_reason\":\""
           << runtime_metadata_object_inspection.failure_reason
           << "\"";

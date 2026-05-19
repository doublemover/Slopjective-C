  manifest << ",\"runtime_metadata_source_ownership_contract_id\":\""
           << runtime_metadata_source_ownership.contract_id
           << "\",\"runtime_metadata_source_schema\":\""
           << runtime_metadata_source_ownership.canonical_source_schema
           << "\",\"runtime_metadata_ivar_source_model\":\""
           << runtime_metadata_source_ownership.ivar_record_source_model
           << "\",\"frontend_owns_runtime_metadata_source_records\":"
           << (runtime_metadata_source_ownership
                       .frontend_owns_runtime_metadata_source_records
                   ? "true"
                   : "false")
           << ",\"runtime_metadata_source_records_ready_for_lowering\":"
           << (runtime_metadata_source_ownership
                       .runtime_metadata_source_records_ready_for_lowering
                   ? "true"
                   : "false")
           << ",\"native_runtime_library_present\":"
           << (runtime_metadata_source_ownership.native_runtime_library_present
                   ? "true"
                   : "false")
           << ",\"runtime_link_test_only\":"
           << (runtime_metadata_source_ownership.runtime_link_test_only ? "true"
                                                                        : "false")
           << ",\"runtime_metadata_source_boundary_fail_closed\":"
           << (runtime_metadata_source_ownership.fail_closed ? "true"
                                                            : "false")
           << ",\"runtime_metadata_source_boundary_ready\":"
           << (IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
                   runtime_metadata_source_ownership)
                   ? "true"
                   : "false")
           << ",\"deterministic_runtime_metadata_source_schema\":"
           << (runtime_metadata_source_ownership.deterministic_source_schema
                   ? "true"
                   : "false")
           << ",\"runtime_metadata_class_record_count\":"
           << runtime_metadata_source_ownership.class_record_count
           << ",\"runtime_metadata_protocol_record_count\":"
           << runtime_metadata_source_ownership.protocol_record_count
           << ",\"runtime_metadata_category_interface_record_count\":"
           << runtime_metadata_source_ownership.category_interface_record_count
           << ",\"runtime_metadata_category_implementation_record_count\":"
           << runtime_metadata_source_ownership
                  .category_implementation_record_count
           << ",\"runtime_metadata_property_record_count\":"
           << runtime_metadata_source_ownership.property_record_count
           << ",\"runtime_metadata_method_record_count\":"
           << runtime_metadata_source_ownership.method_record_count
           << ",\"runtime_metadata_ivar_record_count\":"
           << runtime_metadata_source_ownership.ivar_record_count
           << ",\"runtime_metadata_source_boundary_failure_reason\":\""
           << runtime_metadata_source_ownership.failure_reason
           << "\"";

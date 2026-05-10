  manifest << ",\"runtime_export_enforcement_contract_id\":\""
           << runtime_export_enforcement.contract_id
           << "\",\"runtime_export_metadata_completeness_enforced\":"
           << (runtime_export_enforcement.metadata_completeness_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_duplicate_runtime_identity_suppression_enforced\":"
           << (runtime_export_enforcement
                       .duplicate_runtime_identity_suppression_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_illegal_redeclaration_mix_blocking_enforced\":"
           << (runtime_export_enforcement
                       .illegal_redeclaration_mix_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_metadata_shape_drift_blocking_enforced\":"
           << (runtime_export_enforcement
                       .metadata_shape_drift_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_enforcement_fail_closed\":"
           << (runtime_export_enforcement.fail_closed ? "true" : "false")
           << ",\"runtime_export_ready_for_runtime_export\":"
           << (runtime_export_enforcement.ready_for_runtime_export ? "true"
                                                                   : "false")
           << ",\"runtime_export_duplicate_runtime_identity_sites\":"
           << runtime_export_enforcement.duplicate_runtime_identity_sites
           << ",\"runtime_export_incomplete_declaration_sites\":"
           << runtime_export_enforcement.incomplete_declaration_sites
           << ",\"runtime_export_illegal_redeclaration_mix_sites\":"
           << runtime_export_enforcement.illegal_redeclaration_mix_sites
           << ",\"runtime_export_metadata_shape_drift_sites\":"
           << runtime_export_enforcement.metadata_shape_drift_sites
           << ",\"runtime_export_enforcement_failure_reason\":\""
           << runtime_export_enforcement.failure_reason
           << "\"";

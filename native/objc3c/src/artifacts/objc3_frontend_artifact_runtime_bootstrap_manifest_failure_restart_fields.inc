  manifest << ",\"runtime_bootstrap_failure_restart_semantics_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.contract_id)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_legality_semantics_contract_id)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_reset_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_reset_contract_id)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_semantics_contract_id)
           << "\",\"runtime_bootstrap_failure_restart_semantics_frontend_surface_path\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .frontend_surface_path)
           << "\",\"runtime_bootstrap_failure_restart_semantics_failure_mode\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.failure_mode)
           << "\",\"runtime_bootstrap_failure_restart_semantics_restart_lifecycle_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .restart_lifecycle_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_replay_order_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.replay_order_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_image_local_init_reset_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .image_local_init_reset_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_catalog_retention_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .catalog_retention_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_unsupported_topology_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .unsupported_topology_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_translation_unit_identity_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .translation_unit_identity_model)
           << "\",\"runtime_bootstrap_failure_restart_semantics_translation_unit_identity_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .translation_unit_identity_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_runtime_state_snapshot_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .runtime_state_snapshot_symbol)
           << "\",\"runtime_bootstrap_failure_restart_semantics_replay_registered_images_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .replay_registered_images_symbol)
           << "\",\"runtime_bootstrap_failure_restart_semantics_reset_replay_state_snapshot_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .reset_replay_state_snapshot_symbol)
           << "\",\"runtime_bootstrap_failure_restart_semantics_invalid_descriptor_status_code\":"
           << runtime_bootstrap_failure_restart_semantics
                  .invalid_descriptor_status_code
           << ",\"runtime_bootstrap_failure_restart_semantics_translation_unit_registration_order_ordinal\":"
           << runtime_bootstrap_failure_restart_semantics
                  .translation_unit_registration_order_ordinal
           << ",\"runtime_bootstrap_failure_restart_semantics_fail_closed\":"
           << (runtime_bootstrap_failure_restart_semantics.fail_closed ? "true"
                                                                       : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_semantic_boundary_ready\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .semantic_boundary_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_contract_ready\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .bootstrap_legality_semantics_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_contract_ready\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .bootstrap_semantics_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_reset_contract_ready\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .bootstrap_reset_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_failure_mode_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .failure_mode_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_restart_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .restart_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_replay_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .replay_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_unsupported_topology_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .unsupported_topology_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_deterministic_recovery_semantics_landed\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .deterministic_recovery_semantics_landed
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_runtime_restart_probe_required\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .runtime_restart_probe_required
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_ready_for_lowering_and_runtime\":"
           << (runtime_bootstrap_failure_restart_semantics
                       .ready_for_lowering_and_runtime
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_failure_restart_semantics_semantic_boundary_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .semantic_boundary_replay_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_legality_semantics_replay_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .bootstrap_semantics_replay_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.replay_key)
           << "\",\"runtime_bootstrap_failure_restart_semantics_failure_reason\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.failure_reason)
           << "\"";

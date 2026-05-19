  manifest << ",\"runtime_bootstrap_api_contract_id\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.contract_id)
           << "\",\"runtime_bootstrap_api_public_header_path\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.public_header_path)
           << "\",\"runtime_bootstrap_api_archive_relative_path\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.archive_relative_path)
           << "\",\"runtime_bootstrap_api_registration_status_enum_type\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_status_enum_type)
           << "\",\"runtime_bootstrap_api_image_descriptor_type\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.image_descriptor_type)
           << "\",\"runtime_bootstrap_api_selector_handle_type\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.selector_handle_type)
           << "\",\"runtime_bootstrap_api_registration_snapshot_type\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_snapshot_type)
           << "\",\"runtime_bootstrap_api_registration_entrypoint_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_entrypoint_symbol)
           << "\",\"runtime_bootstrap_api_selector_lookup_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.selector_lookup_symbol)
           << "\",\"runtime_bootstrap_api_dispatch_entrypoint_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.dispatch_entrypoint_symbol)
           << "\",\"runtime_bootstrap_api_state_snapshot_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.state_snapshot_symbol)
           << "\",\"runtime_bootstrap_api_reset_for_testing_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.reset_for_testing_symbol)
           << "\",\"runtime_bootstrap_api_registration_result_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_result_model)
           << "\",\"runtime_bootstrap_api_registration_order_ordinal_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.registration_order_ordinal_model)
           << "\",\"runtime_bootstrap_api_runtime_state_locking_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.runtime_state_locking_model)
           << "\",\"runtime_bootstrap_api_startup_invocation_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.startup_invocation_model)
           << "\",\"runtime_bootstrap_api_image_walk_lifecycle_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.image_walk_lifecycle_model)
           << "\",\"runtime_bootstrap_api_deterministic_reset_lifecycle_model\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.deterministic_reset_lifecycle_model)
           << "\",\"runtime_bootstrap_api_ready_for_registrar_implementation\":"
           << (runtime_bootstrap_api.ready_for_registrar_implementation ? "true"
                                                                        : "false")
           << ",\"runtime_bootstrap_api_support_library_core_feature_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.support_library_core_feature_replay_key)
           << "\",\"runtime_bootstrap_api_support_library_link_wiring_replay_key\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_api.support_library_link_wiring_replay_key)
           << "\""
           << ",\"runtime_bootstrap_api_replay_key\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.replay_key)
           << "\",\"runtime_bootstrap_api_failure_reason\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.failure_reason)
           << "\"";

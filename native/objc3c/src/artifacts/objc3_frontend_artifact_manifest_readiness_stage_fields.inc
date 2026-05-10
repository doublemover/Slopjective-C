  manifest << ",\"toolchain_runtime_ga_operations_cross_lane_integration_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_cross_lane_integration_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_cross_lane_integration_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_cross_lane_integration_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_docs_runbook_sync_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_docs_runbook_sync_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_docs_runbook_sync_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_docs_runbook_sync_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_core_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_core_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_diagnostics_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_diagnostics_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_diagnostics_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_conformance_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_conformance_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_conformance_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_conformance_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_integration_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_integration_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_integration_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_integration_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_performance_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_performance_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_performance_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_performance_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_shard2_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_core_shard2_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_shard2_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_advanced_core_shard2_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_integration_closeout_signoff_consistent\": "
           << (readiness_surface.toolchain_runtime_ga_operations_integration_closeout_signoff_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_integration_closeout_signoff_ready\": "
           << (readiness_surface.toolchain_runtime_ga_operations_integration_closeout_signoff_ready
                   ? "true"
                   : "false")
           << ",\"semantic_integration_surface_built\": "
           << (readiness_surface.semantic_integration_surface_built ? "true" : "false")
           << ",\"executable_metadata_lowering_handoff_ready\": "
           << (readiness_surface.executable_metadata_lowering_handoff_ready ? "true" : "false")
           << ",\"executable_metadata_lowering_handoff_deterministic\": "
           << (readiness_surface.executable_metadata_lowering_handoff_deterministic ? "true"
                                                                                    : "false")
           << ",\"executable_metadata_typed_lowering_handoff_ready\": "
           << (readiness_surface.executable_metadata_typed_lowering_handoff_ready ? "true" : "false")
           << ",\"executable_metadata_typed_lowering_handoff_deterministic\": "
           << (readiness_surface.executable_metadata_typed_lowering_handoff_deterministic ? "true"
                                                                                          : "false")
           << ",\"lowering_boundary_ready\": "
           << (readiness_surface.lowering_boundary_ready ? "true" : "false");

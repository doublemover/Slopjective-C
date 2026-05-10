  const bool lane_advanced_integration_shard3_consistent =
      lane_a_surface.cross_lane_integration_ready &&
      lane_b_surface.release_candidate_replay_dry_run_ready &&
      lane_c_surface.advanced_diagnostics_shard1_ready &&
      lane_d_surface.advanced_edge_compatibility_shard2_ready &&
      !lane_d_surface.advanced_edge_compatibility_shard2_key.empty();
  const bool advanced_integration_shard3_consistent =
      surface.advanced_conformance_shard3_ready &&
      lane_advanced_integration_shard3_consistent;
  const bool advanced_integration_shard3_ready =
      advanced_integration_shard3_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_conformance_shard3_key.empty() &&
      !lane_d_surface.advanced_edge_compatibility_shard2_key.empty();
  surface.advanced_integration_shard3_consistent =
      advanced_integration_shard3_consistent;
  surface.advanced_integration_shard3_ready = advanced_integration_shard3_ready;
  surface.advanced_integration_shard3_key =
      BuildObjc3FinalReadinessGateAdvancedIntegrationShard3Key(
          surface,
          lane_a_surface.cross_lane_integration_ready,
          lane_b_surface.release_candidate_replay_dry_run_ready,
          lane_c_surface.advanced_diagnostics_shard1_ready,
          lane_d_surface.advanced_edge_compatibility_shard2_ready,
          !lane_d_surface.advanced_edge_compatibility_shard2_key.empty());
  surface.advanced_integration_shard3_ready =
      surface.advanced_integration_shard3_ready &&
      !surface.advanced_integration_shard3_key.empty();
  const bool lane_advanced_performance_shard3_consistent =
      lane_a_surface.cross_lane_integration_ready &&
      lane_b_surface.advanced_core_shard1_ready &&
      lane_c_surface.advanced_diagnostics_shard1_ready &&
      lane_d_surface.advanced_diagnostics_shard2_ready &&
      !lane_d_surface.advanced_diagnostics_shard2_key.empty();
  const bool advanced_performance_shard3_consistent =
      surface.advanced_integration_shard3_ready &&
      lane_advanced_performance_shard3_consistent;
  const bool advanced_performance_shard3_ready =
      advanced_performance_shard3_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_integration_shard3_key.empty() &&
      !lane_d_surface.advanced_diagnostics_shard2_key.empty();
  surface.advanced_performance_shard3_consistent =
      advanced_performance_shard3_consistent;
  surface.advanced_performance_shard3_ready = advanced_performance_shard3_ready;
  surface.advanced_performance_shard3_key =
      BuildObjc3FinalReadinessGateAdvancedPerformanceShard3Key(
          surface,
          lane_a_surface.cross_lane_integration_ready,
          lane_b_surface.advanced_core_shard1_ready,
          lane_c_surface.advanced_diagnostics_shard1_ready,
          lane_d_surface.advanced_diagnostics_shard2_ready,
          !lane_d_surface.advanced_diagnostics_shard2_key.empty());
  surface.advanced_performance_shard3_ready =
      surface.advanced_performance_shard3_ready &&
      !surface.advanced_performance_shard3_key.empty();

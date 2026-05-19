  const bool lane_advanced_core_shard4_consistent =
      lane_a_surface.cross_lane_integration_ready &&
      lane_b_surface.advanced_core_shard1_ready &&
      lane_c_surface.advanced_conformance_shard1_ready &&
      lane_d_surface.advanced_conformance_shard2_ready &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  const bool advanced_core_shard4_consistent =
      surface.advanced_performance_shard3_ready &&
      lane_advanced_core_shard4_consistent;
  const bool advanced_core_shard4_ready =
      advanced_core_shard4_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_performance_shard3_key.empty() &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  surface.advanced_core_shard4_consistent = advanced_core_shard4_consistent;
  surface.advanced_core_shard4_ready = advanced_core_shard4_ready;
  surface.advanced_core_shard4_key =
      BuildObjc3FinalReadinessGateAdvancedCoreShard4Key(
          surface,
          lane_a_surface.cross_lane_integration_ready,
          lane_b_surface.advanced_core_shard1_ready,
          lane_c_surface.advanced_conformance_shard1_ready,
          lane_d_surface.advanced_conformance_shard2_ready,
          !lane_d_surface.advanced_conformance_shard2_key.empty());
  surface.advanced_core_shard4_ready =
      surface.advanced_core_shard4_ready &&
      !surface.advanced_core_shard4_key.empty();
  const bool lane_advanced_edge_compatibility_shard4_consistent =
      lane_a_surface.integration_closeout_signoff_ready &&
      lane_b_surface.integration_closeout_signoff_ready &&
      lane_c_surface.advanced_conformance_shard1_ready &&
      lane_d_surface.advanced_conformance_shard2_ready &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  const bool advanced_edge_compatibility_shard4_consistent =
      surface.advanced_core_shard4_ready &&
      lane_advanced_edge_compatibility_shard4_consistent;
  const bool advanced_edge_compatibility_shard4_ready =
      advanced_edge_compatibility_shard4_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_core_shard4_key.empty() &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  surface.advanced_edge_compatibility_shard4_consistent =
      advanced_edge_compatibility_shard4_consistent;
  surface.advanced_edge_compatibility_shard4_ready =
      advanced_edge_compatibility_shard4_ready;
  surface.advanced_edge_compatibility_shard4_key =
      BuildObjc3FinalReadinessGateAdvancedEdgeCompatibilityShard4Key(
          surface,
          lane_a_surface.integration_closeout_signoff_ready,
          lane_b_surface.integration_closeout_signoff_ready,
          lane_c_surface.advanced_conformance_shard1_ready,
          lane_d_surface.advanced_conformance_shard2_ready,
          !lane_d_surface.advanced_conformance_shard2_key.empty());
  surface.advanced_edge_compatibility_shard4_ready =
      surface.advanced_edge_compatibility_shard4_ready &&
      !surface.advanced_edge_compatibility_shard4_key.empty();
  const bool lane_advanced_integration_closeout_signoff_consistent =
      lane_a_surface.integration_closeout_signoff_ready &&
      lane_b_surface.integration_closeout_signoff_ready &&
      lane_c_surface.integration_closeout_signoff_ready &&
      lane_d_surface.integration_closeout_signoff_ready &&
      !lane_d_surface.integration_closeout_signoff_key.empty();
  const bool advanced_integration_closeout_signoff_consistent =
      surface.advanced_edge_compatibility_shard4_ready &&
      lane_advanced_integration_closeout_signoff_consistent;
  const bool advanced_integration_closeout_signoff_ready =
      advanced_integration_closeout_signoff_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_edge_compatibility_shard4_key.empty() &&
      !lane_d_surface.integration_closeout_signoff_key.empty();
  surface.advanced_integration_closeout_signoff_consistent =
      advanced_integration_closeout_signoff_consistent;
  surface.advanced_integration_closeout_signoff_ready =
      advanced_integration_closeout_signoff_ready;
  surface.advanced_integration_closeout_signoff_key =
      BuildObjc3FinalReadinessGateAdvancedIntegrationCloseoutSignoffKey(
          surface,
          lane_a_surface.integration_closeout_signoff_ready,
          lane_b_surface.integration_closeout_signoff_ready,
          lane_c_surface.integration_closeout_signoff_ready,
          lane_d_surface.integration_closeout_signoff_ready,
          !lane_d_surface.integration_closeout_signoff_key.empty());
  surface.advanced_integration_closeout_signoff_ready =
      surface.advanced_integration_closeout_signoff_ready &&
      !surface.advanced_integration_closeout_signoff_key.empty();

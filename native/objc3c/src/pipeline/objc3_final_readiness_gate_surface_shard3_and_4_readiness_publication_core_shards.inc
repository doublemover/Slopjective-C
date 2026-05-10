  const bool lane_advanced_core_shard2_consistent =
      lane_a_surface.recovery_determinism_ready &&
      lane_b_surface.conformance_matrix_ready &&
      lane_c_surface.conformance_corpus_ready &&
      lane_d_surface.diagnostics_hardening_ready &&
      !lane_d_surface.diagnostics_hardening_key.empty();
  const bool advanced_core_shard2_consistent =
      surface.advanced_performance_shard1_ready &&
      lane_advanced_core_shard2_consistent;
  const bool advanced_core_shard2_ready =
      advanced_core_shard2_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_performance_shard1_key.empty() &&
      !lane_d_surface.diagnostics_hardening_key.empty();
  surface.advanced_core_shard2_consistent = advanced_core_shard2_consistent;
  surface.advanced_core_shard2_ready = advanced_core_shard2_ready;
  surface.advanced_core_shard2_key =
      BuildObjc3FinalReadinessGateAdvancedCoreShard2Key(
          surface,
          lane_a_surface.recovery_determinism_ready,
          lane_b_surface.conformance_matrix_ready,
          lane_c_surface.conformance_corpus_ready,
          lane_d_surface.diagnostics_hardening_ready,
          !lane_d_surface.diagnostics_hardening_key.empty());
  surface.advanced_core_shard2_ready =
      surface.advanced_core_shard2_ready &&
      !surface.advanced_core_shard2_key.empty();
  const bool lane_advanced_core_shard3_consistent =
      lane_a_surface.conformance_corpus_ready &&
      lane_b_surface.cross_lane_integration_ready &&
      lane_c_surface.advanced_core_shard1_ready &&
      lane_d_surface.advanced_integration_shard1_ready &&
      !lane_d_surface.advanced_integration_shard1_key.empty();
  const bool advanced_core_shard3_consistent =
      surface.advanced_performance_shard2_ready &&
      lane_advanced_core_shard3_consistent;
  const bool advanced_core_shard3_ready =
      advanced_core_shard3_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_performance_shard2_key.empty() &&
      !lane_d_surface.advanced_integration_shard1_key.empty();
  surface.advanced_core_shard3_consistent = advanced_core_shard3_consistent;
  surface.advanced_core_shard3_ready = advanced_core_shard3_ready;
  surface.advanced_core_shard3_key =
      BuildObjc3FinalReadinessGateAdvancedCoreShard3Key(
          surface,
          lane_a_surface.conformance_corpus_ready,
          lane_b_surface.cross_lane_integration_ready,
          lane_c_surface.advanced_core_shard1_ready,
          lane_d_surface.advanced_integration_shard1_ready,
          !lane_d_surface.advanced_integration_shard1_key.empty());
  surface.advanced_core_shard3_ready =
      surface.advanced_core_shard3_ready &&
      !surface.advanced_core_shard3_key.empty();

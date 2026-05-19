#pragma once

#include <string>

struct Objc3IRFrontendIREmissionReadinessMetadata {
  bool ir_emission_completeness_modular_split_ready = false;
  std::string ir_emission_completeness_modular_split_key;
  bool ir_emission_core_feature_impl_ready = false;
  std::string ir_emission_core_feature_impl_key;
  bool ir_emission_core_feature_expansion_ready = false;
  std::string ir_emission_core_feature_expansion_key;
  bool ir_emission_core_feature_edge_case_compatibility_ready = false;
  std::string ir_emission_core_feature_edge_case_compatibility_key;
  bool ir_emission_core_feature_edge_case_robustness_ready = false;
  std::string ir_emission_core_feature_edge_case_robustness_key;
  bool ir_emission_core_feature_diagnostics_hardening_ready = false;
  std::string ir_emission_core_feature_diagnostics_hardening_key;
  bool ir_emission_core_feature_recovery_determinism_ready = false;
  std::string ir_emission_core_feature_recovery_determinism_key;
  bool ir_emission_core_feature_conformance_matrix_ready = false;
  std::string ir_emission_core_feature_conformance_matrix_key;
  bool ir_emission_core_feature_conformance_corpus_ready = false;
  std::string ir_emission_core_feature_conformance_corpus_key;
  bool ir_emission_core_feature_performance_quality_guardrails_ready = false;
  std::string ir_emission_core_feature_performance_quality_guardrails_key;
  bool ir_emission_core_feature_cross_lane_integration_sync_ready = false;
  std::string ir_emission_core_feature_cross_lane_integration_sync_key;
  bool ir_emission_core_feature_advanced_core_shard1_ready = false;
  std::string ir_emission_core_feature_advanced_core_shard1_key;
  bool ir_emission_core_feature_advanced_edge_compatibility_shard1_ready = false;
  std::string ir_emission_core_feature_advanced_edge_compatibility_shard1_key;
  bool ir_emission_core_feature_advanced_diagnostics_shard1_ready = false;
  std::string ir_emission_core_feature_advanced_diagnostics_shard1_key;
  bool ir_emission_core_feature_advanced_conformance_shard1_ready = false;
  std::string ir_emission_core_feature_advanced_conformance_shard1_key;
  bool ir_emission_core_feature_advanced_integration_shard1_ready = false;
  std::string ir_emission_core_feature_advanced_integration_shard1_key;
};

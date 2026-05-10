#pragma once

#include <string>

#include "ir/objc3_ir_frontend_metadata_ir_emission_readiness.h"

struct Objc3IRFrontendPipelineReadinessMetadata
    : Objc3IRFrontendIREmissionReadinessMetadata {
  bool ownership_aware_lowering_core_feature_expansion_ready = false;
  std::string ownership_aware_lowering_core_feature_expansion_key;
  bool ownership_aware_lowering_performance_quality_guardrails_ready = false;
  std::string ownership_aware_lowering_performance_quality_guardrails_key;
  bool ownership_aware_lowering_cross_lane_integration_ready = false;
  std::string ownership_aware_lowering_cross_lane_integration_key;
  bool lowering_pass_graph_core_feature_ready = false;
  std::string lowering_pass_graph_core_feature_key;
  bool lowering_pass_graph_core_feature_expansion_ready = false;
  std::string lowering_pass_graph_core_feature_expansion_key;
  bool lowering_pass_graph_edge_case_compatibility_ready = false;
  std::string lowering_pass_graph_edge_case_compatibility_key;
  bool lowering_pass_graph_edge_case_robustness_ready = false;
  std::string lowering_pass_graph_edge_case_robustness_key;
  bool lowering_pass_graph_diagnostics_hardening_ready = false;
  std::string lowering_pass_graph_diagnostics_hardening_key;
  bool lowering_pass_graph_recovery_determinism_ready = false;
  std::string lowering_pass_graph_recovery_determinism_key;
  bool lowering_pass_graph_conformance_matrix_ready = false;
  std::string lowering_pass_graph_conformance_matrix_key;
  bool lowering_pass_graph_conformance_corpus_ready = false;
  std::string lowering_pass_graph_conformance_corpus_key;
  bool lowering_pass_graph_performance_quality_guardrails_ready = false;
  std::string lowering_pass_graph_performance_quality_guardrails_key;
};

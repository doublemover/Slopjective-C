#include "ir/objc3_ir_emission_readiness_publication.h"

#include <sstream>
#include <string>

#include "ir/objc3_ir_frontend_metadata.h"

namespace {

void EmitOptionalReplayKey(const char *name, const std::string &key,
                           std::ostringstream &out) {
  if (!key.empty()) {
    out << "; " << name << " = " << key << "\n";
  }
}

void EmitReadyFlag(const char *name, bool ready, std::ostringstream &out) {
  out << "; " << name << "_ready = " << (ready ? "true" : "false") << "\n";
}

}  // namespace

void EmitObjc3IREmissionReadinessPublication(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitOptionalReplayKey(
      "ownership_aware_lowering_core_feature_expansion",
      metadata.ownership_aware_lowering_core_feature_expansion_key, out);
  EmitReadyFlag("ownership_aware_lowering_core_feature_expansion",
                metadata.ownership_aware_lowering_core_feature_expansion_ready,
                out);
  EmitOptionalReplayKey(
      "ownership_aware_lowering_performance_quality_guardrails",
      metadata.ownership_aware_lowering_performance_quality_guardrails_key, out);
  EmitReadyFlag(
      "ownership_aware_lowering_performance_quality_guardrails",
      metadata.ownership_aware_lowering_performance_quality_guardrails_ready,
      out);
  EmitOptionalReplayKey("ownership_aware_lowering_cross_lane_integration",
                        metadata.ownership_aware_lowering_cross_lane_integration_key,
                        out);
  EmitReadyFlag("ownership_aware_lowering_cross_lane_integration",
                metadata.ownership_aware_lowering_cross_lane_integration_ready,
                out);

  EmitOptionalReplayKey("lowering_pass_graph_core_feature",
                        metadata.lowering_pass_graph_core_feature_key, out);
  EmitReadyFlag("lowering_pass_graph_core_feature",
                metadata.lowering_pass_graph_core_feature_ready, out);
  EmitOptionalReplayKey(
      "lowering_pass_graph_core_feature_expansion",
      metadata.lowering_pass_graph_core_feature_expansion_key, out);
  EmitReadyFlag("lowering_pass_graph_core_feature_expansion",
                metadata.lowering_pass_graph_core_feature_expansion_ready, out);
  EmitOptionalReplayKey(
      "lowering_pass_graph_edge_case_compatibility",
      metadata.lowering_pass_graph_edge_case_compatibility_key, out);
  EmitReadyFlag("lowering_pass_graph_edge_case_compatibility",
                metadata.lowering_pass_graph_edge_case_compatibility_ready, out);
  EmitOptionalReplayKey("lowering_pass_graph_edge_case_robustness",
                        metadata.lowering_pass_graph_edge_case_robustness_key,
                        out);
  EmitReadyFlag("lowering_pass_graph_edge_case_robustness",
                metadata.lowering_pass_graph_edge_case_robustness_ready, out);
  EmitOptionalReplayKey("lowering_pass_graph_diagnostics_hardening",
                        metadata.lowering_pass_graph_diagnostics_hardening_key,
                        out);
  EmitReadyFlag("lowering_pass_graph_diagnostics_hardening",
                metadata.lowering_pass_graph_diagnostics_hardening_ready, out);
  EmitOptionalReplayKey("lowering_pass_graph_recovery_determinism",
                        metadata.lowering_pass_graph_recovery_determinism_key,
                        out);
  EmitReadyFlag("lowering_pass_graph_recovery_determinism",
                metadata.lowering_pass_graph_recovery_determinism_ready, out);
  EmitOptionalReplayKey("lowering_pass_graph_conformance_matrix",
                        metadata.lowering_pass_graph_conformance_matrix_key,
                        out);
  EmitReadyFlag("lowering_pass_graph_conformance_matrix",
                metadata.lowering_pass_graph_conformance_matrix_ready, out);
  EmitOptionalReplayKey("lowering_pass_graph_conformance_corpus",
                        metadata.lowering_pass_graph_conformance_corpus_key,
                        out);
  EmitReadyFlag("lowering_pass_graph_conformance_corpus",
                metadata.lowering_pass_graph_conformance_corpus_ready, out);
  EmitOptionalReplayKey(
      "lowering_pass_graph_performance_quality_guardrails",
      metadata.lowering_pass_graph_performance_quality_guardrails_key, out);
  EmitReadyFlag(
      "lowering_pass_graph_performance_quality_guardrails",
      metadata.lowering_pass_graph_performance_quality_guardrails_ready, out);

  EmitOptionalReplayKey("ir_emission_completeness_modular_split",
                        metadata.ir_emission_completeness_modular_split_key,
                        out);
  EmitReadyFlag("ir_emission_completeness_modular_split",
                metadata.ir_emission_completeness_modular_split_ready, out);
  EmitOptionalReplayKey("ir_emission_core_feature_impl",
                        metadata.ir_emission_core_feature_impl_key, out);
  EmitReadyFlag("ir_emission_core_feature_impl",
                metadata.ir_emission_core_feature_impl_ready, out);
  EmitOptionalReplayKey("ir_emission_core_feature_expansion",
                        metadata.ir_emission_core_feature_expansion_key, out);
  EmitReadyFlag("ir_emission_core_feature_expansion",
                metadata.ir_emission_core_feature_expansion_ready, out);
  EmitOptionalReplayKey(
      "ir_emission_core_feature_edge_case_compatibility",
      metadata.ir_emission_core_feature_edge_case_compatibility_key, out);
  EmitReadyFlag("ir_emission_core_feature_edge_case_compatibility",
                metadata.ir_emission_core_feature_edge_case_compatibility_ready,
                out);
  EmitOptionalReplayKey("ir_emission_core_feature_edge_case_robustness",
                        metadata.ir_emission_core_feature_edge_case_robustness_key,
                        out);
  EmitReadyFlag("ir_emission_core_feature_edge_case_robustness",
                metadata.ir_emission_core_feature_edge_case_robustness_ready,
                out);
  EmitOptionalReplayKey("ir_emission_core_feature_diagnostics_hardening",
                        metadata.ir_emission_core_feature_diagnostics_hardening_key,
                        out);
  EmitReadyFlag("ir_emission_core_feature_diagnostics_hardening",
                metadata.ir_emission_core_feature_diagnostics_hardening_ready,
                out);
  EmitOptionalReplayKey("ir_emission_core_feature_recovery_determinism",
                        metadata.ir_emission_core_feature_recovery_determinism_key,
                        out);
  EmitReadyFlag("ir_emission_core_feature_recovery_determinism",
                metadata.ir_emission_core_feature_recovery_determinism_ready,
                out);
  EmitOptionalReplayKey("ir_emission_core_feature_conformance_matrix",
                        metadata.ir_emission_core_feature_conformance_matrix_key,
                        out);
  EmitReadyFlag("ir_emission_core_feature_conformance_matrix",
                metadata.ir_emission_core_feature_conformance_matrix_ready, out);
  EmitOptionalReplayKey("ir_emission_core_feature_conformance_corpus",
                        metadata.ir_emission_core_feature_conformance_corpus_key,
                        out);
  EmitReadyFlag("ir_emission_core_feature_conformance_corpus",
                metadata.ir_emission_core_feature_conformance_corpus_ready, out);
  EmitOptionalReplayKey(
      "ir_emission_core_feature_performance_quality_guardrails",
      metadata.ir_emission_core_feature_performance_quality_guardrails_key, out);
  EmitReadyFlag(
      "ir_emission_core_feature_performance_quality_guardrails",
      metadata.ir_emission_core_feature_performance_quality_guardrails_ready,
      out);
  EmitOptionalReplayKey(
      "ir_emission_core_feature_cross_lane_integration_sync",
      metadata.ir_emission_core_feature_cross_lane_integration_sync_key, out);
  EmitReadyFlag("ir_emission_core_feature_cross_lane_integration_sync",
                metadata.ir_emission_core_feature_cross_lane_integration_sync_ready,
                out);
  EmitOptionalReplayKey("ir_emission_core_feature_advanced_core_shard1",
                        metadata.ir_emission_core_feature_advanced_core_shard1_key,
                        out);
  EmitReadyFlag("ir_emission_core_feature_advanced_core_shard1",
                metadata.ir_emission_core_feature_advanced_core_shard1_ready,
                out);
  EmitOptionalReplayKey(
      "ir_emission_core_feature_advanced_edge_compatibility_shard1",
      metadata.ir_emission_core_feature_advanced_edge_compatibility_shard1_key,
      out);
  EmitReadyFlag(
      "ir_emission_core_feature_advanced_edge_compatibility_shard1",
      metadata.ir_emission_core_feature_advanced_edge_compatibility_shard1_ready,
      out);
  EmitOptionalReplayKey(
      "ir_emission_core_feature_advanced_diagnostics_shard1",
      metadata.ir_emission_core_feature_advanced_diagnostics_shard1_key, out);
  EmitReadyFlag("ir_emission_core_feature_advanced_diagnostics_shard1",
                metadata.ir_emission_core_feature_advanced_diagnostics_shard1_ready,
                out);
  EmitOptionalReplayKey(
      "ir_emission_core_feature_advanced_conformance_shard1",
      metadata.ir_emission_core_feature_advanced_conformance_shard1_key, out);
  EmitReadyFlag("ir_emission_core_feature_advanced_conformance_shard1",
                metadata.ir_emission_core_feature_advanced_conformance_shard1_ready,
                out);
  EmitOptionalReplayKey(
      "ir_emission_core_feature_advanced_integration_shard1",
      metadata.ir_emission_core_feature_advanced_integration_shard1_key, out);
  EmitReadyFlag("ir_emission_core_feature_advanced_integration_shard1",
                metadata.ir_emission_core_feature_advanced_integration_shard1_ready,
                out);
}

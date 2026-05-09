#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"

#include <utility>

namespace {

using objc3::artifacts::frontend::BuildArcDiagnosticsFixitLoweringContract;
using objc3::artifacts::frontend::BuildAutoreleasePoolScopeLoweringContract;
using objc3::artifacts::frontend::BuildOwnershipQualifierLoweringContract;
using objc3::artifacts::frontend::BuildRetainReleaseOperationLoweringContract;
using objc3::artifacts::frontend::BuildWeakUnownedSemanticsLoweringContract;
using objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure;

void AddPostPipelineFailure(
    std::vector<Objc3FrontendArtifactPostPipelineFailure> &failures,
    const char *code,
    std::string message) {
  Objc3FrontendArtifactPostPipelineFailure failure;
  failure.present = true;
  failure.code = code == nullptr ? "" : code;
  failure.message = std::move(message);
  failures.push_back(std::move(failure));
}

void AddOwnershipAwareLoweringReadinessFailures(
    std::vector<Objc3FrontendArtifactPostPipelineFailure> &failures,
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::string scaffold_error;
  if (!IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady(scaffold,
                                                          scaffold_error)) {
    AddPostPipelineFailure(
        failures,
        "O3L305",
        "LLVM IR emission failed: ownership-aware lowering modular split "
        "scaffold check failed: " +
            scaffold_error);
  }
  std::string expansion_error;
  if (!IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady(
          scaffold, expansion_error)) {
    AddPostPipelineFailure(
        failures,
        "O3L310",
        "LLVM IR emission failed: ownership-aware lowering core feature "
        "expansion check failed: " +
            expansion_error);
  }
  std::string edge_case_compatibility_error;
  if (!IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(
          scaffold, edge_case_compatibility_error)) {
    AddPostPipelineFailure(
        failures,
        "O3L312",
        "LLVM IR emission failed: ownership-aware lowering edge-case "
        "compatibility check failed: " +
            edge_case_compatibility_error);
  }
  std::string recovery_determinism_error;
  if (!IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady(
          scaffold, recovery_determinism_error)) {
    AddPostPipelineFailure(
        failures,
        "O3L318",
        "LLVM IR emission failed: ownership-aware lowering recovery "
        "determinism check failed: " +
            recovery_determinism_error);
  }
  std::string conformance_matrix_error;
  if (!IsObjc3OwnershipAwareLoweringBehaviorConformanceMatrixReady(
          scaffold, conformance_matrix_error)) {
    AddPostPipelineFailure(
        failures,
        "O3L319",
        "LLVM IR emission failed: ownership-aware lowering conformance "
        "matrix check failed: " +
            conformance_matrix_error);
  }
  std::string conformance_corpus_error;
  if (!IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady(
          scaffold, conformance_corpus_error)) {
    AddPostPipelineFailure(
        failures,
        "O3L320",
        "LLVM IR emission failed: ownership-aware lowering conformance "
        "corpus check failed: " +
            conformance_corpus_error);
  }
  std::string performance_quality_guardrails_error;
  if (!IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady(
          scaffold, performance_quality_guardrails_error)) {
    AddPostPipelineFailure(
        failures,
        "O3L328",
        "LLVM IR emission failed: ownership-aware lowering performance "
        "quality guardrails check failed: " +
            performance_quality_guardrails_error);
  }
  std::string cross_lane_integration_error;
  if (!IsObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationReady(
          scaffold, cross_lane_integration_error)) {
    AddPostPipelineFailure(
        failures,
        "O3L329",
        "LLVM IR emission failed: ownership-aware lowering cross-lane "
        "integration check failed: " +
            cross_lane_integration_error);
  }
}

}  // namespace

Objc3FrontendArtifactOwnershipAwareLoweringPlan
BuildObjc3FrontendArtifactOwnershipAwareLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result,
    bool metadata_only_ir_emission_mode) {
  Objc3FrontendArtifactOwnershipAwareLoweringPlan plan;
  plan.ownership_qualifier_lowering_contract =
      BuildOwnershipQualifierLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3OwnershipQualifierLoweringContract(
          plan.ownership_qualifier_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid ownership-qualifier lowering "
        "contract");
  }
  plan.ownership_qualifier_lowering_replay_key =
      Objc3OwnershipQualifierLoweringReplayKey(
          plan.ownership_qualifier_lowering_contract);
  plan.retain_release_operation_lowering_contract =
      BuildRetainReleaseOperationLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3RetainReleaseOperationLoweringContract(
          plan.retain_release_operation_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid retain-release operation lowering "
        "contract");
  }
  plan.retain_release_operation_lowering_replay_key =
      Objc3RetainReleaseOperationLoweringReplayKey(
          plan.retain_release_operation_lowering_contract);
  plan.autoreleasepool_scope_lowering_contract =
      BuildAutoreleasePoolScopeLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3AutoreleasePoolScopeLoweringContract(
          plan.autoreleasepool_scope_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid autoreleasepool scope lowering "
        "contract");
  }
  plan.autoreleasepool_scope_lowering_replay_key =
      Objc3AutoreleasePoolScopeLoweringReplayKey(
          plan.autoreleasepool_scope_lowering_contract);
  plan.weak_unowned_semantics_lowering_contract =
      BuildWeakUnownedSemanticsLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3WeakUnownedSemanticsLoweringContract(
          plan.weak_unowned_semantics_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid weak-unowned semantics lowering "
        "contract");
  }
  plan.weak_unowned_semantics_lowering_replay_key =
      Objc3WeakUnownedSemanticsLoweringReplayKey(
          plan.weak_unowned_semantics_lowering_contract);
  plan.arc_diagnostics_fixit_lowering_contract =
      BuildArcDiagnosticsFixitLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ArcDiagnosticsFixitLoweringContract(
          plan.arc_diagnostics_fixit_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid ARC diagnostics/fix-it lowering "
        "contract");
  }
  plan.arc_diagnostics_fixit_lowering_replay_key =
      Objc3ArcDiagnosticsFixitLoweringReplayKey(
          plan.arc_diagnostics_fixit_lowering_contract);
  plan.ownership_aware_lowering_behavior_scaffold =
      BuildObjc3OwnershipAwareLoweringBehaviorScaffold(
          plan.ownership_qualifier_lowering_contract,
          plan.ownership_qualifier_lowering_replay_key,
          plan.retain_release_operation_lowering_contract,
          plan.retain_release_operation_lowering_replay_key,
          plan.autoreleasepool_scope_lowering_contract,
          plan.autoreleasepool_scope_lowering_replay_key,
          plan.weak_unowned_semantics_lowering_contract,
          plan.weak_unowned_semantics_lowering_replay_key,
          plan.arc_diagnostics_fixit_lowering_contract,
          plan.arc_diagnostics_fixit_lowering_replay_key,
          pipeline_result.parse_lowering_readiness_surface
              .compatibility_handoff_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .language_version_pragma_coordinate_order_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_edge_case_robustness_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_replay_key_deterministic,
          pipeline_result.parse_lowering_readiness_surface
              .compatibility_handoff_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_edge_robustness_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_recovery_determinism_hardening_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_recovery_determinism_hardening_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_matrix_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_matrix_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_corpus_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_corpus_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_corpus_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_passed_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_failed_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_key,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .conformance_corpus_ready,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .conformance_corpus_key,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .performance_quality_guardrails_ready,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .performance_quality_guardrails_key);
  if (!metadata_only_ir_emission_mode) {
    AddOwnershipAwareLoweringReadinessFailures(
        plan.post_pipeline_failures,
        plan.ownership_aware_lowering_behavior_scaffold);
  }
  return plan;
}

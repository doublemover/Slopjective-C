#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "sema/objc3_semantic_passes.h"

inline constexpr std::size_t kObjc3TypedSemaToLoweringCoreFeatureCaseCount = 6u;
inline constexpr std::size_t kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount = 4u;
inline constexpr std::size_t kObjc3TypedSemaToLoweringPerformanceQualityGuardrailsCaseCount = 4u;

inline std::size_t Objc3TypedSemaToLoweringParserSnapshotDeclarationBreakdownCount(
    const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.global_decl_count + snapshot.protocol_decl_count +
         snapshot.interface_decl_count + snapshot.implementation_decl_count +
         snapshot.function_decl_count;
}

inline std::size_t Objc3TypedSemaToLoweringParsedProgramTopLevelDeclarationCount(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return ast.globals.size() + ast.protocols.size() + ast.interfaces.size() +
         ast.implementations.size() + ast.functions.size();
}

inline bool IsObjc3TypedSemaToLoweringLanguageVersionPragmaContractConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract) {
  if (!pragma_contract.seen) {
    return pragma_contract.directive_count == 0 &&
           !pragma_contract.duplicate &&
           !pragma_contract.non_leading &&
           pragma_contract.first_line == 0 &&
           pragma_contract.first_column == 0 &&
           pragma_contract.last_line == 0 &&
           pragma_contract.last_column == 0;
  }

  const bool coordinates_present =
      pragma_contract.first_line > 0 &&
      pragma_contract.first_column > 0 &&
      pragma_contract.last_line > 0 &&
      pragma_contract.last_column > 0;
  const bool duplicate_consistent =
      !pragma_contract.duplicate || pragma_contract.directive_count > 1;
  return pragma_contract.directive_count > 0 &&
         coordinates_present &&
         duplicate_consistent;
}

inline bool IsObjc3TypedSemaToLoweringLanguageVersionPragmaCoordinateOrderConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract) {
  if (!pragma_contract.seen) {
    return true;
  }

  const bool first_before_or_equal_last =
      pragma_contract.first_line < pragma_contract.last_line ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column <= pragma_contract.last_column);
  const bool single_directive_coordinates_consistent =
      pragma_contract.directive_count != 1 ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column == pragma_contract.last_column);
  return first_before_or_equal_last &&
         single_directive_coordinates_consistent;
}

inline const char *Objc3TypedSemaToLoweringLanguageProfileName(
    const Objc3FrontendLanguageProfile mode) {
  (void)mode;
  return "canonical";
}

inline std::string BuildObjc3TypedSemaToLoweringCompatibilityHandoffKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    bool compatibility_handoff_consistent) {
  return "language_profile=" +
         std::string(Objc3TypedSemaToLoweringLanguageProfileName(options.language_profile)) +
         ";canonical_literal_rejections=" +
         std::to_string(canonical_literal_rejection_counts.yes_literal_sites) +
         ":" +
         std::to_string(canonical_literal_rejection_counts.no_literal_sites) +
         ":" +
         std::to_string(canonical_literal_rejection_counts.null_literal_sites) +
         ";language_version_pragma=" + (pragma_contract.seen ? "seen" : "none") + ":" +
         std::to_string(pragma_contract.directive_count) + ":" +
         (pragma_contract.duplicate ? "duplicate" : "single") + ":" +
         (pragma_contract.non_leading ? "non-leading" : "leading") +
         ";consistent=" + (compatibility_handoff_consistent ? "true" : "false");
}

inline std::string BuildObjc3TypedSemaToLoweringParseArtifactEdgeRobustnessKey(
    std::size_t parser_token_count,
    std::size_t parser_snapshot_breakdown_count,
    std::size_t ast_top_level_declaration_count,
    bool parser_token_count_budget_consistent,
    bool language_version_pragma_coordinate_order_consistent,
    bool parse_artifact_edge_case_robustness_consistent) {
  return "parser_tokens=" + std::to_string(parser_token_count) +
         ";snapshot_breakdown=" + std::to_string(parser_snapshot_breakdown_count) +
         ";ast_top_level=" + std::to_string(ast_top_level_declaration_count) +
         ";token_budget_consistent=" + (parser_token_count_budget_consistent ? "true" : "false") +
         ";pragma_coordinate_order_consistent=" +
         (language_version_pragma_coordinate_order_consistent ? "true" : "false") +
         ";consistent=" + (parse_artifact_edge_case_robustness_consistent ? "true" : "false");
}

inline std::string BuildObjc3TypedSemaToLoweringCoreFeatureEdgeCaseCompatibilityKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-core-edge-compat:v1:compatibility_handoff_consistent=" +
         std::string(surface.compatibility_handoff_consistent ? "true" : "false") +
         ";language_version_pragma_coordinate_order_consistent=" +
         std::string(surface.language_version_pragma_coordinate_order_consistent ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         std::string(surface.parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         std::string(surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";compatibility_handoff_key=" + surface.compatibility_handoff_key +
         ";parse_artifact_edge_robustness_key=" + surface.parse_artifact_edge_robustness_key +
         ";ready=" + (surface.typed_core_feature_edge_case_compatibility_ready ? "true" : "false");
}

inline std::string BuildObjc3TypedSemaToLoweringCoreFeatureEdgeRobustnessKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-core-edge-robustness:v1:edge_case_compatibility_ready=" +
         std::string(surface.typed_core_feature_edge_case_compatibility_ready ? "true" : "false") +
         ";edge_case_expansion_consistent=" +
         std::string(surface.typed_core_feature_edge_case_expansion_consistent ? "true" : "false") +
         ";edge_case_robustness_ready=" +
         std::string(surface.typed_core_feature_edge_case_robustness_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         std::string(surface.parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";compatibility_handoff_key=" + surface.compatibility_handoff_key +
         ";edge_case_compatibility_key=" + surface.typed_core_feature_edge_case_compatibility_key +
         ";parse_artifact_edge_robustness_key=" + surface.parse_artifact_edge_robustness_key;
}

inline std::string BuildObjc3TypedSemaToLoweringDiagnosticsHardeningKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-diagnostics-hardening:v1:edge_case_robustness_ready=" +
         std::string(surface.typed_core_feature_edge_case_robustness_ready ? "true" : "false") +
         ";typed_diagnostics_hardening_consistent=" +
         std::string(surface.typed_diagnostics_hardening_consistent ? "true" : "false") +
         ";typed_diagnostics_hardening_ready=" +
         std::string(surface.typed_diagnostics_hardening_ready ? "true" : "false") +
         ";typed_handoff_key_deterministic=" +
         std::string(surface.typed_handoff_key_deterministic ? "true" : "false") +
         ";edge_case_robustness_key=" + surface.typed_core_feature_edge_case_robustness_key;
}

inline std::string BuildObjc3TypedSemaToLoweringRecoveryDeterminismKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-recovery-determinism:v1:typed_diagnostics_hardening_ready=" +
         std::string(surface.typed_diagnostics_hardening_ready ? "true" : "false") +
         ";typed_recovery_determinism_consistent=" +
         std::string(surface.typed_recovery_determinism_consistent ? "true" : "false") +
         ";typed_recovery_determinism_ready=" +
         std::string(surface.typed_recovery_determinism_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         std::string(surface.parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";typed_diagnostics_hardening_key=" + surface.typed_diagnostics_hardening_key;
}

inline std::string BuildObjc3TypedSemaToLoweringConformanceMatrixKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-conformance-matrix:v1:typed_recovery_determinism_ready=" +
         std::string(surface.typed_recovery_determinism_ready ? "true" : "false") +
         ";typed_conformance_matrix_consistent=" +
         std::string(surface.typed_conformance_matrix_consistent ? "true" : "false") +
         ";typed_conformance_matrix_ready=" +
         std::string(surface.typed_conformance_matrix_ready ? "true" : "false") +
         ";typed_recovery_determinism_key=" + surface.typed_recovery_determinism_key;
}

inline std::string BuildObjc3TypedSemaToLoweringConformanceCorpusKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-conformance-corpus:v1:typed_conformance_matrix_ready=" +
         std::string(surface.typed_conformance_matrix_ready ? "true" : "false") +
         ";typed_conformance_corpus_consistent=" +
         std::string(surface.typed_conformance_corpus_consistent ? "true" : "false") +
         ";typed_conformance_corpus_ready=" +
         std::string(surface.typed_conformance_corpus_ready ? "true" : "false") +
         ";typed_conformance_matrix_key=" + surface.typed_conformance_matrix_key;
}

inline std::string BuildObjc3TypedSemaToLoweringPerformanceQualityGuardrailsKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-performance-quality-guardrails:v1:case_count=" +
         std::to_string(surface.typed_performance_quality_guardrails_case_count) +
         ";passed_case_count=" +
         std::to_string(surface.typed_performance_quality_guardrails_passed_case_count) +
         ";failed_case_count=" +
         std::to_string(surface.typed_performance_quality_guardrails_failed_case_count) +
         ";typed_conformance_corpus_consistent=" +
         std::string(surface.typed_conformance_corpus_consistent ? "true" : "false") +
         ";typed_conformance_corpus_ready=" +
         std::string(surface.typed_conformance_corpus_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         std::string(surface.parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";semantic_handoff_deterministic=" +
         std::string(surface.semantic_handoff_deterministic ? "true" : "false") +
         ";consistent=" +
         std::string(surface.typed_performance_quality_guardrails_consistent ? "true" : "false") +
         ";ready=" +
         std::string(surface.typed_performance_quality_guardrails_ready ? "true" : "false") +
         ";typed_conformance_corpus_key=" + surface.typed_conformance_corpus_key;
}

inline std::string BuildObjc3TypedSemaToLoweringCrossLaneIntegrationKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-cross-lane-integration:v1:performance_quality_guardrails_ready=" +
         std::string(surface.typed_performance_quality_guardrails_ready ? "true" : "false") +
         ";typed_cross_lane_integration_consistent=" +
         std::string(surface.typed_cross_lane_integration_consistent ? "true" : "false") +
         ";typed_cross_lane_integration_ready=" +
         std::string(surface.typed_cross_lane_integration_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         std::string(surface.parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";typed_performance_quality_guardrails_key=" + surface.typed_performance_quality_guardrails_key;
}

inline std::string BuildObjc3TypedSemaToLoweringDocsRunbookSyncKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-docs-runbook-sync:v1:typed_cross_lane_integration_ready=" +
         std::string(surface.typed_cross_lane_integration_ready ? "true" : "false") +
         ";typed_docs_runbook_sync_consistent=" +
         std::string(surface.typed_docs_runbook_sync_consistent ? "true" : "false") +
         ";typed_docs_runbook_sync_ready=" +
         std::string(surface.typed_docs_runbook_sync_ready ? "true" : "false") +
         ";typed_cross_lane_integration_key=" + surface.typed_cross_lane_integration_key;
}

inline std::string BuildObjc3TypedSemaToLoweringReleaseCandidateReplayDryRunKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-release-candidate-replay-dry-run:v1:typed_docs_runbook_sync_ready=" +
         std::string(surface.typed_docs_runbook_sync_ready ? "true" : "false") +
         ";typed_release_candidate_replay_dry_run_consistent=" +
         std::string(surface.typed_release_candidate_replay_dry_run_consistent ? "true" : "false") +
         ";typed_release_candidate_replay_dry_run_ready=" +
         std::string(surface.typed_release_candidate_replay_dry_run_ready ? "true" : "false") +
         ";typed_docs_runbook_sync_key=" + surface.typed_docs_runbook_sync_key;
}

#include "pipeline/objc3_typed_sema_to_lowering_contract_surface_advanced_keys.h"

#include "pipeline/objc3_typed_sema_to_lowering_contract_surface_handoff_key.h"

inline std::string BuildObjc3TypedSemaToLoweringCoreFeatureKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  std::ostringstream key;
  key << "typed-sema-lowering-core:v1:"
      << "case_count=" << surface.typed_core_feature_case_count
      << ";passed_case_count=" << surface.typed_core_feature_passed_case_count
      << ";failed_case_count=" << surface.typed_core_feature_failed_case_count
      << ";semantic_handoff_consistent=" << (surface.semantic_handoff_consistent ? "true" : "false")
      << ";semantic_handoff_deterministic=" << (surface.semantic_handoff_deterministic ? "true" : "false")
      << ";typed_handoff_key_deterministic=" << (surface.typed_handoff_key_deterministic ? "true" : "false")
      << ";runtime_dispatch_contract_consistent="
      << (surface.runtime_dispatch_contract_consistent ? "true" : "false")
      << ";lowering_boundary_ready=" << (surface.lowering_boundary_ready ? "true" : "false")
      << ";core_feature_expansion_consistent="
      << (surface.typed_core_feature_expansion_consistent ? "true" : "false")
      << ";core_feature_edge_case_compatibility_ready="
      << (surface.typed_core_feature_edge_case_compatibility_ready ? "true" : "false")
      << ";core_feature_edge_case_expansion_consistent="
      << (surface.typed_core_feature_edge_case_expansion_consistent ? "true" : "false")
      << ";core_feature_edge_case_robustness_ready="
      << (surface.typed_core_feature_edge_case_robustness_ready ? "true" : "false")
      << ";typed_diagnostics_hardening_consistent="
      << (surface.typed_diagnostics_hardening_consistent ? "true" : "false")
      << ";typed_diagnostics_hardening_ready="
      << (surface.typed_diagnostics_hardening_ready ? "true" : "false")
      << ";typed_recovery_determinism_consistent="
      << (surface.typed_recovery_determinism_consistent ? "true" : "false")
      << ";typed_recovery_determinism_ready="
      << (surface.typed_recovery_determinism_ready ? "true" : "false")
      << ";typed_conformance_matrix_consistent="
      << (surface.typed_conformance_matrix_consistent ? "true" : "false")
      << ";typed_conformance_matrix_ready="
      << (surface.typed_conformance_matrix_ready ? "true" : "false")
      << ";typed_conformance_corpus_consistent="
      << (surface.typed_conformance_corpus_consistent ? "true" : "false")
      << ";typed_conformance_corpus_ready="
      << (surface.typed_conformance_corpus_ready ? "true" : "false")
      << ";typed_performance_quality_guardrails_consistent="
      << (surface.typed_performance_quality_guardrails_consistent ? "true" : "false")
      << ";typed_performance_quality_guardrails_ready="
      << (surface.typed_performance_quality_guardrails_ready ? "true" : "false")
      << ";typed_cross_lane_integration_consistent="
      << (surface.typed_cross_lane_integration_consistent ? "true" : "false")
      << ";typed_cross_lane_integration_ready="
      << (surface.typed_cross_lane_integration_ready ? "true" : "false")
      << ";typed_docs_runbook_sync_consistent="
      << (surface.typed_docs_runbook_sync_consistent ? "true" : "false")
      << ";typed_docs_runbook_sync_ready="
      << (surface.typed_docs_runbook_sync_ready ? "true" : "false")
      << ";typed_release_candidate_replay_dry_run_consistent="
      << (surface.typed_release_candidate_replay_dry_run_consistent ? "true" : "false")
      << ";typed_release_candidate_replay_dry_run_ready="
      << (surface.typed_release_candidate_replay_dry_run_ready ? "true" : "false")
      << ";typed_advanced_core_shard1_consistent="
      << (surface.typed_advanced_core_shard1_consistent ? "true" : "false")
      << ";typed_advanced_core_shard1_ready="
      << (surface.typed_advanced_core_shard1_ready ? "true" : "false")
      << ";typed_advanced_edge_compatibility_shard1_consistent="
      << (surface.typed_advanced_edge_compatibility_shard1_consistent ? "true" : "false")
      << ";typed_advanced_edge_compatibility_shard1_ready="
      << (surface.typed_advanced_edge_compatibility_shard1_ready ? "true" : "false")
      << ";typed_advanced_diagnostics_shard1_consistent="
      << (surface.typed_advanced_diagnostics_shard1_consistent ? "true" : "false")
      << ";typed_advanced_diagnostics_shard1_ready="
      << (surface.typed_advanced_diagnostics_shard1_ready ? "true" : "false")
      << ";typed_advanced_conformance_shard1_consistent="
      << (surface.typed_advanced_conformance_shard1_consistent ? "true" : "false")
      << ";typed_advanced_conformance_shard1_ready="
      << (surface.typed_advanced_conformance_shard1_ready ? "true" : "false")
      << ";typed_advanced_integration_shard1_consistent="
      << (surface.typed_advanced_integration_shard1_consistent ? "true" : "false")
      << ";typed_advanced_integration_shard1_ready="
      << (surface.typed_advanced_integration_shard1_ready ? "true" : "false")
      << ";typed_advanced_performance_shard1_consistent="
      << (surface.typed_advanced_performance_shard1_consistent ? "true" : "false")
      << ";typed_advanced_performance_shard1_ready="
      << (surface.typed_advanced_performance_shard1_ready ? "true" : "false")
      << ";typed_advanced_core_shard2_consistent="
      << (surface.typed_advanced_core_shard2_consistent ? "true" : "false")
      << ";typed_advanced_core_shard2_ready="
      << (surface.typed_advanced_core_shard2_ready ? "true" : "false")
      << ";typed_advanced_edge_compatibility_shard2_consistent="
      << (surface.typed_advanced_edge_compatibility_shard2_consistent ? "true" : "false")
      << ";typed_advanced_edge_compatibility_shard2_ready="
      << (surface.typed_advanced_edge_compatibility_shard2_ready ? "true" : "false")
      << ";typed_advanced_diagnostics_shard2_consistent="
      << (surface.typed_advanced_diagnostics_shard2_consistent ? "true" : "false")
      << ";typed_advanced_diagnostics_shard2_ready="
      << (surface.typed_advanced_diagnostics_shard2_ready ? "true" : "false")
      << ";typed_advanced_conformance_shard2_consistent="
      << (surface.typed_advanced_conformance_shard2_consistent ? "true" : "false")
      << ";typed_advanced_conformance_shard2_ready="
      << (surface.typed_advanced_conformance_shard2_ready ? "true" : "false")
      << ";typed_advanced_integration_shard2_consistent="
      << (surface.typed_advanced_integration_shard2_consistent ? "true" : "false")
      << ";typed_advanced_integration_shard2_ready="
      << (surface.typed_advanced_integration_shard2_ready ? "true" : "false")
      << ";typed_integration_closeout_signoff_consistent="
      << (surface.typed_integration_closeout_signoff_consistent ? "true" : "false")
      << ";typed_integration_closeout_signoff_ready="
      << (surface.typed_integration_closeout_signoff_ready ? "true" : "false")
      << ";consistent=" << (surface.typed_core_feature_consistent ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3TypedSemaToLoweringCoreFeatureExpansionKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  std::ostringstream key;
  key << "typed-sema-lowering-core-expansion:v1:"
      << "case_count=" << surface.typed_core_feature_expansion_case_count
      << ";passed_case_count=" << surface.typed_core_feature_expansion_passed_case_count
      << ";failed_case_count=" << surface.typed_core_feature_expansion_failed_case_count
      << ";protocol_category_handoff_deterministic="
      << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ";class_protocol_category_linking_handoff_deterministic="
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true" : "false")
      << ";selector_normalization_handoff_deterministic="
      << (surface.selector_normalization_handoff_deterministic ? "true" : "false")
      << ";property_attribute_handoff_deterministic="
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ";consistent=" << (surface.typed_core_feature_expansion_consistent ? "true" : "false");
  return key.str();
}

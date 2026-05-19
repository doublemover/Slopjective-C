#pragma once

#include <cstddef>
#include <string>

struct Objc3LoweringRuntimeDiagnosticsSurfacingScaffold {
  bool stage_diagnostics_bus_consistent = false;
  bool parse_readiness_surface_present = false;
  bool parse_readiness_surface_ready = false;
  bool parse_diagnostics_hardening_consistent = false;
  bool parser_source_precision_scaffold_ready = false;
  bool typed_handoff_key_deterministic = false;
  bool diagnostics_replay_key_ready = false;
  bool modular_split_ready = false;
  std::string parse_artifact_replay_key;
  std::string lowering_boundary_replay_key;
  std::string scaffold_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface {
  bool stage_diagnostics_bus_consistent = false;
  bool parse_readiness_surface_ready = false;
  bool diagnostics_surfacing_scaffold_ready = false;
  bool parser_diagnostic_surface_consistent = false;
  bool parser_diagnostic_code_surface_deterministic = false;
  bool semantic_diagnostics_deterministic = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool replay_keys_ready = false;
  bool lowering_pipeline_ready = false;
  bool core_feature_impl_ready = false;
  std::size_t lexer_diagnostic_count = 0;
  std::size_t parser_diagnostic_count = 0;
  std::size_t semantic_diagnostic_count = 0;
  std::string parse_artifact_replay_key;
  std::string lowering_boundary_replay_key;
  std::string diagnostics_hardening_key;
  std::string core_feature_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface {
  bool core_feature_impl_ready = false;
  bool diagnostics_surfacing_scaffold_ready = false;
  bool diagnostics_hardening_key_consistent = false;
  bool diagnostics_payload_accounting_consistent = false;
  bool expansion_replay_keys_ready = false;
  bool lowering_pipeline_expansion_ready = false;
  bool core_feature_expansion_ready = false;
  std::size_t lexer_diagnostic_count = 0;
  std::size_t parser_diagnostic_count = 0;
  std::size_t semantic_diagnostic_count = 0;
  std::size_t parser_diagnostic_code_count = 0;
  std::string parse_artifact_replay_key;
  std::string lowering_boundary_replay_key;
  std::string diagnostics_hardening_key;
  std::string core_feature_key;
  std::string expansion_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface {
  bool core_feature_expansion_ready = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_artifact_edge_case_robustness_consistent = false;
  bool parse_artifact_replay_key_deterministic = false;
  bool parse_recovery_determinism_hardening_consistent = false;
  bool parse_edge_case_surfaces_consistent = false;
  bool parse_edge_case_surfaces_ready = false;
  bool lowering_pipeline_edge_case_compatibility_ready = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_replay_keys_ready = false;
  bool edge_case_compatibility_ready = false;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_robustness_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string long_tail_grammar_edge_case_compatibility_key;
  std::string parser_diagnostic_grammar_hooks_edge_case_compatibility_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface {
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool parse_edge_case_expansion_consistent = false;
  bool parse_edge_case_robustness_ready = false;
  bool lowering_pipeline_edge_case_robustness_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_robustness_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string long_tail_grammar_edge_case_robustness_key;
  std::string parser_diagnostic_grammar_hooks_edge_case_robustness_key;
  std::string lowering_pipeline_edge_case_robustness_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface {
  bool edge_case_robustness_consistent = false;
  bool edge_case_robustness_ready = false;
  bool parse_diagnostics_hardening_consistent = false;
  bool parse_diagnostics_hardening_ready = false;
  bool semantic_diagnostics_hardening_consistent = false;
  bool semantic_diagnostics_hardening_ready = false;
  bool lowering_pipeline_diagnostics_hardening_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  std::string edge_case_robustness_key;
  std::string parse_artifact_diagnostics_hardening_key;
  std::string long_tail_grammar_diagnostics_hardening_key;
  std::string parser_diagnostic_grammar_hooks_diagnostics_hardening_key;
  std::string semantic_diagnostics_hardening_key;
  std::string lowering_pipeline_diagnostics_hardening_key;
  std::string diagnostics_hardening_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface {
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool parse_recovery_determinism_consistent = false;
  bool parse_recovery_determinism_ready = false;
  bool semantic_recovery_determinism_consistent = false;
  bool semantic_recovery_determinism_ready = false;
  bool lowering_pipeline_recovery_determinism_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  std::string diagnostics_hardening_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string long_tail_grammar_recovery_determinism_key;
  std::string parser_diagnostic_grammar_hooks_recovery_determinism_key;
  std::string semantic_recovery_determinism_key;
  std::string lowering_pipeline_recovery_determinism_key;
  std::string recovery_determinism_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface {
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool parse_conformance_matrix_consistent = false;
  bool parse_conformance_matrix_ready = false;
  bool semantic_conformance_matrix_consistent = false;
  bool semantic_conformance_matrix_ready = false;
  bool lowering_pipeline_conformance_matrix_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  std::size_t parse_lowering_conformance_matrix_case_count = 0;
  std::string recovery_determinism_key;
  std::string parse_lowering_conformance_matrix_key;
  std::string long_tail_grammar_conformance_matrix_key;
  std::string parser_diagnostic_grammar_hooks_conformance_matrix_key;
  std::string semantic_conformance_matrix_key;
  std::string lowering_pipeline_conformance_matrix_key;
  std::string conformance_matrix_key;
  std::string failure_reason;
};

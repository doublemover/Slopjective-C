#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_diagnostics_bus.h"
#include "parse/objc3_parser_contract.h"
#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_manager_contract.h"

inline constexpr std::uint8_t kObjc3DefaultLanguageVersion = 3u;

enum class Objc3FrontendCompatibilityMode : std::uint8_t {
  kCanonical = 0u,
  kLegacy = 1u,
};

struct Objc3FrontendOptions {
  std::uint8_t language_version = kObjc3DefaultLanguageVersion;
  Objc3FrontendCompatibilityMode compatibility_mode = Objc3FrontendCompatibilityMode::kCanonical;
  bool migration_assist = false;
  bool emit_manifest = true;
  bool emit_ir = true;
  bool emit_object = true;
  Objc3LoweringContract lowering;
};

struct Objc3FrontendMigrationHints {
  std::size_t legacy_yes_count = 0;
  std::size_t legacy_no_count = 0;
  std::size_t legacy_null_count = 0;

  std::size_t legacy_total() const { return legacy_yes_count + legacy_no_count + legacy_null_count; }
};

struct Objc3FrontendLanguageVersionPragmaContract {
  bool seen = false;
  std::size_t directive_count = 0;
  bool duplicate = false;
  bool non_leading = false;
  unsigned first_line = 0;
  unsigned first_column = 0;
  unsigned last_line = 0;
  unsigned last_column = 0;
};

struct Objc3TypedSemaToLoweringContractSurface {
  bool semantic_integration_surface_built = false;
  bool semantic_type_metadata_handoff_deterministic = false;
  bool sema_parity_surface_ready = false;
  bool sema_parity_surface_deterministic = false;
  bool executable_metadata_lowering_handoff_ready = false;
  bool executable_metadata_lowering_handoff_deterministic = false;
  bool executable_metadata_typed_lowering_handoff_ready = false;
  bool executable_metadata_typed_lowering_handoff_deterministic = false;
  bool protocol_category_handoff_deterministic = false;
  bool class_protocol_category_linking_handoff_deterministic = false;
  bool selector_normalization_handoff_deterministic = false;
  bool property_attribute_handoff_deterministic = false;
  bool object_pointer_type_handoff_deterministic = false;
  bool symbol_graph_handoff_deterministic = false;
  bool scope_resolution_handoff_deterministic = false;
  bool semantic_handoff_consistent = false;
  bool semantic_handoff_deterministic = false;
  bool runtime_dispatch_contract_consistent = false;
  bool typed_handoff_key_deterministic = false;
  bool typed_core_feature_consistent = false;
  bool typed_core_feature_expansion_consistent = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_artifact_replay_key_deterministic = false;
  bool parse_artifact_edge_case_robustness_consistent = false;
  bool typed_core_feature_edge_case_compatibility_ready = false;
  bool typed_core_feature_edge_case_expansion_consistent = false;
  bool typed_core_feature_edge_case_robustness_ready = false;
  bool typed_diagnostics_hardening_consistent = false;
  bool typed_diagnostics_hardening_ready = false;
  bool typed_recovery_determinism_consistent = false;
  bool typed_recovery_determinism_ready = false;
  bool typed_conformance_matrix_consistent = false;
  bool typed_conformance_matrix_ready = false;
  bool typed_conformance_corpus_consistent = false;
  bool typed_conformance_corpus_ready = false;
  bool typed_performance_quality_guardrails_consistent = false;
  bool typed_performance_quality_guardrails_ready = false;
  bool typed_cross_lane_integration_consistent = false;
  bool typed_cross_lane_integration_ready = false;
  bool typed_docs_runbook_sync_consistent = false;
  bool typed_docs_runbook_sync_ready = false;
  bool typed_release_candidate_replay_dry_run_consistent = false;
  bool typed_release_candidate_replay_dry_run_ready = false;
  bool typed_advanced_core_shard1_consistent = false;
  bool typed_advanced_core_shard1_ready = false;
  bool typed_advanced_edge_compatibility_shard1_consistent = false;
  bool typed_advanced_edge_compatibility_shard1_ready = false;
  bool typed_advanced_diagnostics_shard1_consistent = false;
  bool typed_advanced_diagnostics_shard1_ready = false;
  bool typed_advanced_conformance_shard1_consistent = false;
  bool typed_advanced_conformance_shard1_ready = false;
  bool typed_advanced_integration_shard1_consistent = false;
  bool typed_advanced_integration_shard1_ready = false;
  bool typed_advanced_performance_shard1_consistent = false;
  bool typed_advanced_performance_shard1_ready = false;
  bool typed_advanced_core_shard2_consistent = false;
  bool typed_advanced_core_shard2_ready = false;
  bool typed_advanced_edge_compatibility_shard2_consistent = false;
  bool typed_advanced_edge_compatibility_shard2_ready = false;
  bool typed_advanced_diagnostics_shard2_consistent = false;
  bool typed_advanced_diagnostics_shard2_ready = false;
  bool typed_advanced_conformance_shard2_consistent = false;
  bool typed_advanced_conformance_shard2_ready = false;
  bool typed_advanced_integration_shard2_consistent = false;
  bool typed_advanced_integration_shard2_ready = false;
  bool typed_integration_closeout_signoff_consistent = false;
  bool typed_integration_closeout_signoff_ready = false;
  bool lowering_boundary_ready = false;
  bool ready_for_lowering = false;
  std::size_t typed_core_feature_case_count = 0;
  std::size_t typed_core_feature_passed_case_count = 0;
  std::size_t typed_core_feature_failed_case_count = 0;
  std::size_t typed_core_feature_expansion_case_count = 0;
  std::size_t typed_core_feature_expansion_passed_case_count = 0;
  std::size_t typed_core_feature_expansion_failed_case_count = 0;
  std::size_t typed_performance_quality_guardrails_case_count = 0;
  std::size_t typed_performance_quality_guardrails_passed_case_count = 0;
  std::size_t typed_performance_quality_guardrails_failed_case_count = 0;
  std::string typed_handoff_key;
  std::string executable_metadata_lowering_handoff_key;
  std::string executable_metadata_typed_lowering_handoff_key;
  std::string typed_core_feature_key;
  std::string typed_core_feature_expansion_key;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_robustness_key;
  std::string typed_core_feature_edge_case_compatibility_key;
  std::string typed_core_feature_edge_case_robustness_key;
  std::string typed_diagnostics_hardening_key;
  std::string typed_recovery_determinism_key;
  std::string typed_conformance_matrix_key;
  std::string typed_conformance_corpus_key;
  std::string typed_performance_quality_guardrails_key;
  std::string typed_cross_lane_integration_key;
  std::string typed_docs_runbook_sync_key;
  std::string typed_release_candidate_replay_dry_run_key;
  std::string typed_advanced_core_shard1_key;
  std::string typed_advanced_edge_compatibility_shard1_key;
  std::string typed_advanced_diagnostics_shard1_key;
  std::string typed_advanced_conformance_shard1_key;
  std::string typed_advanced_integration_shard1_key;
  std::string typed_advanced_performance_shard1_key;
  std::string typed_advanced_core_shard2_key;
  std::string typed_advanced_edge_compatibility_shard2_key;
  std::string typed_advanced_diagnostics_shard2_key;
  std::string typed_advanced_conformance_shard2_key;
  std::string typed_advanced_integration_shard2_key;
  std::string typed_integration_closeout_signoff_key;
  std::string lowering_boundary_replay_key;
  std::string failure_reason;
};

struct Objc3ParseLoweringReadinessSurface {
  std::size_t lexer_diagnostic_count = 0;
  std::size_t parser_diagnostic_count = 0;
  std::size_t semantic_diagnostic_count = 0;
  std::size_t parser_token_count = 0;
  std::size_t parser_top_level_declaration_count = 0;
  std::uint64_t parser_contract_snapshot_fingerprint = 0;
  std::uint64_t parser_ast_shape_fingerprint = 0;
  std::uint64_t parser_ast_top_level_layout_fingerprint = 0;
  std::uint64_t ast_shape_fingerprint = 0;
  std::uint64_t ast_top_level_layout_fingerprint = 0;
  bool parser_contract_snapshot_present = false;
  bool parser_contract_deterministic = false;
  bool parser_recovery_replay_ready = false;
  bool long_tail_grammar_core_feature_consistent = false;
  bool long_tail_grammar_handoff_key_deterministic = false;
  bool long_tail_grammar_expansion_accounting_consistent = false;
  bool long_tail_grammar_replay_keys_ready = false;
  bool long_tail_grammar_expansion_ready = false;
  bool long_tail_grammar_compatibility_handoff_ready = false;
  bool long_tail_grammar_edge_case_compatibility_consistent = false;
  bool long_tail_grammar_edge_case_compatibility_ready = false;
  bool long_tail_grammar_edge_case_expansion_consistent = false;
  bool long_tail_grammar_edge_case_robustness_ready = false;
  bool long_tail_grammar_diagnostics_hardening_consistent = false;
  bool long_tail_grammar_diagnostics_hardening_ready = false;
  bool long_tail_grammar_recovery_determinism_consistent = false;
  bool long_tail_grammar_recovery_determinism_ready = false;
  bool long_tail_grammar_conformance_matrix_consistent = false;
  bool long_tail_grammar_conformance_matrix_ready = false;
  bool long_tail_grammar_integration_closeout_consistent = false;
  bool long_tail_grammar_gate_signoff_ready = false;
  bool parse_artifact_handoff_consistent = false;
  bool parse_artifact_handoff_deterministic = false;
  bool parser_diagnostic_surface_consistent = false;
  bool parser_diagnostic_code_surface_deterministic = false;
  bool parser_diagnostic_source_precision_scaffold_consistent = false;
  bool parser_diagnostic_source_precision_scaffold_ready = false;
  bool parser_diagnostic_grammar_hooks_core_feature_consistent = false;
  bool parser_diagnostic_grammar_hooks_core_feature_ready = false;
  bool parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent = false;
  bool parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready = false;
  bool parser_diagnostic_grammar_hooks_core_feature_expansion_ready = false;
  bool parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent = false;
  bool parser_diagnostic_grammar_hooks_edge_case_compatibility_ready = false;
  bool parser_diagnostic_grammar_hooks_edge_case_expansion_consistent = false;
  bool parser_diagnostic_grammar_hooks_edge_case_robustness_ready = false;
  bool parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent = false;
  bool parser_diagnostic_grammar_hooks_diagnostics_hardening_ready = false;
  bool parser_diagnostic_grammar_hooks_recovery_determinism_consistent = false;
  bool parser_diagnostic_grammar_hooks_recovery_determinism_ready = false;
  bool parser_diagnostic_grammar_hooks_conformance_matrix_consistent = false;
  bool parser_diagnostic_grammar_hooks_conformance_matrix_ready = false;
  bool parser_diagnostic_grammar_hooks_conformance_corpus_consistent = false;
  bool parser_diagnostic_grammar_hooks_conformance_corpus_ready = false;
  bool parser_token_count_budget_consistent = false;
  bool parse_artifact_layout_fingerprint_consistent = false;
  bool parse_artifact_fingerprint_consistent = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_artifact_replay_key_deterministic = false;
  bool parse_artifact_diagnostics_hardening_consistent = false;
  bool parse_artifact_edge_case_robustness_consistent = false;
  bool parse_recovery_determinism_hardening_consistent = false;
  bool parse_lowering_conformance_matrix_consistent = false;
  bool parse_lowering_conformance_corpus_consistent = false;
  bool parse_lowering_performance_quality_guardrails_consistent = false;
  bool toolchain_runtime_ga_operations_cross_lane_integration_consistent = false;
  bool toolchain_runtime_ga_operations_cross_lane_integration_ready = false;
  bool toolchain_runtime_ga_operations_docs_runbook_sync_consistent = false;
  bool toolchain_runtime_ga_operations_docs_runbook_sync_ready = false;
  bool toolchain_runtime_ga_operations_advanced_core_consistent = false;
  bool toolchain_runtime_ga_operations_advanced_core_ready = false;
  bool toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent = false;
  bool toolchain_runtime_ga_operations_advanced_edge_compatibility_ready = false;
  bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent = false;
  bool toolchain_runtime_ga_operations_advanced_diagnostics_ready = false;
  bool toolchain_runtime_ga_operations_advanced_conformance_consistent = false;
  bool toolchain_runtime_ga_operations_advanced_conformance_ready = false;
  bool toolchain_runtime_ga_operations_advanced_integration_consistent = false;
  bool toolchain_runtime_ga_operations_advanced_integration_ready = false;
  bool toolchain_runtime_ga_operations_advanced_performance_consistent = false;
  bool toolchain_runtime_ga_operations_advanced_performance_ready = false;
  bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent = false;
  bool toolchain_runtime_ga_operations_advanced_core_shard2_ready = false;
  bool toolchain_runtime_ga_operations_integration_closeout_signoff_consistent = false;
  bool toolchain_runtime_ga_operations_integration_closeout_signoff_ready = false;
  bool semantic_integration_surface_built = false;
  bool semantic_diagnostics_deterministic = false;
  bool semantic_type_metadata_deterministic = false;
  bool executable_metadata_lowering_handoff_ready = false;
  bool executable_metadata_lowering_handoff_deterministic = false;
  bool executable_metadata_typed_lowering_handoff_ready = false;
  bool executable_metadata_typed_lowering_handoff_deterministic = false;
  bool protocol_category_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool selector_normalization_deterministic = false;
  bool property_attribute_deterministic = false;
  bool symbol_graph_deterministic = false;
  bool scope_resolution_deterministic = false;
  bool object_pointer_type_handoff_deterministic = false;
  bool typed_handoff_key_deterministic = false;
  bool typed_sema_core_feature_consistent = false;
  bool typed_sema_core_feature_expansion_consistent = false;
  bool typed_sema_edge_case_compatibility_consistent = false;
  bool typed_sema_edge_case_compatibility_ready = false;
  bool typed_sema_edge_case_expansion_consistent = false;
  bool typed_sema_edge_case_robustness_ready = false;
  bool typed_sema_diagnostics_hardening_consistent = false;
  bool typed_sema_diagnostics_hardening_ready = false;
  bool typed_sema_recovery_determinism_consistent = false;
  bool typed_sema_recovery_determinism_ready = false;
  bool typed_sema_conformance_matrix_consistent = false;
  bool typed_sema_conformance_matrix_ready = false;
  bool typed_sema_conformance_corpus_consistent = false;
  bool typed_sema_conformance_corpus_ready = false;
  bool typed_sema_performance_quality_guardrails_consistent = false;
  bool typed_sema_performance_quality_guardrails_ready = false;
  bool typed_sema_cross_lane_integration_consistent = false;
  bool typed_sema_cross_lane_integration_ready = false;
  bool typed_sema_docs_runbook_sync_consistent = false;
  bool typed_sema_docs_runbook_sync_ready = false;
  bool typed_sema_release_candidate_replay_dry_run_consistent = false;
  bool typed_sema_release_candidate_replay_dry_run_ready = false;
  bool typed_sema_advanced_core_shard1_consistent = false;
  bool typed_sema_advanced_core_shard1_ready = false;
  bool typed_sema_advanced_edge_compatibility_shard1_consistent = false;
  bool typed_sema_advanced_edge_compatibility_shard1_ready = false;
  bool typed_sema_advanced_diagnostics_shard1_consistent = false;
  bool typed_sema_advanced_diagnostics_shard1_ready = false;
  bool typed_sema_advanced_conformance_shard1_consistent = false;
  bool typed_sema_advanced_conformance_shard1_ready = false;
  bool typed_sema_advanced_integration_shard1_consistent = false;
  bool typed_sema_advanced_integration_shard1_ready = false;
  bool typed_sema_advanced_performance_shard1_consistent = false;
  bool typed_sema_advanced_performance_shard1_ready = false;
  bool typed_sema_advanced_core_shard2_consistent = false;
  bool typed_sema_advanced_core_shard2_ready = false;
  bool typed_sema_advanced_edge_compatibility_shard2_consistent = false;
  bool typed_sema_advanced_edge_compatibility_shard2_ready = false;
  bool typed_sema_advanced_diagnostics_shard2_consistent = false;
  bool typed_sema_advanced_diagnostics_shard2_ready = false;
  bool typed_sema_advanced_conformance_shard2_consistent = false;
  bool typed_sema_advanced_conformance_shard2_ready = false;
  bool typed_sema_advanced_integration_shard2_consistent = false;
  bool typed_sema_advanced_integration_shard2_ready = false;
  bool typed_sema_integration_closeout_signoff_consistent = false;
  bool typed_sema_integration_closeout_signoff_ready = false;
  bool lowering_boundary_ready = false;
  bool ready_for_lowering = false;
  std::size_t typed_sema_core_feature_case_count = 0;
  std::size_t typed_sema_core_feature_passed_case_count = 0;
  std::size_t typed_sema_core_feature_failed_case_count = 0;
  std::size_t typed_sema_core_feature_expansion_case_count = 0;
  std::size_t typed_sema_core_feature_expansion_passed_case_count = 0;
  std::size_t typed_sema_core_feature_expansion_failed_case_count = 0;
  std::size_t typed_sema_performance_quality_guardrails_case_count = 0;
  std::size_t typed_sema_performance_quality_guardrails_passed_case_count = 0;
  std::size_t typed_sema_performance_quality_guardrails_failed_case_count = 0;
  std::size_t parse_lowering_conformance_matrix_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_passed_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_failed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_passed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_failed_case_count = 0;
  std::size_t parser_diagnostic_code_count = 0;
  std::size_t parser_diagnostic_grammar_hook_code_count = 0;
  std::size_t parser_diagnostic_grammar_hook_unique_code_count = 0;
  std::size_t parser_diagnostic_coordinate_tagged_count = 0;
  std::size_t long_tail_grammar_construct_count = 0;
  std::size_t long_tail_grammar_covered_construct_count = 0;
  std::uint64_t parser_diagnostic_code_fingerprint = 1469598103934665603ull;
  std::uint64_t parser_diagnostic_source_precision_fingerprint = 1469598103934665603ull;
  std::uint64_t long_tail_grammar_fingerprint = 1469598103934665603ull;
  std::string parse_artifact_handoff_key;
  std::string compatibility_handoff_key;
  std::string parse_artifact_replay_key;
  std::string long_tail_grammar_handoff_key;
  std::string long_tail_grammar_expansion_key;
  std::string long_tail_grammar_edge_case_compatibility_key;
  std::string long_tail_grammar_edge_case_robustness_key;
  std::string long_tail_grammar_diagnostics_hardening_key;
  std::string long_tail_grammar_recovery_determinism_key;
  std::string long_tail_grammar_conformance_matrix_key;
  std::string long_tail_grammar_integration_closeout_key;
  std::string parse_artifact_diagnostics_hardening_key;
  std::string parser_diagnostic_source_precision_scaffold_key;
  std::string parser_diagnostic_grammar_hooks_core_feature_key;
  std::string parser_diagnostic_grammar_hooks_core_feature_expansion_key;
  std::string parser_diagnostic_grammar_hooks_edge_case_compatibility_key;
  std::string parser_diagnostic_grammar_hooks_edge_case_robustness_key;
  std::string parser_diagnostic_grammar_hooks_diagnostics_hardening_key;
  std::string parser_diagnostic_grammar_hooks_recovery_determinism_key;
  std::string parser_diagnostic_grammar_hooks_conformance_matrix_key;
  std::string parser_diagnostic_grammar_hooks_conformance_corpus_key;
  std::string parse_artifact_edge_robustness_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string parse_lowering_conformance_matrix_key;
  std::string parse_lowering_conformance_corpus_key;
  std::string parse_lowering_performance_quality_guardrails_key;
  std::string toolchain_runtime_ga_operations_cross_lane_integration_key;
  std::string toolchain_runtime_ga_operations_docs_runbook_sync_key;
  std::string toolchain_runtime_ga_operations_advanced_core_key;
  std::string toolchain_runtime_ga_operations_advanced_edge_compatibility_key;
  std::string toolchain_runtime_ga_operations_advanced_diagnostics_key;
  std::string toolchain_runtime_ga_operations_advanced_conformance_key;
  std::string toolchain_runtime_ga_operations_advanced_integration_key;
  std::string toolchain_runtime_ga_operations_advanced_performance_key;
  std::string toolchain_runtime_ga_operations_advanced_core_shard2_key;
  std::string toolchain_runtime_ga_operations_integration_closeout_signoff_key;
  std::string executable_metadata_lowering_handoff_key;
  std::string executable_metadata_typed_lowering_handoff_key;
  std::string typed_sema_core_feature_key;
  std::string typed_sema_core_feature_expansion_key;
  std::string typed_sema_edge_case_compatibility_key;
  std::string typed_sema_edge_case_robustness_key;
  std::string typed_sema_diagnostics_hardening_key;
  std::string typed_sema_recovery_determinism_key;
  std::string typed_sema_conformance_matrix_key;
  std::string typed_sema_conformance_corpus_key;
  std::string typed_sema_performance_quality_guardrails_key;
  std::string typed_sema_cross_lane_integration_key;
  std::string typed_sema_docs_runbook_sync_key;
  std::string typed_sema_release_candidate_replay_dry_run_key;
  std::string typed_sema_advanced_core_shard1_key;
  std::string typed_sema_advanced_edge_compatibility_shard1_key;
  std::string typed_sema_advanced_diagnostics_shard1_key;
  std::string typed_sema_advanced_conformance_shard1_key;
  std::string typed_sema_advanced_integration_shard1_key;
  std::string typed_sema_advanced_performance_shard1_key;
  std::string typed_sema_advanced_core_shard2_key;
  std::string typed_sema_advanced_edge_compatibility_shard2_key;
  std::string typed_sema_advanced_diagnostics_shard2_key;
  std::string typed_sema_advanced_conformance_shard2_key;
  std::string typed_sema_advanced_integration_shard2_key;
  std::string typed_sema_integration_closeout_signoff_key;
  std::string lowering_boundary_replay_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold {
  bool sema_pass_flow_summary_present = false;
  bool sema_pass_flow_summary_ready = false;
  bool sema_parity_surface_present = false;
  bool sema_parity_surface_ready = false;
  bool diagnostics_taxonomy_consistent = false;
  bool arc_diagnostics_fixit_summary_deterministic = false;
  bool arc_diagnostics_fixit_handoff_deterministic = false;
  bool typed_sema_handoff_deterministic = false;
  bool modular_split_ready = false;
  std::string sema_pass_flow_handoff_key;
  std::string typed_handoff_key;
  std::string scaffold_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface {
  bool sema_pass_flow_summary_ready = false;
  bool sema_parity_surface_ready = false;
  bool diagnostics_taxonomy_consistent = false;
  bool arc_diagnostics_fixit_summary_deterministic = false;
  bool arc_diagnostics_fixit_handoff_deterministic = false;
  bool typed_sema_handoff_deterministic = false;
  bool modular_split_ready = false;
  bool diagnostics_case_accounting_consistent = false;
  bool arc_diagnostics_fixit_case_accounting_consistent = false;
  bool replay_keys_ready = false;
  bool core_feature_impl_ready = false;
  std::size_t diagnostics_total = 0;
  std::size_t diagnostics_after_pass_final = 0;
  std::size_t diagnostics_emitted_total = 0;
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t ownership_arc_contract_violation_sites = 0;
  std::string sema_pass_flow_handoff_key;
  std::string typed_handoff_key;
  std::string recovery_replay_key;
  std::string arc_diagnostics_fixit_replay_key;
  std::string core_feature_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface {
  bool core_feature_impl_ready = false;
  bool modular_split_ready = false;
  bool typed_handoff_key_consistent = false;
  bool arc_diagnostics_fixit_replay_key_consistent = false;
  bool diagnostics_fixit_payload_accounting_consistent = false;
  bool expansion_replay_keys_ready = false;
  bool core_feature_expansion_ready = false;
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t ownership_arc_contract_violation_sites = 0;
  std::string sema_pass_flow_handoff_key;
  std::string typed_handoff_key;
  std::string recovery_replay_key;
  std::string arc_diagnostics_fixit_replay_key;
  std::string core_feature_key;
  std::string expansion_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface {
  bool core_feature_expansion_ready = false;
  bool typed_handoff_key_consistent = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_artifact_edge_case_robustness_consistent = false;
  bool parse_artifact_replay_key_deterministic = false;
  bool parse_recovery_determinism_hardening_consistent = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t ownership_arc_contract_violation_sites = 0;
  std::string typed_handoff_key;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_robustness_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string expansion_key;
  std::string edge_case_compatibility_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface {
  bool core_feature_expansion_ready = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t ownership_arc_contract_violation_sites = 0;
  std::string typed_handoff_key;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_robustness_key;
  std::string long_tail_grammar_edge_case_robustness_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface {
  bool edge_case_robustness_ready = false;
  bool edge_case_expansion_consistent = false;
  bool parser_diagnostic_surface_consistent = false;
  bool parser_diagnostic_code_surface_deterministic = false;
  bool parse_artifact_diagnostics_hardening_consistent = false;
  bool long_tail_grammar_diagnostics_hardening_consistent = false;
  bool long_tail_grammar_diagnostics_hardening_ready = false;
  bool semantic_diagnostics_deterministic = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  std::size_t parser_diagnostic_code_count = 0;
  std::size_t parser_diagnostic_grammar_hook_code_count = 0;
  std::size_t parser_diagnostic_coordinate_tagged_count = 0;
  std::string edge_case_robustness_key;
  std::string parse_artifact_diagnostics_hardening_key;
  std::string long_tail_grammar_diagnostics_hardening_key;
  std::string typed_handoff_key;
  std::string diagnostics_hardening_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface {
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool parse_recovery_determinism_hardening_consistent = false;
  bool long_tail_grammar_recovery_determinism_consistent = false;
  bool long_tail_grammar_recovery_determinism_ready = false;
  bool parser_recovery_replay_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  std::size_t parser_diagnostic_code_count = 0;
  std::size_t parser_diagnostic_grammar_hook_code_count = 0;
  std::size_t parser_diagnostic_coordinate_tagged_count = 0;
  std::string edge_case_robustness_key;
  std::string parse_artifact_diagnostics_hardening_key;
  std::string typed_handoff_key;
  std::string diagnostics_hardening_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string long_tail_grammar_recovery_determinism_key;
  std::string recovery_determinism_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface {
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool parse_lowering_conformance_matrix_consistent = false;
  bool long_tail_grammar_conformance_matrix_consistent = false;
  bool long_tail_grammar_conformance_matrix_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  std::size_t parse_lowering_conformance_matrix_case_count = 0;
  std::string recovery_determinism_key;
  std::string parse_lowering_conformance_matrix_key;
  std::string long_tail_grammar_conformance_matrix_key;
  std::string conformance_matrix_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface {
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool parse_lowering_conformance_corpus_consistent = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  std::size_t parse_lowering_conformance_corpus_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_passed_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_failed_case_count = 0;
  std::string conformance_matrix_key;
  std::string parse_lowering_conformance_corpus_key;
  std::string conformance_corpus_key;
  std::string failure_reason;
};

struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface {
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool parse_lowering_performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  std::size_t parse_lowering_performance_quality_guardrails_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_passed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_failed_case_count = 0;
  std::string conformance_corpus_key;
  std::string parse_lowering_performance_quality_guardrails_key;
  std::string performance_quality_guardrails_key;
  std::string failure_reason;
};

struct Objc3SemanticStabilitySpecDeltaClosureScaffold {
  bool typed_surface_present = false;
  bool parse_readiness_surface_present = false;
  bool semantic_handoff_deterministic = false;
  bool typed_core_feature_expansion_consistent = false;
  bool parse_lowering_conformance_matrix_consistent = false;
  bool parse_lowering_conformance_corpus_consistent = false;
  bool parse_lowering_performance_quality_guardrails_consistent = false;
  bool spec_delta_closed = false;
  bool modular_split_ready = false;
  std::string typed_handoff_key;
  std::string parse_artifact_replay_key;
  std::string scaffold_key;
  std::string failure_reason;
};

struct Objc3SemanticStabilityCoreFeatureImplementationSurface {
  bool semantic_handoff_deterministic = false;
  bool typed_core_feature_consistent = false;
  bool typed_core_feature_expansion_consistent = false;
  bool typed_sema_core_feature_consistent = false;
  bool typed_sema_core_feature_expansion_consistent = false;
  bool parse_lowering_conformance_matrix_consistent = false;
  bool parse_lowering_conformance_corpus_consistent = false;
  bool parse_lowering_performance_quality_guardrails_consistent = false;
  bool spec_delta_closed = false;
  bool modular_split_ready = false;
  bool typed_core_feature_expansion_accounting_consistent = false;
  bool parse_conformance_accounting_consistent = false;
  bool replay_keys_ready = false;
  bool expansion_ready = false;
  bool core_feature_impl_ready = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  bool integration_closeout_consistent = false;
  bool gate_signoff_ready = false;
  std::size_t typed_core_feature_case_count = 0;
  std::size_t typed_core_feature_passed_case_count = 0;
  std::size_t typed_core_feature_failed_case_count = 0;
  std::size_t typed_core_feature_expansion_case_count = 0;
  std::size_t typed_core_feature_expansion_passed_case_count = 0;
  std::size_t typed_core_feature_expansion_failed_case_count = 0;
  std::size_t parse_lowering_conformance_matrix_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_passed_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_failed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_passed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_failed_case_count = 0;
  std::string typed_handoff_key;
  std::string parse_artifact_replay_key;
  std::string core_feature_key;
  std::string expansion_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string integration_closeout_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeStabilityInvariantScaffold {
  bool typed_surface_present = false;
  bool parse_readiness_surface_present = false;
  bool lowering_boundary_ready = false;
  bool runtime_dispatch_contract_consistent = false;
  bool typed_handoff_key_deterministic = false;
  bool typed_core_feature_consistent = false;
  bool parse_ready_for_lowering = false;
  bool invariant_proofs_ready = false;
  bool modular_split_ready = false;
  std::string lowering_boundary_replay_key;
  std::string typed_handoff_key;
  std::string parse_artifact_replay_key;
  std::string scaffold_key;
  std::string failure_reason;
};

struct Objc3LoweringPipelinePassGraphScaffold {
  bool lex_stage_ready = false;
  bool parse_stage_ready = false;
  bool sema_stage_ready = false;
  bool typed_surface_ready = false;
  bool parse_lowering_readiness_ready = false;
  bool semantic_stability_scaffold_ready = false;
  bool lowering_runtime_invariant_scaffold_ready = false;
  bool lowering_ir_boundary_ready = false;
  bool runtime_dispatch_declaration_ready = false;
  bool ir_emission_entrypoint_ready = false;
  bool pass_graph_ready = false;
  std::string lowering_boundary_replay_key;
  std::string runtime_dispatch_declaration_replay_key;
  std::string pass_graph_key;
  std::string failure_reason;
};

struct Objc3LoweringPipelinePassGraphCoreFeatureSurface {
  bool scaffold_ready = false;
  bool lowering_boundary_replay_key_consistent = false;
  bool runtime_dispatch_declaration_consistent = false;
  bool direct_ir_entrypoint_enabled = false;
  bool dispatch_shape_sharding_ready = false;
  bool llc_object_emission_route_deterministic = false;
  bool replay_proof_artifact_key_ready = false;
  bool edge_case_dispatch_shape_coverage_ready = false;
  bool replay_proof_expansion_ready = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  bool expansion_ready = false;
  bool core_feature_ready = false;
  std::size_t parse_lowering_performance_quality_guardrails_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_passed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_failed_case_count = 0;
  std::string lowering_boundary_replay_key;
  std::string runtime_dispatch_declaration_replay_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string expansion_key;
  std::string core_feature_key;
  std::string failure_reason;
};

struct Objc3IREmissionCompletenessScaffold {
  bool pass_graph_scaffold_ready = false;
  bool core_feature_ready = false;
  bool expansion_ready = false;
  bool edge_case_compatibility_ready = false;
  bool metadata_transport_ready = false;
  bool modular_split_ready = false;
  std::string pass_graph_key;
  std::string core_feature_key;
  std::string expansion_key;
  std::string edge_case_compatibility_key;
  std::string scaffold_key;
  std::string failure_reason;
};

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

struct Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface {
  bool lowering_boundary_ready = false;
  bool runtime_dispatch_contract_consistent = false;
  bool typed_handoff_key_deterministic = false;
  bool typed_core_feature_consistent = false;
  bool parse_ready_for_lowering = false;
  bool invariant_proofs_ready = false;
  bool modular_split_ready = false;
  bool typed_expansion_accounting_consistent = false;
  bool parse_conformance_accounting_consistent = false;
  bool replay_keys_ready = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_edge_case_robustness_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_consistent = false;
  bool cross_lane_integration_ready = false;
  bool integration_closeout_consistent = false;
  bool gate_signoff_ready = false;
  bool expansion_ready = false;
  bool core_feature_impl_ready = false;
  std::size_t typed_core_feature_case_count = 0;
  std::size_t typed_core_feature_passed_case_count = 0;
  std::size_t typed_core_feature_failed_case_count = 0;
  std::size_t typed_core_feature_expansion_case_count = 0;
  std::size_t typed_core_feature_expansion_passed_case_count = 0;
  std::size_t typed_core_feature_expansion_failed_case_count = 0;
  std::size_t parse_lowering_conformance_matrix_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_passed_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_failed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_passed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_failed_case_count = 0;
  std::string lowering_boundary_replay_key;
  std::string typed_handoff_key;
  std::string parse_artifact_replay_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string cross_lane_integration_key;
  std::string integration_closeout_key;
  std::string edge_case_compatibility_key;
  std::string expansion_key;
  std::string core_feature_key;
  std::string failure_reason;
};

struct Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface {
  bool scaffold_ready = false;
  bool backend_route_deterministic = false;
  bool compile_status_success = false;
  bool backend_output_recorded = false;
  bool backend_dispatch_consistent = false;
  bool backend_output_path_deterministic = false;
  bool backend_output_payload_consistent = false;
  bool core_feature_expansion_ready = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool core_feature_impl_ready = false;
  std::string backend_route_key;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string failure_reason;
};

struct Objc3FinalReadinessGateCoreFeatureImplementationSurface {
  bool governance_contract_ready = false;
  bool modular_split_ready = false;
  bool lane_a_core_feature_ready = false;
  bool lane_b_core_feature_ready = false;
  bool lane_c_core_feature_ready = false;
  bool lane_d_core_feature_ready = false;
  bool dependency_chain_ready = false;
  bool core_feature_expansion_consistent = false;
  bool core_feature_expansion_ready = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_consistent = false;
  bool cross_lane_integration_ready = false;
  bool docs_runbook_sync_consistent = false;
  bool docs_runbook_sync_ready = false;
  bool release_candidate_replay_dry_run_consistent = false;
  bool release_candidate_replay_dry_run_ready = false;
  bool advanced_core_shard1_consistent = false;
  bool advanced_core_shard1_ready = false;
  bool advanced_edge_compatibility_shard1_consistent = false;
  bool advanced_edge_compatibility_shard1_ready = false;
  bool advanced_diagnostics_shard1_consistent = false;
  bool advanced_diagnostics_shard1_ready = false;
  bool advanced_conformance_shard1_consistent = false;
  bool advanced_conformance_shard1_ready = false;
  bool advanced_integration_shard1_consistent = false;
  bool advanced_integration_shard1_ready = false;
  bool advanced_performance_shard1_consistent = false;
  bool advanced_performance_shard1_ready = false;
  bool advanced_core_shard2_consistent = false;
  bool advanced_core_shard2_ready = false;
  bool advanced_core_shard3_consistent = false;
  bool advanced_core_shard3_ready = false;
  bool advanced_edge_compatibility_shard2_consistent = false;
  bool advanced_edge_compatibility_shard2_ready = false;
  bool advanced_edge_compatibility_shard3_consistent = false;
  bool advanced_edge_compatibility_shard3_ready = false;
  bool advanced_diagnostics_shard2_consistent = false;
  bool advanced_diagnostics_shard2_ready = false;
  bool advanced_diagnostics_shard3_consistent = false;
  bool advanced_diagnostics_shard3_ready = false;
  bool advanced_conformance_shard3_consistent = false;
  bool advanced_conformance_shard3_ready = false;
  bool advanced_integration_shard3_consistent = false;
  bool advanced_integration_shard3_ready = false;
  bool advanced_performance_shard3_consistent = false;
  bool advanced_performance_shard3_ready = false;
  bool advanced_core_shard4_consistent = false;
  bool advanced_core_shard4_ready = false;
  bool advanced_edge_compatibility_shard4_consistent = false;
  bool advanced_edge_compatibility_shard4_ready = false;
  bool m248_integration_closeout_signoff_consistent = false;
  bool m248_integration_closeout_signoff_ready = false;
  bool advanced_conformance_shard2_consistent = false;
  bool advanced_conformance_shard2_ready = false;
  bool advanced_integration_shard2_consistent = false;
  bool advanced_integration_shard2_ready = false;
  bool advanced_performance_shard2_consistent = false;
  bool advanced_performance_shard2_ready = false;
  bool integration_closeout_signoff_consistent = false;
  bool integration_closeout_signoff_ready = false;
  bool core_feature_impl_ready = false;
  std::string governance_key;
  std::string modular_split_key;
  std::string lane_a_key;
  std::string lane_b_key;
  std::string lane_c_key;
  std::string lane_d_key;
  std::string core_feature_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string cross_lane_integration_key;
  std::string docs_runbook_sync_key;
  std::string release_candidate_replay_dry_run_key;
  std::string advanced_core_shard1_key;
  std::string advanced_edge_compatibility_shard1_key;
  std::string advanced_diagnostics_shard1_key;
  std::string advanced_conformance_shard1_key;
  std::string advanced_integration_shard1_key;
  std::string advanced_performance_shard1_key;
  std::string advanced_core_shard2_key;
  std::string advanced_core_shard3_key;
  std::string advanced_edge_compatibility_shard2_key;
  std::string advanced_edge_compatibility_shard3_key;
  std::string advanced_diagnostics_shard2_key;
  std::string advanced_diagnostics_shard3_key;
  std::string advanced_conformance_shard3_key;
  std::string advanced_integration_shard3_key;
  std::string advanced_performance_shard3_key;
  std::string advanced_core_shard4_key;
  std::string advanced_edge_compatibility_shard4_key;
  std::string m248_integration_closeout_signoff_key;
  std::string advanced_conformance_shard2_key;
  std::string advanced_integration_shard2_key;
  std::string advanced_performance_shard2_key;
  std::string integration_closeout_signoff_key;
  std::string failure_reason;
};

struct Objc3FrontendProtocolCategorySummary {
  std::size_t declared_protocols = 0;
  std::size_t declared_categories = 0;
  std::size_t resolved_protocol_symbols = 0;
  std::size_t resolved_category_symbols = 0;
  std::size_t protocol_method_symbols = 0;
  std::size_t category_method_symbols = 0;
  std::size_t linked_category_symbols = 0;
  bool deterministic_protocol_category_handoff = true;
};

struct Objc3FrontendClassProtocolCategoryLinkingSummary {
  std::size_t declared_class_interfaces = 0;
  std::size_t declared_class_implementations = 0;
  std::size_t resolved_class_interfaces = 0;
  std::size_t resolved_class_implementations = 0;
  std::size_t linked_class_method_symbols = 0;
  std::size_t linked_category_method_symbols = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic_class_protocol_category_linking_handoff = true;
};

struct Objc3FrontendSelectorNormalizationSummary {
  std::size_t method_declaration_entries = 0;
  std::size_t normalized_method_declarations = 0;
  std::size_t selector_piece_entries = 0;
  std::size_t selector_piece_parameter_links = 0;
  bool deterministic_selector_normalization_handoff = true;
};

struct Objc3FrontendPropertyAttributeSummary {
  std::size_t property_declaration_entries = 0;
  std::size_t property_attribute_entries = 0;
  std::size_t property_attribute_value_entries = 0;
  std::size_t property_accessor_modifier_entries = 0;
  std::size_t property_getter_selector_entries = 0;
  std::size_t property_setter_selector_entries = 0;
  bool deterministic_property_attribute_handoff = true;
};

struct Objc3FrontendObjectPointerNullabilityGenericsSummary {
  std::size_t object_pointer_type_spellings = 0;
  std::size_t pointer_declarator_entries = 0;
  std::size_t pointer_declarator_depth_total = 0;
  std::size_t pointer_declarator_token_entries = 0;
  std::size_t nullability_suffix_entries = 0;
  std::size_t generic_suffix_entries = 0;
  std::size_t terminated_generic_suffix_entries = 0;
  std::size_t unterminated_generic_suffix_entries = 0;
  bool deterministic_object_pointer_nullability_generics_handoff = true;
};

struct Objc3FrontendSymbolGraphScopeResolutionSummary {
  std::size_t global_symbol_nodes = 0;
  std::size_t function_symbol_nodes = 0;
  std::size_t interface_symbol_nodes = 0;
  std::size_t implementation_symbol_nodes = 0;
  std::size_t interface_property_symbol_nodes = 0;
  std::size_t implementation_property_symbol_nodes = 0;
  std::size_t interface_method_symbol_nodes = 0;
  std::size_t implementation_method_symbol_nodes = 0;
  std::size_t top_level_scope_symbols = 0;
  std::size_t nested_scope_symbols = 0;
  std::size_t scope_frames_total = 0;
  std::size_t implementation_interface_resolution_sites = 0;
  std::size_t implementation_interface_resolution_hits = 0;
  std::size_t implementation_interface_resolution_misses = 0;
  std::size_t method_resolution_sites = 0;
  std::size_t method_resolution_hits = 0;
  std::size_t method_resolution_misses = 0;
  bool deterministic_symbol_graph_handoff = true;
  bool deterministic_scope_resolution_handoff = true;
  std::string deterministic_handoff_key;

  std::size_t symbol_nodes_total() const {
    return global_symbol_nodes + function_symbol_nodes + interface_symbol_nodes + implementation_symbol_nodes +
           interface_property_symbol_nodes + implementation_property_symbol_nodes + interface_method_symbol_nodes +
           implementation_method_symbol_nodes;
  }

  std::size_t resolution_sites_total() const {
    return implementation_interface_resolution_sites + method_resolution_sites;
  }

  std::size_t resolution_hits_total() const {
    return implementation_interface_resolution_hits + method_resolution_hits;
  }

  std::size_t resolution_misses_total() const {
    return implementation_interface_resolution_misses + method_resolution_misses;
  }
};

struct Objc3RuntimeMetadataClassSourceRecord {
  std::string record_kind;
  std::string name;
  std::string super_name;
  bool has_super = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataProtocolSourceRecord {
  std::string name;
  std::vector<std::string> inherited_protocols_lexicographic;
  bool is_forward_declaration = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataCategorySourceRecord {
  std::string record_kind;
  std::string class_name;
  std::string category_name;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataPropertySourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string property_name;
  std::string type_name;
  bool has_getter = false;
  std::string getter_selector;
  bool has_setter = false;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataMethodSourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string selector;
  bool is_class_method = false;
  bool has_body = false;
  std::size_t parameter_count = 0;
  std::string return_type_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataIvarSourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string property_name;
  std::string ivar_binding_symbol;
  std::string source_model = kObjc3RuntimeMetadataIvarSourceModel;
  unsigned line = 1;
  unsigned column = 1;
};

inline constexpr const char *kObjc3RuntimeExportLegalityContractId =
    "objc3c-runtime-export-legality-freeze/m251-b001-v1";
inline constexpr const char *kObjc3RuntimeExportEnforcementContractId =
    "objc3c-runtime-export-enforcement/m251-b002-v1";

struct Objc3RuntimeMetadataSourceRecordSet {
  std::vector<Objc3RuntimeMetadataClassSourceRecord> classes_lexicographic;
  std::vector<Objc3RuntimeMetadataProtocolSourceRecord> protocols_lexicographic;
  std::vector<Objc3RuntimeMetadataCategorySourceRecord> categories_lexicographic;
  std::vector<Objc3RuntimeMetadataPropertySourceRecord> properties_lexicographic;
  std::vector<Objc3RuntimeMetadataMethodSourceRecord> methods_lexicographic;
  std::vector<Objc3RuntimeMetadataIvarSourceRecord> ivars_lexicographic;
  bool deterministic = false;
};

inline bool IsReadyObjc3RuntimeMetadataSourceRecordSet(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return records.deterministic;
}

inline constexpr const char *kObjc3ExecutableMetadataSourceGraphContractId =
    "objc3c-executable-metadata-source-graph-completeness/m252-a002-v1";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphOwnerIdentityModel =
    "semantic-link-symbol-and-runtime-owner-identity";
inline constexpr const char *kObjc3ExecutableMetadataMetaclassNodePolicy =
    "first-class-metaclass-nodes-derived-from-interface-runtime-owner-identities";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphEdgeOrderingModel =
    "lexicographic-kind-source-target";
inline constexpr const char *kObjc3ExecutableMetadataSemanticConsistencyContractId =
    "objc3c-executable-metadata-semantic-consistency-freeze/m252-b001-v1";
inline constexpr const char *kObjc3ExecutableMetadataSemanticValidationContractId =
    "objc3c-executable-metadata-semantic-validation/m252-b002-v1";
inline constexpr const char *kObjc3ExecutableMetadataLoweringHandoffContractId =
    "objc3c-executable-metadata-lowering-handoff-freeze/m252-c001-v1";
inline constexpr const char *kObjc3ExecutableMetadataTypedLoweringHandoffContractId =
    "objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1";
inline constexpr const char *kObjc3ExecutableMetadataTypedLoweringManifestSchemaOrderingModel =
    "contract-header-then-source-graph-payload-v1";

struct Objc3ExecutableMetadataInterfaceGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  bool has_super = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  std::size_t class_method_count = 0;
  std::size_t instance_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataImplementationGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string interface_owner_identity;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  bool has_matching_interface = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  std::size_t class_method_count = 0;
  std::size_t instance_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataClassGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  bool has_interface = false;
  bool has_implementation = false;
  bool has_super = false;
  std::size_t interface_property_count = 0;
  std::size_t implementation_property_count = 0;
  std::size_t interface_method_count = 0;
  std::size_t implementation_method_count = 0;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  std::size_t interface_instance_method_count = 0;
  std::size_t implementation_instance_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataMetaclassGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string class_owner_identity;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string super_metaclass_owner_identity;
  bool derived_from_interface = false;
  bool has_implementation = false;
  bool has_super = false;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataProtocolGraphNode {
  std::string protocol_name;
  std::string owner_identity;
  std::vector<std::string> inherited_protocol_owner_identities_lexicographic;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  bool is_forward_declaration = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataCategoryGraphNode {
  std::string class_name;
  std::string category_name;
  std::string owner_identity;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string class_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  bool has_interface = false;
  bool has_implementation = false;
  std::size_t interface_property_count = 0;
  std::size_t implementation_property_count = 0;
  std::size_t interface_method_count = 0;
  std::size_t implementation_method_count = 0;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataPropertyGraphNode {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string property_name;
  std::string type_name;
  bool has_getter = false;
  std::string getter_selector;
  bool has_setter = false;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataMethodGraphNode {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string selector;
  bool is_class_method = false;
  bool has_body = false;
  std::size_t parameter_count = 0;
  std::string return_type_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataIvarGraphNode {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string property_owner_identity;
  std::string property_name;
  std::string ivar_binding_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataGraphEdge {
  std::string edge_kind;
  std::string source_owner_identity;
  std::string target_owner_identity;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataSourceGraph {
  std::string contract_id = kObjc3ExecutableMetadataSourceGraphContractId;
  std::string owner_identity_model =
      kObjc3ExecutableMetadataSourceGraphOwnerIdentityModel;
  std::string metaclass_node_policy =
      kObjc3ExecutableMetadataMetaclassNodePolicy;
  std::string edge_ordering_model =
      kObjc3ExecutableMetadataSourceGraphEdgeOrderingModel;
  std::vector<Objc3ExecutableMetadataInterfaceGraphNode>
      interface_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataImplementationGraphNode>
      implementation_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataClassGraphNode> class_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataMetaclassGraphNode>
      metaclass_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataProtocolGraphNode>
      protocol_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataCategoryGraphNode>
      category_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataPropertyGraphNode>
      property_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataMethodGraphNode>
      method_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataIvarGraphNode> ivar_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataGraphEdge> owner_edges_lexicographic;
  bool deterministic = false;
  bool source_graph_complete = false;
  bool ready_for_semantic_closure = false;
  bool ready_for_lowering = false;
};

inline bool IsReadyObjc3ExecutableMetadataSourceGraph(
    const Objc3ExecutableMetadataSourceGraph &graph) {
  return graph.deterministic && graph.source_graph_complete &&
         graph.ready_for_semantic_closure && !graph.ready_for_lowering &&
         !graph.contract_id.empty() && !graph.owner_identity_model.empty() &&
         !graph.metaclass_node_policy.empty() &&
         !graph.edge_ordering_model.empty();
}

struct Objc3ExecutableMetadataSemanticConsistencyBoundary {
  std::string contract_id =
      kObjc3ExecutableMetadataSemanticConsistencyContractId;
  std::string executable_metadata_source_graph_contract_id;
  bool semantic_boundary_frozen = false;
  bool lowering_admission_ready = false;
  bool fail_closed = false;
  bool source_graph_ready = false;
  bool protocol_category_handoff_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool selector_normalization_deterministic = false;
  bool property_attribute_deterministic = false;
  bool symbol_graph_scope_resolution_deterministic = false;
  bool protocol_inheritance_edges_complete = false;
  bool category_attachment_edges_complete = false;
  bool declaration_export_owner_split_complete = false;
  bool property_method_ivar_owner_edges_complete = false;
  bool semantic_conflict_diagnostics_enforcement_pending = true;
  bool duplicate_export_owner_enforcement_pending = true;
  bool lowering_admission_pending = true;
  std::size_t protocol_node_count = 0;
  std::size_t category_node_count = 0;
  std::size_t property_node_count = 0;
  std::size_t method_node_count = 0;
  std::size_t ivar_node_count = 0;
  std::size_t owner_edge_count = 0;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
    const Objc3ExecutableMetadataSemanticConsistencyBoundary &boundary) {
  return !boundary.contract_id.empty() &&
         !boundary.executable_metadata_source_graph_contract_id.empty() &&
         boundary.semantic_boundary_frozen &&
         !boundary.lowering_admission_ready &&
         boundary.fail_closed &&
         boundary.source_graph_ready &&
         boundary.protocol_category_handoff_deterministic &&
         boundary.class_protocol_category_linking_deterministic &&
         boundary.selector_normalization_deterministic &&
         boundary.property_attribute_deterministic &&
         boundary.symbol_graph_scope_resolution_deterministic &&
         boundary.protocol_inheritance_edges_complete &&
         boundary.category_attachment_edges_complete &&
         boundary.declaration_export_owner_split_complete &&
         boundary.property_method_ivar_owner_edges_complete &&
         boundary.semantic_conflict_diagnostics_enforcement_pending &&
         boundary.duplicate_export_owner_enforcement_pending &&
         boundary.lowering_admission_pending &&
         boundary.failure_reason.empty();
}

struct Objc3ExecutableMetadataSemanticValidationSurface {
  std::string contract_id =
      kObjc3ExecutableMetadataSemanticValidationContractId;
  std::string executable_metadata_semantic_consistency_contract_id;
  bool semantic_consistency_ready = false;
  bool method_lookup_override_conflict_handoff_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool class_inheritance_edges_complete = false;
  bool protocol_inheritance_edges_complete = false;
  bool metaclass_edges_complete = false;
  bool inheritance_chain_cycle_free = false;
  bool superclass_targets_resolved = false;
  bool protocol_inheritance_targets_resolved = false;
  bool metaclass_targets_resolved = false;
  bool metaclass_lineage_aligned = false;
  bool method_override_edges_complete = false;
  bool override_lookup_complete = false;
  bool override_conflicts_absent = false;
  bool protocol_composition_valid = false;
  bool inheritance_validation_ready = false;
  bool override_validation_ready = false;
  bool protocol_composition_validation_ready = false;
  bool metaclass_relationship_validation_ready = false;
  bool semantic_validation_complete = false;
  bool lowering_admission_ready = false;
  bool fail_closed = false;
  std::size_t class_inheritance_edge_count = 0;
  std::size_t protocol_inheritance_edge_count = 0;
  std::size_t metaclass_super_edge_count = 0;
  std::size_t override_edge_count = 0;
  std::size_t class_method_override_edge_count = 0;
  std::size_t instance_method_override_edge_count = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataSemanticValidationSurface(
    const Objc3ExecutableMetadataSemanticValidationSurface &surface) {
  return !surface.contract_id.empty() &&
         !surface.executable_metadata_semantic_consistency_contract_id.empty() &&
         surface.semantic_consistency_ready &&
         surface.method_lookup_override_conflict_handoff_deterministic &&
         surface.class_protocol_category_linking_deterministic &&
         surface.class_inheritance_edges_complete &&
         surface.protocol_inheritance_edges_complete &&
         surface.metaclass_edges_complete &&
         surface.inheritance_chain_cycle_free &&
         surface.superclass_targets_resolved &&
         surface.protocol_inheritance_targets_resolved &&
         surface.metaclass_targets_resolved &&
         surface.metaclass_lineage_aligned &&
         surface.method_override_edges_complete &&
         surface.override_lookup_complete &&
         surface.override_conflicts_absent &&
         surface.protocol_composition_valid &&
         surface.inheritance_validation_ready &&
         surface.override_validation_ready &&
         surface.protocol_composition_validation_ready &&
         surface.metaclass_relationship_validation_ready &&
         surface.semantic_validation_complete &&
         !surface.lowering_admission_ready &&
         surface.fail_closed &&
         surface.failure_reason.empty();
}

struct Objc3ExecutableMetadataLoweringHandoffSurface {
  std::string contract_id = kObjc3ExecutableMetadataLoweringHandoffContractId;
  std::string executable_metadata_source_graph_contract_id;
  std::string executable_metadata_semantic_consistency_contract_id;
  std::string executable_metadata_semantic_validation_contract_id;
  bool source_graph_ready = false;
  bool semantic_consistency_ready = false;
  bool semantic_validation_ready = false;
  bool semantic_type_metadata_handoff_deterministic = false;
  bool protocol_category_handoff_deterministic = false;
  bool class_protocol_category_linking_handoff_deterministic = false;
  bool selector_normalization_handoff_deterministic = false;
  bool property_attribute_handoff_deterministic = false;
  bool symbol_graph_scope_resolution_handoff_deterministic = false;
  bool property_synthesis_ivar_binding_handoff_deterministic = false;
  bool lowering_schema_frozen = false;
  bool fail_closed = false;
  bool ready_for_lowering = false;
  std::size_t interface_node_count = 0;
  std::size_t implementation_node_count = 0;
  std::size_t class_node_count = 0;
  std::size_t metaclass_node_count = 0;
  std::size_t protocol_node_count = 0;
  std::size_t category_node_count = 0;
  std::size_t property_node_count = 0;
  std::size_t method_node_count = 0;
  std::size_t ivar_node_count = 0;
  std::size_t owner_edge_count = 0;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  return !surface.contract_id.empty() &&
         !surface.executable_metadata_source_graph_contract_id.empty() &&
         !surface.executable_metadata_semantic_consistency_contract_id.empty() &&
         !surface.executable_metadata_semantic_validation_contract_id.empty() &&
         surface.source_graph_ready &&
         surface.semantic_consistency_ready &&
         surface.semantic_validation_ready &&
         surface.semantic_type_metadata_handoff_deterministic &&
         surface.protocol_category_handoff_deterministic &&
         surface.class_protocol_category_linking_handoff_deterministic &&
         surface.selector_normalization_handoff_deterministic &&
         surface.property_attribute_handoff_deterministic &&
         surface.symbol_graph_scope_resolution_handoff_deterministic &&
         surface.property_synthesis_ivar_binding_handoff_deterministic &&
         surface.lowering_schema_frozen &&
         surface.fail_closed &&
         !surface.ready_for_lowering &&
         !surface.replay_key.empty() &&
         surface.failure_reason.empty();
}

struct Objc3ExecutableMetadataTypedLoweringHandoff {
  std::string contract_id = kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string executable_metadata_lowering_handoff_contract_id;
  std::string executable_metadata_source_graph_contract_id;
  std::string executable_metadata_semantic_consistency_contract_id;
  std::string executable_metadata_semantic_validation_contract_id;
  std::string manifest_schema_ordering_model =
      kObjc3ExecutableMetadataTypedLoweringManifestSchemaOrderingModel;
  bool source_graph_ready = false;
  bool semantic_consistency_ready = false;
  bool semantic_validation_ready = false;
  bool lowering_handoff_surface_ready = false;
  bool deterministic = false;
  bool manifest_schema_frozen = false;
  bool fail_closed = false;
  bool ready_for_lowering = false;
  Objc3ExecutableMetadataSourceGraph source_graph;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  return !surface.contract_id.empty() &&
         !surface.executable_metadata_lowering_handoff_contract_id.empty() &&
         !surface.executable_metadata_source_graph_contract_id.empty() &&
         !surface.executable_metadata_semantic_consistency_contract_id.empty() &&
         !surface.executable_metadata_semantic_validation_contract_id.empty() &&
         !surface.manifest_schema_ordering_model.empty() &&
         surface.source_graph_ready &&
         surface.semantic_consistency_ready &&
         surface.semantic_validation_ready &&
         surface.lowering_handoff_surface_ready &&
         surface.deterministic &&
         surface.manifest_schema_frozen &&
         surface.fail_closed &&
         surface.ready_for_lowering &&
         !surface.replay_key.empty() &&
         surface.failure_reason.empty();
}

struct Objc3RuntimeMetadataSourceOwnershipBoundary {
  std::string contract_id = kObjc3RuntimeMetadataSourceOwnershipContractId;
  std::string canonical_source_schema = kObjc3RuntimeMetadataCanonicalSourceSchema;
  std::string class_record_ast_anchor = kObjc3RuntimeMetadataClassAstAnchor;
  std::string protocol_record_ast_anchor = kObjc3RuntimeMetadataProtocolAstAnchor;
  std::string category_record_ast_anchor = kObjc3RuntimeMetadataCategoryAstAnchor;
  std::string property_record_ast_anchor = kObjc3RuntimeMetadataPropertyAstAnchor;
  std::string method_record_ast_anchor = kObjc3RuntimeMetadataMethodAstAnchor;
  std::string ivar_record_ast_anchor = kObjc3RuntimeMetadataIvarAstAnchor;
  std::string ivar_record_source_model = kObjc3RuntimeMetadataIvarSourceModel;
  bool frontend_owns_runtime_metadata_source_records = false;
  bool runtime_metadata_source_records_ready_for_lowering = false;
  bool native_runtime_library_present = false;
  bool runtime_shim_test_only = true;
  bool deterministic_source_schema = false;
  bool fail_closed = false;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_interface_record_count = 0;
  std::size_t category_implementation_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::string failure_reason;

  std::size_t category_record_count() const {
    return category_interface_record_count + category_implementation_record_count;
  }
};

inline bool IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary &boundary) {
  return boundary.frontend_owns_runtime_metadata_source_records &&
         !boundary.runtime_metadata_source_records_ready_for_lowering &&
         !boundary.native_runtime_library_present &&
         boundary.runtime_shim_test_only &&
         boundary.deterministic_source_schema &&
         boundary.fail_closed &&
         !boundary.contract_id.empty() &&
         !boundary.canonical_source_schema.empty() &&
         !boundary.class_record_ast_anchor.empty() &&
         !boundary.protocol_record_ast_anchor.empty() &&
         !boundary.category_record_ast_anchor.empty() &&
         !boundary.property_record_ast_anchor.empty() &&
         !boundary.method_record_ast_anchor.empty() &&
         !boundary.ivar_record_ast_anchor.empty() &&
         !boundary.ivar_record_source_model.empty() &&
         boundary.ivar_record_count <= boundary.property_record_count &&
         boundary.failure_reason.empty();
}

struct Objc3RuntimeExportLegalityBoundary {
  std::string contract_id = kObjc3RuntimeExportLegalityContractId;
  bool semantic_boundary_frozen = false;
  bool metadata_export_enforcement_ready = false;
  bool fail_closed = false;
  bool semantic_integration_surface_built = false;
  bool sema_type_metadata_handoff_deterministic = false;
  bool typed_sema_surface_ready = false;
  bool typed_sema_surface_deterministic = false;
  bool runtime_metadata_source_boundary_ready = false;
  bool protocol_category_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool selector_normalization_deterministic = false;
  bool property_attribute_deterministic = false;
  bool object_pointer_surface_deterministic = false;
  bool symbol_graph_scope_resolution_deterministic = false;
  bool property_synthesis_ivar_binding_deterministic = false;
  bool duplicate_runtime_identity_enforcement_pending = true;
  bool incomplete_declaration_export_blocking_pending = true;
  bool illegal_redeclaration_mix_export_blocking_pending = true;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  std::size_t property_attribute_invalid_entries = 0;
  std::size_t property_attribute_contract_violations = 0;
  std::size_t invalid_type_annotation_sites = 0;
  std::size_t property_ivar_binding_missing = 0;
  std::size_t property_ivar_binding_conflicts = 0;
  std::size_t implementation_resolution_misses = 0;
  std::size_t method_resolution_misses = 0;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeExportLegalityBoundary(
    const Objc3RuntimeExportLegalityBoundary &boundary) {
  return !boundary.contract_id.empty() &&
         boundary.semantic_boundary_frozen &&
         !boundary.metadata_export_enforcement_ready &&
         boundary.fail_closed &&
         boundary.sema_type_metadata_handoff_deterministic &&
         boundary.typed_sema_surface_ready &&
         boundary.typed_sema_surface_deterministic &&
         boundary.runtime_metadata_source_boundary_ready &&
         boundary.protocol_category_deterministic &&
         boundary.class_protocol_category_linking_deterministic &&
         boundary.selector_normalization_deterministic &&
         boundary.property_attribute_deterministic &&
         boundary.object_pointer_surface_deterministic &&
         boundary.symbol_graph_scope_resolution_deterministic &&
         boundary.property_synthesis_ivar_binding_deterministic &&
         boundary.duplicate_runtime_identity_enforcement_pending &&
         boundary.incomplete_declaration_export_blocking_pending &&
         boundary.illegal_redeclaration_mix_export_blocking_pending &&
         boundary.invalid_protocol_composition_sites <=
             boundary.protocol_record_count + boundary.category_record_count &&
         boundary.ivar_record_count <= boundary.property_record_count &&
         boundary.failure_reason.empty();
}

struct Objc3RuntimeExportEnforcementSummary {
  std::string contract_id = kObjc3RuntimeExportEnforcementContractId;
  bool metadata_completeness_enforced = false;
  bool duplicate_runtime_identity_suppression_enforced = false;
  bool illegal_redeclaration_mix_blocking_enforced = false;
  bool metadata_shape_drift_blocking_enforced = false;
  bool fail_closed = false;
  bool ready_for_runtime_export = false;
  std::size_t duplicate_runtime_identity_sites = 0;
  std::size_t incomplete_declaration_sites = 0;
  std::size_t illegal_redeclaration_mix_sites = 0;
  std::size_t metadata_shape_drift_sites = 0;
  unsigned first_failure_line = 1;
  unsigned first_failure_column = 1;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeExportEnforcementSummary(
    const Objc3RuntimeExportEnforcementSummary &summary) {
  return !summary.contract_id.empty() &&
         summary.metadata_completeness_enforced &&
         summary.duplicate_runtime_identity_suppression_enforced &&
         summary.illegal_redeclaration_mix_blocking_enforced &&
         summary.metadata_shape_drift_blocking_enforced &&
         summary.fail_closed &&
         summary.ready_for_runtime_export &&
         summary.duplicate_runtime_identity_sites == 0 &&
         summary.incomplete_declaration_sites == 0 &&
         summary.illegal_redeclaration_mix_sites == 0 &&
         summary.metadata_shape_drift_sites == 0 &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataSectionAbiFreezeSummary {
  std::string contract_id = kObjc3RuntimeMetadataSectionAbiContractId;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool object_file_section_inventory_frozen = false;
  bool symbol_policy_frozen = false;
  bool visibility_model_frozen = false;
  bool retention_policy_frozen = false;
  bool runtime_metadata_source_boundary_ready = false;
  bool runtime_export_legality_boundary_ready = false;
  bool runtime_export_enforcement_ready = false;
  bool ready_for_section_scaffold = false;
  std::string logical_image_info_section =
      kObjc3RuntimeMetadataLogicalImageInfoSection;
  std::string logical_class_descriptor_section =
      kObjc3RuntimeMetadataLogicalClassDescriptorSection;
  std::string logical_protocol_descriptor_section =
      kObjc3RuntimeMetadataLogicalProtocolDescriptorSection;
  std::string logical_category_descriptor_section =
      kObjc3RuntimeMetadataLogicalCategoryDescriptorSection;
  std::string logical_property_descriptor_section =
      kObjc3RuntimeMetadataLogicalPropertyDescriptorSection;
  std::string logical_ivar_descriptor_section =
      kObjc3RuntimeMetadataLogicalIvarDescriptorSection;
  std::string descriptor_symbol_prefix =
      kObjc3RuntimeMetadataDescriptorSymbolPrefix;
  std::string aggregate_symbol_prefix =
      kObjc3RuntimeMetadataAggregateSymbolPrefix;
  std::string image_info_symbol = kObjc3RuntimeMetadataImageInfoSymbol;
  std::string descriptor_linkage =
      kObjc3RuntimeMetadataDescriptorLinkagePolicy;
  std::string aggregate_linkage =
      kObjc3RuntimeMetadataAggregateLinkagePolicy;
  std::string metadata_visibility = kObjc3RuntimeMetadataVisibilityPolicy;
  std::string retention_root = kObjc3RuntimeMetadataRetentionPolicyRoot;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary &summary) {
  return !summary.contract_id.empty() &&
         summary.boundary_frozen &&
         summary.fail_closed &&
         summary.object_file_section_inventory_frozen &&
         summary.symbol_policy_frozen &&
         summary.visibility_model_frozen &&
         summary.retention_policy_frozen &&
         summary.runtime_metadata_source_boundary_ready &&
         summary.runtime_export_legality_boundary_ready &&
         summary.runtime_export_enforcement_ready &&
         summary.ready_for_section_scaffold &&
         !summary.logical_image_info_section.empty() &&
         !summary.logical_class_descriptor_section.empty() &&
         !summary.logical_protocol_descriptor_section.empty() &&
         !summary.logical_category_descriptor_section.empty() &&
         !summary.logical_property_descriptor_section.empty() &&
         !summary.logical_ivar_descriptor_section.empty() &&
         !summary.descriptor_symbol_prefix.empty() &&
         !summary.aggregate_symbol_prefix.empty() &&
         !summary.image_info_symbol.empty() &&
         !summary.descriptor_linkage.empty() &&
         !summary.aggregate_linkage.empty() &&
         !summary.metadata_visibility.empty() &&
         !summary.retention_root.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataSectionScaffoldSummary {
  std::string contract_id = kObjc3RuntimeMetadataSectionScaffoldContractId;
  std::string abi_contract_id = kObjc3RuntimeMetadataSectionAbiContractId;
  bool scaffold_emitted = false;
  bool fail_closed = false;
  bool uses_llvm_used = false;
  bool image_info_emitted = false;
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::size_t total_retained_global_count = 0;
  std::string image_info_symbol = kObjc3RuntimeMetadataImageInfoSymbol;
  std::string class_aggregate_symbol =
      kObjc3RuntimeMetadataClassDescriptorAggregateSymbol;
  std::string protocol_aggregate_symbol =
      kObjc3RuntimeMetadataProtocolDescriptorAggregateSymbol;
  std::string category_aggregate_symbol =
      kObjc3RuntimeMetadataCategoryDescriptorAggregateSymbol;
  std::string property_aggregate_symbol =
      kObjc3RuntimeMetadataPropertyDescriptorAggregateSymbol;
  std::string ivar_aggregate_symbol =
      kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataSectionScaffoldSummary(
    const Objc3RuntimeMetadataSectionScaffoldSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.abi_contract_id.empty() &&
         summary.scaffold_emitted &&
         summary.fail_closed &&
         summary.uses_llvm_used &&
         summary.image_info_emitted &&
         summary.total_descriptor_count ==
             summary.class_descriptor_count +
                 summary.protocol_descriptor_count +
                 summary.category_descriptor_count +
                 summary.property_descriptor_count +
                 summary.ivar_descriptor_count &&
         summary.total_retained_global_count ==
             summary.total_descriptor_count + 6u &&
         !summary.image_info_symbol.empty() &&
         !summary.class_aggregate_symbol.empty() &&
         !summary.protocol_aggregate_symbol.empty() &&
         !summary.category_aggregate_symbol.empty() &&
         !summary.property_aggregate_symbol.empty() &&
         !summary.ivar_aggregate_symbol.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataObjectInspectionHarnessSummary {
  std::string contract_id = kObjc3RuntimeMetadataObjectInspectionContractId;
  std::string scaffold_contract_id = kObjc3RuntimeMetadataSectionScaffoldContractId;
  bool matrix_published = false;
  bool fail_closed = false;
  bool uses_llvm_readobj = false;
  bool uses_llvm_objdump = false;
  std::size_t matrix_row_count = 0;
  std::string fixture_path = kObjc3RuntimeMetadataObjectInspectionFixturePath;
  std::string emit_prefix = kObjc3RuntimeMetadataObjectInspectionEmitPrefix;
  std::string object_relative_path =
      kObjc3RuntimeMetadataObjectInspectionObjectRelativePath;
  std::string section_inventory_row_key =
      kObjc3RuntimeMetadataObjectInspectionSectionInventoryRowKey;
  std::string section_inventory_command =
      kObjc3RuntimeMetadataObjectInspectionSectionCommand;
  std::string symbol_inventory_row_key =
      kObjc3RuntimeMetadataObjectInspectionSymbolInventoryRowKey;
  std::string symbol_inventory_command =
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.scaffold_contract_id.empty() &&
         summary.matrix_published &&
         summary.fail_closed &&
         summary.uses_llvm_readobj &&
         summary.uses_llvm_objdump &&
         summary.matrix_row_count == 2u &&
         !summary.fixture_path.empty() &&
         !summary.emit_prefix.empty() &&
         !summary.object_relative_path.empty() &&
         !summary.section_inventory_row_key.empty() &&
         !summary.section_inventory_command.empty() &&
         !summary.symbol_inventory_row_key.empty() &&
         !summary.symbol_inventory_command.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataSourceToSectionMatrixRow {
  std::string row_key;
  std::string graph_node_kind;
  std::string emission_mode;
  std::string logical_section;
  std::string payload_role;
  std::string descriptor_symbol_family;
  std::string aggregate_symbol;
  std::string relocation_behavior;
  std::string proof_fixture_path;
  std::string proof_mode;
  std::string section_inventory_command;
  std::string symbol_inventory_command;
};

struct Objc3RuntimeMetadataSourceToSectionMatrixSummary {
  std::string contract_id = kObjc3RuntimeMetadataSourceToSectionMatrixContractId;
  std::string source_graph_contract_id =
      kObjc3ExecutableMetadataSourceGraphContractId;
  std::string section_abi_contract_id = kObjc3RuntimeMetadataSectionAbiContractId;
  std::string section_scaffold_contract_id =
      kObjc3RuntimeMetadataSectionScaffoldContractId;
  std::string object_inspection_contract_id =
      kObjc3RuntimeMetadataObjectInspectionContractId;
  std::string manifest_surface_path =
      kObjc3RuntimeMetadataSourceToSectionMatrixSurfacePath;
  std::string row_ordering_model =
      kObjc3RuntimeMetadataSourceToSectionMatrixOrderingModel;
  bool matrix_published = false;
  bool fail_closed = false;
  bool source_graph_ready = false;
  bool section_abi_ready = false;
  bool section_scaffold_ready = false;
  bool object_inspection_ready = false;
  bool supported_node_coverage_complete = false;
  bool explicit_non_goals_published = false;
  bool row_ordering_frozen = false;
  std::size_t matrix_row_count = 0;
  std::array<Objc3RuntimeMetadataSourceToSectionMatrixRow, 9u> rows = {};
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  if (summary.contract_id.empty() || summary.source_graph_contract_id.empty() ||
      summary.section_abi_contract_id.empty() ||
      summary.section_scaffold_contract_id.empty() ||
      summary.object_inspection_contract_id.empty() ||
      summary.manifest_surface_path.empty() ||
      summary.row_ordering_model.empty() || !summary.matrix_published ||
      !summary.fail_closed || !summary.source_graph_ready ||
      !summary.section_abi_ready || !summary.section_scaffold_ready ||
      !summary.object_inspection_ready ||
      !summary.supported_node_coverage_complete ||
      !summary.explicit_non_goals_published || !summary.row_ordering_frozen ||
      summary.matrix_row_count != summary.rows.size() || summary.replay_key.empty() ||
      !summary.failure_reason.empty()) {
    return false;
  }
  for (const auto &row : summary.rows) {
    if (row.row_key.empty() || row.graph_node_kind.empty() ||
        row.emission_mode.empty() || row.logical_section.empty() ||
        row.payload_role.empty() || row.descriptor_symbol_family.empty() ||
        row.aggregate_symbol.empty() || row.relocation_behavior.empty() ||
        row.proof_fixture_path.empty() || row.proof_mode.empty() ||
        row.section_inventory_command.empty() ||
        row.symbol_inventory_command.empty()) {
      return false;
    }
  }
  return true;
}

struct Objc3ExecutableMetadataDebugProjectionMatrixRow {
  std::string row_key;
  std::string artifact_kind;
  std::string fixture_path;
  std::string emit_prefix = kObjc3ExecutableMetadataDebugProjectionEmitPrefix;
  std::string artifact_relative_path;
  std::string probe_command;
  std::string inspection_command;
  std::string expected_anchor;
};

struct Objc3ExecutableMetadataDebugProjectionSummary {
  std::string contract_id = kObjc3ExecutableMetadataDebugProjectionContractId;
  std::string typed_lowering_handoff_contract_id =
      kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string source_graph_contract_id =
      kObjc3ExecutableMetadataSourceGraphContractId;
  std::string named_metadata_name =
      kObjc3ExecutableMetadataDebugProjectionNamedMetadataName;
  std::string manifest_surface_path =
      kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath;
  std::string typed_handoff_surface_path =
      kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath;
  std::string source_graph_surface_path =
      kObjc3ExecutableMetadataSourceGraphManifestSurfacePath;
  bool matrix_published = false;
  bool fail_closed = false;
  bool manifest_debug_surface_published = false;
  bool ir_named_metadata_published = false;
  bool replay_anchor_deterministic = false;
  bool active_typed_handoff_ready = false;
  std::size_t matrix_row_count = 0;
  std::array<Objc3ExecutableMetadataDebugProjectionMatrixRow, 3u> rows = {};
  std::string replay_key;
  std::string active_typed_handoff_replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary) {
  if (summary.contract_id.empty() || summary.typed_lowering_handoff_contract_id.empty() ||
      summary.source_graph_contract_id.empty() ||
      summary.named_metadata_name.empty() ||
      summary.manifest_surface_path.empty() ||
      summary.typed_handoff_surface_path.empty() ||
      summary.source_graph_surface_path.empty() || !summary.matrix_published ||
      !summary.fail_closed || !summary.manifest_debug_surface_published ||
      !summary.ir_named_metadata_published ||
      !summary.replay_anchor_deterministic || summary.matrix_row_count != summary.rows.size() ||
      summary.replay_key.empty() || !summary.failure_reason.empty()) {
    return false;
  }
  for (const auto &row : summary.rows) {
    if (row.row_key.empty() || row.artifact_kind.empty() || row.fixture_path.empty() ||
        row.emit_prefix.empty() || row.artifact_relative_path.empty() ||
        row.probe_command.empty() || row.inspection_command.empty() ||
        row.expected_anchor.empty()) {
      return false;
    }
  }
  return true;
}

struct Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary {
  std::string contract_id =
      kObjc3ExecutableMetadataRuntimeIngestPackagingContractId;
  std::string typed_lowering_handoff_contract_id =
      kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string debug_projection_contract_id =
      kObjc3ExecutableMetadataDebugProjectionContractId;
  std::string packaging_surface_path =
      kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath;
  std::string typed_handoff_surface_path =
      kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath;
  std::string debug_projection_surface_path =
      kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath;
  std::string packaging_payload_model =
      kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel;
  std::string transport_artifact_relative_path =
      kObjc3ExecutableMetadataRuntimeIngestPackagingTransportArtifact;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool typed_lowering_handoff_ready = false;
  bool debug_projection_ready = false;
  bool manifest_transport_frozen = false;
  bool runtime_section_emission_not_yet_landed = false;
  bool startup_registration_not_yet_landed = false;
  bool runtime_loader_registration_not_yet_landed = false;
  bool explicit_non_goals_published = false;
  bool ready_for_packaging_implementation = false;
  std::string typed_lowering_handoff_replay_key;
  std::string debug_projection_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &summary) {
  return !summary.contract_id.empty() &&
         !summary.typed_lowering_handoff_contract_id.empty() &&
         !summary.debug_projection_contract_id.empty() &&
         !summary.packaging_surface_path.empty() &&
         !summary.typed_handoff_surface_path.empty() &&
         !summary.debug_projection_surface_path.empty() &&
         !summary.packaging_payload_model.empty() &&
         !summary.transport_artifact_relative_path.empty() &&
         summary.boundary_frozen && summary.fail_closed &&
         summary.typed_lowering_handoff_ready && summary.debug_projection_ready &&
         summary.manifest_transport_frozen &&
         summary.runtime_section_emission_not_yet_landed &&
         summary.startup_registration_not_yet_landed &&
         summary.runtime_loader_registration_not_yet_landed &&
         summary.explicit_non_goals_published &&
         summary.ready_for_packaging_implementation &&
         !summary.typed_lowering_handoff_replay_key.empty() &&
         !summary.debug_projection_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary {
  std::string contract_id =
      kObjc3ExecutableMetadataRuntimeIngestBinaryBoundaryContractId;
  std::string packaging_contract_id =
      kObjc3ExecutableMetadataRuntimeIngestPackagingContractId;
  std::string typed_lowering_handoff_contract_id =
      kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string debug_projection_contract_id =
      kObjc3ExecutableMetadataDebugProjectionContractId;
  std::string packaging_surface_path =
      kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath;
  std::string binary_boundary_surface_path =
      kObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySurfacePath;
  std::string payload_model =
      kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel;
  std::string envelope_format =
      kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeFormat;
  std::string artifact_relative_path =
      kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactRelativePath;
  std::string artifact_suffix =
      kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactSuffix;
  std::string binary_magic = kObjc3ExecutableMetadataRuntimeIngestBinaryMagic;
  std::uint32_t envelope_version =
      kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeVersion;
  std::uint32_t chunk_count =
      kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeChunkCount;
  std::array<std::string, 3u> chunk_names = {
      kObjc3ExecutableMetadataRuntimeIngestBinaryPackagingChunkName,
      kObjc3ExecutableMetadataRuntimeIngestBinaryTypedHandoffChunkName,
      kObjc3ExecutableMetadataRuntimeIngestBinaryDebugProjectionChunkName};
  bool fail_closed = false;
  bool packaging_contract_ready = false;
  bool typed_lowering_handoff_ready = false;
  bool debug_projection_ready = false;
  bool binary_payload_present = false;
  bool binary_boundary_emitted = false;
  bool binary_envelope_deterministic = false;
  bool ready_for_section_emission_handoff = false;
  std::size_t payload_bytes = 0;
  std::string packaging_contract_replay_key;
  std::string typed_lowering_handoff_replay_key;
  std::string debug_projection_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary) {
  if (summary.contract_id.empty() || summary.packaging_contract_id.empty() ||
      summary.typed_lowering_handoff_contract_id.empty() ||
      summary.debug_projection_contract_id.empty() ||
      summary.packaging_surface_path.empty() ||
      summary.binary_boundary_surface_path.empty() ||
      summary.payload_model.empty() || summary.envelope_format.empty() ||
      summary.artifact_relative_path.empty() || summary.artifact_suffix.empty() ||
      summary.binary_magic.empty() || summary.envelope_version == 0u ||
      summary.chunk_count != summary.chunk_names.size() || !summary.fail_closed ||
      !summary.packaging_contract_ready ||
      !summary.typed_lowering_handoff_ready ||
      !summary.debug_projection_ready || !summary.binary_payload_present ||
      !summary.binary_boundary_emitted ||
      !summary.binary_envelope_deterministic ||
      !summary.ready_for_section_emission_handoff || summary.payload_bytes == 0u ||
      summary.packaging_contract_replay_key.empty() ||
      summary.typed_lowering_handoff_replay_key.empty() ||
      summary.debug_projection_replay_key.empty() || summary.replay_key.empty() ||
      !summary.failure_reason.empty()) {
    return false;
  }
  for (const auto &chunk_name : summary.chunk_names) {
    if (chunk_name.empty()) {
      return false;
    }
  }
  return true;
}

struct Objc3RuntimeSupportLibraryContractSummary {
  std::string contract_id = kObjc3RuntimeSupportLibraryContractId;
  std::string metadata_scaffold_contract_id =
      kObjc3RuntimeMetadataSectionScaffoldContractId;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool target_name_frozen = false;
  bool exported_entrypoints_frozen = false;
  bool ownership_boundaries_frozen = false;
  bool build_constraints_frozen = false;
  bool shim_remains_test_only = false;
  bool native_runtime_library_present = false;
  bool driver_link_wiring_pending = true;
  bool ready_for_runtime_library_skeleton = false;
  std::string cmake_target_name = kObjc3RuntimeSupportLibraryTargetName;
  std::string public_header_path = kObjc3RuntimeSupportLibraryPublicHeaderPath;
  std::string source_root = kObjc3RuntimeSupportLibrarySourceRoot;
  std::string library_kind = kObjc3RuntimeSupportLibraryKind;
  std::string archive_basename = kObjc3RuntimeSupportLibraryArchiveBasename;
  std::string register_image_symbol =
      kObjc3RuntimeSupportLibraryRegisterImageSymbol;
  std::string lookup_selector_symbol =
      kObjc3RuntimeSupportLibraryLookupSelectorSymbol;
  std::string dispatch_i32_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;
  std::string driver_link_mode = kObjc3RuntimeSupportLibraryDriverLinkMode;
  std::string compiler_ownership_boundary =
      kObjc3RuntimeSupportLibraryCompilerOwnershipBoundary;
  std::string runtime_ownership_boundary =
      kObjc3RuntimeSupportLibraryRuntimeOwnershipBoundary;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeSupportLibraryContractSummary(
    const Objc3RuntimeSupportLibraryContractSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.metadata_scaffold_contract_id.empty() &&
         summary.boundary_frozen &&
         summary.fail_closed &&
         summary.target_name_frozen &&
         summary.exported_entrypoints_frozen &&
         summary.ownership_boundaries_frozen &&
         summary.build_constraints_frozen &&
         summary.shim_remains_test_only &&
         !summary.native_runtime_library_present &&
         summary.driver_link_wiring_pending &&
         summary.ready_for_runtime_library_skeleton &&
         !summary.cmake_target_name.empty() &&
         !summary.public_header_path.empty() &&
         !summary.source_root.empty() &&
         !summary.library_kind.empty() &&
         !summary.archive_basename.empty() &&
         !summary.register_image_symbol.empty() &&
         !summary.lookup_selector_symbol.empty() &&
         !summary.dispatch_i32_symbol.empty() &&
         !summary.reset_for_testing_symbol.empty() &&
         !summary.driver_link_mode.empty() &&
         !summary.compiler_ownership_boundary.empty() &&
         !summary.runtime_ownership_boundary.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeSupportLibraryCoreFeatureSummary {
  std::string contract_id = kObjc3RuntimeSupportLibraryCoreFeatureContractId;
  std::string support_library_contract_id = kObjc3RuntimeSupportLibraryContractId;
  std::string metadata_scaffold_contract_id =
      kObjc3RuntimeMetadataSectionScaffoldContractId;
  bool fail_closed = false;
  bool native_runtime_library_sources_present = false;
  bool native_runtime_library_header_present = false;
  bool native_runtime_library_archive_build_enabled = false;
  bool native_runtime_library_entrypoints_implemented = false;
  bool selector_lookup_stateful = false;
  bool deterministic_dispatch_formula_matches_test_shim = false;
  bool reset_for_testing_supported = false;
  bool shim_remains_test_only = false;
  bool driver_link_wiring_pending = true;
  bool ready_for_driver_link_wiring = false;
  std::string cmake_target_name = kObjc3RuntimeSupportLibraryTargetName;
  std::string public_header_path = kObjc3RuntimeSupportLibraryPublicHeaderPath;
  std::string source_root = kObjc3RuntimeSupportLibrarySourceRoot;
  std::string implementation_source_path =
      kObjc3RuntimeSupportLibraryImplementationSourcePath;
  std::string library_kind = kObjc3RuntimeSupportLibraryKind;
  std::string archive_basename = kObjc3RuntimeSupportLibraryArchiveBasename;
  std::string archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string probe_source_path = kObjc3RuntimeSupportLibraryProbeSourcePath;
  std::string register_image_symbol =
      kObjc3RuntimeSupportLibraryRegisterImageSymbol;
  std::string lookup_selector_symbol =
      kObjc3RuntimeSupportLibraryLookupSelectorSymbol;
  std::string dispatch_i32_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;
  std::string driver_link_mode = kObjc3RuntimeSupportLibraryDriverLinkMode;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.support_library_contract_id.empty() &&
         !summary.metadata_scaffold_contract_id.empty() &&
         summary.fail_closed &&
         summary.native_runtime_library_sources_present &&
         summary.native_runtime_library_header_present &&
         summary.native_runtime_library_archive_build_enabled &&
         summary.native_runtime_library_entrypoints_implemented &&
         summary.selector_lookup_stateful &&
         summary.deterministic_dispatch_formula_matches_test_shim &&
         summary.reset_for_testing_supported &&
         summary.shim_remains_test_only &&
         summary.driver_link_wiring_pending &&
         summary.ready_for_driver_link_wiring &&
         !summary.cmake_target_name.empty() &&
         !summary.public_header_path.empty() &&
         !summary.source_root.empty() &&
         !summary.implementation_source_path.empty() &&
         !summary.library_kind.empty() &&
         !summary.archive_basename.empty() &&
         !summary.archive_relative_path.empty() &&
         !summary.probe_source_path.empty() &&
         !summary.register_image_symbol.empty() &&
         !summary.lookup_selector_symbol.empty() &&
         !summary.dispatch_i32_symbol.empty() &&
         !summary.reset_for_testing_symbol.empty() &&
         !summary.driver_link_mode.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeSupportLibraryLinkWiringSummary {
  std::string contract_id = kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string support_library_core_feature_contract_id =
      kObjc3RuntimeSupportLibraryCoreFeatureContractId;
  bool fail_closed = false;
  bool runtime_library_archive_available = false;
  bool compatibility_dispatch_alias_exported = false;
  bool driver_emits_runtime_link_contract = false;
  bool execution_smoke_consumes_runtime_library = false;
  bool shim_remains_test_only = false;
  bool ready_for_runtime_library_consumption = false;
  std::string archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string compatibility_dispatch_symbol =
      kObjc3RuntimeSupportLibraryCompatibilityDispatchSymbol;
  std::string runtime_dispatch_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string execution_smoke_script_path =
      kObjc3RuntimeSupportLibraryExecutionSmokeScriptPath;
  std::string driver_link_mode = kObjc3RuntimeSupportLibraryLinkWiringMode;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryLinkWiringSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.support_library_core_feature_contract_id.empty() &&
         summary.fail_closed &&
         summary.runtime_library_archive_available &&
         summary.compatibility_dispatch_alias_exported &&
         summary.driver_emits_runtime_link_contract &&
         summary.execution_smoke_consumes_runtime_library &&
         summary.shim_remains_test_only &&
         summary.ready_for_runtime_library_consumption &&
         !summary.archive_relative_path.empty() &&
         !summary.compatibility_dispatch_symbol.empty() &&
         !summary.runtime_dispatch_symbol.empty() &&
         !summary.execution_smoke_script_path.empty() &&
         !summary.driver_link_mode.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeTranslationUnitRegistrationContractSummary {
  std::string contract_id =
      kObjc3RuntimeTranslationUnitRegistrationContractId;
  std::string binary_boundary_contract_id =
      kObjc3ExecutableMetadataRuntimeIngestBinaryBoundaryContractId;
  std::string archive_static_link_contract_id =
      kObjc3RuntimeArchiveStaticLinkDiscoveryContractId;
  std::string object_emission_closeout_contract_id =
      kObjc3RuntimeMetadataObjectEmissionCloseoutContractId;
  std::string runtime_support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string registration_surface_path =
      kObjc3RuntimeTranslationUnitRegistrationSurfacePath;
  std::string registration_payload_model =
      kObjc3RuntimeTranslationUnitRegistrationPayloadModel;
  std::array<std::string, 3u> runtime_owned_payload_artifacts = {
      kObjc3RuntimeTranslationUnitRegistrationPayloadArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationLinkerResponseArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationDiscoveryArtifactRelativePath};
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string constructor_root_ownership_model =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootOwnershipModel;
  std::string constructor_emission_mode =
      kObjc3RuntimeTranslationUnitRegistrationConstructorEmissionMode;
  std::string constructor_priority_policy =
      kObjc3RuntimeTranslationUnitRegistrationConstructorPriorityPolicy;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool binary_boundary_ready = false;
  bool archive_static_link_surface_ready = false;
  bool object_emission_closeout_surface_ready = false;
  bool runtime_support_library_link_wiring_ready = false;
  bool runtime_owned_payload_inventory_published = false;
  bool constructor_root_reserved_not_emitted = false;
  bool startup_registration_not_yet_landed = false;
  bool runtime_bootstrap_not_yet_landed = false;
  bool explicit_non_goals_published = false;
  bool ready_for_registration_manifest_implementation = false;
  std::size_t runtime_owned_payload_artifact_count = 0;
  std::string binary_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.binary_boundary_contract_id.empty() &&
         !summary.archive_static_link_contract_id.empty() &&
         !summary.object_emission_closeout_contract_id.empty() &&
         !summary.runtime_support_library_link_wiring_contract_id.empty() &&
         !summary.registration_surface_path.empty() &&
         !summary.registration_payload_model.empty() &&
         summary.boundary_frozen &&
         summary.fail_closed &&
         summary.binary_boundary_ready &&
         summary.archive_static_link_surface_ready &&
         summary.object_emission_closeout_surface_ready &&
         summary.runtime_support_library_link_wiring_ready &&
         summary.runtime_owned_payload_inventory_published &&
         summary.constructor_root_reserved_not_emitted &&
         summary.startup_registration_not_yet_landed &&
         summary.runtime_bootstrap_not_yet_landed &&
         summary.explicit_non_goals_published &&
         summary.ready_for_registration_manifest_implementation &&
         summary.runtime_owned_payload_artifact_count ==
             summary.runtime_owned_payload_artifacts.size() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.constructor_root_ownership_model.empty() &&
         !summary.constructor_emission_mode.empty() &&
         !summary.constructor_priority_policy.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.binary_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeTranslationUnitRegistrationManifestSummary {
  std::string contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string translation_unit_registration_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationContractId;
  std::string runtime_support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string manifest_surface_path =
      kObjc3RuntimeTranslationUnitRegistrationManifestSurfacePath;
  std::string manifest_payload_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestPayloadModel;
  std::string manifest_artifact_relative_path =
      kObjc3RuntimeTranslationUnitRegistrationManifestArtifactRelativePath;
  std::array<std::string, 3u> runtime_owned_payload_artifacts = {
      kObjc3RuntimeTranslationUnitRegistrationPayloadArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationLinkerResponseArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationDiscoveryArtifactRelativePath};
  std::string runtime_support_library_archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string constructor_root_ownership_model =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootOwnershipModel;
  std::string manifest_authority_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel;
  std::string constructor_init_stub_symbol_prefix =
      kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix;
  std::string constructor_init_stub_ownership_model =
      kObjc3RuntimeTranslationUnitRegistrationInitStubOwnershipModel;
  std::string constructor_priority_policy =
      kObjc3RuntimeTranslationUnitRegistrationManifestPriorityPolicy;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  bool fail_closed = false;
  bool translation_unit_registration_contract_ready = false;
  bool runtime_support_library_link_wiring_ready = false;
  bool runtime_manifest_template_published = false;
  bool constructor_root_manifest_authoritative = false;
  bool constructor_root_reserved_for_lowering = false;
  bool init_stub_emission_deferred_to_lowering = false;
  bool runtime_registration_artifact_emitted_by_driver = false;
  bool ready_for_lowering_init_stub_emission = false;
  std::size_t runtime_owned_payload_artifact_count = 0;
  std::string translation_unit_registration_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.translation_unit_registration_contract_id.empty() &&
         !summary.runtime_support_library_link_wiring_contract_id.empty() &&
         !summary.manifest_surface_path.empty() &&
         !summary.manifest_payload_model.empty() &&
         !summary.manifest_artifact_relative_path.empty() &&
         summary.fail_closed &&
         summary.translation_unit_registration_contract_ready &&
         summary.runtime_support_library_link_wiring_ready &&
         summary.runtime_manifest_template_published &&
         summary.constructor_root_manifest_authoritative &&
         summary.constructor_root_reserved_for_lowering &&
         summary.init_stub_emission_deferred_to_lowering &&
         summary.runtime_registration_artifact_emitted_by_driver &&
         summary.ready_for_lowering_init_stub_emission &&
         summary.runtime_owned_payload_artifact_count ==
             summary.runtime_owned_payload_artifacts.size() &&
         !summary.runtime_support_library_archive_relative_path.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.constructor_root_ownership_model.empty() &&
         !summary.manifest_authority_model.empty() &&
         !summary.constructor_init_stub_symbol_prefix.empty() &&
         !summary.constructor_init_stub_ownership_model.empty() &&
         !summary.constructor_priority_policy.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.translation_unit_registration_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeStartupBootstrapInvariantSummary {
  std::string contract_id = kObjc3RuntimeStartupBootstrapInvariantContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string bootstrap_surface_path =
      kObjc3RuntimeStartupBootstrapInvariantSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string realization_order_policy =
      kObjc3RuntimeStartupBootstrapRealizationOrderPolicy;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string image_local_initialization_scope =
      kObjc3RuntimeStartupBootstrapImageLocalInitializationScope;
  std::string constructor_root_uniqueness_policy =
      kObjc3RuntimeStartupBootstrapConstructorRootUniquenessPolicy;
  std::string constructor_root_consumption_model =
      kObjc3RuntimeStartupBootstrapConsumptionModel;
  std::string startup_execution_mode =
      kObjc3RuntimeStartupBootstrapExecutionMode;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string manifest_authority_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  bool fail_closed = false;
  bool registration_manifest_contract_ready = false;
  bool duplicate_registration_semantics_frozen = false;
  bool realization_order_semantics_frozen = false;
  bool failure_mode_semantics_frozen = false;
  bool image_local_initialization_scope_frozen = false;
  bool constructor_root_uniqueness_frozen = false;
  bool startup_execution_not_yet_landed = false;
  bool live_duplicate_registration_enforcement_not_yet_landed = false;
  bool image_local_realization_not_yet_landed = false;
  bool ready_for_bootstrap_implementation = false;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.realization_order_policy.empty() &&
         !summary.failure_mode.empty() &&
         !summary.image_local_initialization_scope.empty() &&
         !summary.constructor_root_uniqueness_policy.empty() &&
         !summary.constructor_root_consumption_model.empty() &&
         !summary.startup_execution_mode.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.manifest_authority_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         summary.fail_closed &&
         summary.registration_manifest_contract_ready &&
         summary.duplicate_registration_semantics_frozen &&
         summary.realization_order_semantics_frozen &&
         summary.failure_mode_semantics_frozen &&
         summary.image_local_initialization_scope_frozen &&
         summary.constructor_root_uniqueness_frozen &&
         summary.startup_execution_not_yet_landed &&
         summary.live_duplicate_registration_enforcement_not_yet_landed &&
         summary.image_local_realization_not_yet_landed &&
         summary.ready_for_bootstrap_implementation &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3FrontendPipelineResult {
  Objc3ParsedProgram program;
  Objc3ParserContractSnapshot parser_contract_snapshot;
  Objc3FrontendDiagnosticsBus stage_diagnostics;
  Objc3TypedSemaToLoweringContractSurface typed_sema_to_lowering_contract_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold
      semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface
      semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface
      semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface
      semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface
      semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface
      semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface
      semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface
      semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface
      semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface
      semantic_diagnostic_taxonomy_and_fixit_performance_quality_guardrails_surface;
  Objc3ParseLoweringReadinessSurface parse_lowering_readiness_surface;
  Objc3SemanticStabilitySpecDeltaClosureScaffold semantic_stability_spec_delta_closure_scaffold;
  Objc3SemanticStabilityCoreFeatureImplementationSurface
      semantic_stability_core_feature_implementation_surface;
  Objc3LoweringRuntimeStabilityInvariantScaffold lowering_runtime_stability_invariant_scaffold;
  Objc3LoweringPipelinePassGraphScaffold lowering_pipeline_pass_graph_scaffold;
  Objc3LoweringPipelinePassGraphCoreFeatureSurface lowering_pipeline_pass_graph_core_feature_surface;
  Objc3IREmissionCompletenessScaffold ir_emission_completeness_scaffold;
  Objc3LoweringRuntimeDiagnosticsSurfacingScaffold
      lowering_runtime_diagnostics_surfacing_scaffold;
  Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
      lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
      lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface
      lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface
      lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
      lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
      lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
      lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface;
  Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface
      lowering_runtime_stability_core_feature_implementation_surface;
  Objc3FrontendMigrationHints migration_hints;
  Objc3FrontendLanguageVersionPragmaContract language_version_pragma_contract;
  Objc3SemanticIntegrationSurface integration_surface;
  Objc3SemanticTypeMetadataHandoff sema_type_metadata_handoff;
  Objc3FrontendProtocolCategorySummary protocol_category_summary;
  Objc3FrontendClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3FrontendSelectorNormalizationSummary selector_normalization_summary;
  Objc3FrontendPropertyAttributeSummary property_attribute_summary;
  Objc3FrontendObjectPointerNullabilityGenericsSummary object_pointer_nullability_generics_summary;
  Objc3FrontendSymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3RuntimeMetadataSourceRecordSet runtime_metadata_source_records;
  Objc3ExecutableMetadataSourceGraph executable_metadata_source_graph;
  Objc3ExecutableMetadataSemanticConsistencyBoundary
      executable_metadata_semantic_consistency_boundary;
  Objc3ExecutableMetadataSemanticValidationSurface
      executable_metadata_semantic_validation_surface;
  Objc3ExecutableMetadataLoweringHandoffSurface
      executable_metadata_lowering_handoff_surface;
  Objc3ExecutableMetadataTypedLoweringHandoff
      executable_metadata_typed_lowering_handoff;
  Objc3RuntimeMetadataSourceOwnershipBoundary runtime_metadata_source_ownership_boundary;
  Objc3RuntimeExportLegalityBoundary runtime_export_legality_boundary;
  Objc3RuntimeExportEnforcementSummary runtime_export_enforcement_summary;
  std::array<std::size_t, 3> sema_diagnostics_after_pass = {0, 0, 0};
  Objc3SemaPassFlowSummary sema_pass_flow_summary;
  Objc3SemaParityContractSurface sema_parity_surface;
};

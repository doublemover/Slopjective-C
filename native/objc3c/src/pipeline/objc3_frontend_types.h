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
#include "token/objc3_token_contract.h"

inline constexpr std::uint8_t kObjc3DefaultLanguageVersion = 3u;

enum class Objc3FrontendCompatibilityMode : std::uint8_t {
  kCanonical = 0u,
  kLegacy = 1u,
};

enum class Objc3FrontendArcMode : std::uint8_t {
  kDisabled = 0u,
  kEnabled = 1u,
};

struct Objc3FrontendOptions {
  std::uint8_t language_version = kObjc3DefaultLanguageVersion;
  Objc3FrontendCompatibilityMode compatibility_mode = Objc3FrontendCompatibilityMode::kCanonical;
  Objc3FrontendArcMode arc_mode = Objc3FrontendArcMode::kDisabled;
  bool migration_assist = false;
  bool emit_manifest = true;
  bool emit_ir = true;
  bool emit_object = true;
  std::uint64_t bootstrap_registration_order_ordinal = 1u;
  std::vector<std::string> imported_runtime_surface_paths;
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

struct Objc3FrontendNamedIdentifierPragmaContract {
  bool seen = false;
  std::size_t directive_count = 0;
  bool duplicate = false;
  bool non_leading = false;
  unsigned first_line = 0;
  unsigned first_column = 0;
  unsigned last_line = 0;
  unsigned last_column = 0;
  std::string identifier;
};

struct Objc3FrontendBootstrapRegistrationSourcePragmaContract {
  Objc3FrontendNamedIdentifierPragmaContract registration_descriptor;
  Objc3FrontendNamedIdentifierPragmaContract image_root;
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

inline constexpr const char *kObjc3Part3TypeSourceClosureContractId =
    "objc3c-part3-type-source-closure/m265-a002-v1";
inline constexpr const char *kObjc3Part3TypeSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part3_type_source_closure";
inline constexpr const char *kObjc3Part3TypeSourceClosureSourceModel =
    "protocol-optional-partitions-object-pointer-nullability-generic-suffixes-optional-bindings-optional-sends-optional-member-access-nil-coalescing-and-typed-keypaths-are-live-parser-owned-source-surfaces";
inline constexpr const char *kObjc3Part3TypeSourceClosureFailureModel =
    "typed-keypath-literals-remain-source-sema-surfaces-while-native-lowering-now-emits-stable-descriptor-handles-for-the-validated-single-component-subset";
inline constexpr const char *kObjc3Part3TypeSemanticModelContractId =
    "objc3c-part3-type-semantic-model/m265-b001-v1";
inline constexpr const char *kObjc3Part3TypeSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part3_type_semantic_model";
inline constexpr const char *kObjc3Part3TypeSemanticModelCoreRule =
    "optional-bindings-optional-sends-erased-generic-metadata-and-typed-keypath-shape-obey-one-fail-closed-sema-model-before-lowering";

struct Objc3FrontendPart3TypeSourceClosureSummary {
  std::string contract_id = kObjc3Part3TypeSourceClosureContractId;
  std::string frontend_surface_path = kObjc3Part3TypeSourceClosureSurfacePath;
  std::string source_model = kObjc3Part3TypeSourceClosureSourceModel;
  std::string failure_model = kObjc3Part3TypeSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimProtocolOptionalPartitions,
      kObjc3SourceOnlyFeatureClaimObjectPointerNullabilitySuffixes,
      kObjc3SourceOnlyFeatureClaimPragmaticGenericSuffixes,
      kObjc3SourceOnlyFeatureClaimOptionalBindings,
      kObjc3SourceOnlyFeatureClaimOptionalSends,
      kObjc3SourceOnlyFeatureClaimNilCoalescing,
      kObjc3SourceOnlyFeatureClaimTypedKeyPathLiterals,
  };
  std::vector<std::string> unsupported_claim_ids = {};
  std::size_t protocol_required_method_count = 0;
  std::size_t protocol_optional_method_count = 0;
  std::size_t protocol_required_property_count = 0;
  std::size_t protocol_optional_property_count = 0;
  std::size_t object_pointer_type_spelling_sites = 0;
  std::size_t pointer_declarator_entries = 0;
  std::size_t nullability_suffix_entries = 0;
  std::size_t generic_suffix_entries = 0;
  std::size_t optional_binding_sites = 0;
  std::size_t guard_binding_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t optional_member_access_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  bool protocol_optional_partition_source_supported = false;
  bool object_pointer_nullability_source_supported = false;
  bool pragmatic_generic_suffix_source_supported = false;
  bool optional_binding_source_supported = false;
  bool optional_send_source_supported = false;
  bool nil_coalescing_source_supported = false;
  bool typed_keypath_literal_source_supported = false;
  bool optional_member_access_fail_closed = false;
  bool nil_coalescing_fail_closed = false;
  bool typed_keypath_literal_fail_closed = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3Part5ControlFlowSourceClosureContractId =
    "objc3c-part5-control-flow-source-closure/m266-a002-v1";
inline constexpr const char *kObjc3Part5ControlFlowSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure";
inline constexpr const char *kObjc3Part5ControlFlowSourceClosureSourceModel =
    "guard-condition-lists-defer-statements-and-statement-match-patterns-are-live-frontend-owned-control-flow-surfaces-while-match-expression-guarded-patterns-and-type-test-patterns-remain-fail-closed";
inline constexpr const char *kObjc3Part5ControlFlowSourceClosureFailureModel =
    "match-remains-statement-only-and-guarded-or-type-test-patterns-remain-fail-closed-until-later-m266-sema-lowering-and-runtime-work";

struct Objc3FrontendPart5ControlFlowSourceClosureSummary {
  std::string contract_id = kObjc3Part5ControlFlowSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3Part5ControlFlowSourceClosureSurfacePath;
  std::string source_model = kObjc3Part5ControlFlowSourceClosureSourceModel;
  std::string failure_model = kObjc3Part5ControlFlowSourceClosureFailureModel;
  std::vector<std::string> supported_construct_ids = {
      kObjc3Part5SourceSurfaceGuardBindings,
      kObjc3Part5SourceSurfaceGuardConditionLists,
      kObjc3Part5SourceSurfaceSwitchCasePatterns,
      kObjc3Part5SourceSurfaceDeferStatements,
      kObjc3Part5SourceSurfaceMatchStatement,
      kObjc3Part5SourceSurfaceMatchWildcardPatterns,
      kObjc3Part5SourceSurfaceMatchLiteralPatterns,
      kObjc3Part5SourceSurfaceMatchBindingPatterns,
      kObjc3Part5SourceSurfaceMatchResultCasePatterns,
  };
  std::vector<std::string> fail_closed_construct_ids = {
      kObjc3Part5FailClosedConstructMatchExpression,
      kObjc3Part5FailClosedConstructGuardedPatterns,
      kObjc3Part5FailClosedConstructMatchTypeTestPatterns,
  };
  std::size_t guard_binding_sites = 0;
  std::size_t guard_binding_clause_sites = 0;
  std::size_t guard_boolean_condition_sites = 0;
  std::size_t switch_case_pattern_sites = 0;
  std::size_t switch_default_pattern_sites = 0;
  std::size_t defer_keyword_sites = 0;
  std::size_t match_statement_sites = 0;
  std::size_t match_case_pattern_sites = 0;
  std::size_t match_default_sites = 0;
  std::size_t match_wildcard_pattern_sites = 0;
  std::size_t match_literal_pattern_sites = 0;
  std::size_t match_binding_pattern_sites = 0;
  std::size_t match_result_case_pattern_sites = 0;
  bool guard_binding_source_supported = false;
  bool guard_condition_list_source_supported = false;
  bool switch_case_pattern_source_supported = false;
  bool match_statement_source_supported = false;
  bool match_wildcard_pattern_source_supported = false;
  bool match_literal_pattern_source_supported = false;
  bool match_binding_pattern_source_supported = false;
  bool match_result_case_pattern_source_supported = false;
  bool defer_statement_source_supported = false;
  bool defer_keyword_reserved = false;
  bool defer_fail_closed = false;
  bool match_expression_fail_closed = false;
  bool guarded_pattern_fail_closed = false;
  bool type_test_pattern_fail_closed = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3Part6ErrorSourceClosureContractId =
    "objc3c-part6-error-source-closure/m267-a001-v1";
inline constexpr const char *kObjc3Part6ErrorSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part6_error_source_closure";
inline constexpr const char *kObjc3Part6ErrorSourceClosureSourceModel =
    "throws-declarations-result-carrier-profiles-nserror-bridging-profiles-and-canonical-error-bridge-markers-are-live-frontend-owned-source-surfaces-while-try-throw-and-do-catch-remain-reserved-fail-closed";
inline constexpr const char *kObjc3Part6ErrorSourceClosureFailureModel =
    "try-expressions-throw-statements-and-do-catch-remain-parse-owned-fail-closed-boundaries-until-runnable-part6-sema-lowering-and-runtime-work";

struct Objc3FrontendPart6ErrorSourceClosureSummary {
  std::string contract_id = kObjc3Part6ErrorSourceClosureContractId;
  std::string frontend_surface_path = kObjc3Part6ErrorSourceClosureSurfacePath;
  std::string source_model = kObjc3Part6ErrorSourceClosureSourceModel;
  std::string failure_model = kObjc3Part6ErrorSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimThrowsDeclarations,
      kObjc3SourceOnlyFeatureClaimResultCarrierProfiles,
      kObjc3SourceOnlyFeatureClaimNSErrorBridgingProfiles,
  };
  std::vector<std::string> fail_closed_construct_ids = {
      kObjc3Part6FailClosedConstructTryExpressions,
      kObjc3Part6FailClosedConstructThrowStatements,
      kObjc3Part6FailClosedConstructDoCatchStatements,
  };
  std::size_t function_throws_declaration_sites = 0;
  std::size_t method_throws_declaration_sites = 0;
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t objc_nserror_attribute_sites = 0;
  std::size_t objc_status_code_attribute_sites = 0;
  std::size_t status_code_success_clause_sites = 0;
  std::size_t status_code_error_type_clause_sites = 0;
  std::size_t status_code_mapping_clause_sites = 0;
  std::size_t try_keyword_sites = 0;
  std::size_t throw_keyword_sites = 0;
  std::size_t catch_keyword_sites = 0;
  bool throws_declaration_source_supported = false;
  bool result_carrier_source_supported = false;
  bool ns_error_bridging_source_supported = false;
  bool error_bridge_marker_source_supported = false;
  bool try_keyword_reserved = false;
  bool throw_keyword_reserved = false;
  bool catch_keyword_reserved = false;
  bool try_fail_closed = false;
  bool throw_fail_closed = false;
  bool do_catch_fail_closed = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3Part7AsyncSourceClosureContractId =
    "objc3c-part7-async-source-closure/m268-a002-v1";
inline constexpr const char *kObjc3Part7AsyncSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part7_async_source_closure";
inline constexpr const char *kObjc3Part7AsyncSourceClosureSourceModel =
    "async-entry-await-expression-and-executor-affinity-syntax-are-live-frontend-owned-source-surfaces-while-continuation-lowering-suspension-cleanup-and-runtime-scheduling-remain-later-m268-work";
inline constexpr const char *kObjc3Part7AsyncSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-runnable-continuation-abi-suspension-cleanup-or-executor-runtime-behavior";
inline constexpr const char *kObjc3Part7ActorMemberIsolationSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part7_actor_member_and_isolation_source_closure";
inline constexpr const char *kObjc3Part7ActorMemberIsolationSourceClosureSourceModel =
    "actor-class-declarations-actor-members-and-objc-nonisolated-annotations-are-live-frontend-owned-source-surfaces-while-actor-legality-diagnostics-and-runnable-actor-runtime-behavior-remain-later-m270-work";
inline constexpr const char *kObjc3Part7ActorMemberIsolationSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-actor-member-legality-diagnostics-cross-actor-enforcement-or-runnable-actor-runtime-behavior";

struct Objc3FrontendPart7AsyncSourceClosureSummary {
  std::string contract_id = kObjc3Part7AsyncSourceClosureContractId;
  std::string frontend_surface_path = kObjc3Part7AsyncSourceClosureSurfacePath;
  std::string source_model = kObjc3Part7AsyncSourceClosureSourceModel;
  std::string failure_model = kObjc3Part7AsyncSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimAsyncDeclarations,
      kObjc3SourceOnlyFeatureClaimAwaitExpressions,
      kObjc3SourceOnlyFeatureClaimExecutorAffinityAttributes,
  };
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t async_method_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_expression_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t executor_main_sites = 0;
  std::size_t executor_global_sites = 0;
  std::size_t executor_named_sites = 0;
  bool async_function_source_supported = false;
  bool async_method_source_supported = false;
  bool await_expression_source_supported = false;
  bool executor_attribute_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart7ActorMemberIsolationSourceClosureSummary {
  std::string contract_id = kObjc3ActorMemberIsolationSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3Part7ActorMemberIsolationSourceClosureSurfacePath;
  std::string source_model =
      kObjc3Part7ActorMemberIsolationSourceClosureSourceModel;
  std::string failure_model =
      kObjc3Part7ActorMemberIsolationSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimActorDeclarationMarkers,
      kObjc3SourceOnlyFeatureClaimActorMemberSurfaces,
      kObjc3SourceOnlyFeatureClaimIsolationAnnotationMarkers,
      kObjc3SourceOnlyFeatureClaimActorMetadataSurfaces,
  };
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_property_sites = 0;
  std::size_t objc_nonisolated_annotation_sites = 0;
  std::size_t actor_member_executor_annotation_sites = 0;
  std::size_t actor_async_method_sites = 0;
  std::size_t actor_member_metadata_sites = 0;
  bool actor_declaration_source_supported = false;
  bool actor_member_source_supported = false;
  bool isolation_annotation_source_supported = false;
  bool actor_metadata_surface_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3Part7TaskGroupCancellationSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part7_task_group_and_cancellation_source_closure";
inline constexpr const char *kObjc3Part7TaskGroupCancellationSourceClosureSourceModel =
    "task-creation-task-group-and-cancellation-call-surfaces-are-live-frontend-owned-source-surfaces-while-runnable-task-allocation-executor-hops-and-scheduler-execution-remain-later-m269-work";
inline constexpr const char *kObjc3Part7TaskGroupCancellationSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-runnable-task-allocation-task-group-execution-or-scheduler-backed-cancellation-runtime-behavior";

inline constexpr const char *kObjc3Part8SystemExtensionSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part8_resource_borrowed_and_capture_list_source_closure";
inline constexpr const char *kObjc3Part8SystemExtensionSourceClosureSourceModel =
    "resource-let-attributes-borrowed-pointer-qualifiers-borrowed-return-relations-and-explicit-block-capture-lists-are-live-frontend-owned-source-surfaces-while-legality-diagnostics-cleanup-lowering-and-runtime-lifetime-enforcement-remain-later-m271-work";
inline constexpr const char *kObjc3Part8SystemExtensionSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-resource-cleanup-lowering-borrowed-escape-enforcement-or-runnable-capture-ownership-runtime-behavior";
inline constexpr const char *kObjc3Part8CleanupResourceCaptureSourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part8_cleanup_resource_and_capture_source_completion";
inline constexpr const char *kObjc3Part8CleanupResourceCaptureSourceCompletionSourceModel =
    "cleanup-hooks-resource-sugar-and-explicit-block-capture-surface-forms-are-live-frontend-owned-source-surfaces-while-legality-diagnostics-cleanup-lowering-and-runtime-resource-ownership-remain-later-m271-work";
inline constexpr const char *kObjc3Part8CleanupResourceCaptureSourceCompletionFailureModel =
    "frontend-source-completion-does-not-yet-claim-cleanup-lowering-resource-runtime-behavior-or-borrowed-pointer-semantic-enforcement";
inline constexpr const char *kObjc3Part8RetainableCFamilySourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part8_retainable_c_family_source_completion";
inline constexpr const char *kObjc3Part8RetainableCFamilySourceCompletionSourceModel =
    "retainable-c-family-callable-annotations-and-compatibility-aliases-are-live-frontend-owned-source-surfaces-while-family-legality-arc-integration-and-runtime-interop-remain-later-m271-work";
inline constexpr const char *kObjc3Part8RetainableCFamilySourceCompletionFailureModel =
    "frontend-source-completion-does-not-yet-claim-retainable-c-family-legality-arc-interop-or-runnable-family-runtime-behavior";
inline constexpr const char *kObjc3Part9DispatchIntentSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part9_dispatch_intent_and_dynamism_source_closure";
inline constexpr const char *kObjc3Part9DispatchIntentSourceClosureSourceModel =
    "direct-final-sealed-and-dynamism-control-attributes-are-live-frontend-owned-source-surfaces-while-legality-lowering-metadata-and-runtime-dispatch-realization-remain-later-m272-work";
inline constexpr const char *kObjc3Part9DispatchIntentSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-direct-dispatch-legality-final-sealed-enforcement-or-runnable-dispatch-boundary-behavior";
inline constexpr const char *kObjc3Part9DispatchIntentSourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part9_dispatch_intent_attribute_and_defaulting_source_completion";
inline constexpr const char *kObjc3Part9DispatchIntentSourceCompletionSourceModel =
    "prefixed-container-attributes-direct-members-defaulting-and-dynamic-opt-out-surfaces-are-live-frontend-owned-source-surfaces-while-part9-legality-lowering-metadata-and-runtime-dispatch-realization-remain-later-m272-work";
inline constexpr const char *kObjc3Part9DispatchIntentSourceCompletionFailureModel =
    "frontend-source-completion-does-not-yet-claim-final-sealed-legality-enforcement-direct-call-lowering-or-runnable-dispatch-boundary-behavior";
inline constexpr const char *kObjc3Part10MetaprogrammingSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part10_derive_macro_property_behavior_source_closure";
inline constexpr const char *kObjc3Part10MetaprogrammingSourceClosureSourceModel =
    "derive-markers-macro-markers-and-property-behavior-markers-are-live-parser-owned-source-surfaces-while-expansion-synthesis-and-runtime-behavior-remain-deferred";
inline constexpr const char *kObjc3Part10MetaprogrammingSourceClosureFailureModel =
    "part10-stays-source-closure-only-with-no-macro-expansion-derived-conformance-synthesis-or-property-runtime-claims-yet";
inline constexpr const char *kObjc3Part10MacroPackageProvenanceSourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part10_macro_package_and_provenance_source_completion";
inline constexpr const char *kObjc3Part10MacroPackageProvenanceSourceCompletionSourceModel =
    "macro-package-markers-macro-provenance-markers-and-expansion-visible-source-state-are-live-frontend-owned-surfaces-while-expansion-execution-and-sandboxing-remain-deferred";
inline constexpr const char *kObjc3Part10MacroPackageProvenanceSourceCompletionFailureModel =
    "part10-source-completion-does-not-yet-claim-macro-expansion-engine-sandbox-or-runtime-package-loading";
inline constexpr const char *kObjc3Part10PropertyBehaviorSourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part10_property_behavior_and_synthesized_declaration_source_completion";
inline constexpr const char *kObjc3Part10PropertyBehaviorSourceCompletionSourceModel =
    "behavior-bearing-properties-and-their-deterministic-synthesized-binding-and-accessor-visibility-source-state-are-live-frontend-owned-surfaces-while-real-property-behavior-expansion-and-runtime-hooks-remain-deferred";
inline constexpr const char *kObjc3Part10PropertyBehaviorSourceCompletionFailureModel =
    "part10-source-completion-does-not-yet-claim-property-behavior-expansion-runtime-hooks-or-executable-synthesized-declaration-materialization";
inline constexpr const char *kObjc3Part11ForeignImportSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part11_foreign_declaration_and_import_source_closure";
inline constexpr const char *kObjc3Part11ForeignImportSourceClosureSourceModel =
    "foreign-callable-markers-import-module-annotations-and-base-interop-annotation-surfaces-are-live-frontend-owned-source-surfaces-while-cpp-swift-annotation-completion-interface-preservation-lowering-and-runtime-bridge-generation-remain-later-m274-work";
inline constexpr const char *kObjc3Part11ForeignImportSourceClosureFailureModel =
    "part11-source-closure-does-not-yet-claim-cpp-swift-specific-annotation-completion-interface-emission-ffi-lowering-or-runnable-bridge-generation";

struct Objc3FrontendPart8SystemExtensionSourceClosureSummary {
  std::string contract_id = kObjc3Part8SystemExtensionSourceClosureContractId;
  std::string frontend_surface_path = kObjc3Part8SystemExtensionSourceClosureSurfacePath;
  std::string source_model = kObjc3Part8SystemExtensionSourceClosureSourceModel;
  std::string failure_model = kObjc3Part8SystemExtensionSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimResourceHandleAnnotations,
      kObjc3SourceOnlyFeatureClaimBorrowedPointerAnnotations,
      kObjc3SourceOnlyFeatureClaimBorrowedReturnRelations,
      kObjc3SourceOnlyFeatureClaimExplicitBlockCaptureLists,
  };
  std::size_t resource_attribute_sites = 0;
  std::size_t resource_close_clause_sites = 0;
  std::size_t resource_invalid_clause_sites = 0;
  std::size_t borrowed_pointer_sites = 0;
  std::size_t returns_borrowed_attribute_sites = 0;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_weak_sites = 0;
  std::size_t explicit_capture_unowned_sites = 0;
  std::size_t explicit_capture_move_sites = 0;
  std::size_t explicit_capture_plain_sites = 0;
  bool resource_attribute_source_supported = false;
  bool borrowed_pointer_source_supported = false;
  bool returns_borrowed_source_supported = false;
  bool explicit_capture_list_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary {
  std::string contract_id =
      kObjc3Part8CleanupResourceCaptureSurfaceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3Part8CleanupResourceCaptureSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3Part8CleanupResourceCaptureSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3Part8CleanupResourceCaptureSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimCleanupHookAnnotations,
      kObjc3SourceOnlyFeatureClaimResourceHandleAnnotations,
      kObjc3SourceOnlyFeatureClaimCleanupResourceSugar,
      kObjc3SourceOnlyFeatureClaimExplicitBlockCaptureLists,
  };
  std::size_t cleanup_attribute_sites = 0;
  std::size_t cleanup_sugar_sites = 0;
  std::size_t resource_attribute_sites = 0;
  std::size_t resource_sugar_sites = 0;
  std::size_t resource_close_clause_sites = 0;
  std::size_t resource_invalid_clause_sites = 0;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_weak_sites = 0;
  std::size_t explicit_capture_unowned_sites = 0;
  std::size_t explicit_capture_move_sites = 0;
  std::size_t explicit_capture_plain_sites = 0;
  bool cleanup_attribute_source_supported = false;
  bool resource_sugar_source_supported = false;
  bool explicit_capture_list_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart8RetainableCFamilySourceCompletionSummary {
  std::string contract_id =
      kObjc3Part8RetainableCFamilySourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3Part8RetainableCFamilySourceCompletionSurfacePath;
  std::string source_model =
      kObjc3Part8RetainableCFamilySourceCompletionSourceModel;
  std::string failure_model =
      kObjc3Part8RetainableCFamilySourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimRetainableCFamilyCallableAnnotations,
      kObjc3SourceOnlyFeatureClaimRetainableCFamilyCompatibilityAliases,
  };
  std::size_t family_retain_sites = 0;
  std::size_t family_release_sites = 0;
  std::size_t family_autorelease_sites = 0;
  std::size_t compatibility_returns_retained_sites = 0;
  std::size_t compatibility_returns_not_retained_sites = 0;
  std::size_t compatibility_consumed_sites = 0;
  bool callable_annotation_source_supported = false;
  bool compatibility_alias_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart9DispatchIntentSourceClosureSummary {
  std::string contract_id = kObjc3Part9DispatchIntentSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3Part9DispatchIntentSourceClosureSurfacePath;
  std::string source_model = kObjc3Part9DispatchIntentSourceClosureSourceModel;
  std::string failure_model = kObjc3Part9DispatchIntentSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimDirectMethodAnnotations,
      kObjc3SourceOnlyFeatureClaimDirectMembersClassAnnotations,
      kObjc3SourceOnlyFeatureClaimFinalAnnotations,
      kObjc3SourceOnlyFeatureClaimSealedAnnotations,
      kObjc3SourceOnlyFeatureClaimDynamicMethodAnnotations,
  };
  std::size_t direct_callable_sites = 0;
  std::size_t final_callable_sites = 0;
  std::size_t dynamic_callable_sites = 0;
  std::size_t direct_members_container_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t actor_container_sites = 0;
  bool callable_annotation_source_supported = false;
  bool container_annotation_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart9DispatchIntentSourceCompletionSummary {
  std::string contract_id = kObjc3Part9DispatchIntentSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3Part9DispatchIntentSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3Part9DispatchIntentSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3Part9DispatchIntentSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimPrefixedDispatchIntentAttributes,
      kObjc3SourceOnlyFeatureClaimDirectMembersDefaultingSurfaces,
      kObjc3SourceOnlyFeatureClaimDynamicOptOutDefaultingSurfaces,
      kObjc3SourceOnlyFeatureClaimFinalAnnotations,
      kObjc3SourceOnlyFeatureClaimSealedAnnotations,
  };
  std::size_t prefixed_container_attribute_sites = 0;
  std::size_t direct_members_container_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t effective_direct_member_sites = 0;
  std::size_t direct_members_defaulted_method_sites = 0;
  std::size_t direct_members_dynamic_opt_out_sites = 0;
  bool prefixed_attribute_source_supported = false;
  bool defaulting_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart10MetaprogrammingSourceClosureSummary {
  std::string contract_id = kObjc3Part10MetaprogrammingSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3Part10MetaprogrammingSourceClosureSurfacePath;
  std::string source_model =
      kObjc3Part10MetaprogrammingSourceClosureSourceModel;
  std::string failure_model =
      kObjc3Part10MetaprogrammingSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimDeriveMarkers,
      kObjc3SourceOnlyFeatureClaimMacroMarkers,
      kObjc3SourceOnlyFeatureClaimPropertyBehaviorMarkers,
  };
  std::size_t derive_marker_sites = 0;
  std::size_t macro_marker_sites = 0;
  std::size_t property_behavior_sites = 0;
  bool derive_marker_source_supported = false;
  bool macro_marker_source_supported = false;
  bool property_behavior_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart10MacroPackageProvenanceSourceCompletionSummary {
  std::string contract_id =
      kObjc3Part10MacroPackageProvenanceSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3Part10MacroPackageProvenanceSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3Part10MacroPackageProvenanceSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3Part10MacroPackageProvenanceSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimMacroMarkers,
      kObjc3SourceOnlyFeatureClaimMacroPackageMarkers,
      kObjc3SourceOnlyFeatureClaimMacroProvenanceMarkers,
      kObjc3SourceOnlyFeatureClaimMacroExpansionVisibleState,
  };
  std::size_t macro_marker_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t expansion_visible_macro_sites = 0;
  bool macro_package_source_supported = false;
  bool macro_provenance_source_supported = false;
  bool expansion_visible_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart10PropertyBehaviorSourceCompletionSummary {
  std::string contract_id =
      kObjc3Part10PropertyBehaviorSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3Part10PropertyBehaviorSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3Part10PropertyBehaviorSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3Part10PropertyBehaviorSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimPropertyBehaviorMarkers,
      kObjc3SourceOnlyFeatureClaimPropertyBehaviorSynthesisVisibility,
  };
  std::size_t property_behavior_sites = 0;
  std::size_t interface_property_behavior_sites = 0;
  std::size_t implementation_property_behavior_sites = 0;
  std::size_t protocol_property_behavior_sites = 0;
  std::size_t synthesized_binding_visible_sites = 0;
  std::size_t synthesized_getter_visible_sites = 0;
  std::size_t synthesized_setter_visible_sites = 0;
  bool property_behavior_source_supported = false;
  bool synthesized_declaration_visibility_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart11ForeignImportSourceClosureSummary {
  std::string contract_id = kObjc3Part11ForeignImportSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3Part11ForeignImportSourceClosureSurfacePath;
  std::string source_model =
      kObjc3Part11ForeignImportSourceClosureSourceModel;
  std::string failure_model =
      kObjc3Part11ForeignImportSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimForeignDeclarationMarkers,
      kObjc3SourceOnlyFeatureClaimImportedModuleAnnotations,
      kObjc3SourceOnlyFeatureClaimInteropAnnotationMarkers,
  };
  std::size_t foreign_callable_sites = 0;
  std::size_t extern_foreign_callable_sites = 0;
  std::size_t import_module_annotation_sites = 0;
  std::size_t imported_module_name_sites = 0;
  std::size_t interop_annotation_sites = 0;
  bool foreign_declaration_source_supported = false;
  bool imported_surface_source_supported = false;
  bool interop_annotation_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary {
  std::string contract_id = kObjc3Part7TaskGroupCancellationSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3Part7TaskGroupCancellationSourceClosureSurfacePath;
  std::string source_model = kObjc3Part7TaskGroupCancellationSourceClosureSourceModel;
  std::string failure_model = kObjc3Part7TaskGroupCancellationSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimTaskCreationSites,
      kObjc3SourceOnlyFeatureClaimSupportedTaskGroupSurface,
      kObjc3SourceOnlyFeatureClaimTaskExecutorCancellationProfiles,
  };
  std::size_t async_callable_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  bool task_creation_source_supported = false;
  bool task_group_source_supported = false;
  bool cancellation_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
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
  std::vector<std::string> adopted_protocols_lexicographic;
  bool has_super = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
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
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataMethodSourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string selector;
  bool is_class_method = false;
  bool has_body = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
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
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
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

inline constexpr const char
    *kObjc3RuntimeAwareImportModuleSurfaceContractId =
        "objc3c-runtime-aware-import-module-surface/m258-a001-v1";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureContractId =
        "objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_aware_import_module_frontend_closure";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosurePayloadModel =
        "runtime-aware-import-module-surface-json-v1";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureArtifactSuffix =
        ".runtime-import-surface.json";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath =
        "module.runtime-import-surface.json";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendAuthorityModel =
        "runtime-import-surface-artifact-derived-from-frozen-import-surface-and-runtime-metadata-source-records";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendPayloadOwnershipModel =
        "compiler-emits-runtime-import-surface-artifact-frontend-and-later-module-consumers-own-cross-translation-unit-handoff";

struct Objc3RuntimeAwareImportModuleFrontendClosureSummary {
  std::string contract_id =
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId;
  std::string source_surface_contract_id =
      kObjc3RuntimeAwareImportModuleSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureSurfacePath;
  std::string payload_model =
      kObjc3RuntimeAwareImportModuleFrontendClosurePayloadModel;
  std::string artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3RuntimeAwareImportModuleFrontendAuthorityModel;
  std::string payload_ownership_model =
      kObjc3RuntimeAwareImportModuleFrontendPayloadOwnershipModel;
  std::string module_name;
  std::size_t protocol_decl_count = 0;
  std::size_t interface_decl_count = 0;
  std::size_t implementation_decl_count = 0;
  std::size_t interface_category_decl_count = 0;
  std::size_t implementation_category_decl_count = 0;
  std::size_t function_decl_count = 0;
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t superclass_reference_count = 0;
  std::size_t protocol_reference_count = 0;
  std::size_t property_accessor_reference_count = 0;
  std::size_t property_ivar_binding_reference_count = 0;
  std::size_t method_selector_reference_count = 0;
  std::size_t metadata_reference_count = 0;
  bool fail_closed = false;
  bool source_surface_contract_ready = false;
  bool runtime_metadata_source_records_ready = false;
  bool frontend_surface_published = false;
  bool import_artifact_template_published = false;
  bool runtime_aware_import_declarations_landed = false;
  bool module_metadata_import_surface_landed = false;
  bool runtime_owned_declaration_import_landed = false;
  bool runtime_metadata_reference_import_landed = false;
  bool public_frontend_api_module_surface_landed = false;
  bool ready_for_import_artifact_emission = false;
  bool ready_for_frontend_module_consumption = false;
  std::string source_surface_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.artifact_relative_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.payload_ownership_model.empty() &&
         !summary.module_name.empty() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.metadata_reference_count ==
             summary.superclass_reference_count +
                 summary.protocol_reference_count +
                 summary.property_accessor_reference_count +
                 summary.property_ivar_binding_reference_count +
                 summary.method_selector_reference_count &&
         summary.fail_closed && summary.source_surface_contract_ready &&
         summary.runtime_metadata_source_records_ready &&
         summary.frontend_surface_published &&
         summary.import_artifact_template_published &&
         summary.runtime_aware_import_declarations_landed &&
         summary.module_metadata_import_surface_landed &&
         summary.runtime_owned_declaration_import_landed &&
         summary.runtime_metadata_reference_import_landed &&
         summary.public_frontend_api_module_surface_landed &&
         summary.ready_for_import_artifact_emission &&
         summary.ready_for_frontend_module_consumption &&
         !summary.source_surface_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationContractId =
        "objc3c-cross-module-runtime-metadata-semantic-preservation/m258-b001-v1";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_cross_module_runtime_metadata_semantic_preservation_contract";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationAuthorityModel =
        "semantic-preservation-freeze-derived-from-runtime-import-surface-and-runtime-metadata-source-records";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationConformanceShapeModel =
        "superclass-protocol-and-category-attachment-shape";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationDispatchTraitModel =
        "selector-classness-accessor-ivar-binding-and-body-availability";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationEffectTraitModel =
        "property-attribute-and-ownership-effect-profiles";

struct Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary {
  std::string contract_id =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationContractId;
  std::string source_frontend_closure_contract_id =
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId;
  std::string frontend_surface_path =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationSurfacePath;
  std::string source_artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationAuthorityModel;
  std::string conformance_shape_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationConformanceShapeModel;
  std::string dispatch_trait_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationDispatchTraitModel;
  std::string effect_trait_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationEffectTraitModel;
  std::string module_name;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t superclass_edge_count = 0;
  std::size_t protocol_conformance_edge_count = 0;
  std::size_t category_attachment_count = 0;
  std::size_t property_accessor_trait_count = 0;
  std::size_t property_ivar_binding_trait_count = 0;
  std::size_t method_selector_trait_count = 0;
  std::size_t class_method_trait_count = 0;
  std::size_t instance_method_trait_count = 0;
  std::size_t implemented_method_count = 0;
  std::size_t declaration_only_method_count = 0;
  std::size_t property_attribute_profile_count = 0;
  std::size_t ownership_effect_profile_count = 0;
  std::size_t executable_binding_trait_count = 0;
  std::size_t optional_send_site_count = 0;
  std::size_t typed_keypath_literal_site_count = 0;
  std::size_t live_optional_lowering_site_count = 0;
  std::size_t live_typed_keypath_artifact_site_count = 0;
  std::size_t imported_part3_optional_keypath_module_count = 0;
  std::size_t imported_optional_runtime_ready_module_count = 0;
  std::size_t imported_typed_keypath_runtime_ready_module_count = 0;
  bool fail_closed = false;
  bool source_frontend_closure_ready = false;
  bool runtime_metadata_source_records_ready = false;
  bool semantic_surface_published = false;
  bool imported_conformance_shape_landed = false;
  bool imported_dispatch_traits_landed = false;
  bool imported_effect_traits_landed = false;
  bool imported_runtime_metadata_semantics_landed = false;
  bool imported_part3_type_surface_landed = false;
  bool imported_optional_runtime_semantics_landed = false;
  bool imported_typed_keypath_runtime_semantics_landed = false;
  bool ready_for_imported_metadata_semantic_rules = false;
  bool ready_for_cross_module_dispatch_equivalence = false;
  std::string source_frontend_closure_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_frontend_closure_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.source_artifact_relative_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.conformance_shape_model.empty() &&
         !summary.dispatch_trait_model.empty() &&
         !summary.effect_trait_model.empty() && !summary.module_name.empty() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.method_record_count ==
             summary.class_method_trait_count +
                 summary.instance_method_trait_count &&
         summary.method_record_count ==
             summary.implemented_method_count +
                 summary.declaration_only_method_count &&
         summary.fail_closed && summary.source_frontend_closure_ready &&
         summary.runtime_metadata_source_records_ready &&
         summary.semantic_surface_published &&
         !summary.imported_conformance_shape_landed &&
         !summary.imported_dispatch_traits_landed &&
         !summary.imported_effect_traits_landed &&
         !summary.imported_runtime_metadata_semantics_landed &&
         !summary.ready_for_imported_metadata_semantic_rules &&
         !summary.ready_for_cross_module_dispatch_equivalence &&
         !summary.source_frontend_closure_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesContractId =
    "objc3c-imported-runtime-metadata-semantic-rules/m258-b002-v1";
inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_imported_runtime_metadata_semantic_rules";
inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesAuthorityModel =
    "import-surface-artifact-consumption-derived-semantic-rules";
inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesInputModel =
    "filesystem-runtime-import-surface-artifact-path-list";

struct Objc3ImportedRuntimeMetadataSemanticRulesSummary {
  std::string contract_id =
      kObjc3ImportedRuntimeMetadataSemanticRulesContractId;
  std::string source_semantic_preservation_contract_id =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationContractId;
  std::string frontend_surface_path =
      kObjc3ImportedRuntimeMetadataSemanticRulesSurfacePath;
  std::string authority_model =
      kObjc3ImportedRuntimeMetadataSemanticRulesAuthorityModel;
  std::string input_model = kObjc3ImportedRuntimeMetadataSemanticRulesInputModel;
  std::vector<std::string> imported_module_names_lexicographic;
  std::size_t imported_input_path_count = 0;
  std::size_t imported_module_count = 0;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t superclass_edge_count = 0;
  std::size_t protocol_conformance_edge_count = 0;
  std::size_t category_attachment_count = 0;
  std::size_t property_accessor_trait_count = 0;
  std::size_t property_ivar_binding_trait_count = 0;
  std::size_t method_selector_trait_count = 0;
  std::size_t class_method_trait_count = 0;
  std::size_t instance_method_trait_count = 0;
  std::size_t implemented_method_count = 0;
  std::size_t declaration_only_method_count = 0;
  std::size_t property_attribute_profile_count = 0;
  std::size_t ownership_effect_profile_count = 0;
  std::size_t executable_binding_trait_count = 0;
  std::size_t optional_send_site_count = 0;
  std::size_t typed_keypath_literal_site_count = 0;
  std::size_t live_optional_lowering_site_count = 0;
  std::size_t live_typed_keypath_artifact_site_count = 0;
  std::size_t imported_part3_optional_keypath_module_count = 0;
  std::size_t imported_optional_runtime_ready_module_count = 0;
  std::size_t imported_typed_keypath_runtime_ready_module_count = 0;
  bool fail_closed = false;
  bool source_semantic_preservation_contract_ready = false;
  bool semantic_surface_published = false;
  bool imported_runtime_surface_inputs_present = false;
  bool imported_runtime_surface_inputs_loaded = false;
  bool imported_conformance_shape_landed = false;
  bool imported_dispatch_traits_landed = false;
  bool imported_effect_traits_landed = false;
  bool imported_runtime_metadata_semantics_landed = false;
  bool imported_part3_type_surface_landed = false;
  bool imported_optional_runtime_semantics_landed = false;
  bool imported_typed_keypath_runtime_semantics_landed = false;
  bool ready_for_imported_metadata_semantic_rules = false;
  bool ready_for_cross_module_dispatch_equivalence = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_semantic_preservation_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         summary.imported_module_count ==
             summary.imported_module_names_lexicographic.size() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.method_record_count ==
             summary.class_method_trait_count +
                 summary.instance_method_trait_count &&
         summary.method_record_count ==
             summary.implemented_method_count +
                 summary.declaration_only_method_count &&
         summary.imported_input_path_count >= summary.imported_module_count &&
         summary.fail_closed &&
         summary.source_semantic_preservation_contract_ready &&
         summary.semantic_surface_published &&
         summary.imported_runtime_surface_inputs_loaded &&
         summary.imported_conformance_shape_landed &&
         summary.imported_dispatch_traits_landed &&
         summary.imported_effect_traits_landed &&
         summary.imported_runtime_metadata_semantics_landed &&
         summary.ready_for_imported_metadata_semantic_rules &&
         summary.ready_for_cross_module_dispatch_equivalence &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringContractId =
    "objc3c-serialized-runtime-metadata-import-lowering/m258-c001-v1";
inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_serialized_runtime_metadata_import_lowering_contract";
inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringAuthorityModel =
    "serialized-metadata-import-lowering-freeze-derived-from-imported-runtime-semantic-rules";
inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringInputModel =
    "filesystem-runtime-import-surface-artifact-path-list";

struct Objc3SerializedRuntimeMetadataImportLoweringSummary {
  std::string contract_id =
      kObjc3SerializedRuntimeMetadataImportLoweringContractId;
  std::string source_imported_semantic_rules_contract_id =
      kObjc3ImportedRuntimeMetadataSemanticRulesContractId;
  std::string frontend_surface_path =
      kObjc3SerializedRuntimeMetadataImportLoweringSurfacePath;
  std::string source_artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3SerializedRuntimeMetadataImportLoweringAuthorityModel;
  std::string input_model =
      kObjc3SerializedRuntimeMetadataImportLoweringInputModel;
  std::size_t imported_input_path_count = 0;
  std::size_t imported_module_count = 0;
  bool fail_closed = false;
  bool source_imported_semantic_rules_ready = false;
  bool semantic_surface_published = false;
  bool imported_surface_ingest_landed = false;
  bool serialized_metadata_rehydration_landed = false;
  bool incremental_reuse_landed = false;
  bool imported_metadata_ir_lowering_landed = false;
  bool public_live_imported_payload_abi_landed = false;
  bool ready_for_serialized_metadata_lowering_impl = false;
  bool ready_for_incremental_reuse_impl = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_imported_semantic_rules_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.source_artifact_relative_path.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         summary.imported_input_path_count >= summary.imported_module_count &&
         summary.fail_closed &&
         summary.source_imported_semantic_rules_ready &&
         summary.semantic_surface_published &&
         summary.imported_surface_ingest_landed &&
         !summary.serialized_metadata_rehydration_landed &&
         !summary.incremental_reuse_landed &&
         !summary.imported_metadata_ir_lowering_landed &&
         !summary.public_live_imported_payload_abi_landed &&
         !summary.ready_for_serialized_metadata_lowering_impl &&
         !summary.ready_for_incremental_reuse_impl &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseContractId =
    "objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_serialized_runtime_metadata_artifact_reuse";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName =
    "serialized_runtime_metadata_reuse_payload";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseAuthorityModel =
    "runtime-import-surface-nested-serialized-runtime-metadata-payload";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseInputModel =
    "filesystem-runtime-import-surface-artifact-path-list";

struct Objc3SerializedRuntimeMetadataArtifactReuseSummary {
  std::string contract_id =
      kObjc3SerializedRuntimeMetadataArtifactReuseContractId;
  std::string source_serialized_import_lowering_contract_id =
      kObjc3SerializedRuntimeMetadataImportLoweringContractId;
  std::string frontend_surface_path =
      kObjc3SerializedRuntimeMetadataArtifactReuseSurfacePath;
  std::string artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string payload_member_name =
      kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName;
  std::string authority_model =
      kObjc3SerializedRuntimeMetadataArtifactReuseAuthorityModel;
  std::string input_model =
      kObjc3SerializedRuntimeMetadataArtifactReuseInputModel;
  std::vector<std::string> reused_module_names_lexicographic;
  std::size_t reused_module_count = 0;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t metadata_reference_count = 0;
  bool fail_closed = false;
  bool source_serialized_import_lowering_ready = false;
  bool semantic_surface_published = false;
  bool serialized_metadata_rehydration_landed = false;
  bool artifact_reuse_landed = false;
  bool downstream_module_consumption_ready = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_serialized_import_lowering_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.artifact_relative_path.empty() &&
         !summary.payload_member_name.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         summary.reused_module_count ==
             summary.reused_module_names_lexicographic.size() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.fail_closed &&
         summary.source_serialized_import_lowering_ready &&
         summary.semantic_surface_published &&
         summary.serialized_metadata_rehydration_landed &&
         summary.artifact_reuse_landed &&
         summary.downstream_module_consumption_ready &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationContractId =
        "objc3c-cross-module-build-runtime-orchestration/m258-d001-v1";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_cross_module_build_runtime_orchestration_contract";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationAuthorityModel =
        "serialized-runtime-import-surface-reuse-payload-plus-local-registration-manifest";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationInputModel =
        "filesystem-runtime-import-surface-artifact-path-list-plus-local-registration-manifest";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationRegistrationScopeModel =
        "translation-unit-registration-manifests-remain-image-local-until-cross-module-registration-aggregation-lands";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationPackagingModel =
        "no-cross-module-link-plan-artifact-or-imported-registration-manifest-ingest-during-freeze";

struct Objc3CrossModuleBuildRuntimeOrchestrationSummary {
  std::string contract_id =
      kObjc3CrossModuleBuildRuntimeOrchestrationContractId;
  std::string source_serialized_runtime_metadata_artifact_reuse_contract_id =
      kObjc3SerializedRuntimeMetadataArtifactReuseContractId;
  std::string source_local_registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string source_imported_runtime_metadata_semantic_rules_contract_id =
      kObjc3ImportedRuntimeMetadataSemanticRulesContractId;
  std::string frontend_surface_path =
      kObjc3CrossModuleBuildRuntimeOrchestrationSurfacePath;
  std::string import_artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string local_registration_manifest_artifact_relative_path =
      kObjc3RuntimeTranslationUnitRegistrationManifestArtifactRelativePath;
  std::string authority_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationAuthorityModel;
  std::string input_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationInputModel;
  std::string registration_scope_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationRegistrationScopeModel;
  std::string packaging_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationPackagingModel;
  std::vector<std::string> module_names_lexicographic;
  std::size_t module_image_count = 0;
  std::size_t direct_import_input_count = 0;
  std::size_t local_class_descriptor_count = 0;
  std::size_t local_protocol_descriptor_count = 0;
  std::size_t local_category_descriptor_count = 0;
  std::size_t local_property_descriptor_count = 0;
  std::size_t local_ivar_descriptor_count = 0;
  std::size_t local_total_descriptor_count = 0;
  std::size_t transitive_runtime_owned_declaration_count = 0;
  std::size_t transitive_metadata_reference_count = 0;
  std::size_t imported_optional_send_site_count = 0;
  std::size_t imported_typed_keypath_literal_site_count = 0;
  std::size_t imported_live_optional_lowering_site_count = 0;
  std::size_t imported_live_typed_keypath_artifact_site_count = 0;
  bool fail_closed = false;
  bool source_serialized_runtime_metadata_artifact_reuse_ready = false;
  bool source_local_registration_manifest_ready = false;
  bool source_imported_runtime_metadata_semantic_rules_ready = false;
  bool semantic_surface_published = false;
  bool local_registration_manifest_emitted = false;
  bool imported_part3_type_surface_landed = false;
  bool imported_optional_runtime_semantics_landed = false;
  bool imported_typed_keypath_runtime_semantics_landed = false;
  bool cross_module_link_plan_artifact_landed = false;
  bool imported_registration_manifest_loading_landed = false;
  bool runtime_archive_aggregation_landed = false;
  bool cross_module_runtime_registration_landed = false;
  bool cross_module_launch_orchestration_landed = false;
  bool public_cross_module_orchestration_abi_landed = false;
  bool ready_for_packaging_and_runtime_registration_impl = false;
  std::string source_serialized_runtime_metadata_replay_key;
  std::string source_local_registration_manifest_replay_key;
  std::string source_imported_runtime_metadata_semantic_rules_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary
              .source_serialized_runtime_metadata_artifact_reuse_contract_id
              .empty() &&
         !summary.source_local_registration_manifest_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.import_artifact_relative_path.empty() &&
         !summary.local_registration_manifest_artifact_relative_path.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         !summary.registration_scope_model.empty() &&
         !summary.packaging_model.empty() &&
         summary.module_image_count == summary.module_names_lexicographic.size() &&
         summary.module_image_count > 0 &&
         summary.direct_import_input_count + 1 <= summary.module_image_count &&
         summary.local_total_descriptor_count ==
             summary.local_class_descriptor_count +
                 summary.local_protocol_descriptor_count +
                 summary.local_category_descriptor_count +
                 summary.local_property_descriptor_count +
                 summary.local_ivar_descriptor_count &&
         summary.fail_closed &&
         summary.source_serialized_runtime_metadata_artifact_reuse_ready &&
         summary.source_local_registration_manifest_ready &&
         summary.semantic_surface_published &&
         summary.local_registration_manifest_emitted &&
         !summary.cross_module_link_plan_artifact_landed &&
         !summary.imported_registration_manifest_loading_landed &&
         !summary.runtime_archive_aggregation_landed &&
         !summary.cross_module_runtime_registration_landed &&
         !summary.cross_module_launch_orchestration_landed &&
         !summary.public_cross_module_orchestration_abi_landed &&
         !summary.ready_for_packaging_and_runtime_registration_impl &&
         !summary.source_serialized_runtime_metadata_replay_key.empty() &&
         !summary.source_local_registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ExecutableMetadataSourceGraphContractId =
    "objc3c-executable-metadata-source-graph-completeness/m252-a002-v1";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphOwnerIdentityModel =
    "semantic-link-symbol-and-runtime-owner-identity";
inline constexpr const char *kObjc3ExecutableMetadataMetaclassNodePolicy =
    "first-class-metaclass-nodes-derived-from-interface-runtime-owner-identities";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphEdgeOrderingModel =
    "lexicographic-kind-source-target";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassSourceClosureContractId =
        "objc3c-executable-class-metaclass-source-closure/m256-a002-v1";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassParentIdentityModel =
        "declaration-owned-class-parent-plus-metaclass-parent-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassMethodOwnerIdentityModel =
        "declaration-owned-instance-class-method-owner-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassObjectIdentityModel =
        "declaration-owned-class-and-metaclass-object-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataProtocolCategorySourceClosureContractId =
        "objc3c-executable-protocol-category-source-closure/m256-a003-v1";
inline constexpr const char
    *kObjc3ExecutableMetadataProtocolInheritanceIdentityModel =
        "protocol-declaration-owned-inherited-protocol-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataCategoryAttachmentIdentityModel =
        "category-declaration-owned-class-interface-implementation-attachment-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataProtocolCategoryConformanceIdentityModel =
        "category-declaration-owned-adopted-protocol-conformance-identities";
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
  std::string super_metaclass_owner_identity;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_super = false;
  bool declaration_complete = false;
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
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_matching_interface = false;
  bool has_super = false;
  bool declaration_complete = false;
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
  std::string super_metaclass_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_interface = false;
  bool has_implementation = false;
  bool has_super = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  bool realization_identity_complete = false;
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
  bool declaration_complete = false;
  bool inherited_protocol_identity_complete = false;
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
  bool declaration_complete = false;
  bool attachment_identity_complete = false;
  bool conformance_identity_complete = false;
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
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
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
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
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
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
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
  std::string class_metaclass_source_closure_contract_id =
      kObjc3ExecutableMetadataClassMetaclassSourceClosureContractId;
  std::string class_metaclass_parent_identity_model =
      kObjc3ExecutableMetadataClassMetaclassParentIdentityModel;
  std::string class_metaclass_method_owner_identity_model =
      kObjc3ExecutableMetadataClassMetaclassMethodOwnerIdentityModel;
  std::string class_metaclass_object_identity_model =
      kObjc3ExecutableMetadataClassMetaclassObjectIdentityModel;
  std::string protocol_category_source_closure_contract_id =
      kObjc3ExecutableMetadataProtocolCategorySourceClosureContractId;
  std::string protocol_inheritance_identity_model =
      kObjc3ExecutableMetadataProtocolInheritanceIdentityModel;
  std::string category_attachment_identity_model =
      kObjc3ExecutableMetadataCategoryAttachmentIdentityModel;
  std::string protocol_category_conformance_identity_model =
      kObjc3ExecutableMetadataProtocolCategoryConformanceIdentityModel;
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
  bool class_metaclass_declaration_closure_complete = false;
  bool class_metaclass_parent_identity_closure_complete = false;
  bool class_metaclass_method_owner_identity_closure_complete = false;
  bool class_metaclass_object_identity_closure_complete = false;
  bool protocol_category_declaration_closure_complete = false;
  bool protocol_inheritance_identity_closure_complete = false;
  bool category_attachment_identity_closure_complete = false;
  bool protocol_category_conformance_identity_closure_complete = false;
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
         !graph.edge_ordering_model.empty() &&
         !graph.class_metaclass_source_closure_contract_id.empty() &&
         !graph.class_metaclass_parent_identity_model.empty() &&
         !graph.class_metaclass_method_owner_identity_model.empty() &&
         !graph.class_metaclass_object_identity_model.empty() &&
         !graph.protocol_category_source_closure_contract_id.empty() &&
         !graph.protocol_inheritance_identity_model.empty() &&
         !graph.category_attachment_identity_model.empty() &&
         !graph.protocol_category_conformance_identity_model.empty() &&
         graph.class_metaclass_declaration_closure_complete &&
         graph.class_metaclass_parent_identity_closure_complete &&
         graph.class_metaclass_method_owner_identity_closure_complete &&
         graph.class_metaclass_object_identity_closure_complete &&
         graph.protocol_category_declaration_closure_complete &&
         graph.protocol_inheritance_identity_closure_complete &&
         graph.category_attachment_identity_closure_complete &&
         graph.protocol_category_conformance_identity_closure_complete;
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
  std::string launch_integration_contract_id =
      "objc3c-runtime-launch-integration/m254-d004-v1";
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
  std::string runtime_library_resolution_model =
      "registration-manifest-runtime-archive-path-is-authoritative";
  std::string driver_linker_flag_consumption_model =
      "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands";
  std::string compile_wrapper_command_surface =
      "scripts/objc3c_native_compile.ps1";
  std::string compile_proof_command_surface =
      "scripts/run_objc3c_native_compile_proof.ps1";
  std::string execution_smoke_command_surface =
      "scripts/check_objc3c_native_execution_smoke.ps1";
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool translation_unit_registration_contract_ready = false;
  bool runtime_support_library_link_wiring_ready = false;
  bool runtime_manifest_template_published = false;
  bool constructor_root_manifest_authoritative = false;
  bool constructor_root_reserved_for_lowering = false;
  bool init_stub_emission_deferred_to_lowering = false;
  bool runtime_registration_artifact_emitted_by_driver = false;
  bool ready_for_lowering_init_stub_emission = false;
  bool launch_integration_ready = false;
  std::size_t runtime_owned_payload_artifact_count = 0;
  std::string translation_unit_registration_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.launch_integration_contract_id.empty() &&
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
         summary.launch_integration_ready &&
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
         !summary.runtime_library_resolution_model.empty() &&
         !summary.driver_linker_flag_consumption_model.empty() &&
         !summary.compile_wrapper_command_surface.empty() &&
         !summary.compile_proof_command_surface.empty() &&
         !summary.execution_smoke_command_surface.empty() &&
         summary.total_descriptor_count ==
             summary.class_descriptor_count +
                 summary.protocol_descriptor_count +
                 summary.category_descriptor_count +
                 summary.property_descriptor_count +
                 summary.ivar_descriptor_count &&
         summary.translation_unit_registration_order_ordinal > 0 &&
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

struct Objc3RuntimeBootstrapSemanticsSummary {
  std::string contract_id = kObjc3RuntimeBootstrapSemanticsContractId;
  std::string bootstrap_invariant_contract_id =
      kObjc3RuntimeStartupBootstrapInvariantContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string bootstrap_surface_path =
      kObjc3RuntimeBootstrapSemanticsSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string realization_order_policy =
      kObjc3RuntimeStartupBootstrapRealizationOrderPolicy;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string image_local_initialization_scope =
      kObjc3RuntimeStartupBootstrapImageLocalInitializationScope;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string manifest_authority_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string runtime_library_archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string registration_result_model =
      kObjc3RuntimeBootstrapResultModel;
  std::string registration_order_ordinal_model =
      kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  int success_status_code = kObjc3RuntimeBootstrapSuccessStatusCode;
  int invalid_descriptor_status_code =
      kObjc3RuntimeBootstrapInvalidDescriptorStatusCode;
  int duplicate_registration_status_code =
      kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode;
  int out_of_order_status_code =
      kObjc3RuntimeBootstrapOutOfOrderStatusCode;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool bootstrap_invariant_contract_ready = false;
  bool registration_manifest_contract_ready = false;
  bool live_runtime_enforcement_landed = false;
  bool registration_manifest_bootstrap_semantics_published = false;
  bool runtime_probe_required = false;
  bool no_partial_commit_on_failure = false;
  bool ready_for_constructor_root_implementation = false;
  std::string bootstrap_invariant_replay_key;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapSemanticsSummary(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_invariant_contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.realization_order_policy.empty() &&
         !summary.failure_mode.empty() &&
         !summary.image_local_initialization_scope.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.manifest_authority_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.runtime_library_archive_relative_path.empty() &&
         !summary.registration_result_model.empty() &&
         !summary.registration_order_ordinal_model.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed &&
         summary.bootstrap_invariant_contract_ready &&
         summary.registration_manifest_contract_ready &&
         summary.live_runtime_enforcement_landed &&
         summary.registration_manifest_bootstrap_semantics_published &&
         summary.runtime_probe_required &&
         summary.no_partial_commit_on_failure &&
         summary.ready_for_constructor_root_implementation &&
         !summary.bootstrap_invariant_replay_key.empty() &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapLoweringSummary {
  std::string contract_id = kObjc3RuntimeBootstrapLoweringContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string registration_descriptor_frontend_closure_contract_id =
      kObjc3RuntimeBootstrapRegistrationDescriptorHandoffContractId;
  std::string registration_descriptor_artifact =
      kObjc3RuntimeBootstrapRegistrationDescriptorArtifact;
  std::string bootstrap_surface_path =
      "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract";
  std::string lowering_boundary_model =
      kObjc3RuntimeBootstrapLoweringBoundaryModel;
  std::string registration_descriptor_handoff_model =
      kObjc3RuntimeBootstrapRegistrationDescriptorHandoffModel;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string constructor_init_stub_symbol_prefix =
      kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix;
  std::string registration_table_symbol_prefix =
      kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix;
  std::string image_local_init_state_symbol_prefix =
      kObjc3RuntimeBootstrapImageLocalInitStateSymbolPrefix;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string global_ctor_list_model =
      kObjc3RuntimeBootstrapGlobalCtorListModel;
  std::string registration_table_layout_model =
      kObjc3RuntimeBootstrapRegistrationTableLayoutModel;
  std::string image_local_initialization_model =
      kObjc3RuntimeBootstrapImageLocalInitializationModel;
  std::uint64_t registration_table_abi_version =
      kObjc3RuntimeBootstrapRegistrationTableAbiVersion;
  std::uint64_t registration_table_pointer_field_count =
      kObjc3RuntimeBootstrapRegistrationTablePointerFieldCount;
  std::string constructor_root_emission_state =
      kObjc3RuntimeBootstrapConstructorRootEmissionState;
  std::string init_stub_emission_state =
      kObjc3RuntimeBootstrapInitStubEmissionState;
  std::string registration_table_emission_state =
      kObjc3RuntimeBootstrapRegistrationTableEmissionState;
  bool fail_closed = false;
  bool registration_manifest_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool registration_descriptor_frontend_closure_contract_ready = false;
  bool lowering_contract_published = false;
  bool manifest_authority_preserved = false;
  bool no_bootstrap_ir_materialization_yet = false;
  bool bootstrap_ir_materialization_landed = false;
  bool image_local_initialization_landed = false;
  bool ready_for_bootstrap_materialization = false;
  std::string registration_manifest_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string registration_descriptor_frontend_closure_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapLoweringSummary(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.registration_descriptor_frontend_closure_contract_id.empty() &&
         !summary.registration_descriptor_artifact.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.lowering_boundary_model.empty() &&
         !summary.registration_descriptor_handoff_model.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.constructor_init_stub_symbol_prefix.empty() &&
         !summary.registration_table_symbol_prefix.empty() &&
         !summary.image_local_init_state_symbol_prefix.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.global_ctor_list_model.empty() &&
         !summary.registration_table_layout_model.empty() &&
         !summary.image_local_initialization_model.empty() &&
         summary.registration_table_abi_version > 0 &&
         summary.registration_table_pointer_field_count > 0 &&
         !summary.constructor_root_emission_state.empty() &&
         !summary.init_stub_emission_state.empty() &&
         !summary.registration_table_emission_state.empty() &&
         summary.fail_closed &&
         summary.registration_manifest_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.registration_descriptor_frontend_closure_contract_ready &&
         summary.lowering_contract_published &&
         summary.manifest_authority_preserved &&
         !summary.no_bootstrap_ir_materialization_yet &&
         summary.bootstrap_ir_materialization_landed &&
         summary.image_local_initialization_landed &&
         summary.ready_for_bootstrap_materialization &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.registration_descriptor_frontend_closure_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary {
  std::string contract_id =
      kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string source_surface_path =
      kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfacePath;
  std::string registration_descriptor_pragma_name =
      kObjc3BootstrapRegistrationDescriptorPragmaName;
  std::string image_root_pragma_name = kObjc3BootstrapImageRootPragmaName;
  std::string module_identity_source =
      kObjc3RuntimeBootstrapModuleIdentitySourceModel;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string bootstrap_visible_metadata_ownership_model =
      kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel;
  std::string module_name = "objc3_module";
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") + kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  bool fail_closed = false;
  bool registration_manifest_contract_ready = false;
  bool source_surface_frozen = false;
  bool prelude_pragma_contract_published = false;
  bool registration_descriptor_identifier_resolved = false;
  bool image_root_identifier_resolved = false;
  bool bootstrap_visible_metadata_ownership_published = false;
  bool ready_for_descriptor_frontend_closure = false;
  bool registration_descriptor_pragma_seen = false;
  bool registration_descriptor_pragma_duplicate = false;
  bool registration_descriptor_pragma_non_leading = false;
  std::size_t registration_descriptor_pragma_directive_count = 0;
  bool image_root_pragma_seen = false;
  bool image_root_pragma_duplicate = false;
  bool image_root_pragma_non_leading = false;
  std::size_t image_root_pragma_directive_count = 0;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.source_surface_path.empty() &&
         !summary.registration_descriptor_pragma_name.empty() &&
         !summary.image_root_pragma_name.empty() &&
         !summary.module_identity_source.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         !summary.bootstrap_visible_metadata_ownership_model.empty() &&
         !summary.module_name.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         summary.fail_closed &&
         summary.registration_manifest_contract_ready &&
         summary.source_surface_frozen &&
         summary.prelude_pragma_contract_published &&
         summary.registration_descriptor_identifier_resolved &&
         summary.image_root_identifier_resolved &&
         summary.bootstrap_visible_metadata_ownership_published &&
         summary.ready_for_descriptor_frontend_closure &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary {
  std::string contract_id =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string source_surface_contract_id =
      kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureSurfacePath;
  std::string payload_model =
      kObjc3RuntimeRegistrationDescriptorFrontendClosurePayloadModel;
  std::string artifact_relative_path =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3RuntimeRegistrationDescriptorFrontendAuthorityModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string payload_ownership_model =
      kObjc3RuntimeRegistrationDescriptorPayloadOwnershipModel;
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string bootstrap_visible_metadata_ownership_model =
      kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel;
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool source_surface_contract_ready = false;
  bool registration_manifest_contract_ready = false;
  bool descriptor_frontend_surface_published = false;
  bool descriptor_artifact_template_published = false;
  bool descriptor_fields_resolved = false;
  bool ready_for_descriptor_artifact_emission = false;
  bool ready_for_registration_descriptor_lowering = false;
  std::string source_surface_replay_key;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.source_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.artifact_relative_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.payload_ownership_model.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         !summary.bootstrap_visible_metadata_ownership_model.empty() &&
         summary.total_descriptor_count ==
             summary.class_descriptor_count +
                 summary.protocol_descriptor_count +
                 summary.category_descriptor_count +
                 summary.property_descriptor_count +
                 summary.ivar_descriptor_count &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed && summary.source_surface_contract_ready &&
         summary.registration_manifest_contract_ready &&
         summary.descriptor_frontend_surface_published &&
         summary.descriptor_artifact_template_published &&
         summary.descriptor_fields_resolved &&
         summary.ready_for_descriptor_artifact_emission &&
         summary.ready_for_registration_descriptor_lowering &&
         !summary.source_surface_replay_key.empty() &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapLegalityFailureContractSummary {
  std::string contract_id = kObjc3BootstrapLegalityFailureContractId;
  std::string registration_descriptor_frontend_closure_contract_id =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3BootstrapLegalityFailureSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string image_registration_order_invariant =
      kObjc3BootstrapLegalityImageOrderInvariantModel;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string restart_lifecycle_model =
      kObjc3RuntimeBootstrapResetLifecycleModel;
  std::string replay_order_model = kObjc3RuntimeBootstrapReplayOrderModel;
  std::string image_local_init_reset_model =
      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
  std::string catalog_retention_model =
      kObjc3RuntimeBootstrapCatalogRetentionModel;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool registration_descriptor_frontend_closure_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool semantic_boundary_ready = false;
  bool duplicate_registration_policy_frozen = false;
  bool image_order_invariant_frozen = false;
  bool bootstrap_rejection_frozen = false;
  bool restart_boundary_frozen = false;
  bool semantic_diagnostics_required = false;
  bool ready_for_lowering_and_runtime = false;
  std::string registration_descriptor_frontend_closure_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string semantic_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(
    const Objc3RuntimeBootstrapLegalityFailureContractSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_descriptor_frontend_closure_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed &&
         summary.registration_descriptor_frontend_closure_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.semantic_boundary_ready &&
         summary.duplicate_registration_policy_frozen &&
         summary.image_order_invariant_frozen &&
         summary.bootstrap_rejection_frozen &&
         summary.restart_boundary_frozen &&
         summary.semantic_diagnostics_required &&
         summary.ready_for_lowering_and_runtime &&
         !summary.registration_descriptor_frontend_closure_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapLegalitySemanticsSummary {
  std::string contract_id = kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_legality_failure_contract_id =
      kObjc3BootstrapLegalityFailureContractId;
  std::string registration_descriptor_frontend_closure_contract_id =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3BootstrapLegalitySemanticsSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string image_registration_order_invariant =
      kObjc3BootstrapLegalityImageOrderInvariantModel;
  std::string cross_image_legality_model =
      kObjc3BootstrapLegalityCrossImageLegalityModel;
  std::string semantic_diagnostic_model =
      kObjc3BootstrapLegalitySemanticDiagnosticModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string translation_unit_identity_key;
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool semantic_boundary_ready = false;
  bool bootstrap_legality_failure_contract_ready = false;
  bool registration_descriptor_frontend_closure_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool duplicate_registration_semantics_landed = false;
  bool image_order_semantics_landed = false;
  bool cross_image_legality_semantics_landed = false;
  bool semantic_diagnostics_landed = false;
  bool ready_for_lowering_and_runtime = false;
  std::string semantic_boundary_replay_key;
  std::string bootstrap_legality_failure_contract_replay_key;
  std::string registration_descriptor_frontend_closure_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapLegalitySemanticsSummary(
    const Objc3RuntimeBootstrapLegalitySemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_failure_contract_id.empty() &&
         !summary.registration_descriptor_frontend_closure_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.cross_image_legality_model.empty() &&
         !summary.semantic_diagnostic_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.translation_unit_identity_key.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed && summary.semantic_boundary_ready &&
         summary.bootstrap_legality_failure_contract_ready &&
         summary.registration_descriptor_frontend_closure_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.duplicate_registration_semantics_landed &&
         summary.image_order_semantics_landed &&
         summary.cross_image_legality_semantics_landed &&
         summary.semantic_diagnostics_landed &&
         summary.ready_for_lowering_and_runtime &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.bootstrap_legality_failure_contract_replay_key.empty() &&
         !summary.registration_descriptor_frontend_closure_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapFailureRestartSemanticsSummary {
  std::string contract_id = kObjc3BootstrapFailureRestartSemanticsContractId;
  std::string bootstrap_legality_semantics_contract_id =
      kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_reset_contract_id =
      kObjc3RuntimeBootstrapResetContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3BootstrapFailureRestartSemanticsSurfacePath;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string restart_lifecycle_model =
      kObjc3RuntimeBootstrapResetLifecycleModel;
  std::string replay_order_model = kObjc3RuntimeBootstrapReplayOrderModel;
  std::string image_local_init_reset_model =
      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
  std::string catalog_retention_model =
      kObjc3RuntimeBootstrapCatalogRetentionModel;
  std::string unsupported_topology_model =
      kObjc3BootstrapFailureRestartUnsupportedTopologyModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string translation_unit_identity_key;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  std::string replay_registered_images_symbol =
      kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
  std::string reset_replay_state_snapshot_symbol =
      kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
  int invalid_descriptor_status_code =
      kObjc3RuntimeBootstrapInvalidDescriptorStatusCode;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool semantic_boundary_ready = false;
  bool bootstrap_legality_semantics_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool bootstrap_reset_contract_ready = false;
  bool failure_mode_semantics_landed = false;
  bool restart_semantics_landed = false;
  bool replay_semantics_landed = false;
  bool unsupported_topology_semantics_landed = false;
  bool deterministic_recovery_semantics_landed = false;
  bool runtime_restart_probe_required = false;
  bool ready_for_lowering_and_runtime = false;
  std::string semantic_boundary_replay_key;
  std::string bootstrap_legality_semantics_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapFailureRestartSemanticsSummary(
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_semantics_contract_id.empty() &&
         !summary.bootstrap_reset_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.unsupported_topology_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.translation_unit_identity_key.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         !summary.replay_registered_images_symbol.empty() &&
         !summary.reset_replay_state_snapshot_symbol.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed && summary.semantic_boundary_ready &&
         summary.bootstrap_legality_semantics_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.bootstrap_reset_contract_ready &&
         summary.failure_mode_semantics_landed &&
         summary.restart_semantics_landed &&
         summary.replay_semantics_landed &&
         summary.unsupported_topology_semantics_landed &&
         summary.deterministic_recovery_semantics_landed &&
         summary.runtime_restart_probe_required &&
         summary.ready_for_lowering_and_runtime &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.bootstrap_legality_semantics_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary {
  std::string contract_id =
      kObjc3CompatibilityStrictnessClaimSemanticsContractId;
  std::string runnable_feature_claim_inventory_contract_id =
      kObjc3RunnableFeatureClaimInventoryContractId;
  std::string feature_claim_truth_surface_contract_id =
      kObjc3FeatureClaimStrictnessTruthSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3CompatibilityStrictnessClaimSemanticsSurfacePath;
  std::string semantic_model =
      kObjc3CompatibilityStrictnessClaimSemanticModel;
  std::string downgrade_model =
      kObjc3CompatibilityStrictnessClaimDowngradeModel;
  std::string rejection_model =
      kObjc3CompatibilityStrictnessClaimRejectionModel;
  std::string canonical_interface_truth_model =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfaceTruthModel;
  std::string separate_compilation_macro_truth_model =
      kObjc3CompatibilityStrictnessClaimSeparateCompilationMacroTruthModel;
  std::string canonical_interface_payload_mode =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode;
  std::string effective_compatibility_mode = "canonical";
  std::vector<std::string> suppressed_macro_claim_ids;
  bool migration_assist_enabled = false;
  std::size_t valid_compatibility_mode_count = 0;
  std::size_t live_selection_surface_count = 0;
  std::size_t valid_selection_combination_count = 0;
  std::size_t runnable_feature_claim_count = 0;
  std::size_t downgraded_source_only_claim_count = 0;
  std::size_t rejected_unsupported_feature_claim_count = 0;
  std::size_t rejected_selection_surface_count = 0;
  std::size_t suppressed_macro_claim_count = 0;
  // M259-B002/M264-B002 unsupported-feature enforcement anchor: the frontend
  // mirror of the sema packet must preserve zero-site runnable proofs and
  // deterministic early rejection accounting for accepted advanced surfaces.
  std::size_t live_unsupported_feature_family_count = 0;
  std::size_t live_unsupported_feature_site_count = 0;
  std::size_t live_unsupported_feature_diagnostic_count = 0;
  std::size_t throws_source_rejection_site_count = 0;
  std::size_t blocks_source_rejection_site_count = 0;
  std::size_t arc_source_rejection_site_count = 0;
  bool fail_closed = false;
  bool semantic_boundary_ready = false;
  bool compatibility_mode_semantics_landed = false;
  bool migration_assist_semantics_landed = false;
  bool source_only_claim_downgrade_semantics_landed = false;
  bool unsupported_feature_claim_rejection_semantics_landed = false;
  bool live_unsupported_feature_source_rejection_landed = false;
  bool strictness_selection_rejection_semantics_landed = false;
  bool feature_macro_claim_suppression_semantics_landed = false;
  bool canonical_interface_truth_semantics_landed = false;
  bool separate_compilation_macro_truth_semantics_landed = false;
  bool selected_configuration_valid = false;
  bool selected_configuration_downgraded = false;
  bool selected_configuration_rejected = false;
  bool ready_for_lowering_and_runtime = false;
  std::string semantic_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary &summary) {
  const bool compatibility_mode_valid =
      summary.effective_compatibility_mode == "canonical" ||
      summary.effective_compatibility_mode == "legacy";
  return !summary.contract_id.empty() &&
         !summary.runnable_feature_claim_inventory_contract_id.empty() &&
         !summary.feature_claim_truth_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.semantic_model.empty() && !summary.downgrade_model.empty() &&
         !summary.rejection_model.empty() &&
         !summary.canonical_interface_truth_model.empty() &&
         !summary.separate_compilation_macro_truth_model.empty() &&
         summary.canonical_interface_payload_mode ==
             kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode &&
         compatibility_mode_valid && summary.fail_closed && summary.semantic_boundary_ready &&
         summary.suppressed_macro_claim_ids.size() ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.suppressed_macro_claim_ids[0] ==
             kObjc3SuppressedMacroClaimStrictnessLevel &&
         summary.suppressed_macro_claim_ids[1] ==
             kObjc3SuppressedMacroClaimConcurrencyMode &&
         summary.suppressed_macro_claim_ids[2] ==
             kObjc3SuppressedMacroClaimConcurrencyStrict &&
         summary.valid_compatibility_mode_count ==
             kObjc3CompatibilityStrictnessClaimValidCompatibilityModeCount &&
         summary.live_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount &&
         summary.valid_selection_combination_count ==
             kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount &&
         summary.runnable_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRunnableFeatureCount &&
         summary.downgraded_source_only_claim_count ==
             kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount &&
         summary.rejected_unsupported_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRejectedFeatureCount &&
         summary.live_unsupported_feature_family_count <=
             summary.rejected_unsupported_feature_claim_count &&
         summary.throws_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.blocks_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.arc_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.live_unsupported_feature_diagnostic_count ==
             summary.live_unsupported_feature_site_count &&
         summary.throws_source_rejection_site_count +
                 summary.blocks_source_rejection_site_count +
                 summary.arc_source_rejection_site_count ==
             summary.live_unsupported_feature_site_count &&
         summary.rejected_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount &&
         summary.suppressed_macro_claim_count ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.compatibility_mode_semantics_landed &&
         summary.migration_assist_semantics_landed &&
         summary.source_only_claim_downgrade_semantics_landed &&
         summary.unsupported_feature_claim_rejection_semantics_landed &&
         summary.live_unsupported_feature_source_rejection_landed &&
         summary.strictness_selection_rejection_semantics_landed &&
         summary.feature_macro_claim_suppression_semantics_landed &&
         summary.canonical_interface_truth_semantics_landed &&
         summary.separate_compilation_macro_truth_semantics_landed &&
         summary.selected_configuration_valid &&
         !summary.selected_configuration_downgraded &&
         !summary.selected_configuration_rejected &&
         summary.ready_for_lowering_and_runtime &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendPart3TypeSourceClosureSummary(
    const Objc3FrontendPart3TypeSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3Part3TypeSourceClosureContractId &&
         summary.frontend_surface_path == kObjc3Part3TypeSourceClosureSurfacePath &&
         summary.source_model == kObjc3Part3TypeSourceClosureSourceModel &&
         summary.failure_model == kObjc3Part3TypeSourceClosureFailureModel &&
         summary.source_only_claim_ids.size() == 7 &&
         summary.source_only_claim_ids[0] ==
             kObjc3SourceOnlyFeatureClaimProtocolOptionalPartitions &&
         summary.source_only_claim_ids[1] ==
             kObjc3SourceOnlyFeatureClaimObjectPointerNullabilitySuffixes &&
         summary.source_only_claim_ids[2] ==
             kObjc3SourceOnlyFeatureClaimPragmaticGenericSuffixes &&
         summary.source_only_claim_ids[3] ==
             kObjc3SourceOnlyFeatureClaimOptionalBindings &&
         summary.source_only_claim_ids[4] ==
             kObjc3SourceOnlyFeatureClaimOptionalSends &&
         summary.source_only_claim_ids[5] ==
             kObjc3SourceOnlyFeatureClaimNilCoalescing &&
         summary.source_only_claim_ids[6] ==
             kObjc3SourceOnlyFeatureClaimTypedKeyPathLiterals &&
         summary.unsupported_claim_ids.empty() &&
         summary.protocol_optional_partition_source_supported &&
         summary.object_pointer_nullability_source_supported &&
         summary.pragmatic_generic_suffix_source_supported &&
         summary.optional_binding_source_supported &&
         summary.optional_send_source_supported &&
         summary.nil_coalescing_source_supported &&
         summary.typed_keypath_literal_source_supported &&
         !summary.optional_member_access_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendPart5ControlFlowSourceClosureSummary(
    const Objc3FrontendPart5ControlFlowSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3Part5ControlFlowSourceClosureContractId &&
         summary.frontend_surface_path ==
             kObjc3Part5ControlFlowSourceClosureSurfacePath &&
         summary.source_model == kObjc3Part5ControlFlowSourceClosureSourceModel &&
         summary.failure_model ==
             kObjc3Part5ControlFlowSourceClosureFailureModel &&
         summary.supported_construct_ids.size() == 8 &&
         summary.supported_construct_ids[0] ==
             kObjc3Part5SourceSurfaceGuardBindings &&
         summary.supported_construct_ids[1] ==
             kObjc3Part5SourceSurfaceGuardConditionLists &&
         summary.supported_construct_ids[2] ==
             kObjc3Part5SourceSurfaceSwitchCasePatterns &&
         summary.supported_construct_ids[3] ==
             kObjc3Part5SourceSurfaceDeferStatements &&
         summary.supported_construct_ids[4] ==
             kObjc3Part5SourceSurfaceMatchStatement &&
         summary.supported_construct_ids[5] ==
             kObjc3Part5SourceSurfaceMatchWildcardPatterns &&
         summary.supported_construct_ids[6] ==
             kObjc3Part5SourceSurfaceMatchLiteralPatterns &&
         summary.supported_construct_ids[7] ==
             kObjc3Part5SourceSurfaceMatchBindingPatterns &&
         summary.supported_construct_ids[8] ==
             kObjc3Part5SourceSurfaceMatchResultCasePatterns &&
         summary.fail_closed_construct_ids.size() == 3 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3Part5FailClosedConstructMatchExpression &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3Part5FailClosedConstructGuardedPatterns &&
         summary.fail_closed_construct_ids[2] ==
             kObjc3Part5FailClosedConstructMatchTypeTestPatterns &&
         summary.guard_binding_source_supported &&
         summary.guard_condition_list_source_supported &&
         summary.switch_case_pattern_source_supported &&
         summary.defer_statement_source_supported &&
         summary.match_statement_source_supported &&
         summary.match_wildcard_pattern_source_supported &&
         summary.match_literal_pattern_source_supported &&
         summary.match_binding_pattern_source_supported &&
         summary.match_result_case_pattern_source_supported &&
         summary.defer_keyword_reserved && !summary.defer_fail_closed &&
         summary.match_expression_fail_closed &&
         summary.guarded_pattern_fail_closed &&
         summary.type_test_pattern_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendPart6ErrorSourceClosureSummary(
    const Objc3FrontendPart6ErrorSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3Part6ErrorSourceClosureContractId &&
         summary.frontend_surface_path ==
             kObjc3Part6ErrorSourceClosureSurfacePath &&
         summary.source_model == kObjc3Part6ErrorSourceClosureSourceModel &&
         summary.failure_model == kObjc3Part6ErrorSourceClosureFailureModel &&
         summary.source_only_claim_ids.size() == 3 &&
         summary.source_only_claim_ids[0] ==
             kObjc3SourceOnlyFeatureClaimThrowsDeclarations &&
         summary.source_only_claim_ids[1] ==
             kObjc3SourceOnlyFeatureClaimResultCarrierProfiles &&
         summary.source_only_claim_ids[2] ==
             kObjc3SourceOnlyFeatureClaimNSErrorBridgingProfiles &&
         summary.fail_closed_construct_ids.size() == 3 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3Part6FailClosedConstructTryExpressions &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3Part6FailClosedConstructThrowStatements &&
         summary.fail_closed_construct_ids[2] ==
             kObjc3Part6FailClosedConstructDoCatchStatements &&
         summary.throws_declaration_source_supported &&
         summary.result_carrier_source_supported &&
         summary.ns_error_bridging_source_supported &&
         summary.error_bridge_marker_source_supported &&
         summary.try_keyword_reserved && summary.throw_keyword_reserved &&
         summary.catch_keyword_reserved && summary.try_fail_closed &&
         summary.throw_fail_closed && summary.do_catch_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3VersionedConformanceReportLoweringSummary {
  std::string contract_id =
      kObjc3VersionedConformanceReportLoweringContractId;
  std::string semantic_contract_id =
      kObjc3VersionedConformanceReportLoweringSemanticContractId;
  std::string runnable_feature_claim_inventory_contract_id =
      kObjc3RunnableFeatureClaimInventoryContractId;
  std::string feature_claim_truth_surface_contract_id =
      kObjc3FeatureClaimStrictnessTruthSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3VersionedConformanceReportLoweringSurfacePath;
  std::string artifact_suffix =
      kObjc3VersionedConformanceReportLoweringArtifactSuffix;
  std::string artifact_schema_id =
      kObjc3VersionedConformanceReportLoweringArtifactSchemaId;
  std::string payload_model =
      kObjc3VersionedConformanceReportLoweringPayloadModel;
  std::string authority_model =
      kObjc3VersionedConformanceReportLoweringAuthorityModel;
  std::string known_unsupported_model =
      kObjc3VersionedConformanceReportKnownUnsupportedModel;
  std::string selection_model =
      kObjc3VersionedConformanceReportSelectionModel;
  std::string canonical_interface_mode =
      kObjc3VersionedConformanceReportCanonicalInterfaceMode;
  std::string publication_model =
      kObjc3VersionedConformanceReportPublicationModel;
  std::string effective_compatibility_mode = "canonical";
  std::vector<std::string> runnable_feature_claim_ids;
  std::vector<std::string> source_only_feature_claim_ids;
  std::vector<std::string> unsupported_feature_claim_ids;
  std::vector<std::string> suppressed_macro_claim_ids;
  bool fail_closed = false;
  bool semantic_surface_published = false;
  bool runnable_claim_inventory_ready = false;
  bool feature_claim_truth_surface_ready = false;
  bool semantic_boundary_ready = false;
  bool known_unsupported_surface_published = false;
  bool compatibility_selection_truthful = false;
  bool strictness_selection_fail_closed = false;
  bool strict_concurrency_selection_fail_closed = false;
  bool canonical_interface_truthful = false;
  bool feature_macro_truthful = false;
  bool ready_for_runtime_conformance_publication = false;
  bool migration_assist_enabled = false;
  std::size_t runnable_feature_claim_count = 0;
  std::size_t source_only_feature_claim_count = 0;
  std::size_t unsupported_feature_claim_count = 0;
  std::size_t live_unsupported_feature_family_count = 0;
  std::size_t live_unsupported_feature_site_count = 0;
  std::size_t live_unsupported_feature_diagnostic_count = 0;
  std::string runnable_feature_claim_inventory_replay_key;
  std::string feature_claim_truth_surface_replay_key;
  std::string semantic_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3VersionedConformanceReportLoweringSummary(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  const bool compatibility_mode_valid =
      summary.effective_compatibility_mode == "canonical" ||
      summary.effective_compatibility_mode == "legacy";
  return !summary.contract_id.empty() &&
         !summary.semantic_contract_id.empty() &&
         !summary.runnable_feature_claim_inventory_contract_id.empty() &&
         !summary.feature_claim_truth_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.artifact_suffix.empty() &&
         !summary.artifact_schema_id.empty() &&
         !summary.payload_model.empty() &&
         !summary.authority_model.empty() &&
         !summary.known_unsupported_model.empty() &&
         !summary.selection_model.empty() &&
         !summary.canonical_interface_mode.empty() &&
         !summary.publication_model.empty() && compatibility_mode_valid &&
         summary.runnable_feature_claim_count ==
             summary.runnable_feature_claim_ids.size() &&
         summary.source_only_feature_claim_count ==
             summary.source_only_feature_claim_ids.size() &&
         summary.unsupported_feature_claim_count ==
             summary.unsupported_feature_claim_ids.size() &&
         summary.live_unsupported_feature_family_count <=
             summary.unsupported_feature_claim_count &&
         summary.live_unsupported_feature_diagnostic_count ==
             summary.live_unsupported_feature_site_count &&
         summary.fail_closed && summary.semantic_surface_published &&
         summary.runnable_claim_inventory_ready &&
         summary.feature_claim_truth_surface_ready &&
         summary.semantic_boundary_ready &&
         summary.known_unsupported_surface_published &&
         summary.compatibility_selection_truthful &&
         summary.strictness_selection_fail_closed &&
         summary.strict_concurrency_selection_fail_closed &&
         summary.canonical_interface_truthful &&
         summary.feature_macro_truthful &&
         summary.ready_for_runtime_conformance_publication &&
         !summary.runnable_feature_claim_inventory_replay_key.empty() &&
         !summary.feature_claim_truth_surface_replay_key.empty() &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapApiSummary {
  std::string contract_id = kObjc3RuntimeBootstrapApiContractId;
  std::string support_library_core_feature_contract_id =
      kObjc3RuntimeSupportLibraryCoreFeatureContractId;
  std::string support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string bootstrap_surface_path = kObjc3RuntimeBootstrapApiSurfacePath;
  std::string public_header_path = kObjc3RuntimeSupportLibraryPublicHeaderPath;
  std::string archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string registration_status_enum_type =
      kObjc3RuntimeBootstrapApiStatusEnumType;
  std::string image_descriptor_type =
      kObjc3RuntimeBootstrapApiImageDescriptorType;
  std::string selector_handle_type =
      kObjc3RuntimeBootstrapApiSelectorHandleType;
  std::string registration_snapshot_type =
      kObjc3RuntimeBootstrapApiRegistrationSnapshotType;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeSupportLibraryRegisterImageSymbol;
  std::string selector_lookup_symbol =
      kObjc3RuntimeSupportLibraryLookupSelectorSymbol;
  std::string dispatch_entrypoint_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string state_snapshot_symbol = kObjc3RuntimeBootstrapStateSnapshotSymbol;
  std::string reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;
  std::string compatibility_dispatch_symbol =
      kObjc3RuntimeSupportLibraryCompatibilityDispatchSymbol;
  std::string registration_result_model = kObjc3RuntimeBootstrapResultModel;
  std::string registration_order_ordinal_model =
      kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel;
  std::string runtime_state_locking_model =
      kObjc3RuntimeBootstrapApiStateLockingModel;
  std::string startup_invocation_model =
      kObjc3RuntimeBootstrapApiStartupInvocationModel;
  std::string image_walk_lifecycle_model =
      kObjc3RuntimeBootstrapApiImageWalkLifecycleModel;
  std::string deterministic_reset_lifecycle_model =
      kObjc3RuntimeBootstrapApiDeterministicResetLifecycleModel;
  bool fail_closed = false;
  bool support_library_core_feature_contract_ready = false;
  bool support_library_link_wiring_contract_ready = false;
  bool api_surface_frozen = false;
  bool registration_entrypoint_frozen = false;
  bool selector_lookup_and_dispatch_frozen = false;
  bool reset_and_snapshot_hooks_frozen = false;
  bool runtime_probe_required = false;
  bool image_walk_not_yet_landed = false;
  bool deterministic_reset_expansion_not_yet_landed = false;
  bool ready_for_registrar_implementation = false;
  std::string support_library_core_feature_replay_key;
  std::string support_library_link_wiring_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapApiSummary(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.support_library_core_feature_contract_id.empty() &&
         !summary.support_library_link_wiring_contract_id.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.public_header_path.empty() &&
         !summary.archive_relative_path.empty() &&
         !summary.registration_status_enum_type.empty() &&
         !summary.image_descriptor_type.empty() &&
         !summary.selector_handle_type.empty() &&
         !summary.registration_snapshot_type.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.selector_lookup_symbol.empty() &&
         !summary.dispatch_entrypoint_symbol.empty() &&
         !summary.state_snapshot_symbol.empty() &&
         !summary.reset_for_testing_symbol.empty() &&
         !summary.compatibility_dispatch_symbol.empty() &&
         !summary.registration_result_model.empty() &&
         !summary.registration_order_ordinal_model.empty() &&
         !summary.runtime_state_locking_model.empty() &&
         !summary.startup_invocation_model.empty() &&
         !summary.image_walk_lifecycle_model.empty() &&
         !summary.deterministic_reset_lifecycle_model.empty() &&
         summary.fail_closed &&
         summary.support_library_core_feature_contract_ready &&
         summary.support_library_link_wiring_contract_ready &&
         summary.api_surface_frozen &&
         summary.registration_entrypoint_frozen &&
         summary.selector_lookup_and_dispatch_frozen &&
         summary.reset_and_snapshot_hooks_frozen &&
         summary.runtime_probe_required &&
         summary.image_walk_not_yet_landed &&
         summary.deterministic_reset_expansion_not_yet_landed &&
         summary.ready_for_registrar_implementation &&
         !summary.support_library_core_feature_replay_key.empty() &&
         !summary.support_library_link_wiring_replay_key.empty() &&
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
  Objc3FrontendBootstrapRegistrationSourcePragmaContract
      bootstrap_registration_source_pragma_contract;
  Objc3SemanticIntegrationSurface integration_surface;
  Objc3SemanticTypeMetadataHandoff sema_type_metadata_handoff;
  Objc3FrontendProtocolCategorySummary protocol_category_summary;
  Objc3FrontendClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3FrontendSelectorNormalizationSummary selector_normalization_summary;
  Objc3FrontendPropertyAttributeSummary property_attribute_summary;
  Objc3FrontendObjectPointerNullabilityGenericsSummary object_pointer_nullability_generics_summary;
  Objc3FrontendPart3TypeSourceClosureSummary part3_type_source_closure_summary;
  Objc3FrontendPart5ControlFlowSourceClosureSummary
      part5_control_flow_source_closure_summary;
  Objc3FrontendPart6ErrorSourceClosureSummary part6_error_source_closure_summary;
  Objc3FrontendPart7AsyncSourceClosureSummary part7_async_source_closure_summary;
  Objc3FrontendPart8SystemExtensionSourceClosureSummary
      part8_system_extension_source_closure_summary;
  Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary
      part8_cleanup_resource_capture_source_completion_summary;
  Objc3FrontendPart8RetainableCFamilySourceCompletionSummary
      part8_retainable_c_family_source_completion_summary;
  Objc3FrontendPart9DispatchIntentSourceClosureSummary
      part9_dispatch_intent_source_closure_summary;
  Objc3FrontendPart9DispatchIntentSourceCompletionSummary
      part9_dispatch_intent_source_completion_summary;
  Objc3FrontendPart10MetaprogrammingSourceClosureSummary
      part10_metaprogramming_source_closure_summary;
  Objc3FrontendPart10MacroPackageProvenanceSourceCompletionSummary
      part10_macro_package_provenance_source_completion_summary;
  Objc3FrontendPart10PropertyBehaviorSourceCompletionSummary
      part10_property_behavior_source_completion_summary;
  Objc3FrontendPart11ForeignImportSourceClosureSummary
      part11_foreign_import_source_closure_summary;
  Objc3FrontendPart7ActorMemberIsolationSourceClosureSummary
      part7_actor_member_isolation_source_closure_summary;
  Objc3Part7ActorIsolationSendableSemanticModelSummary
      part7_actor_isolation_sendable_semantic_model_summary;
  Objc3Part7ActorIsolationSendabilityEnforcementSummary
      part7_actor_isolation_sendability_enforcement_summary;
  Objc3Part7ActorRaceHazardEscapeDiagnosticsSummary
      part7_actor_race_hazard_escape_diagnostics_summary;
  Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary
      part7_task_group_cancellation_source_closure_summary;
  Objc3Part5ControlFlowSemanticModelSummary
      part5_control_flow_semantic_model_summary;
  Objc3Part6ErrorSemanticModelSummary part6_error_semantic_model_summary;
  Objc3Part7TaskExecutorCancellationSemanticModelSummary
      part7_task_executor_cancellation_semantic_model_summary;
  Objc3Part8SystemExtensionSemanticModelSummary
      part8_system_extension_semantic_model_summary;
  Objc3Part10ExpansionBehaviorSemanticModelSummary
      part10_expansion_behavior_semantic_model_summary;
  Objc3Part10DeriveExpansionInventorySummary
      part10_derive_expansion_inventory_summary;
  Objc3Part10MacroSafetySandboxDeterminismSummary
      part10_macro_safety_sandbox_determinism_summary;
  Objc3Part10PropertyBehaviorLegalityCompatibilitySummary
      part10_property_behavior_legality_compatibility_summary;
    Objc3Part9DispatchIntentSemanticModelSummary
        part9_dispatch_intent_semantic_model_summary;
    Objc3Part9DispatchIntentLegalitySummary
        part9_dispatch_intent_legality_summary;
    Objc3Part9DispatchIntentCompatibilitySummary
        part9_dispatch_intent_compatibility_summary;
    Objc3Part8ResourceMoveUseAfterMoveSemanticsSummary
        part8_resource_move_use_after_move_semantics_summary;
  Objc3Part8BorrowedPointerEscapeAnalysisSummary
      part8_borrowed_pointer_escape_analysis_summary;
  Objc3Part8CaptureListRetainableFamilyLegalityCompletionSummary
      part8_capture_list_retainable_family_legality_completion_summary;
  Objc3Part7StructuredTaskCancellationSemanticSummary
      part7_structured_task_cancellation_semantic_summary;
  Objc3Part7ExecutorHopAffinityCompatibilitySummary
      part7_executor_hop_affinity_compatibility_summary;
  Objc3Part7AsyncEffectSuspensionSemanticModelSummary
      part7_async_effect_suspension_semantic_model_summary;
  Objc3Part7AwaitSuspensionResumeSemanticSummary
      part7_await_suspension_resume_semantic_summary;
  Objc3Part7AsyncDiagnosticsCompatibilitySummary
      part7_async_diagnostics_compatibility_summary;
  Objc3Part6TryDoCatchSemanticSummary part6_try_do_catch_semantic_summary;
  Objc3Part6ErrorBridgeLegalitySummary part6_error_bridge_legality_summary;
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

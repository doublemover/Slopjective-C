#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "config/objc3_language_profile.h"
#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_diagnostics_bus.h"
#include "parse/objc3_parser_contract.h"
#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_manager_contract.h"
#include "token/objc3_token_contract.h"

inline constexpr std::uint8_t kObjc3DefaultLanguageVersion =
    objc3c::config::kCanonicalLanguageVersion;

enum class Objc3FrontendLanguageProfile : std::uint8_t {
  kCanonical = 0u,
};

enum class Objc3FrontendArcMode : std::uint8_t {
  kDisabled = 0u,
  kEnabled = 1u,
};

struct Objc3FrontendOptions {
  std::uint8_t language_version = kObjc3DefaultLanguageVersion;
  Objc3FrontendLanguageProfile language_profile = Objc3FrontendLanguageProfile::kCanonical;
  Objc3FrontendArcMode arc_mode = Objc3FrontendArcMode::kDisabled;
  bool legacy_literal_diagnostics = false;
  bool emit_manifest = true;
  bool emit_ir = true;
  bool emit_object = true;
  std::uint64_t bootstrap_registration_order_ordinal = 1u;
  std::string metaprogramming_cache_root_relative_path;
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
  bool advanced_integration_closeout_signoff_consistent = false;
  bool advanced_integration_closeout_signoff_ready = false;
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
  std::string advanced_integration_closeout_signoff_key;
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

inline constexpr const char *kObjc3TypeSystemTypeSourceClosureContractId =
    "objc3c.type_system.type.source.closure.v1";
inline constexpr const char *kObjc3TypeSystemTypeSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_type_system_type_source_closure";
inline constexpr const char *kObjc3TypeSystemTypeSourceClosureSourceModel =
    "protocol-optional-partitions-object-pointer-nullability-generic-suffixes-optional-bindings-optional-sends-optional-member-access-nil-coalescing-and-typed-keypaths-are-live-parser-owned-source-surfaces";
inline constexpr const char *kObjc3TypeSystemTypeSourceClosureFailureModel =
    "typed-keypath-literals-remain-source-sema-surfaces-while-native-lowering-now-emits-stable-descriptor-handles-for-the-validated-single-component-subset";
inline constexpr const char *kObjc3TypeSystemTypeSemanticModelContractId =
    "objc3c.type_system.type.semantic.model.v1";
inline constexpr const char *kObjc3TypeSystemTypeSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_type_system_type_semantic_model";
inline constexpr const char *kObjc3TypeSystemTypeSemanticModelCoreRule =
    "optional-bindings-optional-sends-erased-generic-metadata-and-typed-keypath-shape-obey-one-fail-closed-sema-model-before-lowering";
inline constexpr const char *kObjc3TypeSystemGenericContractPreservationContractId =
    "objc3c.type_system.generic.contract.preservation.v1";
inline constexpr const char *kObjc3TypeSystemNullabilityContractPreservationContractId =
    "objc3c.type_system.nullability.contract.preservation.v1";
inline constexpr const char *kObjc3TypeSystemProtocolContractPreservationContractId =
    "objc3c.type_system.protocol.contract.preservation.v1";

struct Objc3FrontendTypeSystemTypeSourceClosureSummary {
  std::string contract_id = kObjc3TypeSystemTypeSourceClosureContractId;
  std::string frontend_surface_path = kObjc3TypeSystemTypeSourceClosureSurfacePath;
  std::string source_model = kObjc3TypeSystemTypeSourceClosureSourceModel;
  std::string failure_model = kObjc3TypeSystemTypeSourceClosureFailureModel;
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

inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureContractId =
    "objc3c.control_flow.control.flow.source.closure.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_control_flow_control_flow_source_closure";
inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureSourceModel =
    "guard-condition-lists-defer-statements-and-statement-match-patterns-are-live-frontend-owned-control-flow-surfaces-while-match-expression-guarded-patterns-and-type-test-patterns-remain-fail-closed";
inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureFailureModel =
    "match-remains-statement-only-and-guarded-or-type-test-patterns-remain-fail-closed-until-later-sema-lowering-and-runtime-work";

struct Objc3FrontendControlFlowControlFlowSourceClosureSummary {
  std::string contract_id = kObjc3ControlFlowControlFlowSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3ControlFlowControlFlowSourceClosureSurfacePath;
  std::string source_model = kObjc3ControlFlowControlFlowSourceClosureSourceModel;
  std::string failure_model = kObjc3ControlFlowControlFlowSourceClosureFailureModel;
  std::vector<std::string> supported_construct_ids = {
      kObjc3ControlFlowSourceSurfaceGuardBindings,
      kObjc3ControlFlowSourceSurfaceGuardConditionLists,
      kObjc3ControlFlowSourceSurfaceSwitchCasePatterns,
      kObjc3ControlFlowSourceSurfaceDeferStatements,
      kObjc3ControlFlowSourceSurfaceMatchStatement,
      kObjc3ControlFlowSourceSurfaceMatchWildcardPatterns,
      kObjc3ControlFlowSourceSurfaceMatchLiteralPatterns,
      kObjc3ControlFlowSourceSurfaceMatchBindingPatterns,
      kObjc3ControlFlowSourceSurfaceMatchResultCasePatterns,
  };
  std::vector<std::string> fail_closed_construct_ids = {
      kObjc3ControlFlowFailClosedConstructMatchExpression,
      kObjc3ControlFlowFailClosedConstructGuardedPatterns,
      kObjc3ControlFlowFailClosedConstructMatchTypeTestPatterns,
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

inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureContractId =
    "objc3c.error_handling.error.source.closure.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_error_source_closure";
inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureSourceModel =
    "throws-declarations-result-carrier-profiles-nserror-bridging-profiles-and-canonical-error-bridge-markers-are-live-frontend-owned-source-surfaces-while-try-throw-and-do-catch-remain-reserved-fail-closed";
inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureFailureModel =
    "try-expressions-throw-statements-and-do-catch-remain-parse-owned-fail-closed-boundaries-until-runnable-error_handling-sema-lowering-and-runtime-work";

struct Objc3FrontendErrorHandlingErrorSourceClosureSummary {
  std::string contract_id = kObjc3ErrorHandlingErrorSourceClosureContractId;
  std::string frontend_surface_path = kObjc3ErrorHandlingErrorSourceClosureSurfacePath;
  std::string source_model = kObjc3ErrorHandlingErrorSourceClosureSourceModel;
  std::string failure_model = kObjc3ErrorHandlingErrorSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimThrowsDeclarations,
      kObjc3SourceOnlyFeatureClaimResultCarrierProfiles,
      kObjc3SourceOnlyFeatureClaimNSErrorBridgingProfiles,
  };
  std::vector<std::string> fail_closed_construct_ids = {
      kObjc3ErrorHandlingFailClosedConstructTryExpressions,
      kObjc3ErrorHandlingFailClosedConstructThrowStatements,
      kObjc3ErrorHandlingFailClosedConstructDoCatchStatements,
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

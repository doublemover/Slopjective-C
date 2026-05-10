#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3ParserSemaConformanceEvidenceRecord
BuildObjc3ParserSemaConformanceEvidenceRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3ParserSemaConformanceEvidenceRecord record;
  const Objc3ParserSemaConformanceMatrix &matrix =
      surface.parser_sema_conformance_matrix;
  const Objc3ParserSemaConformanceCorpus &corpus =
      surface.parser_sema_conformance_corpus;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.conformance_matrix_deterministic =
      surface.deterministic_parser_sema_conformance_matrix &&
      matrix.deterministic;
  record.conformance_corpus_deterministic =
      surface.deterministic_parser_sema_conformance_corpus &&
      corpus.deterministic;
  record.declaration_count_evidence_ready =
      matrix.top_level_declaration_count_matches &&
      matrix.global_decl_count_matches && matrix.protocol_decl_count_matches &&
      matrix.interface_decl_count_matches &&
      matrix.implementation_decl_count_matches &&
      matrix.function_decl_count_matches;
  record.member_count_evidence_ready =
      matrix.protocol_property_decl_count_matches &&
      matrix.protocol_method_decl_count_matches &&
      matrix.protocol_class_method_decl_count_matches &&
      matrix.protocol_instance_method_decl_count_matches &&
      matrix.interface_property_decl_count_matches &&
      matrix.interface_method_decl_count_matches &&
      matrix.interface_class_method_decl_count_matches &&
      matrix.interface_instance_method_decl_count_matches &&
      matrix.implementation_property_decl_count_matches &&
      matrix.implementation_method_decl_count_matches &&
      matrix.implementation_class_method_decl_count_matches &&
      matrix.implementation_instance_method_decl_count_matches;
  record.category_function_evidence_ready =
      matrix.interface_category_decl_count_matches &&
      matrix.implementation_category_decl_count_matches &&
      matrix.function_prototype_count_matches &&
      matrix.function_pure_count_matches;
  record.fingerprint_evidence_ready =
      matrix.ast_shape_fingerprint_matches &&
      matrix.ast_top_level_layout_fingerprint_matches &&
      matrix.parser_contract_snapshot_fingerprint_matches;
  record.parser_budget_replay_evidence_ready =
      matrix.parser_diagnostic_budget_consistent &&
      matrix.parser_token_top_level_budget_consistent &&
      matrix.parser_subset_count_consistent &&
      matrix.parser_contract_snapshot_deterministic &&
      matrix.parser_recovery_replay_ready;
  record.passed_matrix_evidence_count =
      Objc3SemaEvidenceCount(matrix.top_level_declaration_count_matches) +
      Objc3SemaEvidenceCount(matrix.global_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.protocol_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.interface_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_category_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_category_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_prototype_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_pure_count_matches) +
      Objc3SemaEvidenceCount(matrix.ast_shape_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.ast_top_level_layout_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.parser_contract_snapshot_fingerprint_matches) +
      Objc3SemaEvidenceCount(matrix.parser_diagnostic_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_token_top_level_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_subset_count_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_contract_snapshot_deterministic) +
      Objc3SemaEvidenceCount(matrix.parser_recovery_replay_ready);
  record.required_corpus_case_count = corpus.required_case_count;
  record.passed_corpus_case_count = corpus.passed_case_count;
  record.failed_corpus_case_count = corpus.failed_case_count;
  record.corpus_inventory_ready =
      corpus.required_case_count == 5u &&
      corpus.has_top_level_declaration_count_case &&
      corpus.has_snapshot_fingerprint_case &&
      corpus.has_diagnostic_budget_case && corpus.has_subset_count_case &&
      corpus.has_recovery_replay_case;
  record.corpus_cases_passed =
      corpus.passed_case_count == corpus.required_case_count &&
      corpus.failed_case_count == 0u &&
      corpus.top_level_declaration_count_case_passed &&
      corpus.snapshot_fingerprint_case_passed &&
      corpus.diagnostic_budget_case_passed &&
      corpus.subset_count_case_passed && corpus.recovery_replay_case_passed;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_matrix_evidence_count == 30u &&
      record.passed_matrix_evidence_count ==
          record.required_matrix_evidence_count &&
      record.required_corpus_case_count == 5u &&
      record.passed_corpus_case_count == record.required_corpus_case_count &&
      record.failed_corpus_case_count == 0u &&
      record.conformance_matrix_deterministic &&
      record.conformance_corpus_deterministic &&
      record.declaration_count_evidence_ready &&
      record.member_count_evidence_ready &&
      record.category_function_evidence_ready &&
      record.fingerprint_evidence_ready &&
      record.parser_budget_replay_evidence_ready &&
      record.corpus_inventory_ready && record.corpus_cases_passed;
  return record;
}

Objc3ParserSemaContractReadinessRecord
BuildObjc3ParserSemaContractReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3ParserSemaContractReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.conformance_evidence_ready =
      surface.deterministic_parser_sema_conformance_evidence_record &&
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          surface.parser_sema_conformance_evidence_record);
  record.conformance_matrix_ready =
      record.conformance_evidence_ready &&
      surface.parser_sema_conformance_evidence_record
          .conformance_matrix_deterministic;
  record.conformance_corpus_ready =
      record.conformance_evidence_ready &&
      surface.parser_sema_conformance_evidence_record
          .conformance_corpus_deterministic;
  record.performance_quality_guardrails_ready =
      surface.deterministic_parser_sema_performance_quality_guardrails &&
      surface.parser_sema_performance_quality_guardrails.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          7u,
          surface.parser_sema_performance_quality_guardrails
              .required_guardrail_count,
          surface.parser_sema_performance_quality_guardrails
              .passed_guardrail_count,
          surface.parser_sema_performance_quality_guardrails
              .failed_guardrail_count) &&
      surface.parser_sema_performance_quality_guardrails
          .conformance_matrix_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .conformance_corpus_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .handoff_scaffold_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_diagnostic_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_token_top_level_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_subset_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .corpus_case_budget_consistent;
  record.cross_lane_integration_sync_ready =
      surface.deterministic_parser_sema_cross_lane_integration_sync &&
      surface.parser_sema_cross_lane_integration_sync.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          4u,
          surface.parser_sema_cross_lane_integration_sync.required_sync_count,
          surface.parser_sema_cross_lane_integration_sync.passed_sync_count,
          surface.parser_sema_cross_lane_integration_sync.failed_sync_count) &&
      surface.parser_sema_cross_lane_integration_sync.matrix_consistent &&
      surface.parser_sema_cross_lane_integration_sync.corpus_consistent &&
      surface.parser_sema_cross_lane_integration_sync
          .performance_quality_guardrails_consistent &&
      surface.parser_sema_cross_lane_integration_sync
          .pass_manager_contract_surface_sync;
  record.docs_runbook_sync_ready =
      surface.deterministic_parser_sema_docs_runbook_sync &&
      surface.parser_sema_docs_runbook_sync.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_docs_runbook_sync.required_sync_count,
          surface.parser_sema_docs_runbook_sync.passed_sync_count,
          surface.parser_sema_docs_runbook_sync.failed_sync_count) &&
      surface.parser_sema_docs_runbook_sync.cross_lane_integration_sync_ready &&
      surface.parser_sema_docs_runbook_sync.pass_manager_contract_surface_sync &&
      surface.parser_sema_docs_runbook_sync.parity_surface_sync;
  record.release_candidate_replay_dry_run_ready =
      surface.deterministic_parser_sema_release_candidate_replay_dry_run &&
      surface.parser_sema_release_candidate_replay_dry_run.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_release_candidate_replay_dry_run
              .required_sync_count,
          surface.parser_sema_release_candidate_replay_dry_run
              .passed_sync_count,
          surface.parser_sema_release_candidate_replay_dry_run
              .failed_sync_count) &&
      surface.parser_sema_release_candidate_replay_dry_run
          .docs_runbook_sync_ready &&
      surface.parser_sema_release_candidate_replay_dry_run
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_release_candidate_replay_dry_run.replay_surface_sync;
  record.advanced_core_shard1_ready =
      surface.deterministic_parser_sema_advanced_core_shard1 &&
      surface.parser_sema_advanced_core_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_core_shard1.required_sync_count,
          surface.parser_sema_advanced_core_shard1.passed_sync_count,
          surface.parser_sema_advanced_core_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_core_shard1
          .release_candidate_replay_dry_run_ready &&
      surface.parser_sema_advanced_core_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_core_shard1.shard_surface_sync;
  record.advanced_contract_rejection_shard1_ready =
      surface.deterministic_parser_sema_advanced_contract_rejection_shard1 &&
      surface.parser_sema_advanced_contract_rejection_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_contract_rejection_shard1
              .required_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard1
              .passed_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard1
              .failed_sync_count) &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .advanced_core_shard1_ready &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .shard_surface_sync;
  record.advanced_diagnostics_shard1_ready =
      surface.deterministic_parser_sema_advanced_diagnostics_shard1 &&
      surface.parser_sema_advanced_diagnostics_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_diagnostics_shard1.required_sync_count,
          surface.parser_sema_advanced_diagnostics_shard1.passed_sync_count,
          surface.parser_sema_advanced_diagnostics_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_diagnostics_shard1
          .advanced_contract_rejection_shard1_ready &&
      surface.parser_sema_advanced_diagnostics_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_diagnostics_shard1.shard_surface_sync;
  record.advanced_conformance_shard1_ready =
      surface.deterministic_parser_sema_advanced_conformance_shard1 &&
      surface.parser_sema_advanced_conformance_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_conformance_shard1.required_sync_count,
          surface.parser_sema_advanced_conformance_shard1.passed_sync_count,
          surface.parser_sema_advanced_conformance_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_conformance_shard1
          .advanced_diagnostics_shard1_ready &&
      surface.parser_sema_advanced_conformance_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_conformance_shard1.shard_surface_sync;
  record.advanced_integration_shard1_ready =
      surface.deterministic_parser_sema_advanced_integration_shard1 &&
      surface.parser_sema_advanced_integration_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_integration_shard1.required_sync_count,
          surface.parser_sema_advanced_integration_shard1.passed_sync_count,
          surface.parser_sema_advanced_integration_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_integration_shard1
          .advanced_conformance_shard1_ready &&
      surface.parser_sema_advanced_integration_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_integration_shard1.shard_surface_sync;
  record.advanced_performance_shard1_ready =
      surface.deterministic_parser_sema_advanced_performance_shard1 &&
      surface.parser_sema_advanced_performance_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_performance_shard1.required_sync_count,
          surface.parser_sema_advanced_performance_shard1.passed_sync_count,
          surface.parser_sema_advanced_performance_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_performance_shard1
          .advanced_integration_shard1_ready &&
      surface.parser_sema_advanced_performance_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_performance_shard1.shard_surface_sync;
  record.advanced_core_shard2_ready =
      surface.deterministic_parser_sema_advanced_core_shard2 &&
      surface.parser_sema_advanced_core_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_core_shard2.required_sync_count,
          surface.parser_sema_advanced_core_shard2.passed_sync_count,
          surface.parser_sema_advanced_core_shard2.failed_sync_count) &&
      surface.parser_sema_advanced_core_shard2
          .advanced_performance_shard1_ready &&
      surface.parser_sema_advanced_core_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_core_shard2.shard_surface_sync;
  record.advanced_contract_rejection_shard2_ready =
      surface.deterministic_parser_sema_advanced_contract_rejection_shard2 &&
      surface.parser_sema_advanced_contract_rejection_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_contract_rejection_shard2
              .required_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard2
              .passed_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard2
              .failed_sync_count) &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .advanced_core_shard2_ready &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .shard_surface_sync;
  record.advanced_diagnostics_shard2_ready =
      surface.deterministic_parser_sema_advanced_diagnostics_shard2 &&
      surface.parser_sema_advanced_diagnostics_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_diagnostics_shard2.required_sync_count,
          surface.parser_sema_advanced_diagnostics_shard2.passed_sync_count,
          surface.parser_sema_advanced_diagnostics_shard2.failed_sync_count) &&
      surface.parser_sema_advanced_diagnostics_shard2
          .advanced_contract_rejection_shard2_ready &&
      surface.parser_sema_advanced_diagnostics_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_diagnostics_shard2.shard_surface_sync;
  record.integration_closeout_ready =
      surface.deterministic_parser_sema_integration_closeout_signoff &&
      surface.parser_sema_integration_closeout_signoff.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_integration_closeout_signoff.required_sync_count,
          surface.parser_sema_integration_closeout_signoff.passed_sync_count,
          surface.parser_sema_integration_closeout_signoff.failed_sync_count) &&
      surface.parser_sema_integration_closeout_signoff
          .advanced_diagnostics_shard2_ready &&
      surface.parser_sema_integration_closeout_signoff
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_integration_closeout_signoff.gate_signoff_surface_sync;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.conformance_evidence_ready &&
      record.conformance_matrix_ready && record.conformance_corpus_ready &&
      record.performance_quality_guardrails_ready &&
      record.cross_lane_integration_sync_ready &&
      record.docs_runbook_sync_ready &&
      record.release_candidate_replay_dry_run_ready &&
      record.advanced_core_shard1_ready &&
      record.advanced_contract_rejection_shard1_ready &&
      record.advanced_diagnostics_shard1_ready &&
      record.advanced_conformance_shard1_ready &&
      record.advanced_integration_shard1_ready &&
      record.advanced_performance_shard1_ready &&
      record.advanced_core_shard2_ready &&
      record.advanced_contract_rejection_shard2_ready &&
      record.advanced_diagnostics_shard2_ready &&
      record.integration_closeout_ready;
  return record;
}

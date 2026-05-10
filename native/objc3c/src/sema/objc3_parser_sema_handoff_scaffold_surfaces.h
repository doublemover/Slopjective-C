#pragma once

#include "sema/objc3_parser_sema_handoff_scaffold_model.h"

inline Objc3ParserSemaConformanceEvidenceRecord
BuildObjc3ParserSemaHandoffScaffoldConformanceEvidenceRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffScaffold &scaffold) {
  Objc3SemaParityContractSurface surface;
  surface.parser_sema_conformance_matrix =
      scaffold.parser_sema_conformance_matrix;
  surface.parser_sema_conformance_corpus =
      scaffold.parser_sema_conformance_corpus;
  surface.deterministic_parser_sema_conformance_matrix =
      scaffold.parser_sema_conformance_matrix.deterministic;
  surface.deterministic_parser_sema_conformance_corpus =
      scaffold.parser_sema_conformance_corpus.deterministic;
  return BuildObjc3ParserSemaConformanceEvidenceRecord(input, surface);
}

inline Objc3SemaParityContractSurface
BuildObjc3ParserSemaHandoffScaffoldContractSurface(
    const Objc3ParserSemaHandoffScaffold &scaffold) {
  Objc3SemaParityContractSurface surface;
  surface.parser_sema_conformance_matrix =
      scaffold.parser_sema_conformance_matrix;
  surface.parser_sema_conformance_corpus =
      scaffold.parser_sema_conformance_corpus;
  surface.parser_sema_conformance_evidence_record =
      scaffold.parser_sema_conformance_evidence_record;
  surface.parser_sema_performance_quality_guardrails =
      scaffold.parser_sema_performance_quality_guardrails;
  surface.parser_sema_cross_lane_integration_sync =
      scaffold.parser_sema_cross_lane_integration_sync;
  surface.parser_sema_docs_runbook_sync =
      scaffold.parser_sema_docs_runbook_sync;
  surface.parser_sema_release_candidate_replay_dry_run =
      scaffold.parser_sema_release_candidate_replay_dry_run;
  surface.parser_sema_advanced_core_shard1 =
      scaffold.parser_sema_advanced_core_shard1;
  surface.parser_sema_advanced_contract_rejection_shard1 =
      scaffold.parser_sema_advanced_contract_rejection_shard1;
  surface.parser_sema_advanced_diagnostics_shard1 =
      scaffold.parser_sema_advanced_diagnostics_shard1;
  surface.parser_sema_advanced_conformance_shard1 =
      scaffold.parser_sema_advanced_conformance_shard1;
  surface.parser_sema_advanced_integration_shard1 =
      scaffold.parser_sema_advanced_integration_shard1;
  surface.parser_sema_advanced_performance_shard1 =
      scaffold.parser_sema_advanced_performance_shard1;
  surface.parser_sema_advanced_core_shard2 =
      scaffold.parser_sema_advanced_core_shard2;
  surface.parser_sema_advanced_contract_rejection_shard2 =
      scaffold.parser_sema_advanced_contract_rejection_shard2;
  surface.parser_sema_advanced_diagnostics_shard2 =
      scaffold.parser_sema_advanced_diagnostics_shard2;
  surface.parser_sema_integration_closeout_signoff =
      scaffold.parser_sema_integration_closeout_signoff;
  surface.deterministic_parser_sema_conformance_matrix =
      scaffold.parser_sema_conformance_matrix.deterministic;
  surface.deterministic_parser_sema_conformance_corpus =
      scaffold.parser_sema_conformance_corpus.deterministic;
  surface.deterministic_parser_sema_conformance_evidence_record =
      scaffold.deterministic_parser_sema_conformance_evidence_record;
  surface.deterministic_parser_sema_performance_quality_guardrails =
      scaffold.parser_sema_performance_quality_guardrails.deterministic;
  surface.deterministic_parser_sema_cross_lane_integration_sync =
      scaffold.parser_sema_cross_lane_integration_sync.deterministic;
  surface.deterministic_parser_sema_docs_runbook_sync =
      scaffold.parser_sema_docs_runbook_sync.deterministic;
  surface.deterministic_parser_sema_release_candidate_replay_dry_run =
      scaffold.parser_sema_release_candidate_replay_dry_run.deterministic;
  surface.deterministic_parser_sema_advanced_core_shard1 =
      scaffold.parser_sema_advanced_core_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_contract_rejection_shard1 =
      scaffold.parser_sema_advanced_contract_rejection_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_diagnostics_shard1 =
      scaffold.parser_sema_advanced_diagnostics_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_conformance_shard1 =
      scaffold.parser_sema_advanced_conformance_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_integration_shard1 =
      scaffold.parser_sema_advanced_integration_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_performance_shard1 =
      scaffold.parser_sema_advanced_performance_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_core_shard2 =
      scaffold.parser_sema_advanced_core_shard2.deterministic;
  surface.deterministic_parser_sema_advanced_contract_rejection_shard2 =
      scaffold.parser_sema_advanced_contract_rejection_shard2.deterministic;
  surface.deterministic_parser_sema_advanced_diagnostics_shard2 =
      scaffold.parser_sema_advanced_diagnostics_shard2.deterministic;
  surface.deterministic_parser_sema_integration_closeout_signoff =
      scaffold.parser_sema_integration_closeout_signoff.deterministic;
  return surface;
}

inline Objc3ParserSemaHandoffScaffoldReadinessRecord
BuildObjc3ParserSemaHandoffScaffoldReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffScaffold &scaffold) {
  Objc3ParserSemaHandoffScaffoldReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.parser_sema_contract_handoff_owner =
      scaffold.owner_record.parser_sema_contract_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.owner_record_ready =
      IsReadyObjc3ParserSemaHandoffOwnerRecord(scaffold.owner_record);
  record.snapshot_evidence_ready =
      scaffold.parser_contract_ast_shape_fingerprint_matches &&
      scaffold.parser_contract_ast_top_level_layout_fingerprint_matches &&
      scaffold.parser_contract_snapshot_fingerprint_matches &&
      scaffold.parser_contract_snapshot_matches_program;
  record.snapshot_normalization_ready =
      !scaffold.parser_contract_snapshot_normalization_rejected;
  record.canonical_rejection_ready =
      scaffold.canonical_literal_rejection_handoff.deterministic;
  record.conformance_evidence_ready =
      scaffold.deterministic_parser_sema_conformance_evidence_record &&
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          scaffold.parser_sema_conformance_evidence_record);
  record.parser_contract_readiness_ready =
      scaffold.deterministic_parser_sema_contract_readiness_record &&
      IsReadyObjc3ParserSemaContractReadinessRecord(
          scaffold.parser_sema_contract_readiness_record);
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.handoff_scaffold_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_readiness_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.owner_record_ready && record.snapshot_evidence_ready &&
      record.snapshot_normalization_ready &&
      record.canonical_rejection_ready &&
      record.conformance_evidence_ready &&
      record.parser_contract_readiness_ready;
  return record;
}

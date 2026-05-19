#include "sema/objc3_sema_pass_flow_publication.h"

#include "sema/objc3_sema_pass_flow_scaffold.h"

void PublishObjc3SemaPassFlowSummary(
    const Objc3SemaParserHandoffPublication &handoff_publication,
    const Objc3SemaPassDiagnosticsRun &diagnostics_run,
    Objc3SemaPassManagerResult &result) {
  const Objc3ParserSemaHandoffPublicationEvidenceRecord &handoff_evidence =
      handoff_publication.evidence_record;
  result.sema_pass_flow_summary.diagnostics_after_pass =
      result.diagnostics_after_pass;
  result.sema_pass_flow_summary.diagnostics_emitted_by_pass =
      result.diagnostics_emitted_by_pass;
  result.sema_pass_flow_summary.diagnostics_accounting_consistent =
      result.diagnostics_accounting_consistent;
  result.sema_pass_flow_summary.diagnostics_bus_publish_consistent =
      result.diagnostics_bus_publish_consistent;
  result.sema_pass_flow_summary.diagnostics_canonicalized =
      result.diagnostics_canonicalized;
  result.sema_pass_flow_summary.diagnostics_hardening_satisfied =
      result.diagnostics_hardening_satisfied;
  result.sema_pass_flow_summary.parser_recovery_replay_ready =
      handoff_evidence.parser_recovery_replay_ready;
  result.sema_pass_flow_summary.parser_recovery_replay_case_present =
      handoff_evidence.parser_recovery_replay_case_present;
  result.sema_pass_flow_summary.parser_recovery_replay_case_passed =
      handoff_evidence.parser_recovery_replay_case_passed;
  result.sema_pass_flow_summary.recovery_replay_contract_satisfied =
      handoff_evidence.parser_recovery_replay_contract_satisfied;
  result.sema_pass_flow_summary.recovery_replay_key =
      handoff_evidence.recovery_replay_key;
  result.sema_pass_flow_summary.recovery_replay_key_deterministic =
      handoff_evidence.recovery_replay_key_deterministic;
  result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied =
      handoff_evidence.recovery_determinism_hardening_satisfied;

  FinalizeObjc3SemaPassFlowSummary(
      result.sema_pass_flow_summary,
      result.integration_surface,
      result.type_metadata_handoff,
      diagnostics_run.diagnostics_after_pass_monotonic,
      diagnostics_run.pass_order_matches_contract &&
          diagnostics_run.pass_iteration_count == kObjc3SemaPassOrder.size(),
      result.deterministic_semantic_diagnostics,
      result.deterministic_type_metadata_handoff);

  result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied =
      result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied &&
      result.sema_pass_flow_summary.diagnostics_hardening_satisfied &&
      result.sema_pass_flow_summary.robustness_guardrails_satisfied;

  result.diagnostics_publication_record =
      BuildObjc3SemaDiagnosticsPublicationRecord(
          result.sema_pass_flow_summary,
          result.deterministic_semantic_diagnostics);
  result.deterministic_diagnostics_publication_record =
      IsReadyObjc3SemaDiagnosticsPublicationRecord(
          result.diagnostics_publication_record);

  result.pass_flow_recovery_record = BuildObjc3SemaPassFlowRecoveryRecord(
      result.sema_pass_flow_summary,
      result.diagnostics_publication_record);
  result.deterministic_pass_flow_recovery_record =
      IsReadyObjc3SemaPassFlowRecoveryRecord(
          result.pass_flow_recovery_record);

  result.pass_manager_publication_record =
      BuildObjc3SemaPassManagerPublicationRecord(
          handoff_publication.owner_record,
          result.sema_pass_flow_summary,
          result.diagnostics_publication_record,
          result.deterministic_type_metadata_handoff);
  result.deterministic_pass_manager_publication_record =
      IsReadyObjc3SemaPassManagerPublicationRecord(
          result.pass_manager_publication_record);
}

#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaParityCloseoutPublicationReadinessRecord
BuildObjc3SemaParityCloseoutPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    bool pass_manager_executed,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaParityCloseoutReadinessInputs readiness =
      BuildObjc3SemaParityCloseoutReadinessInputs(surface);
  Objc3SemaParityCloseoutPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.pass_manager_executed = pass_manager_executed;
  record.parser_sema_contract_ready = readiness.parser_sema_contract_ready;
  record.pass_flow_summary_ready = readiness.pass_flow_summary_ready;
  record.publication_records_ready = readiness.publication_records_ready;
  record.diagnostics_publication_ready =
      readiness.diagnostics_publication_ready;
  record.pass_flow_recovery_ready = readiness.pass_flow_recovery_ready;
  record.type_metadata_cardinality_ready =
      readiness.type_metadata_cardinality_ready;
  record.typed_semantic_handoffs_ready =
      readiness.typed_semantic_handoffs_ready;
  record.mapping_summaries_ready = readiness.mapping_summaries_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parity_closeout_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.type_metadata_mapping_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.typed_semantic_handoff_publication_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.pass_manager_executed &&
      IsReadyObjc3SemaParityCloseoutReadinessInputs(readiness);
  return record;
}

Objc3SemaCloseoutSurfaceReadinessRecord
BuildObjc3SemaCloseoutSurfaceReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaCloseoutSurfaceReadinessInputs readiness =
      BuildObjc3SemaCloseoutSurfaceReadinessInputs(surface);
  Objc3SemaCloseoutSurfaceReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.parser_sema_contract_ready = readiness.parser_sema_contract_ready;
  record.parser_sema_conformance_evidence_ready =
      readiness.parser_sema_conformance_evidence_ready;
  record.diagnostics_publication_ready =
      readiness.diagnostics_publication_ready;
  record.pass_flow_recovery_ready = readiness.pass_flow_recovery_ready;
  record.pass_manager_publication_ready =
      readiness.pass_manager_publication_ready;
  record.type_metadata_publication_ready =
      readiness.type_metadata_publication_ready;
  record.type_metadata_mapping_ready = readiness.type_metadata_mapping_ready;
  record.typed_semantic_handoff_ready =
      readiness.typed_semantic_handoff_ready;
  record.parity_closeout_publication_ready =
      readiness.parity_closeout_publication_ready;
  record.parity_validation_ready = readiness.parity_validation_ready;
  record.core_semantic_publication_ready =
      readiness.core_semantic_publication_ready;
  record.module_semantic_publication_ready =
      readiness.module_semantic_publication_ready;
  record.intermodule_flow_publication_ready =
      readiness.intermodule_flow_publication_ready;
  record.concurrency_publication_ready = readiness.concurrency_publication_ready;
  record.unsafe_error_validation_ready =
      readiness.unsafe_error_validation_ready;
  record.control_binding_validation_ready =
      readiness.control_binding_validation_ready;
  record.async_block_message_validation_ready =
      readiness.async_block_message_validation_ready;
  record.dispatch_runtime_arc_validation_ready =
      readiness.dispatch_runtime_arc_validation_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.closeout_surface_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parity_closeout_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      IsReadyObjc3SemaCloseoutSurfaceReadinessInputs(readiness);
  return record;
}

Objc3SemaCloseoutSignoffRecord BuildObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaParityCloseoutPublicationReadinessRecord
      &closeout_readiness =
          surface.parity_closeout_publication_readiness_record;
  return BuildObjc3SemaCloseoutSignoffRecord(
      input,
      surface.ready && surface.deterministic_parity_validation_record &&
          IsReadyObjc3SemaParityValidationRecord(
              surface.parity_validation_record),
      closeout_readiness.parser_sema_contract_ready,
      surface.deterministic_pass_manager_publication_record &&
          IsReadyObjc3SemaPassManagerPublicationRecord(
              surface.pass_manager_publication_record),
      surface.deterministic_type_metadata_publication_record &&
          IsReadyObjc3SemaTypeMetadataPublicationRecord(
              surface.type_metadata_publication_record),
      closeout_readiness.diagnostics_publication_ready,
      closeout_readiness.pass_flow_recovery_ready,
      closeout_readiness.mapping_summaries_ready,
      closeout_readiness.typed_semantic_handoffs_ready);
}

bool IsReadyObjc3SemaParityContractSurface(
    const Objc3SemaParityContractSurface &surface) {
  return IsReadyObjc3SemaParityContractSurfaceReadinessGates(
      BuildObjc3SemaParityContractSurfaceReadinessGates(surface));
}

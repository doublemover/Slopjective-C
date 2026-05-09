struct Objc3SemaParityCloseoutReadinessInputs {
  bool parser_sema_contract_ready = false;
  bool pass_flow_summary_ready = false;
  bool publication_records_ready = false;
  bool diagnostics_publication_ready = false;
  bool pass_flow_recovery_ready = false;
  bool type_metadata_cardinality_ready = false;
  bool typed_semantic_handoffs_ready = false;
  bool mapping_summaries_ready = false;
};

inline Objc3SemaParityCloseoutReadinessInputs
BuildObjc3SemaParityCloseoutReadinessInputs(
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaParityCloseoutReadinessInputs inputs;
  inputs.parser_sema_contract_ready =
      surface.deterministic_parser_sema_contract_readiness_record &&
      IsReadyObjc3ParserSemaContractReadinessRecord(
          surface.parser_sema_contract_readiness_record);
  inputs.pass_flow_summary_ready =
      IsReadyObjc3SemaPassFlowSummary(surface.sema_pass_flow_summary);
  inputs.publication_records_ready =
      surface.deterministic_pass_manager_publication_record &&
      IsReadyObjc3SemaPassManagerPublicationRecord(
          surface.pass_manager_publication_record) &&
      surface.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          surface.type_metadata_publication_record);
  inputs.diagnostics_publication_ready =
      surface.deterministic_diagnostics_publication_record &&
      IsReadyObjc3SemaDiagnosticsPublicationRecord(
          surface.diagnostics_publication_record);
  inputs.pass_flow_recovery_ready =
      surface.deterministic_pass_flow_recovery_record &&
      IsReadyObjc3SemaPassFlowRecoveryRecord(surface.pass_flow_recovery_record);
  inputs.type_metadata_cardinality_ready =
      surface.deterministic_type_metadata_mapping_readiness_record &&
      surface.type_metadata_mapping_readiness_record
          .type_metadata_handoff_ready &&
      surface.type_metadata_mapping_readiness_record.cardinality_consistent;
  inputs.typed_semantic_handoffs_ready =
      surface.deterministic_typed_semantic_handoff_record &&
      IsReadyObjc3SemaTypedSemanticHandoffRecord(
          surface.typed_semantic_handoff_record);
  inputs.mapping_summaries_ready =
      surface.deterministic_type_metadata_mapping_readiness_record &&
      IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
          surface.type_metadata_mapping_readiness_record);
  return inputs;
}

inline bool IsReadyObjc3SemaParityCloseoutReadinessInputs(
    const Objc3SemaParityCloseoutReadinessInputs &inputs) {
  return inputs.parser_sema_contract_ready &&
         inputs.pass_flow_summary_ready && inputs.publication_records_ready &&
         inputs.diagnostics_publication_ready &&
         inputs.pass_flow_recovery_ready &&
         inputs.type_metadata_cardinality_ready &&
         inputs.typed_semantic_handoffs_ready &&
         inputs.mapping_summaries_ready;
}

struct Objc3SemaCloseoutSurfaceReadinessInputs {
  bool parser_sema_contract_ready = false;
  bool parser_sema_conformance_evidence_ready = false;
  bool diagnostics_publication_ready = false;
  bool pass_flow_recovery_ready = false;
  bool pass_manager_publication_ready = false;
  bool type_metadata_publication_ready = false;
  bool type_metadata_mapping_ready = false;
  bool typed_semantic_handoff_ready = false;
  bool parity_closeout_publication_ready = false;
  bool parity_validation_ready = false;
  bool core_semantic_publication_ready = false;
  bool module_semantic_publication_ready = false;
  bool intermodule_flow_publication_ready = false;
  bool concurrency_publication_ready = false;
  bool unsafe_error_validation_ready = false;
  bool control_binding_validation_ready = false;
  bool async_block_message_validation_ready = false;
  bool dispatch_runtime_arc_validation_ready = false;
};

inline Objc3SemaCloseoutSurfaceReadinessInputs
BuildObjc3SemaCloseoutSurfaceReadinessInputs(
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaCloseoutSurfaceReadinessInputs inputs;
  inputs.parser_sema_contract_ready =
      surface.deterministic_parser_sema_contract_readiness_record &&
      IsReadyObjc3ParserSemaContractReadinessRecord(
          surface.parser_sema_contract_readiness_record);
  inputs.parser_sema_conformance_evidence_ready =
      surface.deterministic_parser_sema_conformance_evidence_record &&
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          surface.parser_sema_conformance_evidence_record);
  inputs.diagnostics_publication_ready =
      surface.deterministic_diagnostics_publication_record &&
      IsReadyObjc3SemaDiagnosticsPublicationRecord(
          surface.diagnostics_publication_record);
  inputs.pass_flow_recovery_ready =
      surface.deterministic_pass_flow_recovery_record &&
      IsReadyObjc3SemaPassFlowRecoveryRecord(surface.pass_flow_recovery_record);
  inputs.pass_manager_publication_ready =
      surface.deterministic_pass_manager_publication_record &&
      IsReadyObjc3SemaPassManagerPublicationRecord(
          surface.pass_manager_publication_record);
  inputs.type_metadata_publication_ready =
      surface.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          surface.type_metadata_publication_record);
  inputs.type_metadata_mapping_ready =
      surface.deterministic_type_metadata_mapping_readiness_record &&
      IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
          surface.type_metadata_mapping_readiness_record);
  inputs.typed_semantic_handoff_ready =
      surface.deterministic_typed_semantic_handoff_record &&
      IsReadyObjc3SemaTypedSemanticHandoffRecord(
          surface.typed_semantic_handoff_record);
  inputs.parity_closeout_publication_ready =
      surface.deterministic_parity_closeout_publication_readiness_record &&
      IsReadyObjc3SemaParityCloseoutPublicationReadinessRecord(
          surface.parity_closeout_publication_readiness_record);
  inputs.parity_validation_ready =
      surface.deterministic_parity_validation_record &&
      IsReadyObjc3SemaParityValidationRecord(surface.parity_validation_record);
  inputs.core_semantic_publication_ready =
      surface.deterministic_core_semantic_parity_publication_readiness_record &&
      IsReadyObjc3SemaCoreSemanticParityPublicationReadinessRecord(
          surface.core_semantic_parity_publication_readiness_record);
  inputs.module_semantic_publication_ready =
      surface.deterministic_module_semantic_parity_publication_readiness_record &&
      IsReadyObjc3SemaModuleSemanticParityPublicationReadinessRecord(
          surface.module_semantic_parity_publication_readiness_record);
  inputs.intermodule_flow_publication_ready =
      surface
          .deterministic_intermodule_flow_parity_publication_readiness_record &&
      IsReadyObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
          surface.intermodule_flow_parity_publication_readiness_record);
  inputs.concurrency_publication_ready =
      surface.deterministic_concurrency_parity_publication_readiness_record &&
      IsReadyObjc3SemaConcurrencyParityPublicationReadinessRecord(
          surface.concurrency_parity_publication_readiness_record);
  inputs.unsafe_error_validation_ready =
      surface.deterministic_unsafe_error_parity_validation_readiness_record &&
      IsReadyObjc3SemaUnsafeErrorParityValidationReadinessRecord(
          surface.unsafe_error_parity_validation_readiness_record);
  inputs.control_binding_validation_ready =
      surface.deterministic_control_binding_parity_validation_readiness_record &&
      IsReadyObjc3SemaControlBindingParityValidationReadinessRecord(
          surface.control_binding_parity_validation_readiness_record);
  inputs.async_block_message_validation_ready =
      surface
          .deterministic_async_block_message_parity_validation_readiness_record &&
      IsReadyObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
          surface.async_block_message_parity_validation_readiness_record);
  inputs.dispatch_runtime_arc_validation_ready =
      surface
          .deterministic_dispatch_runtime_arc_parity_validation_readiness_record &&
      IsReadyObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
          surface.dispatch_runtime_arc_parity_validation_readiness_record);
  return inputs;
}

inline bool IsReadyObjc3SemaCloseoutSurfaceReadinessInputs(
    const Objc3SemaCloseoutSurfaceReadinessInputs &inputs) {
  return inputs.parser_sema_contract_ready &&
         inputs.parser_sema_conformance_evidence_ready &&
         inputs.diagnostics_publication_ready &&
         inputs.pass_flow_recovery_ready &&
         inputs.pass_manager_publication_ready &&
         inputs.type_metadata_publication_ready &&
         inputs.type_metadata_mapping_ready &&
         inputs.typed_semantic_handoff_ready &&
         inputs.parity_closeout_publication_ready &&
         inputs.parity_validation_ready &&
         inputs.core_semantic_publication_ready &&
         inputs.module_semantic_publication_ready &&
         inputs.intermodule_flow_publication_ready &&
         inputs.concurrency_publication_ready &&
         inputs.unsafe_error_validation_ready &&
         inputs.control_binding_validation_ready &&
         inputs.async_block_message_validation_ready &&
         inputs.dispatch_runtime_arc_validation_ready;
}

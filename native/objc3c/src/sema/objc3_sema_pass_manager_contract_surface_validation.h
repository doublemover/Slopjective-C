             surface.deterministic_type_boundary_summary_readiness_record &&
         IsReadyObjc3SemaTypeBoundarySummaryReadinessRecord(
             surface.type_boundary_summary_readiness_record) &&
         surface.deterministic_module_type_abi_summary_readiness_record &&
         IsReadyObjc3SemaModuleTypeAbiSummaryReadinessRecord(
             surface.module_type_abi_summary_readiness_record) &&
         surface.deterministic_module_boundary_summary_readiness_record &&
         IsReadyObjc3SemaModuleBoundarySummaryReadinessRecord(
             surface.module_boundary_summary_readiness_record) &&
         surface.deterministic_intermodule_flow_summary_readiness_record &&
         IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
             surface.intermodule_flow_summary_readiness_record) &&
         surface.deterministic_concurrency_parity_publication_readiness_record &&
         IsReadyObjc3SemaConcurrencyParityPublicationReadinessRecord(
             surface.concurrency_parity_publication_readiness_record) &&
         surface.deterministic_unsafe_error_parity_validation_readiness_record &&
         IsReadyObjc3SemaUnsafeErrorParityValidationReadinessRecord(
             surface.unsafe_error_parity_validation_readiness_record) &&
         surface.deterministic_control_binding_parity_validation_readiness_record &&
         IsReadyObjc3SemaControlBindingParityValidationReadinessRecord(
             surface.control_binding_parity_validation_readiness_record) &&
         surface.deterministic_async_block_message_parity_validation_readiness_record &&
         IsReadyObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
             surface.async_block_message_parity_validation_readiness_record) &&
         surface.deterministic_dispatch_runtime_arc_parity_validation_readiness_record &&
         IsReadyObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
             surface.dispatch_runtime_arc_parity_validation_readiness_record);
}

struct Objc3SemaPassManagerResult {
  Objc3ParserContractSnapshot parser_contract_snapshot;
  bool deterministic_parser_sema_handoff = false;
  Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;
  bool deterministic_parser_sema_conformance_matrix = false;
  Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;
  bool deterministic_parser_sema_conformance_corpus = false;
  Objc3ParserSemaPerformanceQualityGuardrails parser_sema_performance_quality_guardrails;
  bool deterministic_parser_sema_performance_quality_guardrails = false;
  Objc3ParserSemaCrossLaneIntegrationSync parser_sema_cross_lane_integration_sync;
  bool deterministic_parser_sema_cross_lane_integration_sync = false;

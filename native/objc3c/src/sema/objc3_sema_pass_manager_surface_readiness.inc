struct Objc3SemaParityContractSurfaceReadinessGates {
  bool surface_ready = false;
  bool parity_closeout_publication_ready = false;
  bool parity_validation_ready = false;
  bool closeout_surface_ready = false;
  bool closeout_signoff_ready = false;
  bool bootstrap_legality_failure_ready = false;
  bool bootstrap_legality_semantics_ready = false;
  bool bootstrap_failure_restart_ready = false;
  bool compatibility_strictness_claim_ready = false;
  bool core_semantic_summary_ready = false;
  bool selector_property_type_annotation_ready = false;
  bool type_boundary_summary_ready = false;
  bool module_type_abi_summary_ready = false;
  bool module_boundary_summary_ready = false;
  bool intermodule_flow_summary_ready = false;
};

inline Objc3SemaParityContractSurfaceReadinessGates
BuildObjc3SemaParityContractSurfaceReadinessGates(
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaParityContractSurfaceReadinessGates gates;
  gates.surface_ready = surface.ready;
  gates.parity_closeout_publication_ready =
      surface.deterministic_parity_closeout_publication_readiness_record &&
      IsReadyObjc3SemaParityCloseoutPublicationReadinessRecord(
          surface.parity_closeout_publication_readiness_record);
  gates.parity_validation_ready =
      surface.deterministic_parity_validation_record &&
      IsReadyObjc3SemaParityValidationRecord(
          surface.parity_validation_record);
  gates.closeout_surface_ready =
      surface.deterministic_closeout_surface_readiness_record &&
      IsReadyObjc3SemaCloseoutSurfaceReadinessRecord(
          surface.closeout_surface_readiness_record);
  gates.closeout_signoff_ready =
      surface.deterministic_closeout_signoff_record &&
      IsReadyObjc3SemaCloseoutSignoffRecord(surface.closeout_signoff_record);
  gates.bootstrap_legality_failure_ready =
      IsReadyObjc3BootstrapLegalityFailureContractSummary(
          surface.bootstrap_legality_failure_contract_summary);
  gates.bootstrap_legality_semantics_ready =
      IsReadyObjc3BootstrapLegalitySemanticsSummary(
          surface.bootstrap_legality_semantics_summary);
  gates.bootstrap_failure_restart_ready =
      IsReadyObjc3BootstrapFailureRestartSemanticsSummary(
          surface.bootstrap_failure_restart_semantics_summary);
  gates.compatibility_strictness_claim_ready =
      IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(
          surface.compatibility_strictness_claim_semantics_summary);
  gates.core_semantic_summary_ready =
      surface.deterministic_core_semantic_summary_readiness_record &&
      IsReadyObjc3SemaCoreSemanticSummaryReadinessRecord(
          surface.core_semantic_summary_readiness_record);
  gates.selector_property_type_annotation_ready =
      surface
          .deterministic_selector_property_type_annotation_readiness_record &&
      IsReadyObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
          surface.selector_property_type_annotation_readiness_record);
  gates.type_boundary_summary_ready =
      surface.deterministic_type_boundary_summary_readiness_record &&
      IsReadyObjc3SemaTypeBoundarySummaryReadinessRecord(
          surface.type_boundary_summary_readiness_record);
  gates.module_type_abi_summary_ready =
      surface.deterministic_module_type_abi_summary_readiness_record &&
      IsReadyObjc3SemaModuleTypeAbiSummaryReadinessRecord(
          surface.module_type_abi_summary_readiness_record);
  gates.module_boundary_summary_ready =
      surface.deterministic_module_boundary_summary_readiness_record &&
      IsReadyObjc3SemaModuleBoundarySummaryReadinessRecord(
          surface.module_boundary_summary_readiness_record);
  gates.intermodule_flow_summary_ready =
      surface.deterministic_intermodule_flow_summary_readiness_record &&
      IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
          surface.intermodule_flow_summary_readiness_record);
  return gates;
}

inline bool IsReadyObjc3SemaParityContractSurfaceReadinessGates(
    const Objc3SemaParityContractSurfaceReadinessGates &gates) {
  return gates.surface_ready && gates.parity_closeout_publication_ready &&
         gates.parity_validation_ready && gates.closeout_surface_ready &&
         gates.closeout_signoff_ready &&
         gates.bootstrap_legality_failure_ready &&
         gates.bootstrap_legality_semantics_ready &&
         gates.bootstrap_failure_restart_ready &&
         gates.compatibility_strictness_claim_ready &&
         gates.core_semantic_summary_ready &&
         gates.selector_property_type_annotation_ready &&
         gates.type_boundary_summary_ready &&
         gates.module_type_abi_summary_ready &&
         gates.module_boundary_summary_ready &&
         gates.intermodule_flow_summary_ready;
}

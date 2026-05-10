  result.intermodule_flow_summary_readiness_record =
      BuildObjc3SemaIntermoduleFlowSummaryReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_cross_module_conformance_handoff,
          result.deterministic_throws_propagation_handoff);
  result.deterministic_intermodule_flow_summary_readiness_record =
      IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
          result.intermodule_flow_summary_readiness_record);
  result.parity_surface.intermodule_flow_summary_readiness_record =
      result.intermodule_flow_summary_readiness_record;
  result.parity_surface
      .deterministic_intermodule_flow_summary_readiness_record =
      result.deterministic_intermodule_flow_summary_readiness_record;
  result.parity_surface.deterministic_cross_module_conformance_handoff =
      result.parity_surface.intermodule_flow_summary_readiness_record
          .cross_module_conformance_ready;
  result.parity_surface.deterministic_throws_propagation_handoff =
      result.parity_surface.intermodule_flow_summary_readiness_record
          .throws_propagation_ready;
  result.parity_surface.ready =
      result.parity_surface
          .deterministic_parity_closeout_publication_readiness_record &&
      IsReadyObjc3SemaParityCloseoutPublicationReadinessRecord(
          result.parity_surface
              .parity_closeout_publication_readiness_record) &&
      result.parity_surface.deterministic_parity_validation_record &&
      IsReadyObjc3SemaParityValidationRecord(
          result.parity_surface.parity_validation_record) &&
      result.parity_surface
          .deterministic_core_semantic_summary_readiness_record &&
      IsReadyObjc3SemaCoreSemanticSummaryReadinessRecord(
          result.parity_surface.core_semantic_summary_readiness_record) &&
      result.parity_surface
          .deterministic_selector_property_type_annotation_readiness_record &&
      IsReadyObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
          result.parity_surface
              .selector_property_type_annotation_readiness_record) &&
      result.parity_surface
          .deterministic_type_boundary_summary_readiness_record &&
      IsReadyObjc3SemaTypeBoundarySummaryReadinessRecord(
          result.parity_surface.type_boundary_summary_readiness_record) &&
      result.parity_surface
          .deterministic_module_type_abi_summary_readiness_record &&
      IsReadyObjc3SemaModuleTypeAbiSummaryReadinessRecord(
          result.parity_surface.module_type_abi_summary_readiness_record) &&
      result.parity_surface
          .deterministic_module_boundary_summary_readiness_record &&
      IsReadyObjc3SemaModuleBoundarySummaryReadinessRecord(
          result.parity_surface.module_boundary_summary_readiness_record) &&
      result.parity_surface
          .deterministic_intermodule_flow_summary_readiness_record &&
      IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
          result.parity_surface.intermodule_flow_summary_readiness_record);

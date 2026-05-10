  result.closeout_surface_readiness_record =
      BuildObjc3SemaCloseoutSurfaceReadinessRecord(
          input, result.parity_surface);
  result.deterministic_closeout_surface_readiness_record =
      IsReadyObjc3SemaCloseoutSurfaceReadinessRecord(
          result.closeout_surface_readiness_record);
  result.parity_surface.closeout_surface_readiness_record =
      result.closeout_surface_readiness_record;
  result.parity_surface.deterministic_closeout_surface_readiness_record =
      result.deterministic_closeout_surface_readiness_record;
  result.core_semantic_summary_readiness_record =
      BuildObjc3SemaCoreSemanticSummaryReadinessRecord(
          input, result.parity_surface);
  result.deterministic_core_semantic_summary_readiness_record =
      IsReadyObjc3SemaCoreSemanticSummaryReadinessRecord(
          result.core_semantic_summary_readiness_record);
  result.parity_surface.core_semantic_summary_readiness_record =
      result.core_semantic_summary_readiness_record;
  result.parity_surface.deterministic_core_semantic_summary_readiness_record =
      result.deterministic_core_semantic_summary_readiness_record;
  result.selector_property_type_annotation_readiness_record =
      BuildObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
          input, result.parity_surface);
  result.deterministic_selector_property_type_annotation_readiness_record =
      IsReadyObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
          result.selector_property_type_annotation_readiness_record);
  result.parity_surface.selector_property_type_annotation_readiness_record =
      result.selector_property_type_annotation_readiness_record;
  result.parity_surface
      .deterministic_selector_property_type_annotation_readiness_record =
      result.deterministic_selector_property_type_annotation_readiness_record;
  result.type_boundary_summary_readiness_record =
      BuildObjc3SemaTypeBoundarySummaryReadinessRecord(
          input, result.parity_surface);
  result.deterministic_type_boundary_summary_readiness_record =
      IsReadyObjc3SemaTypeBoundarySummaryReadinessRecord(
          result.type_boundary_summary_readiness_record);
  result.parity_surface.type_boundary_summary_readiness_record =
      result.type_boundary_summary_readiness_record;
  result.parity_surface.deterministic_type_boundary_summary_readiness_record =
      result.deterministic_type_boundary_summary_readiness_record;
  result.module_type_abi_summary_readiness_record =
      BuildObjc3SemaModuleTypeAbiSummaryReadinessRecord(
          input, result.parity_surface);
  result.deterministic_module_type_abi_summary_readiness_record =
      IsReadyObjc3SemaModuleTypeAbiSummaryReadinessRecord(
          result.module_type_abi_summary_readiness_record);
  result.parity_surface.module_type_abi_summary_readiness_record =
      result.module_type_abi_summary_readiness_record;
  result.parity_surface.deterministic_module_type_abi_summary_readiness_record =
      result.deterministic_module_type_abi_summary_readiness_record;
  result.module_boundary_summary_readiness_record =
      BuildObjc3SemaModuleBoundarySummaryReadinessRecord(
          input, result.parity_surface);
  result.deterministic_module_boundary_summary_readiness_record =
      IsReadyObjc3SemaModuleBoundarySummaryReadinessRecord(
          result.module_boundary_summary_readiness_record);
  result.parity_surface.module_boundary_summary_readiness_record =
      result.module_boundary_summary_readiness_record;
  result.parity_surface
      .deterministic_module_boundary_summary_readiness_record =
      result.deterministic_module_boundary_summary_readiness_record;

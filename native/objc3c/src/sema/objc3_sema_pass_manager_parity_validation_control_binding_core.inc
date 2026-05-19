  result.control_binding_parity_validation_readiness_record =
      BuildObjc3SemaControlBindingParityValidationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_unwind_cleanup_handoff,
          result.deterministic_async_continuation_handoff,
          result.deterministic_symbol_graph_scope_resolution_handoff,
          result.deterministic_method_lookup_override_conflict_handoff,
          result.deterministic_property_synthesis_ivar_binding_handoff,
          result.deterministic_id_class_sel_object_pointer_type_checking_handoff);
  result.deterministic_control_binding_parity_validation_readiness_record =
      IsReadyObjc3SemaControlBindingParityValidationReadinessRecord(
          result.control_binding_parity_validation_readiness_record);
  result.parity_surface.control_binding_parity_validation_readiness_record =
      result.control_binding_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_control_binding_parity_validation_readiness_record =
      result.deterministic_control_binding_parity_validation_readiness_record;
  result.parity_surface.deterministic_unwind_cleanup_handoff =
      result.parity_surface.control_binding_parity_validation_readiness_record
          .unwind_cleanup_ready;
  result.parity_surface.deterministic_async_continuation_handoff =
      result.parity_surface.control_binding_parity_validation_readiness_record
          .async_continuation_ready;

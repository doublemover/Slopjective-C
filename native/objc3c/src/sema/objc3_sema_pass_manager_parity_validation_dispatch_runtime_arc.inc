  result.dispatch_runtime_arc_parity_validation_readiness_record =
      BuildObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_dispatch_abi_marshalling_handoff,
          result.deterministic_nil_receiver_semantics_foldability_handoff,
          result.deterministic_super_dispatch_method_family_handoff,
          result.deterministic_runtime_link_host_link_handoff,
          result.deterministic_retain_release_operation_handoff,
          result.deterministic_weak_unowned_semantics_handoff,
          result.deterministic_arc_diagnostics_fixit_handoff,
          result.deterministic_autoreleasepool_scope_handoff);
  result.deterministic_dispatch_runtime_arc_parity_validation_readiness_record =
      IsReadyObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
          result.dispatch_runtime_arc_parity_validation_readiness_record);
  result.parity_surface.dispatch_runtime_arc_parity_validation_readiness_record =
      result.dispatch_runtime_arc_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_dispatch_runtime_arc_parity_validation_readiness_record =
      result.deterministic_dispatch_runtime_arc_parity_validation_readiness_record;
  result.parity_surface.deterministic_dispatch_abi_marshalling_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .dispatch_abi_marshalling_ready;
  result.parity_surface.deterministic_nil_receiver_semantics_foldability_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .nil_receiver_semantics_foldability_ready;
  result.parity_surface.deterministic_super_dispatch_method_family_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .super_dispatch_method_family_ready;
  result.parity_surface.deterministic_runtime_link_host_link_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .runtime_link_host_link_ready;
  result.parity_surface.deterministic_retain_release_operation_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .retain_release_operation_ready;
  result.parity_surface.deterministic_weak_unowned_semantics_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .weak_unowned_semantics_ready;
  result.parity_surface.deterministic_arc_diagnostics_fixit_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .arc_diagnostics_fixit_ready;
  result.parity_surface.deterministic_autoreleasepool_scope_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .autoreleasepool_scope_ready;

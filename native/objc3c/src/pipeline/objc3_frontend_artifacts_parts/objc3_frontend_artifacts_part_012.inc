    AccumulateBlockSourceStorageAnnotationFromExpr(arg.get(), contract);
  }
}

void AccumulateBlockSourceStorageAnnotationFromStmt(
    const Stmt *stmt,
    Objc3BlockSourceStorageAnnotationContract &contract) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->let_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Assign:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->assign_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Return:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->return_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Expr:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->expr_stmt->value.get(), contract);
      return;
    case Stmt::Kind::If:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->if_stmt->condition.get(), contract);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(then_stmt.get(), contract);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(else_stmt.get(), contract);
      }
      return;
    case Stmt::Kind::DoWhile:
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(body_stmt.get(), contract);
      }
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->do_while_stmt->condition.get(), contract);
      return;
    case Stmt::Kind::For:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->for_stmt->init.value.get(), contract);
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->for_stmt->condition.get(), contract);
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->for_stmt->step.value.get(), contract);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(body_stmt.get(), contract);
      }
      return;
    case Stmt::Kind::Switch:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->switch_stmt->condition.get(), contract);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          AccumulateBlockSourceStorageAnnotationFromStmt(case_stmt.get(), contract);
        }
      }
      return;
    case Stmt::Kind::While:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->while_stmt->condition.get(), contract);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(body_stmt.get(), contract);
      }
      return;
    case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
      for (const auto &body_stmt : stmt->block_stmt->body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(body_stmt.get(), contract);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

Objc3BlockSourceStorageAnnotationContract BuildBlockSourceStorageAnnotationContract(
    const Objc3Program &program) {
  Objc3BlockSourceStorageAnnotationContract contract;

  for (const auto &global : program.globals) {
    AccumulateBlockSourceStorageAnnotationFromExpr(global.value.get(), contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
    }
  }
  for (const auto &protocol : program.protocols) {
    for (const auto &method : protocol.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
      }
    }
  }

  contract.deterministic = contract.non_normalized_sites == 0u &&
                           contract.contract_violation_sites == 0u;
  return contract;
}

Objc3BlockAbiInvokeTrampolineLoweringContract BuildBlockAbiInvokeTrampolineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockAbiInvokeTrampolineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_abi_invoke_trampoline_sites_total;
  contract.invoke_argument_slots_total =
      sema_parity_surface.block_abi_invoke_trampoline_invoke_argument_slots_total;
  contract.capture_word_count_total =
      sema_parity_surface.block_abi_invoke_trampoline_capture_word_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_body_statement_entries_total;
  contract.descriptor_symbolized_sites =
      sema_parity_surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total;
  contract.invoke_trampoline_symbolized_sites =
      sema_parity_surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total;
  contract.missing_invoke_trampoline_sites =
      sema_parity_surface.block_abi_invoke_trampoline_missing_invoke_sites_total;
  contract.non_normalized_layout_sites =
      sema_parity_surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_abi_invoke_trampoline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_abi_invoke_trampoline_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_abi_invoke_trampoline_handoff;
  return contract;
}

Objc3BlockStorageEscapeLoweringContract BuildBlockStorageEscapeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockStorageEscapeLoweringContract contract;
  contract.block_literal_sites = sema_parity_surface.block_storage_escape_sites_total;
  contract.mutable_capture_count_total =
      sema_parity_surface.block_storage_escape_mutable_capture_count_total;
  contract.byref_slot_count_total =
      sema_parity_surface.block_storage_escape_byref_slot_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_storage_escape_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_storage_escape_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_storage_escape_body_statement_entries_total;
  contract.requires_byref_cells_sites =
      sema_parity_surface.block_storage_escape_requires_byref_cells_sites_total;
  contract.escape_analysis_enabled_sites =
      sema_parity_surface.block_storage_escape_escape_analysis_enabled_sites_total;
  contract.escape_to_heap_sites =
      sema_parity_surface.block_storage_escape_escape_to_heap_sites_total;
  contract.escape_profile_normalized_sites =
      sema_parity_surface.block_storage_escape_escape_profile_normalized_sites_total;
  contract.byref_layout_symbolized_sites =
      sema_parity_surface.block_storage_escape_byref_layout_symbolized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_storage_escape_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_storage_escape_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_storage_escape_handoff;
  return contract;
}

Objc3BlockCopyDisposeLoweringContract BuildBlockCopyDisposeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockCopyDisposeLoweringContract contract;
  contract.block_literal_sites = sema_parity_surface.block_copy_dispose_sites_total;
  contract.mutable_capture_count_total =
      sema_parity_surface.block_copy_dispose_mutable_capture_count_total;
  contract.byref_slot_count_total =
      sema_parity_surface.block_copy_dispose_byref_slot_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_copy_dispose_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_copy_dispose_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_copy_dispose_body_statement_entries_total;
  contract.copy_helper_required_sites =
      sema_parity_surface.block_copy_dispose_copy_helper_required_sites_total;
  contract.dispose_helper_required_sites =
      sema_parity_surface.block_copy_dispose_dispose_helper_required_sites_total;
  contract.profile_normalized_sites =
      sema_parity_surface.block_copy_dispose_profile_normalized_sites_total;
  contract.copy_helper_symbolized_sites =
      sema_parity_surface.block_copy_dispose_copy_helper_symbolized_sites_total;
  contract.dispose_helper_symbolized_sites =
      sema_parity_surface.block_copy_dispose_dispose_helper_symbolized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_copy_dispose_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_copy_dispose_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_copy_dispose_handoff;
  return contract;
}

Objc3BlockDeterminismPerfBaselineLoweringContract BuildBlockDeterminismPerfBaselineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockDeterminismPerfBaselineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_determinism_perf_baseline_sites_total;
  contract.baseline_weight_total =
      sema_parity_surface.block_determinism_perf_baseline_weight_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_body_statement_entries_total;
  contract.deterministic_capture_sites =
      sema_parity_surface.block_determinism_perf_baseline_deterministic_capture_sites_total;
  contract.heavy_tier_sites =
      sema_parity_surface.block_determinism_perf_baseline_heavy_tier_sites_total;
  contract.normalized_profile_sites =
      sema_parity_surface.block_determinism_perf_baseline_normalized_profile_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_determinism_perf_baseline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_determinism_perf_baseline_summary.deterministic &&
      sema_parity_surface.deterministic_block_determinism_perf_baseline_handoff;
  return contract;
}

Objc3TypeSystemOptionalKeypathLoweringContract
BuildTypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  Objc3TypeSystemOptionalKeypathLoweringContract contract;
  contract.optional_binding_sites = summary.optional_binding_sites;
  contract.optional_binding_clause_sites = summary.optional_binding_clause_sites;
  contract.optional_send_sites = summary.optional_send_sites;
  contract.nil_coalescing_sites = summary.nil_coalescing_sites;
  contract.typed_keypath_literal_sites = summary.typed_keypath_literal_sites;
  contract.typed_keypath_self_root_sites = summary.typed_keypath_self_root_sites;
  contract.typed_keypath_class_root_sites = summary.typed_keypath_class_root_sites;
  contract.live_optional_lowering_sites =
      summary.optional_binding_sites + summary.optional_send_sites +
      summary.nil_coalescing_sites;
  contract.single_evaluation_nil_short_circuit_sites =
      summary.optional_binding_clause_sites + summary.optional_send_sites +
      summary.nil_coalescing_sites;
  contract.live_typed_keypath_artifact_sites =
      summary.typed_keypath_literal_sites;
  contract.deferred_typed_keypath_sites = 0;
  contract.contract_violation_sites =
      summary.optional_binding_contract_violation_sites +
      summary.optional_send_contract_violation_sites +
      summary.optional_flow_contract_violation_sites;
  contract.deterministic =
      summary.deterministic && contract.contract_violation_sites == 0 &&
      contract.live_typed_keypath_artifact_sites +
              contract.deferred_typed_keypath_sites ==
          contract.typed_keypath_literal_sites;
  return contract;
}

Objc3ControlFlowControlFlowSafetyLoweringContract
BuildControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  Objc3ControlFlowControlFlowSafetyLoweringContract contract;
  contract.guard_statement_sites = summary.guard_exit_enforcement_sites;
  contract.guard_clause_sites = summary.guard_binding_clause_semantic_sites +
                                summary.guard_condition_clause_semantic_sites;
  contract.match_statement_sites = summary.match_statement_semantic_sites;
  contract.defer_statement_sites = summary.defer_statement_semantic_sites;
  // lowering contract anchor: lane-C now treats admitted match
  // statements as live only when they stay inside the current literal/default/
  // wildcard/binding slice. Result-case payload matching remains explicitly
  // fail-closed until a later runtime ABI tranche lands.
  const bool result_case_patterns_present =
      summary.match_result_case_scope_sites > 0u;
  contract.live_guard_short_circuit_sites = contract.guard_statement_sites;
  contract.live_match_dispatch_sites =
      result_case_patterns_present ? 0u : contract.match_statement_sites;
  contract.live_defer_cleanup_sites = contract.defer_statement_sites;
  contract.fail_closed_guard_short_circuit_sites = 0;
  contract.fail_closed_match_dispatch_sites =
      contract.match_statement_sites - contract.live_match_dispatch_sites;
  contract.fail_closed_defer_cleanup_sites = 0;
  contract.deterministic_fail_closed_sites =
      contract.fail_closed_guard_short_circuit_sites +
      contract.fail_closed_match_dispatch_sites +
      contract.fail_closed_defer_cleanup_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic = summary.deterministic &&
                           summary.ready_for_lowering_and_runtime;
  return contract;
}

Objc3LightweightGenericsConstraintLoweringContract BuildLightweightGenericsConstraintLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3LightweightGenericsConstraintLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.lightweight_generic_constraint_sites_total;
  const std::size_t raw_generic_suffix_sites =
      sema_parity_surface.lightweight_generic_constraint_generic_suffix_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.lightweight_generic_constraint_object_pointer_type_sites_total;
  const std::size_t raw_terminated_generic_suffix_sites =
      sema_parity_surface.lightweight_generic_constraint_terminated_generic_suffix_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.lightweight_generic_constraint_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.lightweight_generic_constraint_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.lightweight_generic_constraint_contract_violation_sites_total;

  contract.generic_constraint_sites =
      std::max({raw_sites, raw_generic_suffix_sites, raw_object_pointer_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.generic_suffix_sites =
      std::min(raw_generic_suffix_sites, contract.generic_constraint_sites);
  contract.object_pointer_type_sites =
      std::min(raw_object_pointer_sites, contract.generic_constraint_sites);
  contract.terminated_generic_suffix_sites =
      std::min(raw_terminated_generic_suffix_sites, contract.generic_suffix_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.generic_constraint_sites);
  contract.normalized_constraint_sites =
      std::min(raw_normalized_sites, contract.generic_constraint_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.generic_constraint_sites);

  contract.deterministic =
      sema_parity_surface.lightweight_generic_constraint_summary.deterministic &&
      sema_parity_surface.deterministic_lightweight_generic_constraint_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_constraint_sites == contract.generic_constraint_sites;
  return contract;
}

Objc3NullabilityFlowWarningPrecisionLoweringContract BuildNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NullabilityFlowWarningPrecisionLoweringContract contract;
  contract.nullability_flow_sites =
      sema_parity_surface.nullability_flow_sites_total;
  contract.object_pointer_type_sites = std::max(
      sema_parity_surface.nullability_flow_object_pointer_type_sites_total,
      sema_parity_surface.nullability_flow_nullability_suffix_sites_total);
  contract.nullability_suffix_sites =
      sema_parity_surface.nullability_flow_nullability_suffix_sites_total;
  contract.nullable_suffix_sites =
      sema_parity_surface.nullability_flow_nullable_suffix_sites_total;
  contract.nonnull_suffix_sites =
      sema_parity_surface.nullability_flow_nonnull_suffix_sites_total;
  contract.normalized_sites =
      sema_parity_surface.nullability_flow_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.nullability_flow_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.nullability_flow_warning_precision_summary.deterministic &&
      sema_parity_surface.deterministic_nullability_flow_warning_precision_handoff;
  return contract;
}

Objc3ProtocolQualifiedObjectTypeLoweringContract BuildProtocolQualifiedObjectTypeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ProtocolQualifiedObjectTypeLoweringContract contract;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.protocol_qualified_object_type_sites_total;
  const std::size_t raw_protocol_composition_sites =
      sema_parity_surface.protocol_qualified_object_type_protocol_composition_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.protocol_qualified_object_type_object_pointer_type_sites_total;
  const std::size_t raw_terminated_sites =
      sema_parity_surface.protocol_qualified_object_type_terminated_protocol_composition_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.protocol_qualified_object_type_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.protocol_qualified_object_type_normalized_protocol_composition_sites_total;
  const std::size_t raw_contract_violation_sites =
      sema_parity_surface.protocol_qualified_object_type_contract_violation_sites_total;

  contract.protocol_qualified_object_type_sites = std::max(
      {raw_protocol_sites, raw_protocol_composition_sites, raw_pointer_sites, raw_normalized_sites,
       raw_contract_violation_sites});
  contract.protocol_composition_sites =
      std::min(raw_protocol_composition_sites, contract.protocol_qualified_object_type_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.protocol_composition_sites);
  contract.terminated_protocol_composition_sites =
      std::min(raw_terminated_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.protocol_qualified_object_type_sites);
  contract.normalized_protocol_composition_sites =
      std::min(raw_normalized_sites, contract.protocol_qualified_object_type_sites);
  contract.contract_violation_sites =
      std::min(raw_contract_violation_sites, contract.protocol_qualified_object_type_sites);

  const bool strict_deterministic =
      sema_parity_surface.protocol_qualified_object_type_summary.deterministic &&
      sema_parity_surface.deterministic_protocol_qualified_object_type_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_protocol_composition_sites == contract.protocol_qualified_object_type_sites;
  contract.deterministic = strict_deterministic;
  return contract;
}

Objc3VarianceBridgeCastLoweringContract BuildVarianceBridgeCastLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3VarianceBridgeCastLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.variance_bridge_cast_sites_total;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.variance_bridge_cast_protocol_composition_sites_total;
  const std::size_t raw_ownership_sites =
      sema_parity_surface.variance_bridge_cast_ownership_qualifier_sites_total;
  const std::size_t raw_object_sites =
      sema_parity_surface.variance_bridge_cast_object_pointer_type_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.variance_bridge_cast_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.variance_bridge_cast_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.variance_bridge_cast_contract_violation_sites_total;

  contract.variance_bridge_cast_sites = std::max(
      {raw_sites, raw_protocol_sites, raw_ownership_sites, raw_pointer_sites, raw_normalized_sites,
       raw_violation_sites});
  contract.protocol_composition_sites =
      std::min(raw_protocol_sites, contract.variance_bridge_cast_sites);
  contract.ownership_qualifier_sites =
      std::min(raw_ownership_sites, contract.variance_bridge_cast_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.variance_bridge_cast_sites);
  contract.normalized_sites = std::min(raw_normalized_sites, contract.variance_bridge_cast_sites);
  contract.contract_violation_sites = std::min(raw_violation_sites, contract.variance_bridge_cast_sites);

  contract.deterministic = sema_parity_surface.variance_bridge_cast_summary.deterministic &&
                           sema_parity_surface.deterministic_variance_bridge_cast_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.variance_bridge_cast_sites;
  return contract;
}

Objc3GenericMetadataAbiLoweringContract BuildGenericMetadataAbiLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3GenericMetadataAbiLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.generic_metadata_abi_sites_total;
  const std::size_t raw_generic_suffix_sites =
      sema_parity_surface.generic_metadata_abi_generic_suffix_sites_total;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.generic_metadata_abi_protocol_composition_sites_total;
  const std::size_t raw_ownership_sites =
      sema_parity_surface.generic_metadata_abi_ownership_qualifier_sites_total;
  const std::size_t raw_object_sites =
      sema_parity_surface.generic_metadata_abi_object_pointer_type_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.generic_metadata_abi_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.generic_metadata_abi_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.generic_metadata_abi_contract_violation_sites_total;

  contract.generic_metadata_abi_sites = std::max(
      {raw_sites, raw_generic_suffix_sites, raw_protocol_sites, raw_ownership_sites, raw_pointer_sites,
       raw_normalized_sites, raw_violation_sites});
  contract.generic_suffix_sites =
      std::min(raw_generic_suffix_sites, contract.generic_metadata_abi_sites);
  contract.protocol_composition_sites =
      std::min(raw_protocol_sites, contract.generic_metadata_abi_sites);
  contract.ownership_qualifier_sites =
      std::min(raw_ownership_sites, contract.generic_metadata_abi_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.generic_metadata_abi_sites);
  contract.normalized_sites = std::min(raw_normalized_sites, contract.generic_metadata_abi_sites);
  contract.contract_violation_sites = std::min(raw_violation_sites, contract.generic_metadata_abi_sites);

  contract.deterministic = sema_parity_surface.generic_metadata_abi_summary.deterministic &&
                           sema_parity_surface.deterministic_generic_metadata_abi_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.generic_metadata_abi_sites;
  return contract;
}

Objc3ModuleImportGraphLoweringContract BuildModuleImportGraphLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ModuleImportGraphLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.module_import_graph_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.module_import_graph_import_edge_candidate_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.module_import_graph_namespace_segment_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.module_import_graph_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.module_import_graph_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.module_import_graph_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.module_import_graph_contract_violation_sites_total;

  contract.module_import_graph_sites =
      std::max({raw_sites, raw_import_edge_sites, raw_namespace_segment_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.module_import_graph_sites);
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.module_import_graph_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.module_import_graph_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.module_import_graph_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.module_import_graph_sites);
  contract.deterministic = sema_parity_surface.module_import_graph_summary.deterministic &&
                           sema_parity_surface.deterministic_module_import_graph_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.module_import_graph_sites;
  return contract;
}

Objc3NamespaceCollisionShadowingLoweringContract
BuildNamespaceCollisionShadowingLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NamespaceCollisionShadowingLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.namespace_collision_shadowing_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.namespace_collision_shadowing_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.namespace_collision_shadowing_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.namespace_collision_shadowing_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.namespace_collision_shadowing_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.namespace_collision_shadowing_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.namespace_collision_shadowing_contract_violation_sites_total;

  contract.namespace_collision_shadowing_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.namespace_collision_shadowing_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.namespace_collision_shadowing_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.namespace_collision_shadowing_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.namespace_collision_shadowing_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.namespace_collision_shadowing_sites);
  contract.deterministic = sema_parity_surface.namespace_collision_shadowing_summary.deterministic &&
                           sema_parity_surface.deterministic_namespace_collision_shadowing_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.namespace_collision_shadowing_sites;
  return contract;
}

Objc3PublicPrivateApiPartitionLoweringContract
BuildPublicPrivateApiPartitionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3PublicPrivateApiPartitionLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.public_private_api_partition_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.public_private_api_partition_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.public_private_api_partition_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.public_private_api_partition_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.public_private_api_partition_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.public_private_api_partition_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.public_private_api_partition_contract_violation_sites_total;

  contract.public_private_api_partition_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.public_private_api_partition_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.public_private_api_partition_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.public_private_api_partition_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.public_private_api_partition_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.public_private_api_partition_sites);
  contract.deterministic = sema_parity_surface.public_private_api_partition_summary.deterministic &&
                           sema_parity_surface.deterministic_public_private_api_partition_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.public_private_api_partition_sites;
  return contract;
}

Objc3IncrementalModuleCacheInvalidationLoweringContract
BuildIncrementalModuleCacheInvalidationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3IncrementalModuleCacheInvalidationLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.incremental_module_cache_invalidation_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.incremental_module_cache_invalidation_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.incremental_module_cache_invalidation_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.incremental_module_cache_invalidation_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.incremental_module_cache_invalidation_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.incremental_module_cache_invalidation_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface.incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.incremental_module_cache_invalidation_contract_violation_sites_total;

  contract.incremental_module_cache_invalidation_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.incremental_module_cache_invalidation_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.incremental_module_cache_invalidation_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.incremental_module_cache_invalidation_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.incremental_module_cache_invalidation_sites);
  const std::size_t normalized_budget =
      (contract.incremental_module_cache_invalidation_sites >= contract.normalized_sites)
          ? (contract.incremental_module_cache_invalidation_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.incremental_module_cache_invalidation_sites);
  contract.deterministic =
      sema_parity_surface.incremental_module_cache_invalidation_summary
          .deterministic &&
      sema_parity_surface
          .deterministic_incremental_module_cache_invalidation_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites ==
          contract.incremental_module_cache_invalidation_sites;
  return contract;
}

Objc3CrossModuleConformanceLoweringContract
BuildCrossModuleConformanceLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3CrossModuleConformanceLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.cross_module_conformance_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.cross_module_conformance_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.cross_module_conformance_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.cross_module_conformance_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.cross_module_conformance_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.cross_module_conformance_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface.cross_module_conformance_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.cross_module_conformance_contract_violation_sites_total;

  contract.cross_module_conformance_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.cross_module_conformance_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.cross_module_conformance_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.cross_module_conformance_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.cross_module_conformance_sites);
  const std::size_t normalized_budget =
      (contract.cross_module_conformance_sites >= contract.normalized_sites)
          ? (contract.cross_module_conformance_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.cross_module_conformance_sites);
  contract.deterministic = sema_parity_surface.cross_module_conformance_summary.deterministic &&
                           sema_parity_surface.deterministic_cross_module_conformance_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.cross_module_conformance_sites;
  return contract;
}

Objc3ThrowsPropagationLoweringContract
BuildThrowsPropagationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ThrowsPropagationLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.throws_propagation_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.throws_propagation_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.throws_propagation_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.throws_propagation_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.throws_propagation_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.throws_propagation_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface.throws_propagation_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.throws_propagation_contract_violation_sites_total;

  contract.throws_propagation_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.throws_propagation_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.throws_propagation_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.throws_propagation_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.throws_propagation_sites);
  const std::size_t normalized_budget =
      (contract.throws_propagation_sites >= contract.normalized_sites)
          ? (contract.throws_propagation_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.throws_propagation_sites);
  contract.deterministic = sema_parity_surface.throws_propagation_summary.deterministic &&
                           sema_parity_surface.deterministic_throws_propagation_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.throws_propagation_sites;
  return contract;
}

Objc3ResultLikeLoweringContract BuildResultLikeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ResultLikeLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.result_like_lowering_sites_total;
  const std::size_t raw_success_sites =
      sema_parity_surface.result_like_lowering_result_success_sites_total;
  const std::size_t raw_failure_sites =
      sema_parity_surface.result_like_lowering_result_failure_sites_total;
  const std::size_t raw_branch_sites =
      sema_parity_surface.result_like_lowering_result_branch_sites_total;
  const std::size_t raw_payload_sites =
      sema_parity_surface.result_like_lowering_result_payload_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.result_like_lowering_normalized_sites_total;
  const std::size_t raw_branch_merge_sites =
      sema_parity_surface.result_like_lowering_branch_merge_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.result_like_lowering_contract_violation_sites_total;

  contract.result_like_sites =
      std::max({raw_sites, raw_success_sites, raw_failure_sites,
                raw_branch_sites, raw_payload_sites, raw_normalized_sites,
                raw_branch_merge_sites, raw_violation_sites});
  contract.result_success_sites =
      std::min(raw_success_sites, contract.result_like_sites);
  contract.result_failure_sites =
      std::min(raw_failure_sites, contract.result_like_sites);
  contract.result_branch_sites =
      std::min(raw_branch_sites, contract.result_like_sites);
  contract.result_payload_sites =
      std::min(raw_payload_sites, contract.result_like_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.result_like_sites);
  const std::size_t normalized_budget =
      (contract.result_like_sites >= contract.normalized_sites)
          ? (contract.result_like_sites - contract.normalized_sites)
          : 0;
  contract.branch_merge_sites =
      std::min(raw_branch_merge_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.result_like_sites);
  contract.deterministic =
      sema_parity_surface.deterministic_result_like_lowering_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites + contract.branch_merge_sites ==
          contract.result_like_sites;
  return contract;
}

Objc3NSErrorBridgingLoweringContract BuildNSErrorBridgingLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NSErrorBridgingLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.ns_error_bridging_sites_total;
  const std::size_t raw_parameter_sites =
      sema_parity_surface.ns_error_bridging_ns_error_parameter_sites_total;
  const std::size_t raw_out_parameter_sites =
      sema_parity_surface.ns_error_bridging_ns_error_out_parameter_sites_total;
  const std::size_t raw_bridge_path_sites =
      sema_parity_surface.ns_error_bridging_ns_error_bridge_path_sites_total;
  const std::size_t raw_failable_call_sites =
      sema_parity_surface.ns_error_bridging_failable_call_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.ns_error_bridging_normalized_sites_total;
  const std::size_t raw_bridge_boundary_sites =
      sema_parity_surface.ns_error_bridging_bridge_boundary_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.ns_error_bridging_contract_violation_sites_total;

  contract.ns_error_bridging_sites =
      std::max({raw_sites, raw_parameter_sites, raw_out_parameter_sites,
                raw_bridge_path_sites, raw_failable_call_sites,
                raw_normalized_sites, raw_bridge_boundary_sites,
                raw_violation_sites});
  contract.ns_error_parameter_sites =
      std::min(raw_parameter_sites, contract.ns_error_bridging_sites);
  contract.ns_error_out_parameter_sites =
      std::min(raw_out_parameter_sites, contract.ns_error_parameter_sites);
  contract.ns_error_bridge_path_sites =
      std::min(raw_bridge_path_sites, contract.ns_error_out_parameter_sites);
  contract.failable_call_sites =
      std::min(raw_failable_call_sites, contract.ns_error_bridging_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.ns_error_bridging_sites);
  const std::size_t normalized_budget =
      (contract.ns_error_bridging_sites >= contract.normalized_sites)
          ? (contract.ns_error_bridging_sites - contract.normalized_sites)
          : 0;
  contract.bridge_boundary_sites =
      std::min(raw_bridge_boundary_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.ns_error_bridging_sites);
  contract.deterministic =
      sema_parity_surface.deterministic_ns_error_bridging_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites + contract.bridge_boundary_sites ==
          contract.ns_error_bridging_sites;
  return contract;
}

Objc3UnwindCleanupLoweringContract BuildUnwindCleanupLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3UnwindCleanupLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.unwind_cleanup_sites_total;
  const std::size_t raw_exceptional_exit_sites =
      sema_parity_surface.unwind_cleanup_exceptional_exit_sites_total;
  const std::size_t raw_action_sites =
      sema_parity_surface.unwind_cleanup_action_sites_total;
  const std::size_t raw_scope_sites =
      sema_parity_surface.unwind_cleanup_scope_sites_total;
  const std::size_t raw_resume_sites =
      sema_parity_surface.unwind_cleanup_resume_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.unwind_cleanup_normalized_sites_total;
  const std::size_t raw_fail_closed_sites =
      sema_parity_surface.unwind_cleanup_fail_closed_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.unwind_cleanup_contract_violation_sites_total;

  contract.unwind_cleanup_sites =
      std::max({raw_sites, raw_exceptional_exit_sites, raw_action_sites,
                raw_scope_sites, raw_resume_sites, raw_normalized_sites,
                raw_fail_closed_sites, raw_violation_sites});
  contract.unwind_edge_sites =
      std::min(raw_exceptional_exit_sites, contract.unwind_cleanup_sites);
  contract.cleanup_scope_sites =
      std::min(raw_scope_sites, contract.unwind_cleanup_sites);
  contract.cleanup_emit_sites =
      std::min(raw_action_sites, contract.cleanup_scope_sites);
  contract.landing_pad_sites = 0;
  contract.cleanup_resume_sites =
      std::min(raw_resume_sites, contract.unwind_cleanup_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.unwind_cleanup_sites);
  const std::size_t normalized_budget =
      (contract.unwind_cleanup_sites >= contract.normalized_sites)
          ? (contract.unwind_cleanup_sites - contract.normalized_sites)
          : 0;
  contract.guard_blocked_sites =
      std::min(raw_fail_closed_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.unwind_cleanup_sites);
  contract.deterministic =
      sema_parity_surface.deterministic_unwind_cleanup_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites + contract.guard_blocked_sites ==
          contract.unwind_cleanup_sites;
  return contract;
}

}  // namespace

Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts(const std::filesystem::path &input_path,
                                                        const Objc3FrontendPipelineResult &pipeline_result,
                                                        const Objc3FrontendOptions &options) {
  Objc3FrontendArtifactBundle bundle;
  const Objc3Program &program = Objc3ParsedProgramAst(pipeline_result.program);
  bundle.stage_diagnostics = pipeline_result.stage_diagnostics;
  bundle.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(pipeline_result, options);
  bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);
  if (!bundle.diagnostics.empty()) {
    return bundle;
  }

  const bool metadata_only_ir_emission_mode = [&pipeline_result]() {
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff =
            pipeline_result.executable_metadata_typed_lowering_handoff;
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph =
        pipeline_result.executable_metadata_source_graph;
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership =
            pipeline_result.runtime_metadata_source_ownership_boundary;
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality =
        pipeline_result.runtime_export_legality_boundary;
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement =
        pipeline_result.runtime_export_enforcement_summary;
    const Objc3RuntimeMetadataSectionAbiFreezeSummary runtime_metadata_section_abi =
        BuildRuntimeMetadataSectionAbiFreezeSummary(
            runtime_metadata_source_ownership,
            runtime_export_legality,
            runtime_export_enforcement);
    const Objc3RuntimeMetadataSectionPublicationSummary
        runtime_metadata_section_publication =
            BuildRuntimeMetadataSectionPublicationSummary(
                runtime_metadata_section_abi,
                runtime_export_legality,
                runtime_export_enforcement);
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        runtime_metadata_object_inspection =
            BuildRuntimeMetadataObjectInspectionHarnessSummary(
                runtime_metadata_section_abi,
                runtime_metadata_section_publication);
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary
        runtime_metadata_source_to_section_matrix =
            BuildRuntimeMetadataSourceToSectionMatrixSummary(
                executable_metadata_source_graph,
                runtime_metadata_section_abi,
                runtime_metadata_section_publication,
                runtime_metadata_object_inspection);
    const Objc3ExecutableMetadataDebugProjectionSummary
        executable_metadata_debug_projection =
            BuildExecutableMetadataDebugProjectionSummary(
                executable_metadata_typed_lowering_handoff);
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        executable_metadata_runtime_ingest_packaging_contract =
            BuildExecutableMetadataRuntimeIngestPackagingContractSummary(
                executable_metadata_typed_lowering_handoff,
                executable_metadata_debug_projection);
    const std::string executable_metadata_runtime_ingest_binary_payload =
        BuildExecutableMetadataRuntimeIngestBinaryEnvelope(
            executable_metadata_runtime_ingest_packaging_contract,
            executable_metadata_typed_lowering_handoff,
            executable_metadata_debug_projection);
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
        executable_metadata_runtime_ingest_binary_boundary =
            BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary(
                executable_metadata_runtime_ingest_packaging_contract,
                executable_metadata_typed_lowering_handoff,
                executable_metadata_debug_projection,
                executable_metadata_runtime_ingest_binary_payload);
    return IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
               executable_metadata_typed_lowering_handoff) &&
           IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
               runtime_metadata_source_ownership) &&
           IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality) &&
           IsReadyObjc3RuntimeExportEnforcementSummary(
               runtime_export_enforcement) &&
           IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
               runtime_metadata_section_abi) &&
           IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
               runtime_metadata_section_publication) &&
           IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(
               runtime_metadata_source_to_section_matrix) &&
           IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
               executable_metadata_debug_projection) &&
           IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
               executable_metadata_runtime_ingest_packaging_contract) &&
           IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
               executable_metadata_runtime_ingest_binary_boundary) &&
           runtime_metadata_section_publication.class_descriptor_count > 0u;
  }();
  std::string post_pipeline_failure_code;
  std::string post_pipeline_failure_message;
  const auto record_post_pipeline_failure = [&](const char *code, std::string message) {
    if (!post_pipeline_failure_code.empty()) {
      return;
    }
    post_pipeline_failure_code = code == nullptr ? "" : code;
    post_pipeline_failure_message = std::move(message);
  };

  const Objc3IREmissionCoreFeatureImplementationSurface
      ir_emission_core_feature_impl_surface =
          BuildObjc3IREmissionCoreFeatureImplementationSurface(pipeline_result);

  if (!metadata_only_ir_emission_mode) {
    std::string parse_lowering_readiness_error;
    if (!IsObjc3ParseLoweringReadinessSurfaceReady(bundle.parse_lowering_readiness_surface,
                                                   parse_lowering_readiness_error)) {
      record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: parse-to-lowering readiness check failed: " +
              parse_lowering_readiness_error);
    }

    std::string diagnostics_surfacing_scaffold_error;
    if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldReady(
            pipeline_result.lowering_runtime_diagnostics_surfacing_scaffold,
            diagnostics_surfacing_scaffold_error)) {
      record_post_pipeline_failure("O3L301",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing scaffold check failed: " +
              diagnostics_surfacing_scaffold_error);
    }

  std::string diagnostics_surfacing_core_feature_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface,
          diagnostics_surfacing_core_feature_error)) {
    record_post_pipeline_failure("O3L321",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing core feature check failed: " +
            diagnostics_surfacing_core_feature_error);
  }

  std::string diagnostics_surfacing_core_feature_expansion_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface,
          diagnostics_surfacing_core_feature_expansion_error)) {
    record_post_pipeline_failure("O3L322",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing core feature expansion check failed: " +
            diagnostics_surfacing_core_feature_expansion_error);
  }

  std::string diagnostics_surfacing_edge_case_compatibility_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface,
          diagnostics_surfacing_edge_case_compatibility_error)) {
    record_post_pipeline_failure("O3L323",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing edge-case compatibility check failed: " +
            diagnostics_surfacing_edge_case_compatibility_error);
  }

  std::string diagnostics_surfacing_edge_case_robustness_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface,
          diagnostics_surfacing_edge_case_robustness_error)) {
    record_post_pipeline_failure("O3L324",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing edge-case expansion and robustness check failed: " +
            diagnostics_surfacing_edge_case_robustness_error);
  }

  std::string diagnostics_surfacing_diagnostics_hardening_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface,
          diagnostics_surfacing_diagnostics_hardening_error)) {
    record_post_pipeline_failure("O3L325",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing diagnostics hardening check failed: " +
            diagnostics_surfacing_diagnostics_hardening_error);
  }

  std::string diagnostics_surfacing_recovery_determinism_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface,
          diagnostics_surfacing_recovery_determinism_error)) {
    record_post_pipeline_failure("O3L326",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing recovery/determinism hardening check failed: " +
            diagnostics_surfacing_recovery_determinism_error);
  }

  std::string diagnostics_surfacing_conformance_matrix_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface,
          diagnostics_surfacing_conformance_matrix_error)) {
    record_post_pipeline_failure("O3L327",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing conformance matrix check failed: " +
            diagnostics_surfacing_conformance_matrix_error);
  }

  std::string lowering_pass_graph_error;
  if (!IsObjc3LoweringPipelinePassGraphScaffoldReady(
          pipeline_result.lowering_pipeline_pass_graph_scaffold,
          lowering_pass_graph_error)) {
    record_post_pipeline_failure("O3L301",         "LLVM IR emission failed: lowering pipeline pass-graph scaffold check failed: " +
            lowering_pass_graph_error);
  }

  std::string lowering_pass_graph_core_feature_error;
  // Historical C001 freeze marker:
  // IsObjc3LoweringPipelinePassGraphCoreFeatureSurfaceReady(
  if (!IsObjc3IREmissionCompletenessCoreFeatureReady(
          pipeline_result.ir_emission_completeness_scaffold,
          lowering_pass_graph_core_feature_error)) {
    record_post_pipeline_failure("O3L302",         "LLVM IR emission failed: lowering pipeline pass-graph core feature check failed: " +
            lowering_pass_graph_core_feature_error);
  }

  std::string lowering_pass_graph_expansion_error;
  // Historical C001 freeze marker:
  // IsObjc3LoweringPipelinePassGraphCoreFeatureExpansionReady(
  if (!IsObjc3IREmissionCompletenessExpansionReady(
          pipeline_result.ir_emission_completeness_scaffold,
          lowering_pass_graph_expansion_error)) {
    record_post_pipeline_failure("O3L303",         "LLVM IR emission failed: lowering pipeline pass-graph core feature expansion check failed: " +
            lowering_pass_graph_expansion_error);
  }

  std::string lowering_pass_graph_lane_a_edge_case_compatibility_error;
  if (!IsObjc3LoweringPipelinePassGraphEdgeCaseCompatibilityReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_edge_case_compatibility_error)) {
    record_post_pipeline_failure("O3L305",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A edge-case compatibility check failed: " +
            lowering_pass_graph_lane_a_edge_case_compatibility_error);
  }

  std::string lowering_pass_graph_lane_a_edge_case_robustness_error;
  if (!IsObjc3LoweringPipelinePassGraphEdgeCaseRobustnessReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_edge_case_robustness_error)) {
    record_post_pipeline_failure("O3L307",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A edge-case robustness check failed: " +
            lowering_pass_graph_lane_a_edge_case_robustness_error);
  }

  std::string lowering_pass_graph_lane_a_diagnostics_hardening_error;
  if (!IsObjc3LoweringPipelinePassGraphDiagnosticsHardeningReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_diagnostics_hardening_error)) {
    record_post_pipeline_failure("O3L308",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A diagnostics hardening check failed: " +
            lowering_pass_graph_lane_a_diagnostics_hardening_error);
  }

  std::string lowering_pass_graph_lane_a_recovery_determinism_error;
  if (!IsObjc3LoweringPipelinePassGraphRecoveryDeterminismReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_recovery_determinism_error)) {
    record_post_pipeline_failure("O3L309",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A recovery determinism check failed: " +
            lowering_pass_graph_lane_a_recovery_determinism_error);
  }

  std::string lowering_pass_graph_lane_a_conformance_matrix_error;
  if (!IsObjc3LoweringPipelinePassGraphConformanceMatrixReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_conformance_matrix_error)) {
    record_post_pipeline_failure("O3L311",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A conformance matrix check failed: " +
            lowering_pass_graph_lane_a_conformance_matrix_error);
  }

  std::string lowering_pass_graph_lane_a_conformance_corpus_error;
  if (!IsObjc3LoweringPipelinePassGraphConformanceCorpusReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_conformance_corpus_error)) {
    record_post_pipeline_failure("O3L313",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A conformance corpus check failed: " +
            lowering_pass_graph_lane_a_conformance_corpus_error);
  }
  std::string lowering_pass_graph_lane_a_performance_quality_guardrails_error;
  if (!IsObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_performance_quality_guardrails_error)) {
    record_post_pipeline_failure("O3L315",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A performance quality guardrails check failed: " +
            lowering_pass_graph_lane_a_performance_quality_guardrails_error);
  }

  std::string lowering_pass_graph_edge_case_compatibility_error;
  if (!IsObjc3IREmissionCompletenessEdgeCaseCompatibilityReady(
          pipeline_result.ir_emission_completeness_scaffold,
          lowering_pass_graph_edge_case_compatibility_error)) {
    record_post_pipeline_failure("O3L304",         "LLVM IR emission failed: lowering pipeline pass-graph edge-case compatibility check failed: " +
            lowering_pass_graph_edge_case_compatibility_error);
  }

    std::string ir_emission_core_feature_impl_error;
  if (!IsObjc3IREmissionCoreFeatureImplementationReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_impl_error)) {
    record_post_pipeline_failure("O3L306",         "LLVM IR emission failed: IR emission core feature implementation check failed: " +
            ir_emission_core_feature_impl_error);
  }
  std::string ir_emission_core_feature_expansion_error;
  if (!IsObjc3IREmissionCoreFeatureExpansionReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_expansion_error)) {
    record_post_pipeline_failure("O3L314",         "LLVM IR emission failed: IR emission core feature expansion check failed: " +
            ir_emission_core_feature_expansion_error);
  }
  std::string ir_emission_core_feature_edge_case_compatibility_error;
  if (!IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_edge_case_compatibility_error)) {
    record_post_pipeline_failure("O3L316",         "LLVM IR emission failed: IR emission core feature edge-case compatibility check failed: " +
            ir_emission_core_feature_edge_case_compatibility_error);
  }
  std::string ir_emission_core_feature_edge_case_robustness_error;
  if (!IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_edge_case_robustness_error)) {
    record_post_pipeline_failure("O3L317",         "LLVM IR emission failed: IR emission core feature edge-case robustness check failed: " +
            ir_emission_core_feature_edge_case_robustness_error);
  }
  std::string ir_emission_core_feature_diagnostics_hardening_error;
  if (!IsObjc3IREmissionCoreFeatureDiagnosticsHardeningReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_diagnostics_hardening_error)) {
    record_post_pipeline_failure("O3L318",         "LLVM IR emission failed: IR emission core feature diagnostics hardening check failed: " +
            ir_emission_core_feature_diagnostics_hardening_error);
  }
  std::string ir_emission_core_feature_recovery_determinism_hardening_error;
  if (!IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_recovery_determinism_hardening_error)) {
    record_post_pipeline_failure("O3L319",         "LLVM IR emission failed: IR emission core feature recovery determinism hardening check failed: " +
            ir_emission_core_feature_recovery_determinism_hardening_error);
  }
  std::string ir_emission_core_feature_conformance_matrix_error;
  if (!IsObjc3IREmissionCoreFeatureConformanceMatrixReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_conformance_matrix_error)) {
    record_post_pipeline_failure("O3L320",         "LLVM IR emission failed: IR emission core feature conformance matrix check failed: " +
            ir_emission_core_feature_conformance_matrix_error);
  }
  std::string ir_emission_core_feature_conformance_corpus_error;
  if (!IsObjc3IREmissionCoreFeatureConformanceCorpusReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_conformance_corpus_error)) {
    record_post_pipeline_failure("O3L330",         "LLVM IR emission failed: IR emission core feature conformance corpus check failed: " +
            ir_emission_core_feature_conformance_corpus_error);
  }
  std::string ir_emission_core_feature_performance_quality_guardrails_error;
  if (!IsObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_performance_quality_guardrails_error)) {
    record_post_pipeline_failure("O3L331",         "LLVM IR emission failed: IR emission core feature performance quality guardrails check failed: " +
            ir_emission_core_feature_performance_quality_guardrails_error);
  }
  std::string ir_emission_core_feature_cross_lane_integration_sync_error;
  if (!IsObjc3IREmissionCoreFeatureCrossLaneIntegrationSyncReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_cross_lane_integration_sync_error)) {
    record_post_pipeline_failure("O3L332",         "LLVM IR emission failed: IR emission core feature cross-lane integration sync check failed: " +
            ir_emission_core_feature_cross_lane_integration_sync_error);
  }
  std::string ir_emission_core_feature_advanced_core_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedCoreShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_core_shard1_error)) {
    record_post_pipeline_failure("O3L333",         "LLVM IR emission failed: IR emission core feature advanced core shard 1 check failed: " +
            ir_emission_core_feature_advanced_core_shard1_error);
  }
  std::string ir_emission_core_feature_advanced_edge_compatibility_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_edge_compatibility_shard1_error)) {
    record_post_pipeline_failure("O3L334",         "LLVM IR emission failed: IR emission core feature advanced edge compatibility shard 1 check failed: " +
            ir_emission_core_feature_advanced_edge_compatibility_shard1_error);
  }
  std::string ir_emission_core_feature_advanced_diagnostics_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedDiagnosticsShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_diagnostics_shard1_error)) {
    record_post_pipeline_failure("O3L335",         "LLVM IR emission failed: IR emission core feature advanced diagnostics shard 1 check failed: " +
            ir_emission_core_feature_advanced_diagnostics_shard1_error);
  }
  std::string ir_emission_core_feature_advanced_conformance_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedConformanceShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_conformance_shard1_error)) {
    record_post_pipeline_failure("O3L336",         "LLVM IR emission failed: IR emission core feature advanced conformance shard 1 check failed: " +
            ir_emission_core_feature_advanced_conformance_shard1_error);
  }
  std::string ir_emission_core_feature_advanced_integration_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_integration_shard1_error)) {
    record_post_pipeline_failure("O3L337",         "LLVM IR emission failed: IR emission core feature advanced integration shard 1 check failed: " +
            ir_emission_core_feature_advanced_integration_shard1_error);
  }
  }
  std::vector<const FunctionDecl *> manifest_functions;
  manifest_functions.reserve(program.functions.size());
  std::unordered_set<std::string> manifest_function_names;
  for (const auto &fn : program.functions) {
    if (manifest_function_names.insert(fn.name).second) {
      manifest_functions.push_back(&fn);
    }
  }

  std::size_t scalar_return_i32 = 0;
  std::size_t scalar_return_bool = 0;
  std::size_t scalar_return_void = 0;
  std::size_t scalar_param_i32 = 0;
  std::size_t scalar_param_bool = 0;
  std::size_t vector_signature_functions = 0;
  std::size_t vector_return_signatures = 0;
  std::size_t vector_param_signatures = 0;
  std::size_t vector_i32_signatures = 0;
  std::size_t vector_bool_signatures = 0;
  std::size_t vector_lane2_signatures = 0;
  std::size_t vector_lane4_signatures = 0;
  std::size_t vector_lane8_signatures = 0;
  std::size_t vector_lane16_signatures = 0;
  for (const auto &entry : pipeline_result.integration_surface.functions) {
    const FunctionInfo &signature = entry.second;
    if (signature.return_type == ValueType::Bool) {
      ++scalar_return_bool;
    } else if (signature.return_type == ValueType::Void) {
      ++scalar_return_void;
    } else {
      ++scalar_return_i32;
    }
    for (const ValueType param_type : signature.param_types) {
      if (param_type == ValueType::Bool) {
        ++scalar_param_bool;
      } else {
        ++scalar_param_i32;
      }
    }
  }
  for (const FunctionDecl *fn : manifest_functions) {
    bool has_vector_signature = false;
    if (fn->return_vector_spelling) {
      has_vector_signature = true;
      ++vector_return_signatures;
      if (fn->return_vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++vector_bool_signatures;
      } else {
        ++vector_i32_signatures;
      }
      if (fn->return_vector_lane_count == 2u) {
        ++vector_lane2_signatures;
      } else if (fn->return_vector_lane_count == 4u) {
        ++vector_lane4_signatures;
      } else if (fn->return_vector_lane_count == 8u) {
        ++vector_lane8_signatures;
      } else if (fn->return_vector_lane_count == 16u) {
        ++vector_lane16_signatures;
      }
    }
    for (const FuncParam &param : fn->params) {
      if (!param.vector_spelling) {
        continue;
      }
      has_vector_signature = true;
      ++vector_param_signatures;
      if (param.vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++vector_bool_signatures;
      } else {
        ++vector_i32_signatures;
      }
      if (param.vector_lane_count == 2u) {
        ++vector_lane2_signatures;
      } else if (param.vector_lane_count == 4u) {
        ++vector_lane4_signatures;
      } else if (param.vector_lane_count == 8u) {
        ++vector_lane8_signatures;
      } else if (param.vector_lane_count == 16u) {
        ++vector_lane16_signatures;
      }
    }
    if (has_vector_signature) {
      ++vector_signature_functions;
    }
  }
  const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff = pipeline_result.sema_type_metadata_handoff;
  const Objc3InterfaceImplementationSummary &interface_implementation_summary =
      type_metadata_handoff.interface_implementation_summary;
  const Objc3FrontendProtocolCategorySummary &protocol_category_summary = pipeline_result.protocol_category_summary;
  const Objc3FrontendClassProtocolCategoryLinkingSummary &class_protocol_category_linking_summary =
      pipeline_result.class_protocol_category_linking_summary;
  const Objc3FrontendSelectorNormalizationSummary &selector_normalization_summary =
      pipeline_result.selector_normalization_summary;
  const Objc3FrontendPropertyAttributeSummary &property_attribute_summary =
      pipeline_result.property_attribute_summary;
  const Objc3FrontendObjectPointerNullabilityGenericsSummary &object_pointer_nullability_generics_summary =
      pipeline_result.object_pointer_nullability_generics_summary;
  const Objc3FrontendTypeSystemTypeSourceClosureSummary &type_system_type_source_closure_summary =
      pipeline_result.type_system_type_source_closure_summary;
  const Objc3FrontendControlFlowControlFlowSourceClosureSummary
      &control_flow_control_flow_source_closure_summary =

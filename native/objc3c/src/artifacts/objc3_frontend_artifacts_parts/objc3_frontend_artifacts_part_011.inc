      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"bootstrap_legality_semantics_contract_ready\":"
      << (summary.bootstrap_legality_semantics_contract_ready ? "true"
                                                              : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"bootstrap_reset_contract_ready\":"
      << (summary.bootstrap_reset_contract_ready ? "true" : "false")
      << ",\"failure_mode_semantics_landed\":"
      << (summary.failure_mode_semantics_landed ? "true" : "false")
      << ",\"restart_semantics_landed\":"
      << (summary.restart_semantics_landed ? "true" : "false")
      << ",\"replay_semantics_landed\":"
      << (summary.replay_semantics_landed ? "true" : "false")
      << ",\"unsupported_topology_semantics_landed\":"
      << (summary.unsupported_topology_semantics_landed ? "true" : "false")
      << ",\"deterministic_recovery_semantics_landed\":"
      << (summary.deterministic_recovery_semantics_landed ? "true"
                                                          : "false")
      << ",\"runtime_restart_probe_required\":"
      << (summary.runtime_restart_probe_required ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"bootstrap_legality_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_legality_semantics_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeBootstrapApiReplayKey(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";support_library_core_feature_contract_id="
      << summary.support_library_core_feature_contract_id
      << ";support_library_link_wiring_contract_id="
      << summary.support_library_link_wiring_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";public_header_path=" << summary.public_header_path
      << ";archive_relative_path=" << summary.archive_relative_path
      << ";registration_status_enum_type="
      << summary.registration_status_enum_type
      << ";image_descriptor_type=" << summary.image_descriptor_type
      << ";selector_handle_type=" << summary.selector_handle_type
      << ";registration_snapshot_type="
      << summary.registration_snapshot_type
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";selector_lookup_symbol=" << summary.selector_lookup_symbol
      << ";dispatch_entrypoint_symbol=" << summary.dispatch_entrypoint_symbol
      << ";state_snapshot_symbol=" << summary.state_snapshot_symbol
      << ";reset_for_testing_symbol=" << summary.reset_for_testing_symbol
      << ";compatibility_dispatch_symbol="
      << summary.compatibility_dispatch_symbol
      << ";registration_result_model=" << summary.registration_result_model
      << ";registration_order_ordinal_model="
      << summary.registration_order_ordinal_model
      << ";runtime_state_locking_model="
      << summary.runtime_state_locking_model
      << ";startup_invocation_model=" << summary.startup_invocation_model
      << ";image_walk_lifecycle_model="
      << summary.image_walk_lifecycle_model
      << ";deterministic_reset_lifecycle_model="
      << summary.deterministic_reset_lifecycle_model
      << ";support_library_core_feature_replay_key="
      << summary.support_library_core_feature_replay_key
      << ";support_library_link_wiring_replay_key="
      << summary.support_library_link_wiring_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapApiSummary BuildRuntimeBootstrapApiSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary &runtime_support_library,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeBootstrapApiSummary summary;
  summary.fail_closed = true;
  summary.support_library_core_feature_contract_ready =
      IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
          runtime_support_library);
  summary.support_library_link_wiring_contract_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.api_surface_frozen = true;
  summary.registration_entrypoint_frozen = true;
  summary.selector_lookup_and_dispatch_frozen = true;
  summary.reset_and_snapshot_hooks_frozen = true;
  summary.runtime_probe_required = true;
  summary.image_walk_not_yet_landed = true;
  summary.deterministic_reset_expansion_not_yet_landed = true;
  summary.ready_for_registrar_implementation =
      summary.support_library_core_feature_contract_ready &&
      summary.support_library_link_wiring_contract_ready;
  if (summary.support_library_core_feature_contract_ready) {
    summary.support_library_core_feature_replay_key =
        runtime_support_library.contract_id + ";archive_relative_path=" +
        runtime_support_library.archive_relative_path + ";public_header_path=" +
        runtime_support_library.public_header_path + ";register_image_symbol=" +
        runtime_support_library.register_image_symbol +
        ";lookup_selector_symbol=" +
        runtime_support_library.lookup_selector_symbol +
        ";dispatch_i32_symbol=" + runtime_support_library.dispatch_i32_symbol +
        ";reset_for_testing_symbol=" +
        runtime_support_library.reset_for_testing_symbol;
  }
  if (summary.support_library_link_wiring_contract_ready) {
    summary.support_library_link_wiring_replay_key =
        runtime_support_library_link_wiring.contract_id +
        ";archive_relative_path=" +
        runtime_support_library_link_wiring.archive_relative_path +
        ";compatibility_dispatch_symbol=" +
        runtime_support_library_link_wiring.compatibility_dispatch_symbol +
        ";runtime_dispatch_symbol=" +
        runtime_support_library_link_wiring.runtime_dispatch_symbol +
        ";driver_link_mode=" +
        runtime_support_library_link_wiring.driver_link_mode;
  }
  if (summary.ready_for_registrar_implementation) {
    summary.replay_key = BuildRuntimeBootstrapApiReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapApiSummary(summary)) {
    summary.failure_reason = "runtime bootstrap api summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapApiSummaryJson(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"support_library_core_feature_contract_id\":\""
      << EscapeJsonString(summary.support_library_core_feature_contract_id)
      << "\",\"support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(summary.support_library_link_wiring_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"public_header_path\":\""
      << EscapeJsonString(summary.public_header_path)
      << "\",\"archive_relative_path\":\""
      << EscapeJsonString(summary.archive_relative_path)
      << "\",\"registration_status_enum_type\":\""
      << EscapeJsonString(summary.registration_status_enum_type)
      << "\",\"image_descriptor_type\":\""
      << EscapeJsonString(summary.image_descriptor_type)
      << "\",\"selector_handle_type\":\""
      << EscapeJsonString(summary.selector_handle_type)
      << "\",\"registration_snapshot_type\":\""
      << EscapeJsonString(summary.registration_snapshot_type)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"selector_lookup_symbol\":\""
      << EscapeJsonString(summary.selector_lookup_symbol)
      << "\",\"dispatch_entrypoint_symbol\":\""
      << EscapeJsonString(summary.dispatch_entrypoint_symbol)
      << "\",\"state_snapshot_symbol\":\""
      << EscapeJsonString(summary.state_snapshot_symbol)
      << "\",\"reset_for_testing_symbol\":\""
      << EscapeJsonString(summary.reset_for_testing_symbol)
      << "\",\"compatibility_dispatch_symbol\":\""
      << EscapeJsonString(summary.compatibility_dispatch_symbol)
      << "\",\"registration_result_model\":\""
      << EscapeJsonString(summary.registration_result_model)
      << "\",\"registration_order_ordinal_model\":\""
      << EscapeJsonString(summary.registration_order_ordinal_model)
      << "\",\"runtime_state_locking_model\":\""
      << EscapeJsonString(summary.runtime_state_locking_model)
      << "\",\"startup_invocation_model\":\""
      << EscapeJsonString(summary.startup_invocation_model)
      << "\",\"image_walk_lifecycle_model\":\""
      << EscapeJsonString(summary.image_walk_lifecycle_model)
      << "\",\"deterministic_reset_lifecycle_model\":\""
      << EscapeJsonString(summary.deterministic_reset_lifecycle_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapApiSummary(summary) ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"support_library_core_feature_contract_ready\":"
      << (summary.support_library_core_feature_contract_ready ? "true"
                                                              : "false")
      << ",\"support_library_link_wiring_contract_ready\":"
      << (summary.support_library_link_wiring_contract_ready ? "true"
                                                             : "false")
      << ",\"api_surface_frozen\":"
      << (summary.api_surface_frozen ? "true" : "false")
      << ",\"registration_entrypoint_frozen\":"
      << (summary.registration_entrypoint_frozen ? "true" : "false")
      << ",\"selector_lookup_and_dispatch_frozen\":"
      << (summary.selector_lookup_and_dispatch_frozen ? "true" : "false")
      << ",\"reset_and_snapshot_hooks_frozen\":"
      << (summary.reset_and_snapshot_hooks_frozen ? "true" : "false")
      << ",\"runtime_probe_required\":"
      << (summary.runtime_probe_required ? "true" : "false")
      << ",\"image_walk_not_yet_landed\":"
      << (summary.image_walk_not_yet_landed ? "true" : "false")
      << ",\"deterministic_reset_expansion_not_yet_landed\":"
      << (summary.deterministic_reset_expansion_not_yet_landed ? "true"
                                                               : "false")
      << ",\"ready_for_registrar_implementation\":"
      << (summary.ready_for_registrar_implementation ? "true" : "false")
      << ",\"support_library_core_feature_replay_key\":\""
      << EscapeJsonString(summary.support_library_core_feature_replay_key)
      << "\",\"support_library_link_wiring_replay_key\":\""
      << EscapeJsonString(summary.support_library_link_wiring_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

void AccumulateIdClassSelObjectPointerTypecheckSite(
    bool id_spelling,
    bool class_spelling,
    bool sel_spelling,
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t active_spelling_count =
      (id_spelling ? 1u : 0u) +
      (class_spelling ? 1u : 0u) +
      (sel_spelling ? 1u : 0u) +
      (object_pointer_type_spelling ? 1u : 0u);
  if (active_spelling_count > 1u) {
    contract.deterministic = false;
  }

  if (id_spelling) {
    ++contract.id_typecheck_sites;
  }
  if (class_spelling) {
    ++contract.class_typecheck_sites;
  }
  if (sel_spelling) {
    ++contract.sel_typecheck_sites;
  }
  if (object_pointer_type_spelling) {
    ++contract.object_pointer_typecheck_sites;
    if (object_pointer_type_name.empty()) {
      contract.deterministic = false;
    }
  }

  if (active_spelling_count > 0u) {
    ++contract.total_typecheck_sites;
  }
}

void AccumulateIdClassSelObjectPointerTypecheckMethod(
    const Objc3MethodDecl &method,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  AccumulateIdClassSelObjectPointerTypecheckSite(method.return_id_spelling,
                                                 method.return_class_spelling,
                                                 method.return_sel_spelling,
                                                 method.return_object_pointer_type_spelling,
                                                 method.return_object_pointer_type_name,
                                                 contract);
  for (const auto &param : method.params) {
    AccumulateIdClassSelObjectPointerTypecheckSite(param.id_spelling,
                                                   param.class_spelling,
                                                   param.sel_spelling,
                                                   param.object_pointer_type_spelling,
                                                   param.object_pointer_type_name,
                                                   contract);
  }
}

template <typename Container>
void AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
    const Container &declarations,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      AccumulateIdClassSelObjectPointerTypecheckSite(property.id_spelling,
                                                     property.class_spelling,
                                                     property.sel_spelling,
                                                     property.object_pointer_type_spelling,
                                                     property.object_pointer_type_name,
                                                     contract);
    }
    for (const auto &method : declaration.methods) {
      AccumulateIdClassSelObjectPointerTypecheckMethod(method, contract);
    }
  }
}

Objc3IdClassSelObjectPointerTypecheckContract BuildIdClassSelObjectPointerTypecheckContract(
    const Objc3Program &program) {
  Objc3IdClassSelObjectPointerTypecheckContract contract;
  for (const auto &fn : program.functions) {
    AccumulateIdClassSelObjectPointerTypecheckSite(fn.return_id_spelling,
                                                   fn.return_class_spelling,
                                                   fn.return_sel_spelling,
                                                   fn.return_object_pointer_type_spelling,
                                                   fn.return_object_pointer_type_name,
                                                   contract);
    for (const auto &param : fn.params) {
      AccumulateIdClassSelObjectPointerTypecheckSite(param.id_spelling,
                                                     param.class_spelling,
                                                     param.sel_spelling,
                                                     param.object_pointer_type_spelling,
                                                     param.object_pointer_type_name,
                                                     contract);
    }
  }
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(program.protocols, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(program.interfaces, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(program.implementations, contract);
  return contract;
}

std::size_t CountSelectorPieces(const std::string &selector) {
  if (selector.empty()) {
    return 0;
  }
  std::size_t colons = 0;
  for (char c : selector) {
    if (c == ':') {
      ++colons;
    }
  }
  return colons == 0 ? 1 : colons;
}

void AccumulateMessageSendSelectorLoweringExpr(
    const Expr *expr,
    Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      ++contract.message_send_sites;
      ++contract.receiver_expression_sites;
      if (expr->args.empty()) {
        ++contract.unary_selector_sites;
      } else {
        ++contract.keyword_selector_sites;
      }
      contract.argument_expression_sites += expr->args.size();
      const std::size_t selector_pieces = CountSelectorPieces(expr->selector);
      contract.selector_piece_sites += selector_pieces;
      if (selector_pieces == 0u) {
        contract.deterministic = false;
      } else {
        selector_literals.insert(expr->selector);
      }
      AccumulateMessageSendSelectorLoweringExpr(expr->receiver.get(), contract, selector_literals);
      for (const auto &arg : expr->args) {
        AccumulateMessageSendSelectorLoweringExpr(arg.get(), contract, selector_literals);
      }
      return;
    }
    case Expr::Kind::Binary:
      AccumulateMessageSendSelectorLoweringExpr(expr->left.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(expr->right.get(), contract, selector_literals);
      return;
    case Expr::Kind::Conditional:
      AccumulateMessageSendSelectorLoweringExpr(expr->left.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(expr->right.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(expr->third.get(), contract, selector_literals);
      return;
    case Expr::Kind::Call:
      for (const auto &arg : expr->args) {
        AccumulateMessageSendSelectorLoweringExpr(arg.get(), contract, selector_literals);
      }
      return;
    default:
      return;
  }
}

void AccumulateMessageSendSelectorLoweringForClause(
    const ForClause &clause,
    Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  if (clause.value != nullptr) {
    AccumulateMessageSendSelectorLoweringExpr(clause.value.get(), contract, selector_literals);
  }
}

void AccumulateMessageSendSelectorLoweringStmt(
    const Stmt *stmt,
    Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->let_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->assign_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->return_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->expr_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->if_stmt->condition.get(), contract, selector_literals);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        AccumulateMessageSendSelectorLoweringStmt(then_stmt.get(), contract, selector_literals);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        AccumulateMessageSendSelectorLoweringStmt(else_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->do_while_stmt->condition.get(), contract, selector_literals);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringForClause(stmt->for_stmt->init, contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(stmt->for_stmt->condition.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringForClause(stmt->for_stmt->step, contract, selector_literals);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->switch_stmt->condition.get(), contract, selector_literals);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          AccumulateMessageSendSelectorLoweringStmt(case_stmt.get(), contract, selector_literals);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->while_stmt->condition.get(), contract, selector_literals);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

void AccumulateDispatchSurfaceClassificationExpr(
    const Expr *expr,
    Objc3DispatchSurfaceClassificationContract &contract);

void AccumulateDispatchSurfaceClassificationStmt(
    const Stmt *stmt,
    Objc3DispatchSurfaceClassificationContract &contract) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      AccumulateDispatchSurfaceClassificationExpr(stmt->let_stmt->value.get(),
                                                  contract);
      return;
    case Stmt::Kind::Assign:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->assign_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Return:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->return_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Expr:
      AccumulateDispatchSurfaceClassificationExpr(stmt->expr_stmt->value.get(),
                                                  contract);
      return;
    case Stmt::Kind::If:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->if_stmt->condition.get(), contract);
      for (const auto &nested : stmt->if_stmt->then_body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      for (const auto &nested : stmt->if_stmt->else_body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::DoWhile:
      for (const auto &nested : stmt->do_while_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->do_while_stmt->condition.get(), contract);
      return;
    case Stmt::Kind::For:
      AccumulateDispatchSurfaceClassificationExpr(stmt->for_stmt->init.value.get(),
                                                  contract);
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->for_stmt->condition.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(stmt->for_stmt->step.value.get(),
                                                  contract);
      for (const auto &nested : stmt->for_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::Switch:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->switch_stmt->condition.get(), contract);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        for (const auto &nested : case_stmt.body) {
          AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
        }
      }
      return;
    case Stmt::Kind::While:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->while_stmt->condition.get(), contract);
      for (const auto &nested : stmt->while_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
      for (const auto &nested : stmt->block_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

void AccumulateDispatchSurfaceClassificationExpr(
    const Expr *expr,
    Objc3DispatchSurfaceClassificationContract &contract) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      if (!expr->dispatch_surface_is_normalized) {
        contract.deterministic = false;
      }
      switch (expr->dispatch_surface_kind) {
        case Expr::DispatchSurfaceKind::Instance:
          ++contract.instance_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Class:
          ++contract.class_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Super:
          ++contract.super_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Direct:
          ++contract.direct_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Dynamic:
          ++contract.dynamic_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Unclassified:
        default:
          ++contract.dynamic_dispatch_sites;
          contract.deterministic = false;
          break;
      }
      AccumulateDispatchSurfaceClassificationExpr(expr->receiver.get(), contract);
      for (const auto &arg : expr->args) {
        AccumulateDispatchSurfaceClassificationExpr(arg.get(), contract);
      }
      return;
    }
    case Expr::Kind::Binary:
      AccumulateDispatchSurfaceClassificationExpr(expr->left.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(expr->right.get(), contract);
      return;
    case Expr::Kind::Conditional:
      AccumulateDispatchSurfaceClassificationExpr(expr->left.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(expr->right.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(expr->third.get(), contract);
      return;
    case Expr::Kind::Call:
      AccumulateDispatchSurfaceClassificationExpr(expr->receiver.get(), contract);
      for (const auto &arg : expr->args) {
        AccumulateDispatchSurfaceClassificationExpr(arg.get(), contract);
      }
      return;
    case Expr::Kind::BlockLiteral:
      return;
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::Identifier:
      return;
  }
}

Objc3DispatchSurfaceClassificationContract BuildDispatchSurfaceClassificationContract(
    const Objc3Program &program) {
  Objc3DispatchSurfaceClassificationContract contract;
  for (const auto &global : program.globals) {
    AccumulateDispatchSurfaceClassificationExpr(global.value.get(), contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateDispatchSurfaceClassificationStmt(stmt.get(), contract);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      if (!method_decl.has_body) {
        continue;
      }
      for (const auto &stmt : method_decl.body) {
        AccumulateDispatchSurfaceClassificationStmt(stmt.get(), contract);
      }
    }
  }
  return contract;
}

Objc3MessageSendSelectorLoweringContract BuildMessageSendSelectorLoweringContract(const Objc3Program &program) {
  Objc3MessageSendSelectorLoweringContract contract;
  std::unordered_set<std::string> selector_literals;

  for (const auto &global : program.globals) {
    AccumulateMessageSendSelectorLoweringExpr(global.value.get(), contract, selector_literals);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateMessageSendSelectorLoweringStmt(stmt.get(), contract, selector_literals);
    }
  }
  // parity-expansion anchor: lowering-side selector accounting must
  // walk implementation methods too or runtime legality/method-family evidence
  // drifts from the sema-owned site surface.
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      if (!method_decl.has_body) {
        continue;
      }
      for (const auto &stmt : method_decl.body) {
        AccumulateMessageSendSelectorLoweringStmt(stmt.get(), contract, selector_literals);
      }
    }
  }

  contract.selector_literal_entries = selector_literals.size();
  for (const auto &selector : selector_literals) {
    contract.selector_literal_characters += selector.size();
  }
  return contract;
}

void AccumulateDispatchAbiMarshallingExpr(
    const Expr *expr,
    std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      ++contract.message_send_sites;
      ++contract.receiver_slots_marshaled;
      ++contract.selector_slots_marshaled;
      const std::size_t actual_args = expr->args.size();
      const std::size_t marshalled_args = std::min(actual_args, runtime_dispatch_arg_slots);
      contract.argument_value_slots_marshaled += marshalled_args;
      if (actual_args > runtime_dispatch_arg_slots) {
        contract.deterministic = false;
      }
      contract.argument_padding_slots_marshaled += (runtime_dispatch_arg_slots - marshalled_args);
      contract.argument_total_slots_marshaled += runtime_dispatch_arg_slots;
      AccumulateDispatchAbiMarshallingExpr(expr->receiver.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &arg : expr->args) {
        AccumulateDispatchAbiMarshallingExpr(arg.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    }
    case Expr::Kind::Binary:
      AccumulateDispatchAbiMarshallingExpr(expr->left.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(expr->right.get(), runtime_dispatch_arg_slots, contract);
      return;
    case Expr::Kind::Conditional:
      AccumulateDispatchAbiMarshallingExpr(expr->left.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(expr->right.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(expr->third.get(), runtime_dispatch_arg_slots, contract);
      return;
    case Expr::Kind::Call:
      for (const auto &arg : expr->args) {
        AccumulateDispatchAbiMarshallingExpr(arg.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    default:
      return;
  }
}

void AccumulateDispatchAbiMarshallingForClause(
    const ForClause &clause,
    std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  if (clause.value != nullptr) {
    AccumulateDispatchAbiMarshallingExpr(clause.value.get(), runtime_dispatch_arg_slots, contract);
  }
}

void AccumulateDispatchAbiMarshallingStmt(
    const Stmt *stmt,
    std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->let_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->assign_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->return_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->expr_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->if_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        AccumulateDispatchAbiMarshallingStmt(then_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        AccumulateDispatchAbiMarshallingStmt(else_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->do_while_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingForClause(stmt->for_stmt->init, runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(stmt->for_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingForClause(stmt->for_stmt->step, runtime_dispatch_arg_slots, contract);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->switch_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          AccumulateDispatchAbiMarshallingStmt(case_stmt.get(), runtime_dispatch_arg_slots, contract);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->while_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

Objc3DispatchAbiMarshallingContract BuildDispatchAbiMarshallingContract(
    const Objc3Program &program,
    std::size_t runtime_dispatch_arg_slots) {
  Objc3DispatchAbiMarshallingContract contract;
  contract.runtime_dispatch_arg_slots = runtime_dispatch_arg_slots;

  for (const auto &global : program.globals) {
    AccumulateDispatchAbiMarshallingExpr(global.value.get(), runtime_dispatch_arg_slots, contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateDispatchAbiMarshallingStmt(stmt.get(), runtime_dispatch_arg_slots, contract);
    }
  }
  // parity-expansion anchor: ABI marshalling counts must include
  // implementation method bodies so runtime-link host-link validation stays in
  // lockstep with live super/dynamic dispatch sites.
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      if (!method_decl.has_body) {
        continue;
      }
      for (const auto &stmt : method_decl.body) {
        AccumulateDispatchAbiMarshallingStmt(stmt.get(), runtime_dispatch_arg_slots, contract);
      }
    }
  }

  contract.total_marshaled_slots = contract.receiver_slots_marshaled +
                                   contract.selector_slots_marshaled +
                                   contract.argument_total_slots_marshaled;
  return contract;
}

Objc3NilReceiverSemanticsFoldabilityContract BuildNilReceiverSemanticsFoldabilityContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NilReceiverSemanticsFoldabilityContract contract;
  contract.message_send_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_sites_total;
  contract.receiver_nil_literal_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total;
  contract.nil_receiver_semantics_enabled_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_enabled_sites_total;
  contract.nil_receiver_foldable_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_foldable_sites_total;
  contract.nil_receiver_runtime_dispatch_required_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total;
  contract.non_nil_receiver_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.nil_receiver_semantics_foldability_summary.deterministic &&
      sema_parity_surface.deterministic_nil_receiver_semantics_foldability_handoff;
  return contract;
}

Objc3SuperDispatchMethodFamilyContract BuildSuperDispatchMethodFamilyContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3SuperDispatchMethodFamilyContract contract;
  contract.message_send_sites =
      sema_parity_surface.super_dispatch_method_family_sites_total;
  contract.receiver_super_identifier_sites =
      sema_parity_surface.super_dispatch_method_family_receiver_super_identifier_sites_total;
  contract.super_dispatch_enabled_sites =
      sema_parity_surface.super_dispatch_method_family_enabled_sites_total;
  contract.super_dispatch_requires_class_context_sites =
      sema_parity_surface.super_dispatch_method_family_requires_class_context_sites_total;
  contract.method_family_init_sites =
      sema_parity_surface.super_dispatch_method_family_init_sites_total;
  contract.method_family_copy_sites =
      sema_parity_surface.super_dispatch_method_family_copy_sites_total;
  contract.method_family_mutable_copy_sites =
      sema_parity_surface.super_dispatch_method_family_mutable_copy_sites_total;
  contract.method_family_new_sites =
      sema_parity_surface.super_dispatch_method_family_new_sites_total;
  contract.method_family_none_sites =
      sema_parity_surface.super_dispatch_method_family_none_sites_total;
  contract.method_family_returns_retained_result_sites =
      sema_parity_surface.super_dispatch_method_family_returns_retained_result_sites_total;
  contract.method_family_returns_related_result_sites =
      sema_parity_surface.super_dispatch_method_family_returns_related_result_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.super_dispatch_method_family_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.super_dispatch_method_family_summary.deterministic &&
      sema_parity_surface.deterministic_super_dispatch_method_family_handoff;
  return contract;
}

Objc3RuntimeLinkHostLinkContract BuildRuntimeLinkHostLinkContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3NilReceiverSemanticsFoldabilityContract &nil_receiver_semantics_foldability_contract,
    const Objc3FrontendOptions &options) {
  Objc3RuntimeLinkHostLinkContract contract;
  contract.message_send_sites = dispatch_abi_marshalling_contract.message_send_sites;
  contract.runtime_link_required_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites;
  if (contract.runtime_link_required_sites <= contract.message_send_sites) {
    contract.runtime_link_elided_sites =
        contract.message_send_sites - contract.runtime_link_required_sites;
  } else {
    contract.runtime_link_elided_sites = 0;
    contract.contract_violation_sites = 1;
  }
  contract.runtime_dispatch_arg_slots = options.lowering.max_message_send_args;
  contract.runtime_dispatch_declaration_parameter_count = contract.runtime_dispatch_arg_slots + 2u;
  contract.runtime_dispatch_symbol = options.lowering.runtime_dispatch_symbol;
  contract.default_runtime_dispatch_symbol_binding =
      contract.runtime_dispatch_symbol == kObjc3RuntimeDispatchSymbol;
  contract.deterministic =
      dispatch_abi_marshalling_contract.deterministic &&
      nil_receiver_semantics_foldability_contract.deterministic;
  return contract;
}

Objc3RuntimeDispatchLoweringAbiContract BuildRuntimeDispatchLoweringAbiContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api_summary) {
  Objc3RuntimeDispatchLoweringAbiContract contract;
  contract.message_send_sites = dispatch_abi_marshalling_contract.message_send_sites;
  contract.fixed_argument_slot_count =
      runtime_link_host_link_contract.runtime_dispatch_arg_slots;
  contract.runtime_dispatch_parameter_count =
      runtime_link_host_link_contract
          .runtime_dispatch_declaration_parameter_count;
  contract.canonical_runtime_dispatch_symbol =
      runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
  contract.compatibility_runtime_dispatch_symbol =
      runtime_bootstrap_api_summary.compatibility_dispatch_symbol;
  contract.default_lowering_target_symbol =
      contract.canonical_runtime_dispatch_symbol;
  contract.default_lowering_target_model =
      kObjc3RuntimeDispatchLiveCutoverDefaultTargetModel;
  contract.compatibility_bridge_role_model =
      kObjc3RuntimeDispatchLiveCutoverLanguageProfileModel;
  contract.deferred_cases_model =
      kObjc3RuntimeDispatchLiveCutoverDeferredCasesModel;
  contract.selector_lookup_symbol =
      runtime_bootstrap_api_summary.selector_lookup_symbol;
  contract.selector_handle_type =
      runtime_bootstrap_api_summary.selector_handle_type;
  contract.fail_closed =
      IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api_summary);
  contract.deterministic =
      dispatch_abi_marshalling_contract.deterministic &&
      runtime_link_host_link_contract.deterministic &&
      !runtime_bootstrap_api_summary.replay_key.empty();
  return contract;
}

Objc3OwnershipQualifierLoweringContract BuildOwnershipQualifierLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3OwnershipQualifierLoweringContract contract;
  contract.ownership_qualifier_sites =
      sema_parity_surface.type_annotation_ownership_qualifier_sites_total;
  contract.invalid_ownership_qualifier_sites =
      sema_parity_surface.type_annotation_invalid_ownership_qualifier_sites_total;
  contract.object_pointer_type_annotation_sites =
      sema_parity_surface.type_annotation_object_pointer_type_sites_total;
  contract.deterministic =
      sema_parity_surface.type_annotation_surface_summary.deterministic &&
      sema_parity_surface.deterministic_type_annotation_surface_handoff;
  return contract;
}

Objc3RetainReleaseOperationLoweringContract BuildRetainReleaseOperationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RetainReleaseOperationLoweringContract contract;
  contract.ownership_qualified_sites =
      sema_parity_surface.retain_release_operation_ownership_qualified_sites_total;
  contract.retain_insertion_sites =
      sema_parity_surface.retain_release_operation_retain_insertion_sites_total;
  contract.release_insertion_sites =
      sema_parity_surface.retain_release_operation_release_insertion_sites_total;
  contract.autorelease_insertion_sites =
      sema_parity_surface.retain_release_operation_autorelease_insertion_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.retain_release_operation_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.retain_release_operation_summary.deterministic &&
      sema_parity_surface.deterministic_retain_release_operation_handoff;
  return contract;
}

Objc3AutoreleasePoolScopeLoweringContract BuildAutoreleasePoolScopeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3AutoreleasePoolScopeLoweringContract contract;
  contract.scope_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_symbolized_sites = sema_parity_surface.autoreleasepool_scope_symbolized_sites_total;
  contract.max_scope_depth = sema_parity_surface.autoreleasepool_scope_max_depth_total;
  contract.scope_entry_transition_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_exit_transition_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.contract_violation_sites = sema_parity_surface.autoreleasepool_scope_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.autoreleasepool_scope_summary.deterministic &&
      sema_parity_surface.deterministic_autoreleasepool_scope_handoff;
  return contract;
}

Objc3WeakUnownedSemanticsLoweringContract BuildWeakUnownedSemanticsLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3WeakUnownedSemanticsLoweringContract contract;
  contract.ownership_candidate_sites =
      sema_parity_surface.weak_unowned_semantics_ownership_candidate_sites_total;
  contract.weak_reference_sites =
      sema_parity_surface.weak_unowned_semantics_weak_reference_sites_total;
  contract.unowned_reference_sites =
      sema_parity_surface.weak_unowned_semantics_unowned_reference_sites_total;
  contract.unowned_safe_reference_sites =
      sema_parity_surface.weak_unowned_semantics_unowned_safe_reference_sites_total;
  contract.weak_unowned_conflict_sites =
      sema_parity_surface.weak_unowned_semantics_conflict_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.weak_unowned_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.weak_unowned_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_weak_unowned_semantics_handoff;
  return contract;
}

Objc3ArcDiagnosticsFixitLoweringContract BuildArcDiagnosticsFixitLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ArcDiagnosticsFixitLoweringContract contract;
  contract.ownership_arc_diagnostic_candidate_sites =
      sema_parity_surface.ownership_arc_diagnostic_candidate_sites_total;
  contract.ownership_arc_fixit_available_sites =
      sema_parity_surface.ownership_arc_fixit_available_sites_total;
  contract.ownership_arc_profiled_sites =
      sema_parity_surface.ownership_arc_profiled_sites_total;
  contract.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      sema_parity_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total;
  contract.ownership_arc_empty_fixit_hint_sites =
      sema_parity_surface.ownership_arc_empty_fixit_hint_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.ownership_arc_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.arc_diagnostics_fixit_summary.deterministic &&
      sema_parity_surface.deterministic_arc_diagnostics_fixit_handoff;
  return contract;
}

Objc3BlockLiteralCaptureLoweringContract BuildBlockLiteralCaptureLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockLiteralCaptureLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_literal_capture_semantics_sites_total;
  contract.block_parameter_entries =
      sema_parity_surface.block_literal_capture_semantics_parameter_entries_total;
  contract.block_capture_entries =
      sema_parity_surface.block_literal_capture_semantics_capture_entries_total;
  contract.block_body_statement_entries =
      sema_parity_surface.block_literal_capture_semantics_body_statement_entries_total;
  contract.block_empty_capture_sites =
      sema_parity_surface.block_literal_capture_semantics_empty_capture_sites_total;
  contract.block_nondeterministic_capture_sites =
      sema_parity_surface.block_literal_capture_semantics_nondeterministic_capture_sites_total;
  contract.block_non_normalized_sites =
      sema_parity_surface.block_literal_capture_semantics_non_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_literal_capture_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_literal_capture_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_literal_capture_semantics_handoff;
  return contract;
}

bool IsStrictlySortedUniqueStrings(const std::vector<std::string> &entries) {
  if (!std::is_sorted(entries.begin(), entries.end())) {
    return false;
  }
  return std::adjacent_find(entries.begin(), entries.end()) == entries.end();
}

void AccumulateBlockSourceModelCompletionFromExpr(
    const Expr *expr,
    Objc3BlockSourceModelCompletionContract &contract);

void AccumulateBlockSourceModelCompletionFromStmt(
    const Stmt *stmt,
    Objc3BlockSourceModelCompletionContract &contract) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->let_stmt->value.get(), contract);
      }
      break;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->assign_stmt->value.get(), contract);
      }
      break;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->return_stmt->value.get(), contract);
      }
      break;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->if_stmt->condition.get(), contract);
        for (const auto &child : stmt->if_stmt->then_body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
        for (const auto &child : stmt->if_stmt->else_body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
      }
      break;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        for (const auto &child : stmt->do_while_stmt->body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->do_while_stmt->condition.get(), contract);
      }
      break;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->for_stmt->init.value.get(), contract);
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->for_stmt->condition.get(), contract);
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->for_stmt->step.value.get(), contract);
        for (const auto &child : stmt->for_stmt->body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
      }
      break;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->switch_stmt->condition.get(), contract);
        for (const auto &switch_case : stmt->switch_stmt->cases) {
          for (const auto &child : switch_case.body) {
            AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
          }
        }
      }
      break;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->while_stmt->condition.get(), contract);
        for (const auto &child : stmt->while_stmt->body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
      }
      break;
    case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        for (const auto &child : stmt->block_stmt->body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
      }
      break;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->expr_stmt->value.get(), contract);
      }
      break;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      break;
  }
}

void AccumulateBlockSourceModelCompletionFromExpr(
    const Expr *expr,
    Objc3BlockSourceModelCompletionContract &contract) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    ++contract.block_literal_sites;
    contract.signature_entries_total +=
        expr->block_parameter_signature_entries_lexicographic.size();
    contract.explicit_typed_parameter_entries_total +=
        expr->block_explicit_typed_parameter_count;
    contract.implicit_parameter_entries_total +=
        expr->block_implicit_parameter_count;
    contract.capture_inventory_entries_total +=
        expr->block_capture_inventory_entries_lexicographic.size();
    contract.byvalue_readonly_capture_entries_total +=
        expr->block_byvalue_readonly_capture_count;
    contract.invoke_surface_entries_total +=
        expr->block_invoke_surface_entries_lexicographic.size();

    if (!expr->block_source_model_is_normalized) {
      ++contract.non_normalized_sites;
    }

    bool contract_violation = false;
    contract_violation |=
        expr->block_parameter_signature_entries_lexicographic.size() !=
        expr->block_parameter_count;
    contract_violation |=
        expr->block_explicit_typed_parameter_count +
                expr->block_implicit_parameter_count !=
        expr->block_parameter_count;
    contract_violation |=
        expr->block_capture_inventory_entries_lexicographic.size() !=
        expr->block_capture_count;
    contract_violation |=
        expr->block_byvalue_readonly_capture_count != expr->block_capture_count;
    contract_violation |=
        expr->block_invoke_surface_entries_lexicographic.size() != 2u;
    contract_violation |= expr->block_signature_profile.empty();
    contract_violation |= expr->block_capture_inventory_profile.empty();
    contract_violation |= expr->block_invoke_surface_profile.empty();
    contract_violation |= expr->block_abi_descriptor_symbol.empty();
    contract_violation |= expr->block_invoke_trampoline_symbol.empty();
    contract_violation |= expr->block_source_model_replay_key.empty();
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_parameter_signature_entries_lexicographic);
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_capture_inventory_entries_lexicographic);
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_invoke_surface_entries_lexicographic);

    if (contract_violation) {
      ++contract.contract_violation_sites;
    }
  }

  AccumulateBlockSourceModelCompletionFromExpr(expr->receiver.get(), contract);
  AccumulateBlockSourceModelCompletionFromExpr(expr->left.get(), contract);
  AccumulateBlockSourceModelCompletionFromExpr(expr->right.get(), contract);
  AccumulateBlockSourceModelCompletionFromExpr(expr->third.get(), contract);
  for (const auto &arg : expr->args) {
    AccumulateBlockSourceModelCompletionFromExpr(arg.get(), contract);
  }
}

Objc3BlockSourceModelCompletionContract BuildBlockSourceModelCompletionContract(
    const Objc3Program &program) {
  Objc3BlockSourceModelCompletionContract contract;

  for (const auto &global : program.globals) {
    AccumulateBlockSourceModelCompletionFromExpr(global.value.get(), contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
    }
  }
  for (const auto &protocol : program.protocols) {
    for (const auto &method : protocol.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
      }
    }
  }

  contract.deterministic = contract.non_normalized_sites == 0u &&
                           contract.contract_violation_sites == 0u;
  return contract;
}

void AccumulateBlockSourceStorageAnnotationFromStmt(
    const Stmt *stmt,
    Objc3BlockSourceStorageAnnotationContract &contract);

void AccumulateBlockSourceStorageAnnotationFromExpr(
    const Expr *expr,
    Objc3BlockSourceStorageAnnotationContract &contract) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    ++contract.block_literal_sites;
    contract.capture_entries_total += expr->block_capture_count;
    contract.mutated_capture_entries_total += expr->block_mutated_capture_count;
    contract.byref_capture_entries_total += expr->block_byref_capture_count;
    if (expr->block_copy_helper_intent_required) {
      ++contract.copy_helper_intent_sites;
    }
    if (expr->block_dispose_helper_intent_required) {
      ++contract.dispose_helper_intent_sites;
    }
    if (expr->block_escape_shape_promotes_to_heap_candidate) {
      ++contract.heap_candidate_sites;
    }

    if (expr->block_escape_shape_symbol == "expression-site") {
      ++contract.expression_sites;
    } else if (expr->block_escape_shape_symbol == "global-initializer") {
      ++contract.global_initializer_sites;
    } else if (expr->block_escape_shape_symbol == "binding-initializer") {
      ++contract.binding_initializer_sites;
    } else if (expr->block_escape_shape_symbol == "assignment-value") {
      ++contract.assignment_value_sites;
    } else if (expr->block_escape_shape_symbol == "return-value") {
      ++contract.return_value_sites;
    } else if (expr->block_escape_shape_symbol == "call-argument") {
      ++contract.call_argument_sites;
    } else if (expr->block_escape_shape_symbol == "message-argument") {
      ++contract.message_argument_sites;
    } else {
      ++contract.contract_violation_sites;
    }

    if (!expr->block_source_storage_annotations_are_normalized) {
      ++contract.non_normalized_sites;
    }

    bool contract_violation = false;
    contract_violation |= expr->block_mutated_capture_count >
                          expr->block_capture_count;
    contract_violation |= expr->block_byref_capture_count >
                          expr->block_mutated_capture_count;
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_mutated_capture_names_lexicographic);
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_byref_capture_names_lexicographic);
    contract_violation |= expr->block_escape_shape_symbol.empty();
    contract_violation |= expr->block_escape_shape_profile.empty();
    contract_violation |= expr->block_helper_intent_profile.empty();
    contract_violation |= expr->block_copy_helper_intent_required !=
                          (expr->block_byref_capture_count > 0u);
    contract_violation |= expr->block_dispose_helper_intent_required !=
                          (expr->block_byref_capture_count > 0u);
    contract_violation |= expr->block_escape_shape_promotes_to_heap_candidate !=
                          (expr->block_escape_shape_symbol != "expression-site");
    if (contract_violation) {
      ++contract.contract_violation_sites;
    }
  }

  AccumulateBlockSourceStorageAnnotationFromExpr(expr->receiver.get(), contract);
  AccumulateBlockSourceStorageAnnotationFromExpr(expr->left.get(), contract);
  AccumulateBlockSourceStorageAnnotationFromExpr(expr->right.get(), contract);
  AccumulateBlockSourceStorageAnnotationFromExpr(expr->third.get(), contract);
  for (const auto &arg : expr->args) {

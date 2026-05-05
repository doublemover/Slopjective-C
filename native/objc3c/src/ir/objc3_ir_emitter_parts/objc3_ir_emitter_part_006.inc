                   .metaprogramming_synthesized_emitted_runtime_method_list_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.metaprogramming_synthesized_guard_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.metaprogramming_synthesized_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_metaprogramming_synthesized_emission_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!106 = !{!\""
        << EscapeCStringLiteral(frontend_metadata_
                                    .lowering_metaprogramming_module_interface_replay_preservation_key)
        << "\", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.metaprogramming_module_replay_local_derive_method_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .metaprogramming_module_replay_local_macro_artifact_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .metaprogramming_module_replay_local_interface_property_behavior_artifact_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .metaprogramming_module_replay_local_implementation_property_behavior_artifact_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .metaprogramming_module_replay_local_runtime_method_list_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.metaprogramming_module_replay_imported_module_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .metaprogramming_module_replay_imported_derive_method_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .metaprogramming_module_replay_imported_macro_artifact_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .metaprogramming_module_replay_imported_interface_property_behavior_artifact_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .metaprogramming_module_replay_imported_implementation_property_behavior_artifact_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .metaprogramming_module_replay_imported_runtime_method_list_count)
        << ", i1 "
        << (frontend_metadata_
                    .metaprogramming_module_replay_runtime_import_artifact_ready
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .metaprogramming_module_replay_separate_compilation_preservation_ready
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_metaprogramming_module_interface_replay_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!107 = !{!\""
        << EscapeCStringLiteral(Objc3MetaprogrammingExpansionHostRuntimeBoundarySummary())
        << "\", i1 1, i1 0, i1 0, i1 0, i1 1}\n\n";
    out << "!103 = !{!\""
        << EscapeCStringLiteral(frontend_metadata_
                                    .lowering_dispatch_dispatch_metadata_interface_preservation_key)
        << "\", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .dispatch_dispatch_metadata_local_direct_callable_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .dispatch_dispatch_metadata_local_final_callable_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .dispatch_dispatch_metadata_local_final_container_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .dispatch_dispatch_metadata_local_sealed_container_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_dispatch_metadata_imported_module_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .dispatch_dispatch_metadata_imported_direct_callable_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .dispatch_dispatch_metadata_imported_final_callable_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .dispatch_dispatch_metadata_imported_final_container_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .dispatch_dispatch_metadata_imported_sealed_container_record_count)
        << ", i1 "
        << (frontend_metadata_
                    .dispatch_dispatch_metadata_runtime_import_artifact_ready
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .dispatch_dispatch_metadata_separate_compilation_preservation_ready
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_dispatch_dispatch_metadata_interface_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!98 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ownership_system_extension_lowering_cleanup_hook_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ownership_system_extension_lowering_resource_local_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_cleanup_owned_local_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_resource_move_capture_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_borrowed_parameter_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_borrowed_return_callable_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_borrowed_escape_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_explicit_capture_item_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_retainable_family_callable_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_retainable_family_operation_callable_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_retainable_family_alias_callable_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ownership_system_extension_lowering_guard_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_system_extension_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_ownership_system_extension_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!99 = !{!\""
        << EscapeCStringLiteral(
               kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.ownership_borrowed_retainable_abi_completion_replay_key)
        << "\", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_borrowed_retainable_returns_borrowed_attribute_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ownership_borrowed_retainable_family_retain_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ownership_borrowed_retainable_family_release_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_borrowed_retainable_family_autorelease_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_borrowed_retainable_compatibility_returns_retained_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_borrowed_retainable_compatibility_returns_not_retained_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ownership_borrowed_retainable_compatibility_consumed_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_ownership_borrowed_retainable_abi_completion_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!100 = !{!\""
        << EscapeCStringLiteral(kObjc3OwnershipSystemHelperRuntimeContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipSystemExtensionLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\"objc3_runtime_copy_memory_management_state_for_testing\", !\"objc3_runtime_copy_arc_debug_state_for_testing\"}\n\n";
    out << "!101 = !{!\""
        << EscapeCStringLiteral(
               kObjc3OwnershipLiveCleanupRetainableIntegrationContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipSystemHelperRuntimeContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\"objc3_runtime_copy_memory_management_state_for_testing\", !\"objc3_runtime_copy_arc_debug_state_for_testing\"}\n\n";
    out << "!42 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.await_lowering_suspension_state_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_keyword_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_suspension_point_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_resume_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_state_machine_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_continuation_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_gate_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_await_lowering_suspension_state_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!43 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_async_keyword_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_async_function_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_continuation_allocation_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_continuation_resume_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_continuation_suspend_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_async_state_machine_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_gate_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_async_continuation_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!44 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.error_diagnostics_recovery_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_parser_diagnostic_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_semantic_diagnostic_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_fixit_hint_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_recovery_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_recovery_applied_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_guard_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_error_diagnostics_recovery_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
  }

  void RegisterSelectorLiteral(const std::string &selector) {
    if (selector.empty() ||
        selector_pool_globals_.find(selector) != selector_pool_globals_.end()) {
      return;
    }
    selector_pool_globals_.emplace(selector, "");
  }

  void RegisterRuntimeStringLiteral(const std::string &value) {
    if (value.empty() ||
        runtime_string_pool_globals_.find(value) !=
            runtime_string_pool_globals_.end()) {
      return;
    }
    runtime_string_pool_globals_.emplace(value, "");
  }

  void AssignCanonicalPoolGlobalNames() {
    std::size_t index = 0;
    for (auto &entry : selector_pool_globals_) {
      entry.second = "@__objc3_sel_pool_" +
                     FormatRuntimeMetadataDescriptorOrdinal(index++);
    }
    index = 0;
    for (auto &entry : runtime_string_pool_globals_) {
      entry.second = "@__objc3_str_pool_" +
                     FormatRuntimeMetadataDescriptorOrdinal(index++);
    }
  }

  void RegisterTypedKeyPathLiteral(const Expr &expr) {
    if (!expr.typed_keypath_literal_enabled ||
        !expr.typed_keypath_literal_is_normalized ||
        expr.typed_keypath_components.empty()) {
      return;
    }
    const std::string profile =
        expr.typed_keypath_literal_profile.empty()
            ? std::string("typed-keypath:root=") + expr.typed_keypath_root_name
            : expr.typed_keypath_literal_profile;
    if (typed_keypath_artifacts_.find(profile) != typed_keypath_artifacts_.end()) {
      return;
    }
    TypedKeyPathArtifact artifact;
    artifact.root_is_self = expr.typed_keypath_root_is_self;
    artifact.root_name = expr.typed_keypath_root_name;
    artifact.component_path = JoinStringParts(expr.typed_keypath_components, ".");
    artifact.profile = profile;
    typed_keypath_artifacts_.emplace(profile, std::move(artifact));
    RegisterRuntimeStringLiteral(expr.typed_keypath_root_name);
    RegisterRuntimeStringLiteral(JoinStringParts(expr.typed_keypath_components, "."));
    RegisterRuntimeStringLiteral(profile);
    if (!frontend_metadata_.lowering_generic_metadata_abi_replay_key.empty()) {
      RegisterRuntimeStringLiteral(
          frontend_metadata_.lowering_generic_metadata_abi_replay_key);
    }
  }

  void AssignTypedKeyPathArtifactOrdinals() {
    std::size_t ordinal = 0;
    for (auto &entry : typed_keypath_artifacts_) {
      entry.second.ordinal = ordinal;
      entry.second.descriptor_symbol =
          "@__objc3_keypath_desc_" + FormatRuntimeMetadataDescriptorOrdinal(ordinal);
      ++ordinal;
    }
  }

  void CollectSelectorExpr(const Expr *expr) {
    if (expr == nullptr) {
      return;
    }
    if (expr->typed_keypath_literal_enabled) {
      RegisterTypedKeyPathLiteral(*expr);
    }
    switch (expr->kind) {
      case Expr::Kind::MessageSend:
        RegisterSelectorLiteral(expr->selector);
        CollectSelectorExpr(expr->receiver.get());
        for (const auto &arg : expr->args) {
          CollectSelectorExpr(arg.get());
        }
        return;
      case Expr::Kind::Binary:
        CollectSelectorExpr(expr->left.get());
        CollectSelectorExpr(expr->right.get());
        return;
      case Expr::Kind::Conditional:
        CollectSelectorExpr(expr->left.get());
        CollectSelectorExpr(expr->right.get());
        CollectSelectorExpr(expr->third.get());
        return;
      case Expr::Kind::Call:
        for (const auto &arg : expr->args) {
          CollectSelectorExpr(arg.get());
        }
        return;
      default:
        return;
    }
  }

  void CollectSelectorStmt(const Stmt *stmt) {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt != nullptr) {
          CollectSelectorExpr(stmt->let_stmt->value.get());
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt != nullptr) {
          CollectSelectorExpr(stmt->assign_stmt->value.get());
        }
        return;
      case Stmt::Kind::Return:
        if (stmt->return_stmt != nullptr) {
          CollectSelectorExpr(stmt->return_stmt->value.get());
        }
        return;
      case Stmt::Kind::Expr:
        if (stmt->expr_stmt != nullptr) {
          CollectSelectorExpr(stmt->expr_stmt->value.get());
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->if_stmt->condition.get());
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectSelectorStmt(then_stmt.get());
        }
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectSelectorStmt(else_stmt.get());
        }
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        CollectSelectorExpr(stmt->do_while_stmt->condition.get());
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->for_stmt->init.value.get());
        CollectSelectorExpr(stmt->for_stmt->condition.get());
        CollectSelectorExpr(stmt->for_stmt->step.value.get());
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->switch_stmt->condition.get());
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectSelectorStmt(case_body_stmt.get());
          }
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->while_stmt->condition.get());
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        return;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectSelectorStmt(nested_stmt.get());
        }
        return;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectRuntimeMetadataPoolLiterals() {
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_class_metaclass_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.class_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.protocol_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_category_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.class_name);
      RegisterRuntimeStringLiteral(bundle.category_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      RegisterRuntimeStringLiteral(bundle.record_kind);
      RegisterRuntimeStringLiteral(bundle.category_owner_identity);
      RegisterRuntimeStringLiteral(bundle.class_owner_identity);
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.declaration_owner_identity);
      RegisterRuntimeStringLiteral(bundle.export_owner_identity);
      for (const auto &entry : bundle.entries_lexicographic) {
        RegisterSelectorLiteral(entry.selector);
        RegisterRuntimeStringLiteral(entry.owner_identity);
        RegisterRuntimeStringLiteral(entry.return_type_name);
      }
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_property_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.property_name);
      RegisterRuntimeStringLiteral(bundle.type_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      RegisterRuntimeStringLiteral(bundle.declaration_owner_identity);
      RegisterRuntimeStringLiteral(bundle.export_owner_identity);
      if (bundle.has_getter) {
        RegisterSelectorLiteral(bundle.getter_selector);
      }
      if (bundle.has_setter) {
        RegisterSelectorLiteral(bundle.setter_selector);
      }
      if (!bundle.ivar_binding_symbol.empty()) {
        RegisterRuntimeStringLiteral(bundle.ivar_binding_symbol);
      }
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      RegisterRuntimeStringLiteral(bundle.declaration_owner_identity);
      RegisterRuntimeStringLiteral(bundle.export_owner_identity);
      RegisterRuntimeStringLiteral(bundle.property_owner_identity);
      RegisterRuntimeStringLiteral(bundle.property_name);
      RegisterRuntimeStringLiteral(bundle.ivar_binding_symbol);
    }
  }

  void CollectCanonicalPoolLiterals() {
    for (const auto &global : program_.globals) {
      CollectSelectorExpr(global.value.get());
    }
    for (const auto &fn : program_.functions) {
      for (const auto &stmt : fn.body) {
        CollectSelectorStmt(stmt.get());
      }
    }
    CollectRuntimeMetadataPoolLiterals();
    AssignCanonicalPoolGlobalNames();
    AssignTypedKeyPathArtifactOrdinals();
  }

  static bool IsNameBoundInScopes(const std::vector<std::unordered_set<std::string>> &scopes,
                                  const std::string &name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      if (it->find(name) != it->end()) {
        return true;
      }
    }
    return false;
  }

  void NotePotentialGlobalMutation(const std::string &name,
                                   const std::vector<std::unordered_set<std::string>> &scopes) {
    if (name.empty() || IsNameBoundInScopes(scopes, name)) {
      return;
    }
    if (globals_.find(name) != globals_.end()) {
      mutable_global_symbols_.insert(name);
    }
  }

  void CollectMutableGlobalSymbolsForClause(const ForClause &clause, std::vector<std::unordered_set<std::string>> &scopes) {
    switch (clause.kind) {
      case ForClause::Kind::None:
      case ForClause::Kind::Expr:
        return;
      case ForClause::Kind::Let:
        if (!scopes.empty() && !clause.name.empty()) {
          scopes.back().insert(clause.name);
        }
        return;
      case ForClause::Kind::Assign:
        NotePotentialGlobalMutation(clause.name, scopes);
        return;
    }
  }

  void CollectMutableGlobalSymbolsStmt(const Stmt *stmt, std::vector<std::unordered_set<std::string>> &scopes) {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt != nullptr && !stmt->let_stmt->name.empty() && !scopes.empty()) {
          scopes.back().insert(stmt->let_stmt->name);
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt != nullptr) {
          NotePotentialGlobalMutation(stmt->assign_stmt->name, scopes);
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectMutableGlobalSymbolsStmt(then_stmt.get(), scopes);
        }
        scopes.pop_back();
        scopes.push_back({});
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectMutableGlobalSymbolsStmt(else_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectMutableGlobalSymbolsStmt(loop_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        CollectMutableGlobalSymbolsForClause(stmt->for_stmt->init, scopes);
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectMutableGlobalSymbolsStmt(loop_stmt.get(), scopes);
        }
        scopes.pop_back();
        CollectMutableGlobalSymbolsForClause(stmt->for_stmt->step, scopes);
        scopes.pop_back();
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          scopes.push_back({});
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectMutableGlobalSymbolsStmt(case_body_stmt.get(), scopes);
          }
          scopes.pop_back();
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectMutableGlobalSymbolsStmt(loop_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectMutableGlobalSymbolsStmt(nested_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Return:
      case Stmt::Kind::Expr:
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectMutableGlobalSymbols() {
    mutable_global_symbols_.clear();
    for (const FunctionDecl *fn : function_definitions_) {
      if (fn == nullptr) {
        continue;
      }
      std::vector<std::unordered_set<std::string>> scopes;
      scopes.push_back({});
      for (const auto &param : fn->params) {
        scopes.back().insert(param.name);
      }
      for (const auto &stmt : fn->body) {
        CollectMutableGlobalSymbolsStmt(stmt.get(), scopes);
      }
    }
  }

  bool IsGlobalSymbolWriteTarget(const std::string &name,
                                 const std::vector<std::unordered_set<std::string>> &scopes) const {
    if (name.empty() || IsNameBoundInScopes(scopes, name)) {
      return false;
    }
    return globals_.find(name) != globals_.end();
  }

  void CollectFunctionEffectExpr(const Expr *expr, std::vector<std::unordered_set<std::string>> &scopes,
                                 FunctionEffectInfo &info) const {
    if (expr == nullptr) {
      return;
    }
    switch (expr->kind) {
      case Expr::Kind::Number:
      case Expr::Kind::BoolLiteral:
      case Expr::Kind::NilLiteral:
      case Expr::Kind::Identifier:
      case Expr::Kind::BlockLiteral:
        return;
      case Expr::Kind::Binary:
        CollectFunctionEffectExpr(expr->left.get(), scopes, info);
        CollectFunctionEffectExpr(expr->right.get(), scopes, info);
        return;
      case Expr::Kind::Conditional:
        CollectFunctionEffectExpr(expr->left.get(), scopes, info);
        CollectFunctionEffectExpr(expr->right.get(), scopes, info);
        CollectFunctionEffectExpr(expr->third.get(), scopes, info);
        return;
      case Expr::Kind::Call:
        info.called_functions.insert(expr->ident);
        for (const auto &arg : expr->args) {
          CollectFunctionEffectExpr(arg.get(), scopes, info);
        }
        return;
      case Expr::Kind::MessageSend:
        info.has_message_send = true;
        CollectFunctionEffectExpr(expr->receiver.get(), scopes, info);
        for (const auto &arg : expr->args) {
          CollectFunctionEffectExpr(arg.get(), scopes, info);
        }
        return;
    }
  }

  void CollectFunctionEffectForClause(const ForClause &clause, std::vector<std::unordered_set<std::string>> &scopes,
                                      FunctionEffectInfo &info) const {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return;
      case ForClause::Kind::Expr:
        CollectFunctionEffectExpr(clause.value.get(), scopes, info);
        return;
      case ForClause::Kind::Let:
        CollectFunctionEffectExpr(clause.value.get(), scopes, info);
        if (!scopes.empty() && !clause.name.empty()) {
          scopes.back().insert(clause.name);
        }
        return;
      case ForClause::Kind::Assign:
        if (IsGlobalSymbolWriteTarget(clause.name, scopes)) {
          info.has_global_write = true;
        }
        CollectFunctionEffectExpr(clause.value.get(), scopes, info);
        return;
    }
  }

  void CollectFunctionEffectStmt(const Stmt *stmt, std::vector<std::unordered_set<std::string>> &scopes,
                                 FunctionEffectInfo &info) const {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->let_stmt->value.get(), scopes, info);
        if (!scopes.empty() && !stmt->let_stmt->name.empty()) {
          scopes.back().insert(stmt->let_stmt->name);
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt == nullptr) {
          return;
        }
        if (IsGlobalSymbolWriteTarget(stmt->assign_stmt->name, scopes)) {
          info.has_global_write = true;
        }
        CollectFunctionEffectExpr(stmt->assign_stmt->value.get(), scopes, info);
        return;
      case Stmt::Kind::Return:
        if (stmt->return_stmt != nullptr) {
          CollectFunctionEffectExpr(stmt->return_stmt->value.get(), scopes, info);
        }
        return;
      case Stmt::Kind::Expr:
        if (stmt->expr_stmt != nullptr) {
          CollectFunctionEffectExpr(stmt->expr_stmt->value.get(), scopes, info);
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->if_stmt->condition.get(), scopes, info);
        scopes.push_back({});
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectFunctionEffectStmt(then_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        scopes.push_back({});
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectFunctionEffectStmt(else_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectFunctionEffectStmt(loop_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        CollectFunctionEffectExpr(stmt->do_while_stmt->condition.get(), scopes, info);
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        CollectFunctionEffectForClause(stmt->for_stmt->init, scopes, info);
        CollectFunctionEffectExpr(stmt->for_stmt->condition.get(), scopes, info);
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectFunctionEffectStmt(loop_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        CollectFunctionEffectForClause(stmt->for_stmt->step, scopes, info);
        scopes.pop_back();
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->switch_stmt->condition.get(), scopes, info);
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          scopes.push_back({});
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectFunctionEffectStmt(case_body_stmt.get(), scopes, info);
          }
          scopes.pop_back();
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->while_stmt->condition.get(), scopes, info);
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectFunctionEffectStmt(loop_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectFunctionEffectStmt(nested_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectFunctionEffects() {
    function_effects_.clear();
    impure_functions_.clear();

    for (const FunctionDecl *fn : function_definitions_) {
      if (fn == nullptr) {
        continue;
      }
      FunctionEffectInfo info;
      std::vector<std::unordered_set<std::string>> scopes;
      scopes.push_back({});
      for (const auto &param : fn->params) {
        scopes.back().insert(param.name);
      }
      for (const auto &stmt : fn->body) {
        CollectFunctionEffectStmt(stmt.get(), scopes, info);
      }
      function_effects_[fn->name] = std::move(info);
    }

    for (const auto &entry : function_effects_) {
      const FunctionEffectInfo &info = entry.second;
      if (info.has_global_write || info.has_message_send) {
        impure_functions_.insert(entry.first);
      }
    }

    bool changed = true;
    while (changed) {
      changed = false;
      for (const auto &entry : function_effects_) {
        const std::string &name = entry.first;
        if (impure_functions_.find(name) != impure_functions_.end()) {
          continue;
        }
        for (const std::string &callee : entry.second.called_functions) {
          const bool callee_defined = defined_functions_.find(callee) != defined_functions_.end();
          const bool callee_declared_pure = declared_pure_functions_.find(callee) != declared_pure_functions_.end();
          if ((!callee_defined && !callee_declared_pure) ||
              impure_functions_.find(callee) != impure_functions_.end()) {
            impure_functions_.insert(name);
            changed = true;
            break;
          }
        }
      }
    }
  }

  bool FunctionMayHaveGlobalSideEffects(const std::string &name) const {
    if (name.empty()) {
      return true;
    }
    if (defined_functions_.find(name) == defined_functions_.end()) {
      return declared_pure_functions_.find(name) == declared_pure_functions_.end();
    }
    return impure_functions_.find(name) != impure_functions_.end();
  }

  bool ShouldEmitRuntimeMetadataSectionScaffold() const {
    // emitted metadata inventory freeze anchor: the currently
    // supported emitted inventory is image-info plus class/protocol/category/
    // property/ivar descriptor sections retained via llvm.used. Separate
    // method/selector/string-pool section families remain explicit non-goals
    // until later work extend the inventory model.
    return frontend_metadata_.runtime_metadata_section_ready_for_scaffold &&
           frontend_metadata_.runtime_export_ready_for_runtime_export &&
           frontend_metadata_.runtime_metadata_section_publication_emitted &&
           frontend_metadata_.runtime_metadata_section_publication_fail_closed &&
           frontend_metadata_.runtime_metadata_section_publication_uses_llvm_used &&
           frontend_metadata_.runtime_metadata_section_publication_image_info_emitted;
  }

  static std::string FormatRuntimeMetadataDescriptorOrdinal(std::size_t ordinal) {
    std::ostringstream formatted;
    formatted << std::setw(4) << std::setfill('0') << ordinal;
    return formatted.str();
  }

  std::string BuildRuntimeMetadataDescriptorSymbol(
      const std::string &descriptor_symbol_prefix, const std::string &kind,
      std::size_t ordinal) const {
    return "@" + descriptor_symbol_prefix + kind + "_" +
           FormatRuntimeMetadataDescriptorOrdinal(ordinal);
  }

  std::string BuildRuntimeMetadataAuxiliarySymbol(
      const std::string &descriptor_symbol_prefix, const std::string &kind,
      const std::string &suffix, std::size_t ordinal) const {
    return "@" + descriptor_symbol_prefix + kind + "_" + suffix + "_" +
           FormatRuntimeMetadataDescriptorOrdinal(ordinal);
  }

  bool TryBuildRuntimeMetadataLayoutPolicy(
      Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error) const {
    Objc3RuntimeMetadataLayoutPolicyInput input;
    input.abi_contract_id =
        frontend_metadata_.runtime_metadata_section_abi_contract_id;
    input.scaffold_contract_id =
        frontend_metadata_.runtime_metadata_section_publication_contract_id;
    input.section_boundary_ready =
        frontend_metadata_.runtime_metadata_section_ready_for_scaffold;
    input.runtime_export_ready =
        frontend_metadata_.runtime_export_ready_for_runtime_export;
    input.scaffold_emitted =
        frontend_metadata_.runtime_metadata_section_publication_emitted;
    input.scaffold_fail_closed =
        frontend_metadata_.runtime_metadata_section_publication_fail_closed;
    input.uses_llvm_used =
        frontend_metadata_.runtime_metadata_section_publication_uses_llvm_used;
    input.image_info_emitted =
        frontend_metadata_.runtime_metadata_section_publication_image_info_emitted;
    input.image_info_symbol =
        frontend_metadata_.runtime_metadata_section_publication_image_info_symbol;
    input.image_info_section =
        frontend_metadata_.runtime_metadata_section_logical_image_info_section;
    input.descriptor_symbol_prefix =
        frontend_metadata_.runtime_metadata_section_descriptor_symbol_prefix;
    input.descriptor_linkage =
        frontend_metadata_.runtime_metadata_section_descriptor_linkage;
    input.aggregate_linkage =
        frontend_metadata_.runtime_metadata_section_aggregate_linkage;
    input.metadata_visibility =
        frontend_metadata_.runtime_metadata_section_visibility;
    input.retention_root =
        frontend_metadata_.runtime_metadata_section_retention_root;
    input.total_retained_global_count =
        frontend_metadata_
            .runtime_metadata_section_publication_total_retained_global_count;
    input.families = {{
        {kObjc3RuntimeMetadataLayoutPolicyClassFamily,
         frontend_metadata_
             .runtime_metadata_section_logical_class_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_publication_class_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_publication_class_descriptor_count},
        {kObjc3RuntimeMetadataLayoutPolicyProtocolFamily,
         frontend_metadata_
             .runtime_metadata_section_logical_protocol_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_publication_protocol_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_publication_protocol_descriptor_count},
        {kObjc3RuntimeMetadataLayoutPolicyCategoryFamily,
         frontend_metadata_
             .runtime_metadata_section_logical_category_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_publication_category_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_publication_category_descriptor_count},
        {kObjc3RuntimeMetadataLayoutPolicyPropertyFamily,
         frontend_metadata_
             .runtime_metadata_section_logical_property_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_publication_property_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_publication_property_descriptor_count},
        {kObjc3RuntimeMetadataLayoutPolicyIvarFamily,
         frontend_metadata_.runtime_metadata_section_logical_ivar_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_publication_ivar_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_publication_ivar_descriptor_count},
    }};
    return ::TryBuildObjc3RuntimeMetadataLayoutPolicy(input, policy, error);
  }

  void EmitRuntimeMetadataSectionScaffold(std::ostringstream &out) const {
    if (!ShouldEmitRuntimeMetadataSectionScaffold()) {
      return;
    }

    Objc3RuntimeMetadataLayoutPolicy layout_policy;
    std::string layout_policy_error;
    if (!TryBuildRuntimeMetadataLayoutPolicy(layout_policy, layout_policy_error) ||
        !IsReadyObjc3RuntimeMetadataLayoutPolicy(layout_policy)) {
      return;
    }

    // source-to-section matrix anchor: emitted runtime metadata still
    // materializes only image-info plus class/protocol/category/property/ivar
    // descriptor sections. Interface/implementation/metaclass/method nodes
    // remain explicit no-standalone-emission matrix rows until later work
    // payload work lands.
    // layout/visibility policy anchor: image-info emits first, then
    // class/protocol/category/property/ivar families. Within each family,
    // descriptor ordinals ascend before the family aggregate. Metadata globals
    // stay local-linkage only (private descriptors, internal image-info and
    // aggregates), use no COMDAT, omit explicit hidden visibility spelling,
    // and remain retained through @llvm.used in emission order.
    // normalized layout policy anchor: semantic finalization now
    // happens before emission. This function consumes one normalized lowering
    // policy packet and materializes exactly that plan, rather than
    // re-hardcoding family order or relocation behavior locally.
    // object-format policy expansion anchor: the lowering packet now
    // also decides the emitted section spellings for the host object format.
    // The emitter may not reinterpret that COFF/ELF/Mach-O mapping locally.
    out << "; runtime_metadata_layout_policy = "
        << Objc3RuntimeMetadataLayoutPolicyReplayKey(layout_policy) << "\n";
    out << "; runtime_metadata_section_emission_boundary = "
        << Objc3RuntimeMetadataSectionEmissionBoundarySummary() << "\n";
    // freezes this emitted boundary against the live ctor-root and
    // llvm.global_ctors path rather than the earlier deferred placeholder
    // model, so later multi-image lowering must preserve these names/shapes.
    out << "; runtime_bootstrap_lowering_boundary = "
        << Objc3RuntimeBootstrapLoweringBoundarySummary() << "\n";
    if (frontend_metadata_.versioned_conformance_report_lowering_ready &&
        !frontend_metadata_.versioned_conformance_report_lowering_replay_key
             .empty()) {
      // versioned conformance-report lowering anchor: the emitted
      // IR now advertises the lowered machine-readable capability report
      // boundary that mirrors the manifest sidecar publication.
      out << "; versioned_conformance_report_lowering = "
          << Objc3VersionedConformanceReportLoweringContractSummary()
          << ";replay_key="
          << frontend_metadata_.versioned_conformance_report_lowering_replay_key
          << "\n";
      // capability-reporting anchor: the emitted IR advertises the
      // truthful runtime/public capability payload boundary carried by the
      // conformance sidecar so later publication paths never reconstruct it.
      out << "; runtime_capability_reporting = "
          << Objc3RuntimeCapabilityReportingContractSummary()
          << ";replay_key="
          << frontend_metadata_.versioned_conformance_report_lowering_replay_key
          << "\n";
      out << "; tooling_machine_readable_conformance_report_contract = "
          << Objc3ToolingMachineReadableConformanceReportContractLoweringSummary()
          << ";replay_key="
          << frontend_metadata_.versioned_conformance_report_lowering_replay_key
          << "\n";
      out << "; tooling_feature_aware_conformance_report_emission = "
          << Objc3ToolingFeatureAwareConformanceReportEmissionLoweringSummary()
          << ";replay_key="
          << frontend_metadata_.versioned_conformance_report_lowering_replay_key
          << "\n";
      out << "; tooling_corpus_sharding_release_evidence_packaging = "
          << Objc3ToolingCorpusShardingReleaseEvidencePackagingLoweringSummary()
          << ";replay_key="
          << frontend_metadata_.versioned_conformance_report_lowering_replay_key
          << "\n";
    }
    if (ShouldEmitRuntimeBootstrapLowering()) {
      out << "; runtime_bootstrap_ctor_init_emission = "
          << "contract=objc3c.runtime.constructor.init.stub.emission.v1"
          << ";constructor_root_symbol="
          << frontend_metadata_.runtime_bootstrap_lowering_constructor_root_symbol
          << ";constructor_init_stub_symbol=" << RuntimeBootstrapInitStubSymbol()
          << ";registration_table_symbol="
          << RuntimeBootstrapRegistrationTableSymbol()
          << ";image_descriptor_symbol="
          << RuntimeBootstrapImageDescriptorSymbol()
          << ";registration_entrypoint_symbol="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_entrypoint_symbol
          << ";global_ctor_list_model="
          << frontend_metadata_.runtime_bootstrap_lowering_global_ctor_list_model
          << ";happy_path=register-before-user-main\n";
      out << "; runtime_registration_table_image_local_initialization = "
          << "contract=objc3c.runtime.registration.table.image.local.initialization.v1"
          << ";registration_table_symbol="
          << RuntimeBootstrapRegistrationTableSymbol()
          << ";registration_table_layout_model="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_table_layout_model
          << ";registration_table_abi_version="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_table_abi_version
          << ";registration_table_pointer_field_count="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_table_pointer_field_count
          << ";class_section_root_symbol=__objc3_sec_class_descriptors"
          << ";protocol_section_root_symbol=__objc3_sec_protocol_descriptors"
          << ";category_section_root_symbol=__objc3_sec_category_descriptors"
          << ";property_section_root_symbol=__objc3_sec_property_descriptors"
          << ";ivar_section_root_symbol=__objc3_sec_ivar_descriptors"
          << ";selector_pool_symbol="
          << (selector_pool_globals_.empty() ? "null"
                                             : "@__objc3_sec_selector_pool")
          << ";string_pool_symbol="
          << (runtime_string_pool_globals_.empty() ? "null"
                                                   : "@__objc3_sec_string_pool")
          << ";image_local_init_state_symbol="
          << RuntimeBootstrapImageLocalInitStateSymbol()
          << ";image_local_initialization_model="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_image_local_initialization_model
          << ";happy_path=guarded-once-before-runtime-registration\n";
      out << "; runtime_bootstrap_registrar_image_walk = "
          << "contract=" << kObjc3RuntimeBootstrapRegistrarContractId
          << ";stage_registration_table_symbol="
          << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
          << ";image_walk_snapshot_symbol="
          << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol
          << ";image_walk_model="
          << kObjc3RuntimeBootstrapImageWalkModel
          << ";selector_pool_interning_model="
          << kObjc3RuntimeBootstrapSelectorPoolInterningModel
          << ";realization_staging_model="
          << kObjc3RuntimeBootstrapRealizationStagingModel << "\n";
    }
    if (ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()) {
      // registration-descriptor/image-root lowering anchor: the IR
      // boundary now advertises the concrete identifier-driven globals and
      // sections that the native bootstrap path materializes.
      out << "; runtime_registration_descriptor_image_root_lowering = "
          << Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary()
          << ";registration_descriptor_identifier="
          << frontend_metadata_
                 .runtime_bootstrap_registration_descriptor_identifier
          << ";image_root_identifier="
          << frontend_metadata_.runtime_bootstrap_image_root_identifier
          << ";registration_descriptor_symbol="
          << RuntimeBootstrapRegistrationDescriptorSymbol()
          << ";image_root_symbol=" << RuntimeBootstrapImageRootSymbol()
          << "\n";
      if (frontend_metadata_.runtime_metadata_archive_static_link_discovery_ready) {
        // archive/static-link bootstrap replay corpus anchor: the
        // IR boundary now advertises the retained archive replay proof surface
        // that composes the D003 merge model with the live bootstrap replay
        // runtime over emitted C002 registration-descriptor/image-root globals.
        out << "; runtime_bootstrap_archive_static_link_replay_corpus = "
            << Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary()
            << ";translation_unit_identity_key="
            << frontend_metadata_
                   .runtime_metadata_archive_static_link_translation_unit_identity_key
            << ";registration_descriptor_identifier="
            << frontend_metadata_
                   .runtime_bootstrap_registration_descriptor_identifier
            << ";image_root_identifier="
            << frontend_metadata_.runtime_bootstrap_image_root_identifier
            << ";replay_registered_images_symbol="
            << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
            << ";reset_replay_state_snapshot_symbol="
            << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol << "\n";
      }
    }
    const bool emit_class_metaclass_bundle_payloads =
        frontend_metadata_.runtime_metadata_class_metaclass_emission_ready &&
        frontend_metadata_.runtime_metadata_class_metaclass_emission_fail_closed &&
        !frontend_metadata_
             .runtime_metadata_class_metaclass_emission_contract_id.empty() &&
        frontend_metadata_
                .runtime_metadata_class_metaclass_bundles_lexicographic.size() ==
            layout_policy.families[0].descriptor_count;
    const bool emit_protocol_category_bundle_payloads =
        frontend_metadata_.runtime_metadata_protocol_category_emission_ready &&
        frontend_metadata_
            .runtime_metadata_protocol_category_emission_fail_closed &&
        !frontend_metadata_
             .runtime_metadata_protocol_category_emission_contract_id.empty() &&
        frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic.size() ==
            layout_policy.families[1].descriptor_count &&
        frontend_metadata_.runtime_metadata_category_bundles_lexicographic.size() ==
            layout_policy.families[2].descriptor_count;
    const bool emit_member_table_payloads =
        frontend_metadata_.runtime_metadata_member_table_emission_ready &&
        frontend_metadata_.runtime_metadata_member_table_emission_fail_closed &&
        !frontend_metadata_
             .runtime_metadata_member_table_emission_contract_id.empty() &&
        frontend_metadata_.runtime_metadata_property_bundles_lexicographic.size() ==
            layout_policy.families[3].descriptor_count &&
        frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic.size() ==
            layout_policy.families[4].descriptor_count;
    if (emit_class_metaclass_bundle_payloads) {
      std::size_t total_instance_method_refs = 0;
      std::size_t total_class_method_refs = 0;
      for (const auto &bundle :
           frontend_metadata_
               .runtime_metadata_class_metaclass_bundles_lexicographic) {
        total_instance_method_refs += bundle.instance_method_count;
        total_class_method_refs += bundle.class_method_count;
      }
      out << "; runtime_metadata_class_metaclass_emission = "
          << Objc3RuntimeMetadataClassMetaclassEmissionSummary()
          << ";bundle_count="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_bundles_lexicographic.size()
          << ";instance_method_refs=" << total_instance_method_refs
          << ";class_method_refs=" << total_class_method_refs
          << ";typed_handoff_replay="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_typed_handoff_replay_key
          << "\n";
    }
    if (emit_protocol_category_bundle_payloads) {
      std::size_t total_inherited_protocol_refs = 0;
      for (const auto &bundle :
           frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic) {
        total_inherited_protocol_refs +=
            bundle.inherited_protocol_owner_identities_lexicographic.size();
      }
      std::size_t total_adopted_protocol_refs = 0;
      std::size_t total_category_attachment_refs = 0;
      for (const auto &bundle :
           frontend_metadata_.runtime_metadata_category_bundles_lexicographic) {
        total_adopted_protocol_refs +=
            bundle.adopted_protocol_owner_identities_lexicographic.size();
        total_category_attachment_refs += 3u;
      }
      out << "; runtime_metadata_protocol_category_emission = "
          << Objc3RuntimeMetadataProtocolCategoryEmissionSummary()
          << ";protocol_bundle_count="
          << frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic.size()
          << ";inherited_protocol_refs=" << total_inherited_protocol_refs
          << ";category_bundle_count="
          << frontend_metadata_.runtime_metadata_category_bundles_lexicographic.size()
          << ";adopted_protocol_refs=" << total_adopted_protocol_refs
          << ";attachment_refs=" << total_category_attachment_refs
          << ";typed_handoff_replay="
          << frontend_metadata_
                 .runtime_metadata_protocol_category_typed_handoff_replay_key
          << "\n";
    }
    if (emit_member_table_payloads) {
      std::size_t total_method_entries = 0;
      for (const auto &bundle :

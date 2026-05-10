  void CountDraftSyntaxExpr(const Expr *expr,
                            Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (expr == nullptr) {
      return;
    }
    if (expr->kind == Expr::Kind::BlockLiteral) {
      ++summary.block_literal_sites;
      if (expr->block_has_explicit_capture_list) {
        ++summary.block_explicit_capture_list_sites;
      }
      summary.block_explicit_capture_byref_sites +=
          expr->block_explicit_capture_byref_count;
      summary.block_byref_capture_sites += expr->block_byref_capture_count;
      if (expr->block_escape_shape_promotes_to_heap_candidate) {
        ++summary.block_heap_escape_candidate_sites;
      }
      CountDraftSyntaxStatements(expr->block_body, summary);
    }
    if (expr->try_expression_enabled) {
      ++summary.try_expression_sites;
    }
    if (expr->throw_statement_enabled) {
      ++summary.throw_statement_sites;
    }
    if (expr->await_expression_enabled) {
      ++summary.await_expression_sites;
    }
    CountDraftSyntaxExpr(expr->receiver.get(), summary);
    CountDraftSyntaxExpr(expr->left.get(), summary);
    CountDraftSyntaxExpr(expr->right.get(), summary);
    CountDraftSyntaxExpr(expr->third.get(), summary);
    for (const auto &arg : expr->args) {
      CountDraftSyntaxExpr(arg.get(), summary);
    }
  }

  void CountDraftSyntaxForClause(const ForClause &clause,
                                 Objc3DraftSyntaxSurfaceSummary &summary) const {
    CountDraftSyntaxExpr(clause.value.get(), summary);
  }

  void CountDraftSyntaxBlock(const BlockStmt *block,
                             Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (block == nullptr) {
      return;
    }
    if (block->is_do_catch_scope) {
      ++summary.do_catch_sites;
    }
    CountDraftSyntaxStatements(block->body, summary);
    for (const auto &clause : block->catch_clauses) {
      ++summary.error_catch_clause_sites;
      if (clause.has_binding) {
        ++summary.error_catch_binding_sites;
      }
      if (clause.catch_all) {
        ++summary.error_catch_all_sites;
      }
      CountDraftSyntaxStatements(clause.body, summary);
    }
  }

  void CountDraftSyntaxStmt(const Stmt *stmt,
                            Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (stmt == nullptr) {
      return;
    }
    if (stmt->kind == Stmt::Kind::Defer) {
      ++summary.error_nested_cleanup_marker_sites;
    }
    CountDraftSyntaxBlock(stmt->block_stmt.get(), summary);
    if (stmt->let_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->let_stmt->value.get(), summary);
    }
    if (stmt->assign_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->assign_stmt->value.get(), summary);
    }
    if (stmt->return_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->return_stmt->value.get(), summary);
    }
    if (stmt->expr_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->expr_stmt->value.get(), summary);
    }
    if (stmt->if_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->if_stmt->condition.get(), summary);
      CountDraftSyntaxStatements(stmt->if_stmt->then_body, summary);
      CountDraftSyntaxStatements(stmt->if_stmt->else_body, summary);
      for (const auto &condition : stmt->if_stmt->guard_condition_exprs) {
        CountDraftSyntaxExpr(condition.get(), summary);
      }
    }
    if (stmt->do_while_stmt != nullptr) {
      CountDraftSyntaxStatements(stmt->do_while_stmt->body, summary);
      CountDraftSyntaxExpr(stmt->do_while_stmt->condition.get(), summary);
    }
    if (stmt->for_stmt != nullptr) {
      CountDraftSyntaxForClause(stmt->for_stmt->init, summary);
      CountDraftSyntaxExpr(stmt->for_stmt->condition.get(), summary);
      CountDraftSyntaxForClause(stmt->for_stmt->step, summary);
      CountDraftSyntaxStatements(stmt->for_stmt->body, summary);
    }
    if (stmt->switch_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->switch_stmt->condition.get(), summary);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        CountDraftSyntaxStatements(case_stmt.body, summary);
      }
    }
    if (stmt->while_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->while_stmt->condition.get(), summary);
      CountDraftSyntaxStatements(stmt->while_stmt->body, summary);
    }
  }

  void CountDraftSyntaxStatements(
      const std::vector<std::unique_ptr<Stmt>> &statements,
      Objc3DraftSyntaxSurfaceSummary &summary) const {
    for (const auto &stmt : statements) {
      CountDraftSyntaxStmt(stmt.get(), summary);
    }
  }

  template <typename TCallableDecl>
  void CountDraftSyntaxCallable(const TCallableDecl &decl,
                                Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (decl.throws_declared) {
      ++summary.throws_callable_sites;
    }
    summary.error_bridge_payload_sites += decl.objc_nserror_attribute_sites;
    summary.error_bridge_payload_sites += decl.status_code_success_clause_sites;
    summary.error_bridge_payload_sites += decl.status_code_error_type_clause_sites;
    summary.error_bridge_payload_sites += decl.status_code_mapping_clause_sites;
    summary.error_foreign_boundary_annotation_sites +=
        decl.objc_nserror_attribute_sites + decl.objc_status_code_attribute_sites;
    summary.error_nested_cleanup_marker_sites +=
        decl.cleanup_action_sites + decl.cleanup_scope_sites +
        decl.cleanup_resume_sites;
    if (decl.async_declared) {
      ++summary.async_callable_sites;
    }
    if (decl.objc_nonisolated_declared) {
      ++summary.actor_nonisolated_callable_sites;
    }
    summary.actor_isolation_marker_sites += decl.actor_isolation_decl_sites;
    if (decl.objc_nonisolated_declared) {
      ++summary.actor_isolation_marker_sites;
    }
    summary.actor_sendable_annotation_sites += decl.sendable_annotation_sites;
    summary.task_runtime_construct_sites += decl.runtime_hook_sites;
    summary.task_cancellation_check_sites += decl.cancellation_check_sites;
    summary.task_suspension_point_sites += decl.suspension_point_sites;
    if (decl.objc_macro_declared) {
      ++summary.macro_attribute_sites;
    }
    if (decl.objc_macro_package_declared) {
      ++summary.macro_package_sites;
    }
    if (decl.objc_macro_provenance_declared) {
      ++summary.macro_provenance_sites;
    }
    if (decl.objc_macro_cache_key_declared) {
      ++summary.macro_cache_key_sites;
    }
    if (decl.objc_macro_sandbox_declared) {
      ++summary.macro_sandbox_policy_sites;
    }
    if (decl.objc_foreign_declared || decl.objc_import_module_declared ||
        decl.objc_swift_name_declared || decl.objc_swift_private_declared ||
        decl.objc_cxx_name_declared || decl.objc_header_name_declared ||
        decl.objc_export_header_declared || decl.objc_abi_align_declared ||
        decl.objc_foreign_type_declared || decl.objc_mixed_image_declared ||
        decl.objc_package_entry_declared ||
        decl.objc_nserror_declared || decl.objc_status_code_declared) {
      ++summary.interop_attribute_sites;
    }
    if (decl.objc_export_header_declared) {
      ++summary.interop_header_export_sites;
    }
    if (decl.objc_abi_align_declared) {
      ++summary.interop_abi_alignment_sites;
    }
    if (decl.objc_foreign_type_declared) {
      ++summary.interop_foreign_type_sites;
    }
    if (decl.objc_mixed_image_declared) {
      ++summary.interop_mixed_image_sites;
    }
    if (decl.objc_package_entry_declared) {
      ++summary.interop_package_entry_sites;
    }
    if (decl.objc_import_module_declared) {
      ++summary.interop_import_module_sites;
    }
    if (decl.objc_swift_name_declared) {
      ++summary.interop_swift_annotation_sites;
    }
    if (decl.objc_swift_private_declared) {
      ++summary.interop_swift_annotation_sites;
    }
    if (decl.objc_cxx_name_declared) {
      ++summary.interop_cxx_annotation_sites;
    }
    if (decl.objc_header_name_declared) {
      ++summary.interop_header_import_sites;
    }
    summary.interop_error_bridge_sites +=
        decl.objc_nserror_attribute_sites + decl.objc_status_code_attribute_sites;
    CountDraftSyntaxStatements(decl.body, summary);
  }

  void CountDraftSyntaxProperties(
      const std::vector<Objc3PropertyDecl> &properties,
      Objc3DraftSyntaxSurfaceSummary &summary) const {
    for (const auto &property : properties) {
      summary.property_attribute_sites += property.attributes.size();
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
      if (property.has_getter) {
        ++summary.property_accessor_selector_sites;
      }
      if (property.has_setter) {
        ++summary.property_accessor_selector_sites;
      }
      if (!property.property_synthesis_symbol.empty()) {
        ++summary.property_synthesis_metadata_sites;
      }
      if (!property.ivar_binding_symbol.empty()) {
        ++summary.property_synthesis_metadata_sites;
      }
      if (!property.property_attribute_profile.empty()) {
        ++summary.property_reflection_input_sites;
      }
      if (!property.effective_getter_selector.empty()) {
        ++summary.property_reflection_input_sites;
      }
      if (property.effective_setter_available &&
          !property.effective_setter_selector.empty()) {
        ++summary.property_reflection_input_sites;
      }
      if (property.property_behavior_declared) {
        ++summary.property_reflection_input_sites;
      }
      summary.property_ownership_nullability_sites +=
          (property.is_copy ? 1u : 0u) + (property.is_retain ? 1u : 0u) +
          (property.is_strong ? 1u : 0u) + (property.is_weak ? 1u : 0u) +
          (property.is_unowned ? 1u : 0u) +
          (property.is_unsafe_unretained ? 1u : 0u) +
          (property.is_assign ? 1u : 0u) + (property.is_nullable ? 1u : 0u) +
          (property.is_nonnull ? 1u : 0u) +
          (property.is_null_resettable ? 1u : 0u);
    }
  }

  std::string BuildObjc3DraftSyntaxSurfaceReplayKey(
      const Objc3DraftSyntaxSurfaceSummary &summary) const {
    std::ostringstream out;
    out << "draft-syntax-surfaces:v1"
        << ";blocks=" << summary.block_literal_sites
        << ";block_capture_lists=" << summary.block_explicit_capture_list_sites
        << ";block_byref_explicit=" << summary.block_explicit_capture_byref_sites
        << ";block_byref_captures=" << summary.block_byref_capture_sites
        << ";block_heap_candidates=" << summary.block_heap_escape_candidate_sites
        << ";try=" << summary.try_expression_sites
        << ";throw=" << summary.throw_statement_sites
        << ";do_catch=" << summary.do_catch_sites
        << ";error_catch_clauses=" << summary.error_catch_clause_sites
        << ";error_catch_bindings=" << summary.error_catch_binding_sites
        << ";error_catch_all=" << summary.error_catch_all_sites
        << ";error_bridge_payloads=" << summary.error_bridge_payload_sites
        << ";error_foreign_boundaries="
        << summary.error_foreign_boundary_annotation_sites
        << ";error_nested_cleanup_markers="
        << summary.error_nested_cleanup_marker_sites
        << ";throws_callables=" << summary.throws_callable_sites
        << ";async_callables=" << summary.async_callable_sites
        << ";await=" << summary.await_expression_sites
        << ";actors=" << summary.actor_interface_sites
        << ";actor_nonisolated=" << summary.actor_nonisolated_callable_sites
        << ";actor_isolation_markers=" << summary.actor_isolation_marker_sites
        << ";actor_sendable_markers=" << summary.actor_sendable_annotation_sites
        << ";task_runtime=" << summary.task_runtime_construct_sites
        << ";task_cancellation_checks=" << summary.task_cancellation_check_sites
        << ";task_suspension_points=" << summary.task_suspension_point_sites
        << ";macro_attrs=" << summary.macro_attribute_sites
        << ";macro_packages=" << summary.macro_package_sites
        << ";macro_provenance=" << summary.macro_provenance_sites
        << ";macro_cache_keys=" << summary.macro_cache_key_sites
        << ";macro_sandbox_policies=" << summary.macro_sandbox_policy_sites
        << ";property_behaviors=" << summary.property_behavior_sites
        << ";property_attrs=" << summary.property_attribute_sites
        << ";property_accessor_selectors="
        << summary.property_accessor_selector_sites
        << ";property_synthesis_metadata="
        << summary.property_synthesis_metadata_sites
        << ";property_reflection_inputs="
        << summary.property_reflection_input_sites
        << ";property_ownership_nullability="
        << summary.property_ownership_nullability_sites
        << ";interop_attrs=" << summary.interop_attribute_sites
        << ";interop_import_modules=" << summary.interop_import_module_sites
        << ";interop_swift_annotations="
        << summary.interop_swift_annotation_sites
        << ";interop_cxx_annotations=" << summary.interop_cxx_annotation_sites
        << ";interop_header_imports=" << summary.interop_header_import_sites
        << ";interop_export_headers=" << summary.interop_header_export_sites
        << ";interop_abi_align=" << summary.interop_abi_alignment_sites
        << ";interop_foreign_type=" << summary.interop_foreign_type_sites
        << ";interop_mixed_images=" << summary.interop_mixed_image_sites
        << ";interop_package_entries=" << summary.interop_package_entry_sites
        << ";interop_error_bridges=" << summary.interop_error_bridge_sites
        << ";total=" << summary.draft_syntax_surface_sites;
    return out.str();
  }

  Objc3DraftSyntaxSurfaceSummary BuildObjc3DraftSyntaxSurfaceSummary(
      const Objc3ParsedProgram &program) const {
    Objc3DraftSyntaxSurfaceSummary summary;
    const Objc3Program &ast = Objc3ParsedProgramAst(program);
    for (const auto &global : ast.globals) {
      CountDraftSyntaxExpr(global.value.get(), summary);
    }
    for (const auto &protocol_decl : ast.protocols) {
      CountDraftSyntaxProperties(protocol_decl.properties, summary);
      for (const auto &method : protocol_decl.methods) {
        CountDraftSyntaxCallable(method, summary);
      }
    }
    for (const auto &interface_decl : ast.interfaces) {
      if (interface_decl.is_actor) {
        ++summary.actor_interface_sites;
      }
      CountDraftSyntaxProperties(interface_decl.properties, summary);
      for (const auto &method : interface_decl.methods) {
        CountDraftSyntaxCallable(method, summary);
      }
    }
    for (const auto &implementation_decl : ast.implementations) {
      CountDraftSyntaxProperties(implementation_decl.properties, summary);
      for (const auto &method : implementation_decl.methods) {
        CountDraftSyntaxCallable(method, summary);
      }
    }
    for (const auto &function : ast.functions) {
      CountDraftSyntaxCallable(function, summary);
    }
    summary.draft_syntax_surface_sites =
        summary.block_literal_sites + summary.block_explicit_capture_list_sites +
        summary.block_explicit_capture_byref_sites +
        summary.block_byref_capture_sites +
        summary.block_heap_escape_candidate_sites + summary.try_expression_sites +
        summary.throw_statement_sites + summary.do_catch_sites +
        summary.error_catch_clause_sites + summary.error_catch_binding_sites +
        summary.error_catch_all_sites + summary.error_bridge_payload_sites +
        summary.error_foreign_boundary_annotation_sites +
        summary.error_nested_cleanup_marker_sites +
        summary.throws_callable_sites + summary.async_callable_sites +
        summary.await_expression_sites + summary.actor_interface_sites +
        summary.actor_nonisolated_callable_sites +
        summary.actor_isolation_marker_sites +
        summary.actor_sendable_annotation_sites +
        summary.task_runtime_construct_sites +
        summary.task_cancellation_check_sites +
        summary.task_suspension_point_sites +
        summary.macro_attribute_sites + summary.macro_package_sites +
        summary.macro_provenance_sites + summary.macro_cache_key_sites +
        summary.macro_sandbox_policy_sites + summary.property_behavior_sites +
        summary.property_attribute_sites +
        summary.property_accessor_selector_sites +
        summary.property_synthesis_metadata_sites +
        summary.property_reflection_input_sites +
        summary.property_ownership_nullability_sites +
        summary.interop_attribute_sites + summary.interop_import_module_sites +
        summary.interop_swift_annotation_sites +
        summary.interop_cxx_annotation_sites +
        summary.interop_header_import_sites +
        summary.interop_header_export_sites +
        summary.interop_abi_alignment_sites + summary.interop_foreign_type_sites +
        summary.interop_mixed_image_sites + summary.interop_package_entry_sites +
        summary.interop_error_bridge_sites;
    summary.replay_key = BuildObjc3DraftSyntaxSurfaceReplayKey(summary);
    summary.normalized =
        !summary.replay_key.empty() &&
        summary.draft_syntax_surface_sites >= summary.block_literal_sites &&
        summary.block_explicit_capture_byref_sites <=
            summary.block_byref_capture_sites &&
        summary.actor_nonisolated_callable_sites <=
            summary.actor_isolation_marker_sites + summary.actor_interface_sites &&
        summary.task_cancellation_check_sites <=
            summary.task_runtime_construct_sites + summary.task_suspension_point_sites &&
        summary.draft_syntax_surface_sites >= summary.interop_attribute_sites;
    return summary;
  }

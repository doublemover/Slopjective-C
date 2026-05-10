    method.unsafe_keyword_sites = profile.unsafe_keyword_sites;
    method.pointer_arithmetic_sites = profile.pointer_arithmetic_sites;
    method.raw_pointer_type_sites = profile.raw_pointer_type_sites;
    method.unsafe_operation_sites = profile.unsafe_operation_sites;
    method.unsafe_pointer_extension_normalized_sites = profile.normalized_sites;
    method.unsafe_pointer_extension_gate_blocked_sites = profile.gate_blocked_sites;
    method.unsafe_pointer_extension_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_unsafe_pointer_extension_handoff =
        profile.deterministic_unsafe_pointer_extension_handoff;
    method.unsafe_pointer_extension_profile = BuildUnsafePointerExtensionProfile(
        method.unsafe_pointer_extension_sites,
        method.unsafe_keyword_sites,
        method.pointer_arithmetic_sites,
        method.raw_pointer_type_sites,
        method.unsafe_operation_sites,
        method.unsafe_pointer_extension_normalized_sites,
        method.unsafe_pointer_extension_gate_blocked_sites,
        method.unsafe_pointer_extension_contract_violation_sites,
        method.deterministic_unsafe_pointer_extension_handoff);
    method.unsafe_pointer_extension_profile_is_normalized =
        IsUnsafePointerExtensionProfileNormalized(
            method.unsafe_pointer_extension_sites,
            method.unsafe_keyword_sites,
            method.pointer_arithmetic_sites,
            method.raw_pointer_type_sites,
            method.unsafe_operation_sites,
            method.unsafe_pointer_extension_normalized_sites,
            method.unsafe_pointer_extension_gate_blocked_sites,
            method.unsafe_pointer_extension_contract_violation_sites);
  }

  void FinalizeInlineAsmIntrinsicGovernanceProfile(FunctionDecl &fn) {
    FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(fn);
  }

  void FinalizeInlineAsmIntrinsicGovernanceProfile(Objc3MethodDecl &method) {
    FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(method);
  }

  void AssignObjcMethodLookupOverrideConflictSymbols(Objc3MethodDecl &method,
                                                     const std::string &lookup_owner_symbol,
                                                     const std::string &override_owner_symbol) {
    method.method_lookup_symbol = lookup_owner_symbol + "::" + BuildObjcMethodLookupSymbol(method);
    method.override_lookup_symbol = override_owner_symbol + "::" + BuildObjcMethodOverrideLookupSymbol(method);
    method.conflict_lookup_symbol = BuildObjcMethodConflictLookupSymbol(method);
  }

  void FinalizeObjcMethodLookupOverrideConflictPackets(
      const std::vector<Objc3MethodDecl> &methods,
      std::vector<std::string> &method_lookup_symbols_lexicographic,
      std::vector<std::string> &override_lookup_symbols_lexicographic,
      std::vector<std::string> &conflict_lookup_symbols_lexicographic) {
    method_lookup_symbols_lexicographic = BuildObjcMethodLookupSymbolsLexicographic(methods);
    override_lookup_symbols_lexicographic = BuildObjcMethodOverrideLookupSymbolsLexicographic(methods);
    conflict_lookup_symbols_lexicographic = BuildObjcMethodConflictLookupSymbolsLexicographic(methods);
  }

  void AssignObjcPropertySynthesisIvarBindingSymbols(Objc3PropertyDecl &property,
                                                     const std::string &synthesis_owner_symbol) {
    property.property_synthesis_symbol = synthesis_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property);
    property.ivar_binding_symbol = synthesis_owner_symbol + "::" + BuildObjcIvarBindingSymbol(property);
  }

  struct Objc3ExecutablePropertyLayoutShape {
    std::size_t size_bytes = 0;
    std::size_t alignment_bytes = 1;
  };

  std::string TrimAsciiWhitespaceCopy(const std::string &text) {
    std::size_t start = 0;
    while (start < text.size() &&
           std::isspace(static_cast<unsigned char>(text[start])) != 0) {
      ++start;
    }
    std::size_t end = text.size();
    while (end > start &&
           std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
      --end;
    }
    return text.substr(start, end - start);
  }

  std::string BuildDefaultPropertySetterSelector(const std::string &property_name) {
    if (property_name.empty()) {
      return {};
    }
    std::string selector = "set";
    selector.push_back(static_cast<char>(
        std::toupper(static_cast<unsigned char>(property_name.front()))));
    selector.append(property_name.substr(1));
    selector.push_back(':');
    return selector;
  }

  Objc3ExecutablePropertyLayoutShape ComputeExecutablePropertyLayoutShape(
      const Objc3PropertyDecl &property) {
    const auto scalar_layout = [&](std::size_t size_bytes,
                                   std::size_t alignment_bytes) {
      return Objc3ExecutablePropertyLayoutShape{size_bytes, alignment_bytes};
    };
    if (property.vector_spelling) {
      const std::size_t lane_width =
          property.type == ValueType::Bool ? 1u : 4u;
      const std::size_t size_bytes =
          lane_width * static_cast<std::size_t>(property.vector_lane_count);
      return scalar_layout(size_bytes, std::min<std::size_t>(size_bytes, 16u));
    }
    switch (property.type) {
    case ValueType::Bool:
      return scalar_layout(1u, 1u);
    case ValueType::I32:
      return scalar_layout(4u, 4u);
    case ValueType::Void:
      return scalar_layout(0u, 1u);
    case ValueType::ObjCId:
    case ValueType::ObjCClass:
    case ValueType::ObjCSel:
    case ValueType::ObjCProtocol:
    case ValueType::ObjCInstancetype:
    case ValueType::ObjCObjectPtr:
    case ValueType::Function:
    case ValueType::Unknown:
    default:
      return scalar_layout(8u, 8u);
    }
  }

  std::size_t AlignObjcIvarLayoutOffset(std::size_t offset,
                                        std::size_t alignment) const {
    const std::size_t effective_alignment =
        std::max<std::size_t>(alignment, 1u);
    const std::size_t remainder = offset % effective_alignment;
    return remainder == 0u ? offset
                           : offset + (effective_alignment - remainder);
  }

  bool IsValidObjcIvarLayoutAlignment(std::size_t alignment) const {
    return alignment != 0u && (alignment & (alignment - 1u)) == 0u;
  }

  std::string BuildObjcIvarLayoutReplayKey(
      const Objc3PropertyDecl &property, const Objc3InterfaceDecl &owner) const {
    std::ostringstream out;
    out << "owner=" << owner.semantic_link_symbol
        << ";property=" << property.name
        << ";slot=" << property.executable_ivar_layout_slot_index
        << ";offset=" << property.executable_ivar_layout_offset_bytes
        << ";size=" << property.executable_ivar_layout_size_bytes
        << ";alignment=" << property.executable_ivar_layout_alignment_bytes
        << ";padding=" << property.executable_ivar_layout_padding_bytes
        << ";inherited_slots="
        << property.executable_ivar_layout_inherited_slot_count
        << ";inherited_size="
        << property.executable_ivar_layout_inherited_size_bytes
        << ";owner_size=" << property.executable_ivar_layout_owner_size_bytes;
    return out.str();
  }

  void ClearObjcPropertyIvarLayoutClosure(Objc3PropertyDecl &property) {
    property.executable_ivar_layout_symbol.clear();
    property.executable_ivar_layout_slot_index = 0;
    property.executable_ivar_layout_size_bytes = 0;
    property.executable_ivar_layout_alignment_bytes = 0;
    property.executable_ivar_layout_offset_bytes = 0;
    property.executable_ivar_layout_padding_bytes = 0;
    property.executable_ivar_layout_inherited_slot_count = 0;
    property.executable_ivar_layout_inherited_size_bytes = 0;
    property.executable_ivar_layout_owner_size_bytes = 0;
    property.executable_ivar_init_order_index = 0;
    property.executable_ivar_destroy_order_index = 0;
    property.executable_ivar_layout_valid = false;
    property.executable_ivar_layout_replay_key.clear();
  }

  Objc3InterfaceDecl *FindObjcInterfaceLayoutOwner(
      Objc3ParsedProgram &program, const std::string &name) const {
    if (name.empty()) {
      return nullptr;
    }
    for (auto &decl : MutableObjc3ParsedProgramAst(program).interfaces) {
      if (!decl.has_category && decl.name == name) {
        return &decl;
      }
    }
    return nullptr;
  }

  Objc3PropertyDecl *FindObjcPropertyDeclByName(
      std::vector<Objc3PropertyDecl> &properties,
      const std::string &name) const {
    for (auto &property : properties) {
      if (property.name == name) {
        return &property;
      }
    }
    return nullptr;
  }

  void CopyObjcIvarLayoutClosure(Objc3PropertyDecl &target,
                                 const Objc3PropertyDecl &source) {
    target.executable_ivar_layout_symbol =
        source.executable_ivar_layout_symbol;
    target.executable_ivar_layout_slot_index =
        source.executable_ivar_layout_slot_index;
    target.executable_ivar_layout_size_bytes =
        source.executable_ivar_layout_size_bytes;
    target.executable_ivar_layout_alignment_bytes =
        source.executable_ivar_layout_alignment_bytes;
    target.executable_ivar_layout_offset_bytes =
        source.executable_ivar_layout_offset_bytes;
    target.executable_ivar_layout_padding_bytes =
        source.executable_ivar_layout_padding_bytes;
    target.executable_ivar_layout_inherited_slot_count =
        source.executable_ivar_layout_inherited_slot_count;
    target.executable_ivar_layout_inherited_size_bytes =
        source.executable_ivar_layout_inherited_size_bytes;
    target.executable_ivar_layout_owner_size_bytes =
        source.executable_ivar_layout_owner_size_bytes;
    target.executable_ivar_init_order_index =
        source.executable_ivar_init_order_index;
    target.executable_ivar_destroy_order_index =
        source.executable_ivar_destroy_order_index;
    target.executable_ivar_layout_valid =
        source.executable_ivar_layout_valid;
    target.executable_ivar_layout_replay_key =
        source.executable_ivar_layout_replay_key;
  }

  bool FinalizeObjcInterfacePropertyIvarLayoutClosure(
      Objc3ParsedProgram &program, Objc3InterfaceDecl &decl,
      std::unordered_set<std::string> &visiting,
      std::unordered_set<std::string> &completed) {
    if (decl.has_category) {
      for (auto &property : decl.properties) {
        ClearObjcPropertyIvarLayoutClosure(property);
      }
      FinalizeObjcPropertySynthesisIvarBindingPackets(
          decl.properties, decl.property_synthesis_symbols_lexicographic,
          decl.ivar_binding_symbols_lexicographic);
      return true;
    }

    if (completed.count(decl.name) != 0u) {
      return true;
    }
    if (visiting.count(decl.name) != 0u) {
      diagnostics_.push_back(MakeDiag(
          decl.line, decl.column, "O3P150",
          "cyclic Objective-C interface inheritance cannot produce a stable ivar layout"));
      for (auto &property : decl.properties) {
        ClearObjcPropertyIvarLayoutClosure(property);
      }
      completed.insert(decl.name);
      return false;
    }

    visiting.insert(decl.name);
    std::size_t inherited_slot_count = 0u;
    std::size_t inherited_size_bytes = 0u;
    if (!decl.super_name.empty()) {
      Objc3InterfaceDecl *super_decl =
          FindObjcInterfaceLayoutOwner(program, decl.super_name);
      if (super_decl != nullptr) {
        FinalizeObjcInterfacePropertyIvarLayoutClosure(
            program, *super_decl, visiting, completed);
        for (const auto &property : super_decl->properties) {
          if (property.executable_ivar_layout_valid) {
            inherited_slot_count = std::max(
                inherited_slot_count,
                property.executable_ivar_layout_slot_index + 1u);
            inherited_size_bytes = std::max(
                inherited_size_bytes,
                property.executable_ivar_layout_owner_size_bytes);
          }
        }
      }
    }

    std::size_t running_slot = inherited_slot_count;
    std::size_t running_offset = inherited_size_bytes;
    std::size_t max_alignment = 1u;
    bool layout_valid = true;
    for (auto &property : decl.properties) {
      if (property.executable_ivar_layout_symbol.empty()) {
        ClearObjcPropertyIvarLayoutClosure(property);
        continue;
      }
      if (property.executable_ivar_layout_size_bytes == 0u ||
          !IsValidObjcIvarLayoutAlignment(
              property.executable_ivar_layout_alignment_bytes)) {
        diagnostics_.push_back(MakeDiag(
            property.line, property.column, "O3P151",
            "invalid Objective-C property ivar layout size or alignment"));
        ClearObjcPropertyIvarLayoutClosure(property);
        layout_valid = false;
        continue;
      }

      const std::size_t aligned_offset = AlignObjcIvarLayoutOffset(
          running_offset, property.executable_ivar_layout_alignment_bytes);
      if (aligned_offset < running_offset ||
          aligned_offset >
              std::numeric_limits<std::size_t>::max() -
                  property.executable_ivar_layout_size_bytes) {
        diagnostics_.push_back(MakeDiag(
            property.line, property.column, "O3P152",
            "Objective-C property ivar layout offset overflow"));
        ClearObjcPropertyIvarLayoutClosure(property);
        layout_valid = false;
        continue;
      }

      property.executable_ivar_layout_slot_index = running_slot;
      property.executable_ivar_layout_offset_bytes = aligned_offset;
      property.executable_ivar_layout_padding_bytes =
          aligned_offset - running_offset;
      property.executable_ivar_layout_inherited_slot_count =
          inherited_slot_count;
      property.executable_ivar_layout_inherited_size_bytes =
          inherited_size_bytes;
      property.executable_ivar_init_order_index = running_slot;
      property.executable_ivar_layout_valid = true;
      running_offset =
          aligned_offset + property.executable_ivar_layout_size_bytes;
      max_alignment =
          std::max(max_alignment,
                   property.executable_ivar_layout_alignment_bytes);
      ++running_slot;
    }

    const std::size_t owner_size =
        AlignObjcIvarLayoutOffset(running_offset, max_alignment);
    for (auto &property : decl.properties) {
      if (!property.executable_ivar_layout_valid) {
        continue;
      }
      property.executable_ivar_layout_owner_size_bytes = owner_size;
      property.executable_ivar_destroy_order_index =
          running_slot > property.executable_ivar_layout_slot_index
              ? (running_slot - 1u) -
                    property.executable_ivar_layout_slot_index
              : 0u;
      property.executable_ivar_layout_replay_key =
          BuildObjcIvarLayoutReplayKey(property, decl);
    }

    FinalizeObjcPropertySynthesisIvarBindingPackets(
        decl.properties, decl.property_synthesis_symbols_lexicographic,
        decl.ivar_binding_symbols_lexicographic);
    visiting.erase(decl.name);
    completed.insert(decl.name);
    return layout_valid;
  }

  void FinalizeObjcProgramPropertyIvarLayoutClosure(
      Objc3ParsedProgram &program) {
    std::unordered_set<std::string> visiting;
    std::unordered_set<std::string> completed;
    for (auto &decl : MutableObjc3ParsedProgramAst(program).interfaces) {
      FinalizeObjcInterfacePropertyIvarLayoutClosure(program, decl, visiting,
                                                     completed);
    }

    for (auto &implementation :
         MutableObjc3ParsedProgramAst(program).implementations) {
      Objc3InterfaceDecl *interface_decl =
          FindObjcInterfaceLayoutOwner(program, implementation.name);
      if (implementation.has_category || interface_decl == nullptr) {
        for (auto &property : implementation.properties) {
          ClearObjcPropertyIvarLayoutClosure(property);
        }
      } else {
        for (auto &property : implementation.properties) {
          Objc3PropertyDecl *interface_property =
              FindObjcPropertyDeclByName(interface_decl->properties,
                                         property.name);
          if (interface_property == nullptr) {
            continue;
          }
          CopyObjcIvarLayoutClosure(property, *interface_property);
        }
      }
      FinalizeObjcPropertySynthesisIvarBindingPackets(
          implementation.properties,
          implementation.property_synthesis_symbols_lexicographic,
          implementation.ivar_binding_symbols_lexicographic);
    }
  }

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

  std::string BuildExecutablePropertyAttributeProfile(
      const Objc3PropertyDecl &property) {
    std::vector<std::string> attributes;
    attributes.reserve(property.attributes.size());
    for (const auto &attribute : property.attributes) {
      std::string rendered = attribute.name;
      if (attribute.has_value) {
        rendered += "=" + TrimAsciiWhitespaceCopy(attribute.value);
      }
      attributes.push_back(std::move(rendered));
    }
    std::sort(attributes.begin(), attributes.end());

    std::ostringstream out;
    out << "readonly=" << (property.is_readonly ? 1 : 0)
        << ";readwrite=" << (property.is_readwrite ? 1 : 0)
        << ";atomic=" << (property.is_atomic ? 1 : 0)
        << ";nonatomic=" << (property.is_nonatomic ? 1 : 0)
        << ";copy=" << (property.is_copy ? 1 : 0)
        << ";retain=" << (property.is_retain ? 1 : 0)
        << ";strong=" << (property.is_strong ? 1 : 0)
        << ";weak=" << (property.is_weak ? 1 : 0)
        << ";unowned=" << (property.is_unowned ? 1 : 0)
        << ";unsafe_unretained=" << (property.is_unsafe_unretained ? 1 : 0)
        << ";assign=" << (property.is_assign ? 1 : 0)
        << ";nullable=" << (property.is_nullable ? 1 : 0)
        << ";nonnull=" << (property.is_nonnull ? 1 : 0)
        << ";null_resettable=" << (property.is_null_resettable ? 1 : 0)
        << ";class=" << (property.is_class ? 1 : 0)
        << ";direct=" << (property.is_direct ? 1 : 0)
        << ";attributes=";
    for (std::size_t i = 0; i < attributes.size(); ++i) {
      if (i != 0u) {
        out << ",";
      }
      out << attributes[i];
    }
    return out.str();
  }

  std::string BuildExecutableAccessorOwnershipProfile(
      const Objc3PropertyDecl &property) {
    std::ostringstream out;
    out << "getter=" << property.effective_getter_selector
        << ";setter_available=" << (property.effective_setter_available ? 1 : 0)
        << ";setter=";
    if (property.effective_setter_available) {
      out << property.effective_setter_selector;
    } else {
      out << "<none>";
    }
    out << ";ownership_lifetime=" << property.ownership_lifetime_profile
        << ";runtime_hook=" << property.ownership_runtime_hook_profile;
    return out.str();
  }

  void AssignObjcExecutablePropertySourceModelFields(
      Objc3PropertyDecl &property, const std::string &storage_owner_symbol,
      std::size_t slot_index, bool supports_storage_layout) {
    property.property_attribute_profile =
        BuildExecutablePropertyAttributeProfile(property);
    property.effective_getter_selector =
        property.has_getter ? TrimAsciiWhitespaceCopy(property.getter_selector)
                            : property.name;
    property.effective_setter_available = !property.is_readonly;
    property.effective_setter_selector =
        property.effective_setter_available
            ? (property.has_setter
                   ? TrimAsciiWhitespaceCopy(property.setter_selector)
                   : BuildDefaultPropertySetterSelector(property.name))
            : std::string{};
    property.executable_synthesized_binding_kind =
        supports_storage_layout ? "implicit-ivar" : "none";
    property.executable_synthesized_binding_symbol =
        supports_storage_layout
            ? storage_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property)
            : std::string{};
    if (supports_storage_layout) {
      const Objc3ExecutablePropertyLayoutShape layout_shape =
          ComputeExecutablePropertyLayoutShape(property);
      property.executable_ivar_layout_symbol =
          storage_owner_symbol + "::objc_property_layout:" + property.name;
      property.executable_ivar_layout_slot_index = slot_index;
      property.executable_ivar_layout_size_bytes = layout_shape.size_bytes;
      property.executable_ivar_layout_alignment_bytes =
          layout_shape.alignment_bytes;
      property.executable_ivar_layout_offset_bytes = 0;
      property.executable_ivar_layout_padding_bytes = 0;
      property.executable_ivar_layout_inherited_slot_count = 0;
      property.executable_ivar_layout_inherited_size_bytes = 0;
      property.executable_ivar_layout_owner_size_bytes = 0;
      property.executable_ivar_init_order_index = slot_index;
      property.executable_ivar_destroy_order_index = 0;
      property.executable_ivar_layout_valid = false;
      property.executable_ivar_layout_replay_key.clear();
    } else {
      property.executable_ivar_layout_symbol.clear();
      property.executable_ivar_layout_slot_index = 0;
      property.executable_ivar_layout_size_bytes = 0;
      property.executable_ivar_layout_alignment_bytes = 0;
      property.executable_ivar_layout_offset_bytes = 0;
      property.executable_ivar_layout_padding_bytes = 0;
      property.executable_ivar_layout_inherited_slot_count = 0;
      property.executable_ivar_layout_inherited_size_bytes = 0;
      property.executable_ivar_layout_owner_size_bytes = 0;
      property.executable_ivar_init_order_index = 0;
      property.executable_ivar_destroy_order_index = 0;
      property.executable_ivar_layout_valid = false;
      property.executable_ivar_layout_replay_key.clear();
    }
    property.accessor_ownership_profile =
        BuildExecutableAccessorOwnershipProfile(property);
  }

  void FinalizeObjcPropertySynthesisIvarBindingPackets(
      std::vector<Objc3PropertyDecl> &properties,
      std::vector<std::string> &property_synthesis_symbols_lexicographic,
      std::vector<std::string> &ivar_binding_symbols_lexicographic) {
    const std::size_t property_count = properties.size();
    for (auto &property : properties) {
      if (property.executable_ivar_layout_valid) {
        continue;
      }
      if (!property.executable_ivar_layout_symbol.empty()) {
        property.executable_ivar_init_order_index =
            property.executable_ivar_layout_slot_index;
        property.executable_ivar_destroy_order_index =
            property_count > property.executable_ivar_layout_slot_index
                ? (property_count - 1u) -
                      property.executable_ivar_layout_slot_index
                : 0u;
      } else {
        property.executable_ivar_init_order_index = 0;
        property.executable_ivar_destroy_order_index = 0;
      }
    }
    property_synthesis_symbols_lexicographic = BuildObjcPropertySynthesisSymbolsLexicographic(properties);
    ivar_binding_symbols_lexicographic = BuildObjcIvarBindingSymbolsLexicographic(properties);
  }

  void ConsumeBracedBodyTail() {
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::LBrace)) {
        ++depth;
        continue;
      }
      if (Match(TokenKind::RBrace)) {
        --depth;
        continue;
      }
      Advance();
    }
  }

  std::string ParseObjcPropertyAttributeValueText() {
    std::string value_text;
    while (!At(TokenKind::Eof) && !At(TokenKind::Comma) && !At(TokenKind::RParen)) {
      value_text += Advance().text;
    }
    return value_text;
  }

  bool ParseObjcPropertyAttributes(std::vector<Objc3PropertyAttributeDecl> &attributes) {
    // export-legality anchor: property attribute spelling must stay
    // canonical here so later property/ivar export preconditions can trust the
    // parser-owned source shape without reparsing attributes.
    if (!Match(TokenKind::LParen)) {
      return true;
    }

    while (true) {
      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C @property attribute"));
        return false;
      }

      Objc3PropertyAttributeDecl attribute;
      attribute.name = Previous().text;
      attribute.line = Previous().line;
      attribute.column = Previous().column;
      if (Match(TokenKind::Equal)) {
        attribute.has_value = true;
        attribute.value = ParseObjcPropertyAttributeValueText();
        if (attribute.value.empty()) {
          const Token &token = Peek();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "missing Objective-C @property attribute value"));
          return false;
        }
      }
      if (attribute.name == "behavior" && !attribute.has_value) {
        diagnostics_.push_back(MakeDiag(
            attribute.line, attribute.column, "O3P355",
            "Objective-C @property behavior requires '=Name' payload"));
        return false;
      }
      attributes.push_back(std::move(attribute));

      if (Match(TokenKind::Comma)) {
        continue;
      }
      if (Match(TokenKind::RParen)) {
        return true;
      }

      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C @property attribute list"));
      return false;
    }
  }

  void ApplyObjcPropertyAttributes(Objc3PropertyDecl &property) {
    for (const auto &attribute : property.attributes) {
      if (attribute.name == "readonly") {
        property.is_readonly = true;
      } else if (attribute.name == "readwrite") {
        property.is_readwrite = true;
      } else if (attribute.name == "atomic") {
        property.is_atomic = true;
      } else if (attribute.name == "nonatomic") {
        property.is_nonatomic = true;
      } else if (attribute.name == "copy") {
        property.is_copy = true;
      } else if (attribute.name == "retain") {
        property.is_retain = true;
      } else if (attribute.name == "strong") {
        property.is_strong = true;
      } else if (attribute.name == "weak") {
        property.is_weak = true;
      } else if (attribute.name == "unowned") {
        property.is_unowned = true;
      } else if (attribute.name == "unsafe_unretained") {
        property.is_unsafe_unretained = true;
      } else if (attribute.name == "assign") {
        property.is_assign = true;
      } else if (attribute.name == "nullable") {
        property.is_nullable = true;
      } else if (attribute.name == "nonnull") {
        property.is_nonnull = true;
      } else if (attribute.name == "null_resettable") {
        property.is_null_resettable = true;
      } else if (attribute.name == "class") {
        property.is_class = true;
      } else if (attribute.name == "direct") {
        property.is_direct = true;
      } else if (attribute.name == "getter") {
        property.has_getter = true;
        property.getter_selector = attribute.value;
      } else if (attribute.name == "setter") {
        property.has_setter = true;
        property.setter_selector = attribute.value;
      } else if (attribute.name == "behavior") {
        property.property_behavior_declared = attribute.has_value;
        property.property_behavior_name = attribute.value;
      }
    }
  }

  bool ParseObjcPropertyDecl(Objc3PropertyDecl &property) {
    if (!Match(TokenKind::KwAtProperty)) {
      return false;
    }
    const Token property_marker = Previous();
    property.line = property_marker.line;
    property.column = property_marker.column;

    if (!ParseObjcPropertyAttributes(property.attributes)) {
      return false;
    }

    FuncParam property_type;
    if (!ParseParameterType(property_type)) {
      return false;
    }
    CopyPropertyTypeFromParam(property_type, property);

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C @property identifier"));
      return false;
    }
    property.name = Previous().text;

    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P104", "missing ';' after Objective-C @property declaration"));
      return false;
    }

    ApplyObjcPropertyAttributes(property);
    const Objc3WeakUnownedLifetimeProfile property_lifetime_profile =
        BuildPropertyWeakUnownedLifetimeProfile(property);
    property.ownership_is_weak_reference = property_lifetime_profile.is_weak_reference;
    property.ownership_is_unowned_reference = property_lifetime_profile.is_unowned_reference;
    property.ownership_is_unowned_safe_reference = property_lifetime_profile.is_unowned_safe_reference;
    property.ownership_lifetime_profile = property_lifetime_profile.lifetime_profile;
    property.ownership_runtime_hook_profile = property_lifetime_profile.runtime_hook_profile;
    property.has_weak_unowned_conflict =
        property.is_weak &&
        (property.is_unowned || property.is_unsafe_unretained);
    const Objc3ArcDiagnosticFixitProfile property_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(
            property.ownership_qualifier_spelling,
            false,
            true,
            property.has_weak_unowned_conflict);
    property.ownership_arc_diagnostic_candidate = property_arc_diagnostic_profile.diagnostic_candidate;
    property.ownership_arc_fixit_available = property_arc_diagnostic_profile.fixit_available;
    property.ownership_arc_diagnostic_profile = property_arc_diagnostic_profile.diagnostic_profile;
    property.ownership_arc_fixit_hint = property_arc_diagnostic_profile.fixit_hint;
    return true;
  }

  void SynchronizeObjcContainer() {
    while (!At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtEnd) || At(TokenKind::Minus) || At(TokenKind::Plus) || At(TokenKind::KwAtProperty) ||
          At(TokenKind::KwAtRequired) || At(TokenKind::KwAtOptional)) {
        return;
      }
      if (At(TokenKind::KwAtInterface) || At(TokenKind::KwAtImplementation) || At(TokenKind::KwAtProtocol) ||
          At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
        return;
      }
      if (Match(TokenKind::Semicolon)) {
        return;
      }
      if (Match(TokenKind::LBrace)) {
        ConsumeBracedBodyTail();
        continue;
      }
      Advance();
    }
  }

  bool ParseObjcProtocolCompositionClause(std::vector<std::string> &protocols) {
    if (!Match(TokenKind::Less)) {
      return true;
    }

    while (true) {
      const Token &protocol_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(protocol_token.line, protocol_token.column, "O3P101",
                                        "invalid Objective-C protocol composition identifier"));
        return false;
      }
      protocols.push_back(Previous().text);

      if (Match(TokenKind::Comma)) {

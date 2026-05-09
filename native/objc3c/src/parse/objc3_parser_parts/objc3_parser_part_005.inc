  bool ParseCStyleFunctionParameters(FunctionDecl &fn) {
    if (At(TokenKind::RParen)) {
      return true;
    }

    std::size_t parameter_index = 0;
    while (true) {
      FuncParam param;
      const Token &type_token = Peek();
      param.line = type_token.line;
      param.column = type_token.column;

      const Objc3CStyleFunctionTypeParseResult parsed_type =
          ParseObjc3CStyleFunctionType(tokens_, index_, parameter_index == 0,
                                       type_token.line, type_token.column,
                                       "parameter", diagnostics_);
      if (!parsed_type.ok) {
        return false;
      }
      param.type = parsed_type.type;
      param.id_spelling = parsed_type.id_spelling;
      param.class_spelling = parsed_type.class_spelling;
      param.sel_spelling = parsed_type.sel_spelling;
      param.instancetype_spelling = parsed_type.instancetype_spelling;
      param.object_pointer_type_spelling = parsed_type.object_pointer_spelling;
      param.object_pointer_type_name = parsed_type.object_pointer_name;
      param.has_pointer_declarator = parsed_type.pointer_depth > 0u;
      param.pointer_declarator_depth = parsed_type.pointer_depth;

      if (parameter_index == 0 && param.type == ValueType::Void && At(TokenKind::RParen)) {
        return true;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P101",
                     "expected parameter identifier, found " + DescribeParserDiagnosticToken(token)));
        return false;
      }

      const Token name_token = Advance();
      param.name = name_token.text;
      param.line = name_token.line;
      param.column = name_token.column;
      param.typecheck_family_symbol = BuildObjcTypecheckParamFamilySymbol(param);
      fn.params.push_back(std::move(param));
      ++parameter_index;

      if (!Match(TokenKind::Comma)) {
        return true;
      }
      if (At(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P104",
                     "trailing ',' in C-style parameter list is not allowed"));
        return false;
      }
      if (!AtCStyleTypeLeadToken() && !At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100",
                     "expected parameter declaration after ',' in C-style parameter list, found " +
                         DescribeParserDiagnosticToken(token)));
        return false;
      }
    }
  }

  void ParseTopLevelCStyleFunctionDecl(Objc3ParsedProgram &program) {
    auto fn = std::make_unique<FunctionDecl>();
    const Token &return_type_token = Peek();
    const Objc3CStyleFunctionTypeParseResult parsed_return_type =
        ParseObjc3CStyleFunctionType(tokens_, index_, true,
                                     return_type_token.line,
                                     return_type_token.column,
                                     "function return", diagnostics_);
    if (!parsed_return_type.ok) {
      SynchronizeTopLevel();
      return;
    }
    fn->return_type = parsed_return_type.type;
    fn->return_id_spelling = parsed_return_type.id_spelling;
    fn->return_class_spelling = parsed_return_type.class_spelling;
    fn->return_sel_spelling = parsed_return_type.sel_spelling;
    fn->return_instancetype_spelling = parsed_return_type.instancetype_spelling;
    fn->return_object_pointer_type_spelling =
        parsed_return_type.object_pointer_spelling;
    fn->return_object_pointer_type_name =
        parsed_return_type.object_pointer_name;
    fn->has_return_pointer_declarator = parsed_return_type.pointer_depth > 0u;
    fn->return_pointer_declarator_depth = parsed_return_type.pointer_depth;
    fn->return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(*fn);

    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P101",
                   "expected function identifier, found " + DescribeParserDiagnosticToken(token)));
      SynchronizeTopLevel();
      return;
    }
    const Token name_token = Advance();
    fn->name = name_token.text;
    fn->line = name_token.line;
    fn->column = name_token.column;
    fn->scope_owner_symbol = "global";
    fn->scope_path_lexicographic =
        BuildScopePathLexicographic(fn->scope_owner_symbol, "function:" + fn->name);

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106",
                   "missing '(' after function name; found " + DescribeParserDiagnosticToken(token)));
      SynchronizeTopLevel();
      return;
    }

    if (!ParseCStyleFunctionParameters(*fn)) {
      SynchronizeTopLevel();
      return;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109",
                   "missing ')' after parameters; found " + DescribeParserDiagnosticToken(token)));
      SynchronizeTopLevel();
      return;
    }

    if (Match(TokenKind::Semicolon)) {
      fn->is_prototype = true;
      FinalizeThrowsDeclarationProfile(*fn, true);
      FinalizeResultLikeProfile(*fn);
      FinalizeNSErrorBridgingProfile(*fn);
      FinalizeUnwindCleanupProfile(*fn);
      FinalizeErrorDiagnosticsRecoveryProfile(*fn);
      FinalizeAsyncContinuationProfile(*fn);
      FinalizeAwaitSuspensionProfile(*fn);
      FinalizeActorIsolationSendabilityProfile(*fn);
      FinalizeTaskRuntimeCancellationProfile(*fn);
      FinalizeConcurrencyReplayRaceGuardProfile(*fn);
      FinalizeUnsafePointerExtensionProfile(*fn);
      FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
      ast_builder_.AddFunctionDecl(program, std::move(*fn));
      return;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      SynchronizeTopLevel();
      return;
    }

    fn->body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      SynchronizeTopLevel();
      return;
    }

    FinalizeThrowsDeclarationProfile(*fn, true);
    FinalizeResultLikeProfile(*fn);
    FinalizeNSErrorBridgingProfile(*fn);
    FinalizeUnwindCleanupProfile(*fn);
    FinalizeErrorDiagnosticsRecoveryProfile(*fn);
    FinalizeAsyncContinuationProfile(*fn);
    FinalizeAwaitSuspensionProfile(*fn);
    FinalizeActorIsolationSendabilityProfile(*fn);
    FinalizeTaskRuntimeCancellationProfile(*fn);
    FinalizeConcurrencyReplayRaceGuardProfile(*fn);
    FinalizeUnsafePointerExtensionProfile(*fn);
    FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
    ast_builder_.AddFunctionDecl(program, std::move(*fn));
  }

  void ParseTopLevelFunctionDecl(Objc3ParsedProgram &program) {
    bool is_pure = false;
    bool is_extern = false;
    bool is_async = false;
    std::optional<TokenKind> trailing_qualifier = std::nullopt;

    while (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      if (Match(TokenKind::KwPure)) {
        if (is_pure) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'pure' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_pure = true;
        trailing_qualifier = TokenKind::KwPure;
        continue;
      }

      if (Match(TokenKind::KwExtern)) {
        if (is_extern) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'extern' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_extern = true;
        trailing_qualifier = TokenKind::KwExtern;
        continue;
      }

      if (Match(TokenKind::KwAsync)) {
        if (is_async) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'async' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_async = true;
        trailing_qualifier = TokenKind::KwAsync;
      }
    }

    if (!Match(TokenKind::KwFn)) {
      const Token &token = Peek();
      const std::string message = (trailing_qualifier.has_value() && trailing_qualifier.value() == TokenKind::KwExtern)
                                      ? "expected 'fn' after 'extern'"
                                      : "expected 'fn' after 'pure'";
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", message));
      SynchronizeTopLevel();
      return;
    }

    auto fn = ParseFunction();
    if (fn == nullptr) {
      return;
    }

    fn->is_pure = is_pure;
    fn->async_declared = is_async;
    // ParseFunction finalizes semantic profiles before top-level qualifiers are
    // copied onto the declaration, so async callables need one final pass after
    // the real async bit is attached.
    FinalizeAsyncContinuationProfile(*fn);
    FinalizeAwaitSuspensionProfile(*fn);
    if (is_extern && !fn->is_prototype) {
      diagnostics_.push_back(MakeDiag(fn->line, fn->column, "O3P104", "missing ';' after extern function declaration"));
      return;
    }

    ast_builder_.AddFunctionDecl(program, std::move(*fn));
  }

  bool AtIdentifierColon() const {
    return IsObjc3IdentifierColonStatementLead(tokens_, index_);
  }

  bool AtIdentifierAssignment() const {
    return IsObjc3IdentifierAssignmentStatementLead(tokens_, index_);
  }

  bool AtIdentifierUpdate() const {
    return IsObjc3IdentifierUpdateStatementLead(tokens_, index_);
  }

  bool AtPrefixUpdate() const {
    return IsObjc3PrefixUpdateStatementLead(tokens_, index_);
  }

  bool MatchAssignmentOperator(std::string &op) {
    if (!objc3c::parse::IsObjc3AssignmentOperatorToken(Peek().kind)) {
      return false;
    }
    op = objc3c::parse::Objc3AssignmentOperatorSpelling(Peek().kind);
    Advance();
    return true;
  }

  bool MatchUpdateOperator(std::string &op) {
    if (!objc3c::parse::IsObjc3UpdateOperatorToken(Peek().kind)) {
      return false;
    }
    op = objc3c::parse::Objc3UpdateOperatorSpelling(Peek().kind);
    Advance();
    return true;
  }

  void ParseModule(Objc3ParsedProgram &program) {
    // registration-descriptor/image-root source-surface and
    // frontend-closure anchor: module identity remains parser-owned here,
    // while the paired registration-descriptor and image-root prelude pragmas
    // are lexer-owned inputs that later frontend-manifest code resolves into
    // one bootstrap naming surface and emitted descriptor artifact boundary.
    const Token &name_token = Peek();
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P101", "invalid module identifier"));
      SynchronizeTopLevel();
      return;
    }
    const std::string module_name = Advance().text;
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P104", "missing ';' after module declaration"));
      SynchronizeTopLevel();
      return;
    }
    if (saw_module_declaration_) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3S200", "duplicate module '" + module_name + "'"));
      return;
    }
    saw_module_declaration_ = true;
    ast_builder_.SetModuleName(program, module_name);
  }

  std::unique_ptr<GlobalDecl> ParseGlobalLet() {
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          BuildObjc3InvalidDeclarationIdentifierDiagnostic(name_token));
      SynchronizeTopLevel();
      return nullptr;
    }
    auto decl = std::make_unique<GlobalDecl>(
        BuildObjc3GlobalLetDeclarationIdentity(Previous()));

    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
      SynchronizeTopLevel();
      return nullptr;
    }

    decl->value = ParseExpressionWithBlockLiteralSourceUse(
        BlockLiteralSourceUseKind::GlobalInitializer);
    if (decl->value == nullptr) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          BuildObjc3MissingSemicolonDiagnostic(token, "declaration"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  void CopyMethodReturnTypeFromFunctionDecl(const FunctionDecl &source, Objc3MethodDecl &target) {
    target.return_type = source.return_type;
    target.return_vector_spelling = source.return_vector_spelling;
    target.return_vector_base_spelling = source.return_vector_base_spelling;
    target.return_vector_lane_count = source.return_vector_lane_count;
    target.return_id_spelling = source.return_id_spelling;
    target.return_class_spelling = source.return_class_spelling;
    target.return_sel_spelling = source.return_sel_spelling;
    target.return_instancetype_spelling = source.return_instancetype_spelling;
    target.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
    target.return_object_pointer_type_name = source.return_object_pointer_type_name;
    target.return_typecheck_family_symbol = source.return_typecheck_family_symbol;
    target.has_return_generic_suffix = source.has_return_generic_suffix;
    target.return_generic_suffix_terminated = source.return_generic_suffix_terminated;
    target.return_generic_suffix_text = source.return_generic_suffix_text;
    target.return_generic_line = source.return_generic_line;
    target.return_generic_column = source.return_generic_column;
    target.return_lightweight_generic_constraint_profile_is_normalized =
        source.return_lightweight_generic_constraint_profile_is_normalized;
    target.return_lightweight_generic_constraint_profile =
        source.return_lightweight_generic_constraint_profile;
    target.return_nullability_flow_profile_is_normalized =
        source.return_nullability_flow_profile_is_normalized;
    target.return_nullability_flow_profile =
        source.return_nullability_flow_profile;
    target.return_protocol_qualified_object_type_profile_is_normalized =
        source.return_protocol_qualified_object_type_profile_is_normalized;
    target.return_protocol_qualified_object_type_profile =
        source.return_protocol_qualified_object_type_profile;
    target.return_variance_bridge_cast_profile_is_normalized =
        source.return_variance_bridge_cast_profile_is_normalized;
    target.return_variance_bridge_cast_profile =
        source.return_variance_bridge_cast_profile;
    target.return_generic_metadata_abi_profile_is_normalized =
        source.return_generic_metadata_abi_profile_is_normalized;
    target.return_generic_metadata_abi_profile =
        source.return_generic_metadata_abi_profile;
    target.return_module_import_graph_profile_is_normalized =
        source.return_module_import_graph_profile_is_normalized;
    target.return_module_import_graph_profile =
        source.return_module_import_graph_profile;
    target.return_namespace_collision_shadowing_profile_is_normalized =
        source.return_namespace_collision_shadowing_profile_is_normalized;
    target.return_namespace_collision_shadowing_profile =
        source.return_namespace_collision_shadowing_profile;
    target.return_public_private_api_partition_profile_is_normalized =
        source.return_public_private_api_partition_profile_is_normalized;
    target.return_public_private_api_partition_profile =
        source.return_public_private_api_partition_profile;
    target.return_incremental_module_cache_invalidation_profile_is_normalized =
        source.return_incremental_module_cache_invalidation_profile_is_normalized;
    target.return_incremental_module_cache_invalidation_profile =
        source.return_incremental_module_cache_invalidation_profile;
    target.return_cross_module_conformance_profile_is_normalized =
        source.return_cross_module_conformance_profile_is_normalized;
    target.return_cross_module_conformance_profile =
        source.return_cross_module_conformance_profile;
    target.has_return_pointer_declarator = source.has_return_pointer_declarator;
    target.return_pointer_declarator_depth = source.return_pointer_declarator_depth;
    target.return_pointer_declarator_tokens = source.return_pointer_declarator_tokens;
    target.return_nullability_suffix_tokens = source.return_nullability_suffix_tokens;
    target.has_return_ownership_qualifier = source.has_return_ownership_qualifier;
    target.return_ownership_qualifier_spelling = source.return_ownership_qualifier_spelling;
    target.return_ownership_qualifier_symbol = source.return_ownership_qualifier_symbol;
    target.return_ownership_qualifier_tokens = source.return_ownership_qualifier_tokens;
    target.return_ownership_insert_retain = source.return_ownership_insert_retain;
    target.return_ownership_insert_release = source.return_ownership_insert_release;
    target.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
    target.return_ownership_operation_profile = source.return_ownership_operation_profile;
    target.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
    target.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
    target.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
    target.return_ownership_lifetime_profile = source.return_ownership_lifetime_profile;
    target.return_ownership_runtime_hook_profile = source.return_ownership_runtime_hook_profile;
    target.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
    target.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
    target.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
    target.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
    target.return_borrowed_pointer_qualified = source.return_borrowed_pointer_qualified;
    target.objc_returns_borrowed_declared = source.objc_returns_borrowed_declared;
    target.objc_returns_borrowed_owner_index = source.objc_returns_borrowed_owner_index;
    target.returns_borrowed_profile = source.returns_borrowed_profile;
    target.throws_declared = source.throws_declared;
    target.throws_declaration_profile_is_normalized =
        source.throws_declaration_profile_is_normalized;
    target.throws_declaration_profile = source.throws_declaration_profile;
    target.result_like_profile_is_normalized = source.result_like_profile_is_normalized;
    target.deterministic_result_like_lowering_handoff =
        source.deterministic_result_like_lowering_handoff;
    target.result_like_sites = source.result_like_sites;
    target.result_success_sites = source.result_success_sites;
    target.result_failure_sites = source.result_failure_sites;
    target.result_branch_sites = source.result_branch_sites;
    target.result_payload_sites = source.result_payload_sites;
    target.result_normalized_sites = source.result_normalized_sites;
    target.result_branch_merge_sites = source.result_branch_merge_sites;
    target.result_contract_violation_sites = source.result_contract_violation_sites;
    target.result_like_profile = source.result_like_profile;
    target.ns_error_bridging_profile_is_normalized = source.ns_error_bridging_profile_is_normalized;
    target.deterministic_ns_error_bridging_lowering_handoff =
        source.deterministic_ns_error_bridging_lowering_handoff;
    target.ns_error_bridging_sites = source.ns_error_bridging_sites;
    target.ns_error_parameter_sites = source.ns_error_parameter_sites;
    target.ns_error_out_parameter_sites = source.ns_error_out_parameter_sites;
    target.ns_error_bridge_path_sites = source.ns_error_bridge_path_sites;
    target.failable_call_sites = source.failable_call_sites;
    target.ns_error_bridging_normalized_sites = source.ns_error_bridging_normalized_sites;
    target.ns_error_bridge_boundary_sites = source.ns_error_bridge_boundary_sites;
    target.ns_error_bridging_contract_violation_sites = source.ns_error_bridging_contract_violation_sites;
    target.ns_error_bridging_profile = source.ns_error_bridging_profile;
    target.objc_nserror_declared = source.objc_nserror_declared;
    target.objc_status_code_declared = source.objc_status_code_declared;
    target.error_bridge_marker_profile_is_normalized =
        source.error_bridge_marker_profile_is_normalized;
    target.objc_nserror_attribute_sites = source.objc_nserror_attribute_sites;
    target.objc_status_code_attribute_sites = source.objc_status_code_attribute_sites;
    target.status_code_success_clause_sites = source.status_code_success_clause_sites;
    target.status_code_error_type_clause_sites =
        source.status_code_error_type_clause_sites;
    target.status_code_mapping_clause_sites = source.status_code_mapping_clause_sites;
    target.error_bridge_marker_contract_violation_sites =
        source.error_bridge_marker_contract_violation_sites;
    target.objc_status_code_success_literal =
        source.objc_status_code_success_literal;
    target.objc_status_code_error_type_spelling =
        source.objc_status_code_error_type_spelling;
    target.objc_status_code_mapping_symbol =
        source.objc_status_code_mapping_symbol;
    target.error_bridge_marker_profile = source.error_bridge_marker_profile;
    target.unwind_cleanup_profile_is_normalized = source.unwind_cleanup_profile_is_normalized;
    target.deterministic_unwind_cleanup_handoff =
        source.deterministic_unwind_cleanup_handoff;
    target.unwind_cleanup_sites = source.unwind_cleanup_sites;
    target.exceptional_exit_sites = source.exceptional_exit_sites;
    target.cleanup_action_sites = source.cleanup_action_sites;
    target.cleanup_scope_sites = source.cleanup_scope_sites;
    target.cleanup_resume_sites = source.cleanup_resume_sites;
    target.unwind_cleanup_normalized_sites = source.unwind_cleanup_normalized_sites;
    target.unwind_cleanup_fail_closed_sites = source.unwind_cleanup_fail_closed_sites;
    target.unwind_cleanup_contract_violation_sites =
        source.unwind_cleanup_contract_violation_sites;
    target.unwind_cleanup_profile = source.unwind_cleanup_profile;
    target.error_diagnostics_recovery_profile_is_normalized =
        source.error_diagnostics_recovery_profile_is_normalized;
    target.deterministic_error_diagnostics_recovery_handoff =
        source.deterministic_error_diagnostics_recovery_handoff;
    target.error_diagnostics_recovery_sites =
        source.error_diagnostics_recovery_sites;
    target.diagnostic_emit_sites = source.diagnostic_emit_sites;
    target.recovery_anchor_sites = source.recovery_anchor_sites;
    target.recovery_boundary_sites = source.recovery_boundary_sites;
    target.fail_closed_diagnostic_sites = source.fail_closed_diagnostic_sites;
    target.error_diagnostics_recovery_normalized_sites =
        source.error_diagnostics_recovery_normalized_sites;
    target.error_diagnostics_recovery_gate_blocked_sites =
        source.error_diagnostics_recovery_gate_blocked_sites;
    target.error_diagnostics_recovery_contract_violation_sites =
        source.error_diagnostics_recovery_contract_violation_sites;
    target.error_diagnostics_recovery_profile =
        source.error_diagnostics_recovery_profile;
    target.async_continuation_profile_is_normalized =
        source.async_continuation_profile_is_normalized;
    target.deterministic_async_continuation_handoff =
        source.deterministic_async_continuation_handoff;
    target.async_continuation_sites = source.async_continuation_sites;
    target.async_keyword_sites = source.async_keyword_sites;
    target.async_function_sites = source.async_function_sites;
    target.continuation_allocation_sites = source.continuation_allocation_sites;
    target.continuation_resume_sites = source.continuation_resume_sites;
    target.continuation_suspend_sites = source.continuation_suspend_sites;
    target.async_state_machine_sites = source.async_state_machine_sites;
    target.async_continuation_normalized_sites =
        source.async_continuation_normalized_sites;
    target.async_continuation_gate_blocked_sites =
        source.async_continuation_gate_blocked_sites;
    target.async_continuation_contract_violation_sites =
        source.async_continuation_contract_violation_sites;
    target.async_continuation_profile = source.async_continuation_profile;
    target.await_suspension_profile_is_normalized =
        source.await_suspension_profile_is_normalized;
    target.deterministic_await_suspension_handoff =
        source.deterministic_await_suspension_handoff;
    target.await_suspension_sites = source.await_suspension_sites;
    target.await_keyword_sites = source.await_keyword_sites;
    target.await_suspension_point_sites = source.await_suspension_point_sites;
    target.await_resume_sites = source.await_resume_sites;
    target.await_state_machine_sites = source.await_state_machine_sites;
    target.await_continuation_sites = source.await_continuation_sites;
    target.await_suspension_normalized_sites =
        source.await_suspension_normalized_sites;
    target.await_suspension_gate_blocked_sites =
        source.await_suspension_gate_blocked_sites;
    target.await_suspension_contract_violation_sites =
        source.await_suspension_contract_violation_sites;
    target.await_suspension_profile = source.await_suspension_profile;
    target.actor_isolation_sendability_profile_is_normalized =
        source.actor_isolation_sendability_profile_is_normalized;
    target.deterministic_actor_isolation_sendability_handoff =
        source.deterministic_actor_isolation_sendability_handoff;
    target.actor_isolation_sendability_sites =
        source.actor_isolation_sendability_sites;
    target.actor_isolation_decl_sites = source.actor_isolation_decl_sites;
    target.actor_hop_sites = source.actor_hop_sites;
    target.sendable_annotation_sites = source.sendable_annotation_sites;
    target.non_sendable_crossing_sites = source.non_sendable_crossing_sites;
    target.isolation_boundary_sites = source.isolation_boundary_sites;
    target.actor_isolation_sendability_normalized_sites =
        source.actor_isolation_sendability_normalized_sites;
    target.actor_isolation_sendability_gate_blocked_sites =
        source.actor_isolation_sendability_gate_blocked_sites;
    target.actor_isolation_sendability_contract_violation_sites =
        source.actor_isolation_sendability_contract_violation_sites;
    target.actor_isolation_sendability_profile =
        source.actor_isolation_sendability_profile;
    target.task_runtime_cancellation_profile_is_normalized =
        source.task_runtime_cancellation_profile_is_normalized;
    target.deterministic_task_runtime_cancellation_handoff =
        source.deterministic_task_runtime_cancellation_handoff;
    target.task_runtime_interop_sites = source.task_runtime_interop_sites;
    target.runtime_hook_sites = source.runtime_hook_sites;
    target.cancellation_check_sites = source.cancellation_check_sites;
    target.cancellation_handler_sites = source.cancellation_handler_sites;
    target.suspension_point_sites = source.suspension_point_sites;
    target.cancellation_propagation_sites =
        source.cancellation_propagation_sites;
    target.task_runtime_normalized_sites = source.task_runtime_normalized_sites;
    target.task_runtime_gate_blocked_sites = source.task_runtime_gate_blocked_sites;
    target.task_runtime_contract_violation_sites =
        source.task_runtime_contract_violation_sites;
    target.task_runtime_cancellation_normalized_sites =
        source.task_runtime_cancellation_normalized_sites;
    target.task_runtime_cancellation_gate_blocked_sites =
        source.task_runtime_cancellation_gate_blocked_sites;
    target.task_runtime_cancellation_contract_violation_sites =
        source.task_runtime_cancellation_contract_violation_sites;
    target.task_runtime_cancellation_profile =
        source.task_runtime_cancellation_profile;
    target.concurrency_replay_race_guard_profile_is_normalized =
        source.concurrency_replay_race_guard_profile_is_normalized;
    target.deterministic_concurrency_replay_race_guard_handoff =
        source.deterministic_concurrency_replay_race_guard_handoff;
    target.concurrency_replay_race_guard_sites =
        source.concurrency_replay_race_guard_sites;
    target.concurrency_replay_sites = source.concurrency_replay_sites;
    target.replay_proof_sites = source.replay_proof_sites;
    target.race_guard_sites = source.race_guard_sites;
    target.task_handoff_sites = source.task_handoff_sites;
    target.actor_isolation_sites = source.actor_isolation_sites;
    target.deterministic_schedule_sites = source.deterministic_schedule_sites;
    target.concurrency_replay_guard_blocked_sites =
        source.concurrency_replay_guard_blocked_sites;
    target.concurrency_replay_contract_violation_sites =
        source.concurrency_replay_contract_violation_sites;
    target.concurrency_replay_race_guard_profile =
        source.concurrency_replay_race_guard_profile;
    target.unsafe_pointer_extension_profile_is_normalized =
        source.unsafe_pointer_extension_profile_is_normalized;
    target.deterministic_unsafe_pointer_extension_handoff =
        source.deterministic_unsafe_pointer_extension_handoff;
    target.unsafe_pointer_extension_sites = source.unsafe_pointer_extension_sites;
    target.unsafe_keyword_sites = source.unsafe_keyword_sites;
    target.pointer_arithmetic_sites = source.pointer_arithmetic_sites;
    target.raw_pointer_type_sites = source.raw_pointer_type_sites;
    target.unsafe_operation_sites = source.unsafe_operation_sites;
    target.unsafe_pointer_extension_normalized_sites =
        source.unsafe_pointer_extension_normalized_sites;
    target.unsafe_pointer_extension_gate_blocked_sites =
        source.unsafe_pointer_extension_gate_blocked_sites;
    target.unsafe_pointer_extension_contract_violation_sites =
        source.unsafe_pointer_extension_contract_violation_sites;
    target.unsafe_pointer_extension_profile = source.unsafe_pointer_extension_profile;
    target.inline_asm_intrinsic_governance_profile_is_normalized =
        source.inline_asm_intrinsic_governance_profile_is_normalized;
    target.deterministic_inline_asm_intrinsic_governance_handoff =
        source.deterministic_inline_asm_intrinsic_governance_handoff;
    target.inline_asm_intrinsic_sites = source.inline_asm_intrinsic_sites;
    target.inline_asm_sites = source.inline_asm_sites;
    target.intrinsic_sites = source.intrinsic_sites;
    target.governed_intrinsic_sites = source.governed_intrinsic_sites;
    target.privileged_intrinsic_sites = source.privileged_intrinsic_sites;
    target.inline_asm_intrinsic_normalized_sites =
        source.inline_asm_intrinsic_normalized_sites;
    target.inline_asm_intrinsic_gate_blocked_sites =
        source.inline_asm_intrinsic_gate_blocked_sites;
    target.inline_asm_intrinsic_contract_violation_sites =
        source.inline_asm_intrinsic_contract_violation_sites;
    target.inline_asm_intrinsic_governance_profile =
        source.inline_asm_intrinsic_governance_profile;
  }

  void CopyPropertyTypeFromParam(const FuncParam &source, Objc3PropertyDecl &target) {
    target.type = source.type;
    target.vector_spelling = source.vector_spelling;
    target.vector_base_spelling = source.vector_base_spelling;
    target.vector_lane_count = source.vector_lane_count;
    target.id_spelling = source.id_spelling;
    target.class_spelling = source.class_spelling;
    target.sel_spelling = source.sel_spelling;
    target.instancetype_spelling = source.instancetype_spelling;
    target.object_pointer_type_spelling = source.object_pointer_type_spelling;
    target.object_pointer_type_name = source.object_pointer_type_name;
    target.typecheck_family_symbol = source.typecheck_family_symbol;
    target.has_generic_suffix = source.has_generic_suffix;
    target.generic_suffix_terminated = source.generic_suffix_terminated;
    target.generic_suffix_text = source.generic_suffix_text;
    target.generic_line = source.generic_line;
    target.generic_column = source.generic_column;
    target.lightweight_generic_constraint_profile_is_normalized =
        source.lightweight_generic_constraint_profile_is_normalized;
    target.lightweight_generic_constraint_profile =
        source.lightweight_generic_constraint_profile;
    target.nullability_flow_profile_is_normalized =
        source.nullability_flow_profile_is_normalized;
    target.nullability_flow_profile =
        source.nullability_flow_profile;
    target.protocol_qualified_object_type_profile_is_normalized =
        source.protocol_qualified_object_type_profile_is_normalized;
    target.protocol_qualified_object_type_profile =
        source.protocol_qualified_object_type_profile;
    target.variance_bridge_cast_profile_is_normalized =
        source.variance_bridge_cast_profile_is_normalized;
    target.variance_bridge_cast_profile =
        source.variance_bridge_cast_profile;
    target.generic_metadata_abi_profile_is_normalized =
        source.generic_metadata_abi_profile_is_normalized;
    target.generic_metadata_abi_profile =
        source.generic_metadata_abi_profile;
    target.module_import_graph_profile_is_normalized =
        source.module_import_graph_profile_is_normalized;
    target.module_import_graph_profile =
        source.module_import_graph_profile;
    target.namespace_collision_shadowing_profile_is_normalized =
        source.namespace_collision_shadowing_profile_is_normalized;
    target.namespace_collision_shadowing_profile =
        source.namespace_collision_shadowing_profile;
    target.public_private_api_partition_profile_is_normalized =
        source.public_private_api_partition_profile_is_normalized;
    target.public_private_api_partition_profile =
        source.public_private_api_partition_profile;
    target.incremental_module_cache_invalidation_profile_is_normalized =
        source.incremental_module_cache_invalidation_profile_is_normalized;
    target.incremental_module_cache_invalidation_profile =
        source.incremental_module_cache_invalidation_profile;
    target.cross_module_conformance_profile_is_normalized =
        source.cross_module_conformance_profile_is_normalized;
    target.cross_module_conformance_profile =
        source.cross_module_conformance_profile;
    target.has_pointer_declarator = source.has_pointer_declarator;
    target.pointer_declarator_depth = source.pointer_declarator_depth;
    target.pointer_declarator_tokens = source.pointer_declarator_tokens;
    target.nullability_suffix_tokens = source.nullability_suffix_tokens;
    target.has_ownership_qualifier = source.has_ownership_qualifier;
    target.ownership_qualifier_spelling = source.ownership_qualifier_spelling;
    target.ownership_qualifier_symbol = source.ownership_qualifier_symbol;
    target.ownership_qualifier_tokens = source.ownership_qualifier_tokens;
    target.ownership_insert_retain = source.ownership_insert_retain;
    target.ownership_insert_release = source.ownership_insert_release;
    target.ownership_insert_autorelease = source.ownership_insert_autorelease;
    target.ownership_operation_profile = source.ownership_operation_profile;
    target.ownership_is_weak_reference = source.ownership_is_weak_reference;
    target.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
    target.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
    target.ownership_lifetime_profile = source.ownership_lifetime_profile;
    target.ownership_runtime_hook_profile = source.ownership_runtime_hook_profile;
    target.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
    target.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
    target.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
    target.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
  }

  bool AtThrowsClauseKeyword() const {
    return At(TokenKind::Identifier) && Peek().text == "throws";
  }

  bool AtAsyncClauseKeyword() const {
    return At(TokenKind::KwAsync);
  }

  bool AtIdentifierText(const char *text) const {
    return objc3c::parse::AtIdentifierText(tokens_, index_, text);
  }

  bool AtIdentifierTextOffset(std::size_t offset, const char *text) const {
    return objc3c::parse::AtIdentifierTextOffset(tokens_, index_, offset, text);
  }

  bool ParseOptionalThrowsClause(FunctionDecl &fn) {
    if (!AtThrowsClauseKeyword()) {
      return true;
    }
    const Token throws_token = Advance();
    if (fn.throws_declared) {
      diagnostics_.push_back(MakeDiag(throws_token.line, throws_token.column, "O3P181",
                                      "duplicate 'throws' declaration modifier"));
      return false;
    }
    fn.throws_declared = true;
    return true;
  }

  bool ParseOptionalThrowsClause(Objc3MethodDecl &method) {
    if (!AtThrowsClauseKeyword()) {
      return true;
    }
    const Token throws_token = Advance();
    if (method.throws_declared) {
      diagnostics_.push_back(MakeDiag(throws_token.line, throws_token.column, "O3P181",
                                      "duplicate 'throws' declaration modifier"));
      return false;
    }
    method.throws_declared = true;
    return true;
  }

  bool ParseOptionalAsyncClause(FunctionDecl &fn) {
    if (!AtAsyncClauseKeyword()) {
      return true;
    }
    const Token async_token = Advance();
    if (fn.async_declared) {
      diagnostics_.push_back(MakeDiag(async_token.line, async_token.column, "O3P181",
                                      "duplicate 'async' declaration modifier"));
      return false;
    }
    fn.async_declared = true;
    return true;
  }

  bool ParseOptionalAsyncClause(Objc3MethodDecl &method) {
    if (!AtAsyncClauseKeyword()) {
      return true;
    }
    const Token async_token = Advance();
    if (method.async_declared) {
      diagnostics_.push_back(MakeDiag(async_token.line, async_token.column, "O3P181",
                                      "duplicate 'async' declaration modifier"));
      return false;
    }
    method.async_declared = true;
    return true;
  }

  template <typename TCallableDecl>
  bool ParseExecutorAttributePayload(TCallableDecl &decl, const Token &attribute_token) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P287", "missing '(' after objc_executor attribute"));
      return false;
    }

    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P288", "invalid objc_executor payload"));
      return false;
    }

    const Token kind = Advance();
    decl.executor_affinity_declared = true;
    decl.executor_affinity_kind = kind.text;
    decl.executor_affinity_named = false;
    decl.executor_affinity_name.clear();

    if (kind.text == "main" || kind.text == "global") {
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P289", "missing ')' after objc_executor payload"));
        return false;
      }
      return true;
    }

    if (kind.text != "named") {
      diagnostics_.push_back(MakeDiag(attribute_token.line, attribute_token.column, "O3P290",
                                      "unsupported objc_executor payload '" + kind.text + "'"));
      return false;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P291", "missing '(' after objc_executor named payload"));
      return false;
    }
    if (!At(TokenKind::String)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P292", "objc_executor named payload requires string literal"));
      return false;
    }
    decl.executor_affinity_name = Advance().text;
    decl.executor_affinity_named = true;
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P293",
                                      "missing '))' after objc_executor named payload"));
      return false;
    }
    return true;
  }

  std::string ParseAttributeArgumentText() {
    std::string text;
    while (!At(TokenKind::Eof) && !At(TokenKind::Comma) &&
           !At(TokenKind::RParen)) {
      text += Advance().text;
    }
    return text;
  }

  bool ParseNamedStringAttributePayload(const Token &attribute_name,
                                        const char *attribute_spelling,
                                        std::string &named_value_out) {
    (void)attribute_name;
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P340",
                                      std::string("missing '(' after ") + attribute_spelling + " attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "named") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P341",
                                      std::string(attribute_spelling) + " requires named(\"...\") payload"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P342",
                                      std::string("missing '(' after ") + attribute_spelling + " named payload"));
      return false;
    }
    if (!At(TokenKind::String)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P343",
                                      std::string(attribute_spelling) + " named payload requires string literal"));
      return false;
    }
    named_value_out = Advance().text;
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P344",
                                      std::string("missing '))' after ") + attribute_spelling + " named payload"));
      return false;
    }
    return true;
  }

  template <typename TCallableDecl>
  bool ParseAbiAlignmentAttributePayload(TCallableDecl &decl,
                                         const Token &attribute_name) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P361",
          "missing '(' after objc_abi_align attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "bytes") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P362",
          "objc_abi_align requires bytes: integer payload"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::Colon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P363",
          "missing ':' after objc_abi_align bytes label"));
      return false;
    }
    if (!At(TokenKind::Number)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P364",
          "objc_abi_align bytes payload requires integer literal"));
      return false;
    }
    const Token bytes_token = Advance();
    std::size_t alignment = 0;
    try {
      alignment =
          static_cast<std::size_t>(std::stoull(bytes_token.text, nullptr, 0));
    } catch (...) {
      diagnostics_.push_back(MakeDiag(
          bytes_token.line, bytes_token.column, "O3P364",
          "objc_abi_align bytes payload requires integer literal"));
      return false;
    }
    if (alignment == 0u || (alignment & (alignment - 1u)) != 0u) {
      diagnostics_.push_back(MakeDiag(
          bytes_token.line, bytes_token.column, "O3P365",
          "objc_abi_align bytes payload must be a non-zero power of two"));
      return false;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P366",
          "missing ')' after objc_abi_align payload"));
      return false;
    }
    decl.objc_abi_align_declared = true;
    decl.objc_abi_alignment_bytes = alignment;
    (void)attribute_name;
    return true;
  }

  bool IsBorrowedQualifierSpelling(const std::string &text) const {
    return text == "borrowed";
  }

  std::string BuildReturnsBorrowedProfile(bool declared,
                                          std::size_t owner_index,
                                          bool return_borrowed_pointer_qualified) {
    std::ostringstream out;
    out << "returns-borrowed:declared=" << (declared ? "true" : "false")
        << ";owner_index=" << owner_index
        << ";return_borrowed=" << (return_borrowed_pointer_qualified ? "true" : "false");
    return out.str();
  }

  std::string BuildResourceAttributeProfile(bool declared,
                                            const std::string &close_symbol,
                                            const std::string &invalid_expression) {
    std::ostringstream out;
    out << "resource:declared=" << (declared ? "true" : "false")
        << ";close=" << close_symbol
        << ";invalid=" << invalid_expression;
    return out.str();
  }

  std::string BuildCleanupAttributeProfile(bool declared,
                                           const std::string &cleanup_symbol) {
    std::ostringstream out;
    out << "cleanup:declared=" << (declared ? "true" : "false")
        << ";symbol=" << cleanup_symbol;
    return out.str();
  }

  std::string BuildRetainableCFamilyCallableProfile(
      const std::vector<std::string> &attribute_names,
      const std::vector<std::string> &family_names) {
    std::ostringstream out;
    out << "retainable-c-family:attrs=";
    for (std::size_t index = 0; index < attribute_names.size(); ++index) {
      if (index != 0u) {
        out << ",";
      }
      out << attribute_names[index];
    }
    out << ";families=";
    for (std::size_t index = 0; index < family_names.size(); ++index) {
      if (index != 0u) {
        out << ",";
      }
      out << family_names[index];
    }
    return out.str();
  }

  template <typename TCallableDecl>
  bool ParseReturnsBorrowedAttributePayload(TCallableDecl &decl) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P296",
          "missing '(' after objc_returns_borrowed attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "owner_index") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P297",
          "objc_returns_borrowed requires owner_index clause"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P298",
          "missing '=' after owner_index in objc_returns_borrowed"));
      return false;
    }
    if (!At(TokenKind::Number)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P299",
          "objc_returns_borrowed owner_index requires integer literal"));
      return false;
    }
    decl.objc_returns_borrowed_owner_index =
        static_cast<std::size_t>(std::max(0, std::stoi(Advance().text)));
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P300",
          "missing ')' after objc_returns_borrowed payload"));
      return false;
    }
    decl.objc_returns_borrowed_declared = true;
    return true;
  }

  bool ParseLocalCleanupAttribute(LetStmt &stmt, const Token &) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P301",
          "missing '(' after cleanup"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P302",
          "cleanup requires cleanup function identifier"));
      return false;
    }
    stmt.cleanup_function_symbol = Advance().text;
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P303",
          "missing ')))' after cleanup local annotation"));
      return false;
    }
    stmt.cleanup_attribute_declared = true;
    stmt.cleanup_profile_is_normalized = true;
    stmt.cleanup_profile =
        BuildCleanupAttributeProfile(true, stmt.cleanup_function_symbol);
    return true;
  }

  bool ParseLocalResourceAttribute(LetStmt &stmt, const Token &attribute_name) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P304",
          "missing '(' after objc_resource"));
      return false;
    }
    bool saw_close = false;
    bool saw_invalid = false;
    while (!At(TokenKind::Eof) && !At(TokenKind::RParen)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P305",
            "invalid objc_resource clause label"));
        return false;
      }
      const Token clause = Advance();
      if (!Match(TokenKind::Equal)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P306",
            "missing '=' after objc_resource clause label"));
        return false;
      }
      const std::string value_text = ParseAttributeArgumentText();
      if (value_text.empty()) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P307",
            "objc_resource clause value must not be empty"));
        return false;
      }
      if (clause.text == "close") {
        if (saw_close) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P308",
              "duplicate objc_resource close clause"));
          return false;
        }
        stmt.resource_close_symbol = value_text;
        saw_close = true;
      } else if (clause.text == "invalid") {
        if (saw_invalid) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P309",
              "duplicate objc_resource invalid clause"));
          return false;
        }
        stmt.resource_invalid_expression = value_text;
        saw_invalid = true;
      } else {
        diagnostics_.push_back(MakeDiag(
            clause.line, clause.column, "O3P310",
            "unsupported objc_resource clause '" + clause.text + "'"));
        return false;
      }
      if (At(TokenKind::Comma)) {
        Advance();
      } else {
        break;
      }
    }
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P311",
          "missing ')))' after objc_resource local annotation"));
      return false;
    }
    if (!saw_close || !saw_invalid) {
      diagnostics_.push_back(MakeDiag(
          attribute_name.line, attribute_name.column, "O3P312",
          "objc_resource requires close and invalid clauses"));
      return false;
    }
    stmt.resource_attribute_declared = true;
    stmt.resource_profile_is_normalized = true;
    stmt.resource_profile = BuildResourceAttributeProfile(
        true, stmt.resource_close_symbol, stmt.resource_invalid_expression);
    return true;
  }

  bool ParseLocalStorageAttribute(LetStmt &stmt) {
    if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P314",
          "malformed __attribute__ local annotation"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P315",
          "missing local annotation name after __attribute__(("));
      return false;
    }
    const Token attribute_name = Advance();
    if (attribute_name.text == "cleanup") {
      return ParseLocalCleanupAttribute(stmt, attribute_name);
    }
    if (attribute_name.text == "objc_resource") {
      return ParseLocalResourceAttribute(stmt, attribute_name);
    }
    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, "O3P316",
        "only cleanup and objc_resource are supported on local let bindings in this tranche"));
    return false;
  }

  bool ParseLocalStorageSugar(LetStmt &stmt) {
    if (Match(TokenKind::KwAtCleanup)) {
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P317",
            "missing '(' after @cleanup"));
        return false;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P318",
            "@cleanup requires cleanup function identifier"));
        return false;
      }
      stmt.cleanup_function_symbol = Advance().text;
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P319",
            "missing ')' after @cleanup payload"));
        return false;
      }
      stmt.cleanup_attribute_declared = true;
      stmt.cleanup_sugar_declared = true;
      stmt.cleanup_profile_is_normalized = true;
      stmt.cleanup_profile =
          BuildCleanupAttributeProfile(true, stmt.cleanup_function_symbol);
      return true;
    }
    if (Match(TokenKind::KwAtResource)) {
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P320",
            "missing '(' after @resource"));
        return false;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P321",
            "@resource requires cleanup function identifier"));
        return false;
      }
      stmt.resource_close_symbol = Advance().text;
      if (!Match(TokenKind::Comma)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P322",
            "missing ',' after @resource cleanup function"));
        return false;
      }
      if (!At(TokenKind::Identifier) || Peek().text != "invalid") {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P323",
            "@resource requires invalid clause"));
        return false;
      }
      Advance();
      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P324",
            "missing ':' after invalid in @resource"));
        return false;
      }
      const std::string invalid_expression = ParseAttributeArgumentText();
      if (invalid_expression.empty()) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P325",
            "@resource invalid expression must not be empty"));
        return false;
      }
      stmt.resource_invalid_expression = invalid_expression;
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P326",
            "missing ')' after @resource payload"));
        return false;
      }
      stmt.resource_attribute_declared = true;
      stmt.resource_sugar_declared = true;
      stmt.resource_profile_is_normalized = true;
      stmt.resource_profile = BuildResourceAttributeProfile(
          true, stmt.resource_close_symbol, stmt.resource_invalid_expression);
      return true;
    }
    return false;

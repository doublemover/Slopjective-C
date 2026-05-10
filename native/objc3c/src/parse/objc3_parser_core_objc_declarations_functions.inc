  std::unique_ptr<FunctionDecl> ParseFunction() {
    auto fn = std::make_unique<FunctionDecl>();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after 'fn'"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after 'fn'"
                                                  : "unexpected qualifier 'async' after 'fn'";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                      "invalid function identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    fn->name = Previous().text;
    fn->line = Previous().line;
    fn->column = Previous().column;
    fn->scope_owner_symbol = "global";
    fn->scope_path_lexicographic =
        BuildScopePathLexicographic(fn->scope_owner_symbol, "function:" + fn->name);

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after function name"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after function name"
                                                  : "unexpected qualifier 'async' after function name";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after function name"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseFunctionParameters(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after parameters"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after parameter list"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after parameter list"
                                                  : "unexpected qualifier 'async' after parameter list";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    bool has_return_annotation = false;
    if (!ParseOptionalAsyncClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }
    if (!ParseOptionalThrowsClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Minus)) {
      const Token arrow_start = Previous();
      if (!Match(TokenKind::Greater)) {
        diagnostics_.push_back(
            MakeDiag(arrow_start.line, arrow_start.column, "O3P114", "missing '>' in function return annotation"));
        SynchronizeFunctionTail();
        return nullptr;
      }
      if (!ParseFunctionReturnType(*fn)) {
        SynchronizeFunctionTail();
        return nullptr;
      }
      has_return_annotation = true;
    }

    if (!ParseOptionalThrowsClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseOptionalAsyncClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseOptionalCallableBridgeAttributes(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }
    fn->returns_borrowed_profile = BuildReturnsBorrowedProfile(
        fn->objc_returns_borrowed_declared,
        fn->objc_returns_borrowed_owner_index,
        fn->return_borrowed_pointer_qualified);

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after function return annotation"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after function return annotation"
                                                  : "unexpected qualifier 'async' after function return annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Semicolon)) {
      fn->is_prototype = true;
      FinalizeThrowsDeclarationProfile(*fn, has_return_annotation);
      FinalizeResultLikeProfile(*fn);
      FinalizeNSErrorBridgingProfile(*fn);
      FinalizeErrorBridgeMarkerProfile(*fn);
      FinalizeUnwindCleanupProfile(*fn);
      FinalizeErrorDiagnosticsRecoveryProfile(*fn);
      FinalizeAsyncContinuationProfile(*fn);
      FinalizeAwaitSuspensionProfile(*fn);
      FinalizeActorIsolationSendabilityProfile(*fn);
      FinalizeTaskRuntimeCancellationProfile(*fn);
      FinalizeConcurrencyReplayRaceGuardProfile(*fn);
      FinalizeUnsafePointerExtensionProfile(*fn);
      FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
      return fn;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      if (At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern) || At(TokenKind::KwAsync) || At(TokenKind::KwAtInterface) || At(TokenKind::KwAtImplementation) ||
          At(TokenKind::KwAtProtocol) || At(TokenKind::KwAtProperty) ||
          At(TokenKind::Eof)) {
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P104", "missing ';' after function prototype declaration"));
      } else {
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      }
      SynchronizeTopLevel();
      return nullptr;
    }

    fn->body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      SynchronizeTopLevel();
      return nullptr;
    }
    FinalizeThrowsDeclarationProfile(*fn, has_return_annotation);
    FinalizeResultLikeProfile(*fn);
    FinalizeNSErrorBridgingProfile(*fn);
    FinalizeErrorBridgeMarkerProfile(*fn);
    FinalizeUnwindCleanupProfile(*fn);
    FinalizeErrorDiagnosticsRecoveryProfile(*fn);
    FinalizeAsyncContinuationProfile(*fn);
    FinalizeAwaitSuspensionProfile(*fn);
    FinalizeActorIsolationSendabilityProfile(*fn);
    FinalizeTaskRuntimeCancellationProfile(*fn);
    FinalizeConcurrencyReplayRaceGuardProfile(*fn);
    FinalizeUnsafePointerExtensionProfile(*fn);
    FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
    return fn;
  }

#include "parse/objc3_parser_core_objc_declarations_function_parameters.inc"
#include "parse/objc3_parser_core_objc_declarations_function_return_suffixes.inc"
#include "parse/objc3_parser_core_objc_declarations_function_return_profiles.inc"

  bool ParseFunctionReturnType(FunctionDecl &fn) {
    fn.return_id_spelling = false;
    fn.return_class_spelling = false;
    fn.return_sel_spelling = false;
    fn.return_instancetype_spelling = false;
    fn.return_object_pointer_type_spelling = false;
    fn.return_object_pointer_type_name.clear();
    fn.return_typecheck_family_symbol.clear();
    fn.return_vector_spelling = false;
    fn.return_vector_base_spelling.clear();
    fn.return_vector_lane_count = 1;
    fn.has_return_generic_suffix = false;
    fn.return_generic_suffix_terminated = true;
    fn.return_generic_suffix_text.clear();
    fn.return_generic_line = 1;
    fn.return_generic_column = 1;
    fn.return_lightweight_generic_constraint_profile_is_normalized = false;
    fn.return_lightweight_generic_constraint_profile.clear();
    fn.return_nullability_flow_profile_is_normalized = false;
    fn.return_nullability_flow_profile.clear();
    fn.return_protocol_qualified_object_type_profile_is_normalized = false;
    fn.return_protocol_qualified_object_type_profile.clear();
    fn.return_variance_bridge_cast_profile_is_normalized = false;
    fn.return_variance_bridge_cast_profile.clear();
    fn.return_generic_metadata_abi_profile_is_normalized = false;
    fn.return_generic_metadata_abi_profile.clear();
    fn.return_module_import_graph_profile_is_normalized = false;
    fn.return_module_import_graph_profile.clear();
    fn.return_namespace_collision_shadowing_profile_is_normalized = false;
    fn.return_namespace_collision_shadowing_profile.clear();
    fn.return_public_private_api_partition_profile_is_normalized = false;
    fn.return_public_private_api_partition_profile.clear();
    fn.return_incremental_module_cache_invalidation_profile_is_normalized = false;
    fn.return_incremental_module_cache_invalidation_profile.clear();
    fn.return_cross_module_conformance_profile_is_normalized = false;
    fn.return_cross_module_conformance_profile.clear();
    fn.has_return_pointer_declarator = false;
    fn.return_pointer_declarator_depth = 0;
    fn.return_pointer_declarator_tokens.clear();
    fn.return_nullability_suffix_tokens.clear();
    fn.has_return_ownership_qualifier = false;
    fn.return_ownership_qualifier_spelling.clear();
    fn.return_ownership_qualifier_symbol.clear();
    fn.return_ownership_qualifier_tokens.clear();
    fn.return_ownership_insert_retain = false;
    fn.return_ownership_insert_release = false;
    fn.return_ownership_insert_autorelease = false;
    fn.return_ownership_operation_profile.clear();
    fn.return_ownership_is_weak_reference = false;
    fn.return_ownership_is_unowned_reference = false;
    fn.return_ownership_is_unowned_safe_reference = false;
    fn.return_ownership_lifetime_profile.clear();
    fn.return_ownership_runtime_hook_profile.clear();
    fn.return_ownership_arc_diagnostic_candidate = false;
    fn.return_ownership_arc_fixit_available = false;
    fn.return_ownership_arc_diagnostic_profile.clear();
    fn.return_ownership_arc_fixit_hint.clear();
    fn.return_borrowed_pointer_qualified = false;

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in function return type annotation"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in function return type annotation"
                                                  : "unexpected qualifier 'async' in function return type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      fn.has_return_ownership_qualifier = true;
      fn.return_ownership_qualifier_spelling = qualifier.text;
      fn.return_ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }
    if (At(TokenKind::Identifier) && IsBorrowedQualifierSpelling(Peek().text)) {
      Advance();
      fn.return_borrowed_pointer_qualified = true;
    }

    if (Match(TokenKind::KwI32)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwVoid)) {
      fn.return_type = ValueType::Void;
    } else if (Match(TokenKind::KwId)) {
      fn.return_type = ValueType::ObjCId;
      fn.return_id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      fn.return_type = ValueType::ObjCClass;
      fn.return_class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      fn.return_type = ValueType::ObjCSel;
      fn.return_sel_spelling = true;
    } else if (Match(TokenKind::KwProtocol)) {
      fn.return_type = ValueType::ObjCProtocol;
    } else if (Match(TokenKind::KwInstancetype)) {
      fn.return_type = ValueType::ObjCInstancetype;
      fn.return_instancetype_spelling = true;
    } else if (AtRemovedOptionalTemplateAlias()) {
      return RejectRemovedOptionalTemplateAlias(Peek());
    } else {
      if (At(TokenKind::Identifier)) {
        const Token type_token = Advance();
        ValueType vector_type = ValueType::Unknown;
        std::string vector_base_spelling;
        unsigned vector_lane_count = 1;
        if (TryParseVectorTypeSpelling(type_token, vector_type, vector_base_spelling, vector_lane_count)) {
          fn.return_type = vector_type;
          fn.return_vector_spelling = true;
          fn.return_vector_base_spelling = vector_base_spelling;
          fn.return_vector_lane_count = vector_lane_count;
        } else {
          fn.return_type = ValueType::ObjCObjectPtr;
          fn.return_object_pointer_type_spelling = true;
          fn.return_object_pointer_type_name = type_token.text;
        }
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P114",
                     "expected function return type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', 'void', 'id', "
                     "'Class', 'SEL', 'Protocol', 'instancetype', object pointer spelling, ownership qualifiers "
                     "'__strong/__weak/__autoreleasing/__unsafe_unretained', or vector forms "
                     "'i32x2/i32x4/i32x8/i32x16' and 'boolx2/boolx4/boolx8/boolx16'"));
        return false;
      }
    }

    fn.return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(fn);

    if (!ParseFunctionReturnTypeSuffix(fn)) {
      return false;
    }

    FinalizeFunctionReturnTypeProfiles(fn);

    return true;
  }


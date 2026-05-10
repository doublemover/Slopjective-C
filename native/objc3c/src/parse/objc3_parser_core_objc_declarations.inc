  bool ParseParameterType(FuncParam &param) {
    param.vector_spelling = false;
    param.vector_base_spelling.clear();
    param.vector_lane_count = 1;
    param.id_spelling = false;
    param.class_spelling = false;
    param.sel_spelling = false;
    param.instancetype_spelling = false;
    param.object_pointer_type_spelling = false;
    param.object_pointer_type_name.clear();
    param.typecheck_family_symbol.clear();
    param.has_generic_suffix = false;
    param.generic_suffix_terminated = true;
    param.generic_suffix_text.clear();
    param.generic_line = 1;
    param.generic_column = 1;
    param.lightweight_generic_constraint_profile_is_normalized = false;
    param.lightweight_generic_constraint_profile.clear();
    param.nullability_flow_profile_is_normalized = false;
    param.nullability_flow_profile.clear();
    param.protocol_qualified_object_type_profile_is_normalized = false;
    param.protocol_qualified_object_type_profile.clear();
    param.variance_bridge_cast_profile_is_normalized = false;
    param.variance_bridge_cast_profile.clear();
    param.generic_metadata_abi_profile_is_normalized = false;
    param.generic_metadata_abi_profile.clear();
    param.module_import_graph_profile_is_normalized = false;
    param.module_import_graph_profile.clear();
    param.namespace_collision_shadowing_profile_is_normalized = false;
    param.namespace_collision_shadowing_profile.clear();
    param.public_private_api_partition_profile_is_normalized = false;
    param.public_private_api_partition_profile.clear();
    param.incremental_module_cache_invalidation_profile_is_normalized = false;
    param.incremental_module_cache_invalidation_profile.clear();
    param.cross_module_conformance_profile_is_normalized = false;
    param.cross_module_conformance_profile.clear();
    param.has_pointer_declarator = false;
    param.pointer_declarator_depth = 0;
    param.pointer_declarator_tokens.clear();
    param.nullability_suffix_tokens.clear();
    param.has_ownership_qualifier = false;
    param.ownership_qualifier_spelling.clear();
    param.ownership_qualifier_symbol.clear();
    param.ownership_qualifier_tokens.clear();
    param.ownership_insert_retain = false;
    param.ownership_insert_release = false;
    param.ownership_insert_autorelease = false;
    param.ownership_operation_profile.clear();
    param.ownership_is_weak_reference = false;
    param.ownership_is_unowned_reference = false;
    param.ownership_is_unowned_safe_reference = false;
    param.ownership_lifetime_profile.clear();
    param.ownership_runtime_hook_profile.clear();
    param.ownership_arc_diagnostic_candidate = false;
    param.ownership_arc_fixit_available = false;
    param.ownership_arc_diagnostic_profile.clear();
    param.ownership_arc_fixit_hint.clear();
    param.borrowed_pointer_qualified = false;
    param.borrowed_pointer_profile.clear();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in parameter type annotation"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in parameter type annotation"
                                                  : "unexpected qualifier 'async' in parameter type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      param.has_ownership_qualifier = true;
      param.ownership_qualifier_spelling = qualifier.text;
      param.ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }
    if (At(TokenKind::Identifier) && IsBorrowedQualifierSpelling(Peek().text)) {
      Advance();

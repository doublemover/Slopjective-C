  }

  template <typename TCallableDecl>
  bool ParseStatusCodeBridgeAttributePayload(TCallableDecl &decl,
                                             const Token &attribute_token) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P271",
          "missing '(' after objc_status_code attribute"));
      return false;
    }

    bool saw_success = false;
    bool saw_error_type = false;
    bool saw_mapping = false;
    while (!At(TokenKind::Eof) && !At(TokenKind::RParen)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P272",
                     "invalid objc_status_code clause label"));
        return false;
      }
      const Token clause = Advance();
      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P273",
            "missing ':' after objc_status_code clause label"));
        return false;
      }
      const std::string value_text = ParseAttributeArgumentText();
      if (value_text.empty()) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P274",
                     "objc_status_code clause value must not be empty"));
        return false;
      }

      if (clause.text == "success") {
        if (saw_success) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P275",
              "duplicate objc_status_code success clause"));
          return false;
        }
        decl.objc_status_code_success_literal = value_text;
        saw_success = true;
      } else if (clause.text == "error_type") {
        if (saw_error_type) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P276",
              "duplicate objc_status_code error_type clause"));
          return false;
        }
        decl.objc_status_code_error_type_spelling = value_text;
        saw_error_type = true;
      } else if (clause.text == "mapping") {
        if (saw_mapping) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P277",
              "duplicate objc_status_code mapping clause"));
          return false;
        }
        decl.objc_status_code_mapping_symbol = value_text;
        saw_mapping = true;
      } else {
        diagnostics_.push_back(MakeDiag(
            clause.line, clause.column, "O3P278",
            "unsupported objc_status_code clause '" + clause.text + "'"));
        return false;
      }

      if (At(TokenKind::Comma)) {
        Advance();
      } else {
        break;
      }
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P279",
          "missing ')' after objc_status_code attribute payload"));
      return false;
    }

    if (!saw_success || !saw_error_type || !saw_mapping) {
      diagnostics_.push_back(MakeDiag(
          attribute_token.line, attribute_token.column, "O3P280",
          "objc_status_code requires success, error_type, and mapping clauses"));
      return false;
    }

    decl.objc_status_code_declared = true;
    return true;
  }

  template <typename TCallableDecl>
  bool ParseRetainableCFamilyCallableAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    const auto record_no_payload =
        [this, &decl, &attribute_name]() {
          decl.retainable_c_family_callable_attributes.push_back(
              attribute_name.text);
          decl.retainable_c_family_profile_is_normalized = true;
          decl.retainable_c_family_profile =
              BuildRetainableCFamilyCallableProfile(
                  decl.retainable_c_family_callable_attributes,
                  decl.retainable_c_family_names);
        };
    const auto record_family_payload =
        [this, &decl, &attribute_name]() -> bool {
          if (!Match(TokenKind::LParen)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(
                token.line, token.column, "O3P327",
                "missing '(' after retainable C-family callable attribute"));
            return false;
          }
          if (!At(TokenKind::Identifier)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(
                token.line, token.column, "O3P328",
                "retainable C-family callable attribute requires family identifier"));
            return false;
          }
          decl.retainable_c_family_callable_attributes.push_back(
              attribute_name.text);
          decl.retainable_c_family_names.push_back(Advance().text);
          if (!Match(TokenKind::RParen)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(
                token.line, token.column, "O3P329",
                "missing ')' after retainable C-family attribute payload"));
            return false;
          }
          decl.retainable_c_family_profile_is_normalized = true;
          decl.retainable_c_family_profile =
              BuildRetainableCFamilyCallableProfile(
                  decl.retainable_c_family_callable_attributes,
                  decl.retainable_c_family_names);
          return true;
        };

    if (attribute_name.text == "objc_family_retain" ||
        attribute_name.text == "objc_family_release" ||
        attribute_name.text == "objc_family_autorelease") {
      return record_family_payload();
    }
    if (attribute_name.text == "os_returns_retained" ||
        attribute_name.text == "os_returns_not_retained" ||
        attribute_name.text == "os_consumed" ||
        attribute_name.text == "cf_returns_retained" ||
        attribute_name.text == "cf_returns_not_retained" ||
        attribute_name.text == "cf_consumed" ||
        attribute_name.text == "ns_returns_retained" ||
        attribute_name.text == "ns_returns_not_retained" ||
        attribute_name.text == "ns_consumed") {
      record_no_payload();
      return true;
    }
    return false;
  }

  template <typename TCallableDecl>
  bool ParseDispatchIntentCallableAttribute(TCallableDecl &decl,
                                            const Token &attribute_name) {
    auto record_duplicate =
        [&](const char *code, const std::string &message) {
          diagnostics_.push_back(
              MakeDiag(attribute_name.line, attribute_name.column, code,
                       message));
          return false;
        };

    if (attribute_name.text == "objc_direct") {
      if (decl.objc_direct_declared) {
        return record_duplicate("O3P330", "duplicate objc_direct attribute");
      }
      decl.objc_direct_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_final") {
      if (decl.objc_final_declared) {
        return record_duplicate("O3P331", "duplicate objc_final attribute");
      }
      decl.objc_final_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_dynamic") {
      if (decl.objc_dynamic_declared) {
        return record_duplicate("O3P332", "duplicate objc_dynamic attribute");
      }
      decl.objc_dynamic_declared = true;
      return true;
    }
    return false;
  }

  bool ParseSingleContainerDispatchAttribute(Objc3InterfaceDecl &decl) {
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P333",
          "invalid Objective-C container attribute name"));
      return false;
    }

    const Token attribute_name = Advance();
    auto record_duplicate =
        [&](const char *code, const std::string &message) {
          diagnostics_.push_back(
              MakeDiag(attribute_name.line, attribute_name.column, code,
                       message));
          return false;
        };

    if (attribute_name.text == "objc_direct_members") {
      if (decl.objc_direct_members_declared) {
        return record_duplicate("O3P334",
                                "duplicate objc_direct_members attribute");
      }
      decl.objc_direct_members_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_derive") {
      if (decl.objc_derive_declared) {
        return record_duplicate("O3P345", "duplicate objc_derive attribute");
      }
      // source-closure anchor: derive admission remains parser-owned
      // container attribute handling on top of the existing identifier stream.
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_derive",
                                            decl.objc_derive_name)) {
        return false;
      }
      decl.objc_derive_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_final") {
      if (decl.objc_final_declared) {
        return record_duplicate("O3P335", "duplicate objc_final attribute");
      }
      decl.objc_final_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_sealed") {
      if (decl.objc_sealed_declared) {
        return record_duplicate("O3P336", "duplicate objc_sealed attribute");
      }
      decl.objc_sealed_declared = true;
      return true;
    }

    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, "O3P337",
        "unsupported Objective-C container attribute '" + attribute_name.text +
            "'"));
    return false;
  }

  bool ParseOptionalContainerDispatchAttributes(Objc3InterfaceDecl &decl) {
    while (AtIdentifierText("__attribute__")) {
      Advance();
      if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P338",
            "malformed __attribute__ Objective-C container annotation"));
        return false;
      }

      do {
        if (!ParseSingleContainerDispatchAttribute(decl)) {
          return false;
        }
      } while (Match(TokenKind::Comma));

      if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P339",
            "missing '))' after Objective-C container attribute list"));
        return false;
      }
    }
    return true;
  }

  template <typename TCallableDecl>
  bool ParseSingleCallableBridgeAttribute(TCallableDecl &decl) {
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P281",
                   "invalid callable attribute name"));
      return false;
    }
    const Token attribute_name = Advance();
    if (attribute_name.text == "objc_nserror") {
      if (decl.objc_nserror_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P282",
                     "duplicate objc_nserror attribute"));
        return false;
      }
      decl.objc_nserror_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_status_code") {
      if (decl.objc_status_code_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P283",
                     "duplicate objc_status_code attribute"));
        return false;
      }
      return ParseStatusCodeBridgeAttributePayload(decl, attribute_name);
    }
    if (attribute_name.text == "objc_executor") {
      if (decl.executor_affinity_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P294",
                     "duplicate objc_executor attribute"));
        return false;
      }
      return ParseExecutorAttributePayload(decl, attribute_name);
    }
    if (attribute_name.text == "objc_macro") {
      if (decl.objc_macro_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P346",
                     "duplicate objc_macro attribute"));
        return false;
      }
      // source-closure anchor: macro admission remains parser-owned
      // callable attribute handling without claiming expansion/runtime behavior.
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_macro",
                                            decl.objc_macro_name)) {
        return false;
      }
      decl.objc_macro_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_package") {
      if (decl.objc_macro_package_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P347",
                     "duplicate objc_macro_package attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_macro_package",
                                            decl.objc_macro_package_name)) {
        return false;
      }
      decl.objc_macro_package_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_provenance") {
      if (decl.objc_macro_provenance_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P348",
                     "duplicate objc_macro_provenance attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_provenance",
              decl.objc_macro_provenance_name)) {
        return false;
      }
      decl.objc_macro_provenance_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_cache_key") {
      if (decl.objc_macro_cache_key_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P367",
                     "duplicate objc_macro_cache_key attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_cache_key",
              decl.objc_macro_cache_key_name)) {
        return false;
      }
      decl.objc_macro_cache_key_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_sandbox") {
      if (decl.objc_macro_sandbox_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P368",
                     "duplicate objc_macro_sandbox attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_macro_sandbox",
                                            decl.objc_macro_sandbox_name)) {
        return false;
      }
      decl.objc_macro_sandbox_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_foreign") {
      if (decl.objc_foreign_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P349",
                     "duplicate objc_foreign attribute"));
        return false;
      }
      // source-closure anchor: base foreign declaration markers stay
      // parser-owned callable attributes until later interop sema/lowering
      // work lands.
      decl.objc_foreign_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_import_module") {
      if (decl.objc_import_module_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P350",
                     "duplicate objc_import_module attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_import_module",
                                            decl.objc_import_module_name)) {
        return false;
      }
      decl.objc_import_module_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_swift_name") {
      if (decl.objc_swift_name_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P351",
                     "duplicate objc_swift_name attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_swift_name",
                                            decl.objc_swift_name)) {
        return false;
      }
      decl.objc_swift_name_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_swift_private") {
      if (decl.objc_swift_private_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P352",
                     "duplicate objc_swift_private attribute"));
        return false;
      }
      decl.objc_swift_private_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_cxx_name") {
      if (decl.objc_cxx_name_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P353",
                     "duplicate objc_cxx_name attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_cxx_name",
                                            decl.objc_cxx_name)) {
        return false;
      }
      decl.objc_cxx_name_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_header_name") {
      if (decl.objc_header_name_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P354",
                     "duplicate objc_header_name attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_header_name",
                                            decl.objc_header_name)) {
        return false;
      }
      decl.objc_header_name_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_export_header") {
      if (decl.objc_export_header_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P356",
                     "duplicate objc_export_header attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_export_header",
                                            decl.objc_export_header_name)) {
        return false;
      }
      decl.objc_export_header_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_abi_align") {
      if (decl.objc_abi_align_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P357",
                     "duplicate objc_abi_align attribute"));
        return false;
      }
      return ParseAbiAlignmentAttributePayload(decl, attribute_name);
    }
    if (attribute_name.text == "objc_foreign_type") {
      if (decl.objc_foreign_type_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P358",
                     "duplicate objc_foreign_type attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_foreign_type",
                                            decl.objc_foreign_type_name)) {
        return false;
      }
      decl.objc_foreign_type_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_mixed_image") {
      if (decl.objc_mixed_image_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P359",
                     "duplicate objc_mixed_image attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_mixed_image",
                                            decl.objc_mixed_image_name)) {
        return false;
      }
      decl.objc_mixed_image_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_package_entry") {
      if (decl.objc_package_entry_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P360",
                     "duplicate objc_package_entry attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(attribute_name, "objc_package_entry",
                                            decl.objc_package_entry_name)) {
        return false;
      }
      decl.objc_package_entry_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_nonisolated") {
      // source-surface anchor: nonisolated actor-member admission is
      // parser-owned callable attribute handling, not a standalone keyword.
      if (decl.objc_nonisolated_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P295",
                     "duplicate objc_nonisolated attribute"));
        return false;
      }
      decl.objc_nonisolated_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_returns_borrowed") {
      if (decl.objc_returns_borrowed_declared) {
        diagnostics_.push_back(
            MakeDiag(attribute_name.line, attribute_name.column, "O3P312",
                     "duplicate objc_returns_borrowed attribute"));
        return false;
      }
      return ParseReturnsBorrowedAttributePayload(decl);
    }
    if (ParseRetainableCFamilyCallableAttribute(decl, attribute_name)) {
      return true;
    }
    if (ParseDispatchIntentCallableAttribute(decl, attribute_name)) {
      return true;
    }

    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, "O3P284",
        "unsupported callable attribute '" + attribute_name.text + "'"));
    return false;
  }

  template <typename TCallableDecl>
  bool ParseOptionalCallableBridgeAttributes(TCallableDecl &decl) {
    while (AtIdentifierText("__attribute__")) {
      Advance();
      if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P285",
            "malformed __attribute__ callable annotation"));
        return false;
      }

      do {
        if (!ParseSingleCallableBridgeAttribute(decl)) {
          return false;
        }
      } while (Match(TokenKind::Comma));

      if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P286",
            "missing '))' after callable attribute list"));
        return false;
      }
    }
    return true;
  }

  template <typename TCallableDecl>
  void FinalizeErrorBridgeMarkerProfile(TCallableDecl &decl) {
    decl.objc_nserror_attribute_sites = decl.objc_nserror_declared ? 1u : 0u;
    decl.objc_status_code_attribute_sites =
        decl.objc_status_code_declared ? 1u : 0u;
    decl.status_code_success_clause_sites =
        decl.objc_status_code_success_literal.empty() ? 0u : 1u;
    decl.status_code_error_type_clause_sites =
        decl.objc_status_code_error_type_spelling.empty() ? 0u : 1u;
    decl.status_code_mapping_clause_sites =
        decl.objc_status_code_mapping_symbol.empty() ? 0u : 1u;
    decl.error_bridge_marker_contract_violation_sites = 0u;
    if (decl.objc_nserror_declared && decl.objc_status_code_declared) {
      decl.error_bridge_marker_contract_violation_sites += 1u;
    }
    if (decl.objc_status_code_declared &&
        (decl.status_code_success_clause_sites != 1u ||
         decl.status_code_error_type_clause_sites != 1u ||
         decl.status_code_mapping_clause_sites != 1u)) {
      decl.error_bridge_marker_contract_violation_sites += 1u;
    }
    decl.error_bridge_marker_profile = std::string("error-bridge-markers:objc_nserror=") +
                                       (decl.objc_nserror_declared ? "true" : "false") +
                                       ";objc_status_code=" +
                                       (decl.objc_status_code_declared ? "true" : "false") +
                                       ";success=" + decl.objc_status_code_success_literal +
                                       ";error_type=" +
                                       decl.objc_status_code_error_type_spelling +
                                       ";mapping=" +
                                       decl.objc_status_code_mapping_symbol +
                                       ";contract_violation_sites=" +
                                       std::to_string(
                                           decl.error_bridge_marker_contract_violation_sites);
    decl.error_bridge_marker_profile_is_normalized =
        decl.error_bridge_marker_contract_violation_sites == 0u &&
        (!decl.objc_status_code_declared ||
         (decl.status_code_success_clause_sites == 1u &&
          decl.status_code_error_type_clause_sites == 1u &&
          decl.status_code_mapping_clause_sites == 1u));
  }

  void FinalizeThrowsDeclarationProfile(FunctionDecl &fn, bool has_return_annotation) {
    fn.throws_declaration_profile = BuildThrowsDeclarationProfile(
        fn.throws_declared,
        has_return_annotation,
        fn.is_prototype,
        !fn.is_prototype,
        false,
        false,
        fn.params.size(),
        0u);
    fn.throws_declaration_profile_is_normalized = IsThrowsDeclarationProfileNormalized(
        fn.is_prototype,
        !fn.is_prototype,
        false,
        0u);
  }

  void FinalizeThrowsDeclarationProfile(Objc3MethodDecl &method) {
    method.throws_declaration_profile = BuildThrowsDeclarationProfile(
        method.throws_declared,
        true,
        !method.has_body,
        method.has_body,
        true,
        method.is_class_method,
        method.params.size(),
        method.selector_pieces.size());
    method.throws_declaration_profile_is_normalized = IsThrowsDeclarationProfileNormalized(
        !method.has_body,
        method.has_body,
        true,
        method.selector_pieces.size());
  }

  void FinalizeResultLikeProfile(FunctionDecl &fn) {
    const Objc3ResultLikeProfile profile = BuildResultLikeProfileFromBody(fn.body);
    fn.result_like_sites = profile.result_like_sites;
    fn.result_success_sites = profile.result_success_sites;
    fn.result_failure_sites = profile.result_failure_sites;
    fn.result_branch_sites = profile.result_branch_sites;
    fn.result_payload_sites = profile.result_payload_sites;
    fn.result_normalized_sites = profile.normalized_sites;
    fn.result_branch_merge_sites = profile.branch_merge_sites;
    fn.result_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_result_like_lowering_handoff =
        profile.deterministic_result_like_lowering_handoff;
    fn.result_like_profile = BuildResultLikeProfile(
        fn.result_like_sites,
        fn.result_success_sites,
        fn.result_failure_sites,
        fn.result_branch_sites,
        fn.result_payload_sites,
        fn.result_normalized_sites,
        fn.result_branch_merge_sites,
        fn.result_contract_violation_sites,
        fn.deterministic_result_like_lowering_handoff);
    fn.result_like_profile_is_normalized = IsResultLikeProfileNormalized(
        fn.result_like_sites,
        fn.result_success_sites,
        fn.result_failure_sites,
        fn.result_branch_sites,
        fn.result_payload_sites,
        fn.result_normalized_sites,
        fn.result_branch_merge_sites,
        fn.result_contract_violation_sites);
  }

  void FinalizeResultLikeProfile(Objc3MethodDecl &method) {
    const Objc3ResultLikeProfile profile = BuildResultLikeProfileFromOpaqueBody(method.has_body);
    method.result_like_sites = profile.result_like_sites;
    method.result_success_sites = profile.result_success_sites;
    method.result_failure_sites = profile.result_failure_sites;
    method.result_branch_sites = profile.result_branch_sites;
    method.result_payload_sites = profile.result_payload_sites;
    method.result_normalized_sites = profile.normalized_sites;
    method.result_branch_merge_sites = profile.branch_merge_sites;
    method.result_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_result_like_lowering_handoff =
        profile.deterministic_result_like_lowering_handoff;
    method.result_like_profile = BuildResultLikeProfile(
        method.result_like_sites,
        method.result_success_sites,
        method.result_failure_sites,
        method.result_branch_sites,
        method.result_payload_sites,
        method.result_normalized_sites,
        method.result_branch_merge_sites,
        method.result_contract_violation_sites,
        method.deterministic_result_like_lowering_handoff);
    method.result_like_profile_is_normalized = IsResultLikeProfileNormalized(
        method.result_like_sites,
        method.result_success_sites,
        method.result_failure_sites,
        method.result_branch_sites,
        method.result_payload_sites,
        method.result_normalized_sites,
        method.result_branch_merge_sites,
        method.result_contract_violation_sites);
  }

  void FinalizeNSErrorBridgingProfile(FunctionDecl &fn) {
    const Objc3NSErrorBridgingProfile profile = BuildNSErrorBridgingProfileFromFunction(fn);
    fn.ns_error_bridging_sites = profile.ns_error_bridging_sites;
    fn.ns_error_parameter_sites = profile.ns_error_parameter_sites;
    fn.ns_error_out_parameter_sites = profile.ns_error_out_parameter_sites;
    fn.ns_error_bridge_path_sites = profile.ns_error_bridge_path_sites;
    fn.failable_call_sites = profile.failable_call_sites;
    fn.ns_error_bridging_normalized_sites = profile.normalized_sites;
    fn.ns_error_bridge_boundary_sites = profile.bridge_boundary_sites;
    fn.ns_error_bridging_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_ns_error_bridging_lowering_handoff =
        profile.deterministic_ns_error_bridging_lowering_handoff;
    fn.ns_error_bridging_profile = BuildNSErrorBridgingProfile(
        fn.ns_error_bridging_sites,
        fn.ns_error_parameter_sites,
        fn.ns_error_out_parameter_sites,
        fn.ns_error_bridge_path_sites,
        fn.failable_call_sites,
        fn.ns_error_bridging_normalized_sites,
        fn.ns_error_bridge_boundary_sites,
        fn.ns_error_bridging_contract_violation_sites,
        fn.deterministic_ns_error_bridging_lowering_handoff);
    fn.ns_error_bridging_profile_is_normalized = IsNSErrorBridgingProfileNormalized(
        fn.ns_error_bridging_sites,
        fn.ns_error_parameter_sites,
        fn.ns_error_out_parameter_sites,
        fn.ns_error_bridge_path_sites,
        fn.failable_call_sites,
        fn.ns_error_bridging_normalized_sites,
        fn.ns_error_bridge_boundary_sites,
        fn.ns_error_bridging_contract_violation_sites);
  }

  void FinalizeNSErrorBridgingProfile(Objc3MethodDecl &method) {
    const Objc3NSErrorBridgingProfile profile = BuildNSErrorBridgingProfileFromOpaqueBody(method);
    method.ns_error_bridging_sites = profile.ns_error_bridging_sites;
    method.ns_error_parameter_sites = profile.ns_error_parameter_sites;
    method.ns_error_out_parameter_sites = profile.ns_error_out_parameter_sites;
    method.ns_error_bridge_path_sites = profile.ns_error_bridge_path_sites;
    method.failable_call_sites = profile.failable_call_sites;
    method.ns_error_bridging_normalized_sites = profile.normalized_sites;
    method.ns_error_bridge_boundary_sites = profile.bridge_boundary_sites;
    method.ns_error_bridging_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_ns_error_bridging_lowering_handoff =
        profile.deterministic_ns_error_bridging_lowering_handoff;
    method.ns_error_bridging_profile = BuildNSErrorBridgingProfile(
        method.ns_error_bridging_sites,
        method.ns_error_parameter_sites,
        method.ns_error_out_parameter_sites,
        method.ns_error_bridge_path_sites,
        method.failable_call_sites,
        method.ns_error_bridging_normalized_sites,
        method.ns_error_bridge_boundary_sites,
        method.ns_error_bridging_contract_violation_sites,
        method.deterministic_ns_error_bridging_lowering_handoff);
    method.ns_error_bridging_profile_is_normalized = IsNSErrorBridgingProfileNormalized(
        method.ns_error_bridging_sites,
        method.ns_error_parameter_sites,
        method.ns_error_out_parameter_sites,
        method.ns_error_bridge_path_sites,
        method.failable_call_sites,
        method.ns_error_bridging_normalized_sites,
        method.ns_error_bridge_boundary_sites,
        method.ns_error_bridging_contract_violation_sites);
  }

  void FinalizeUnwindCleanupProfile(FunctionDecl &fn) {
    const Objc3UnwindCleanupProfile profile = BuildUnwindCleanupProfileFromFunction(fn);
    fn.unwind_cleanup_sites = profile.unwind_cleanup_sites;
    fn.exceptional_exit_sites = profile.exceptional_exit_sites;
    fn.cleanup_action_sites = profile.cleanup_action_sites;
    fn.cleanup_scope_sites = profile.cleanup_scope_sites;
    fn.cleanup_resume_sites = profile.cleanup_resume_sites;
    fn.unwind_cleanup_normalized_sites = profile.normalized_sites;
    fn.unwind_cleanup_fail_closed_sites = profile.fail_closed_sites;
    fn.unwind_cleanup_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_unwind_cleanup_handoff =
        profile.deterministic_unwind_cleanup_handoff;
    fn.unwind_cleanup_profile = BuildUnwindCleanupProfile(
        fn.unwind_cleanup_sites,
        fn.exceptional_exit_sites,
        fn.cleanup_action_sites,
        fn.cleanup_scope_sites,
        fn.cleanup_resume_sites,
        fn.unwind_cleanup_normalized_sites,
        fn.unwind_cleanup_fail_closed_sites,
        fn.unwind_cleanup_contract_violation_sites,
        fn.deterministic_unwind_cleanup_handoff);
    fn.unwind_cleanup_profile_is_normalized = IsUnwindCleanupProfileNormalized(
        fn.unwind_cleanup_sites,
        fn.exceptional_exit_sites,
        fn.cleanup_action_sites,
        fn.cleanup_scope_sites,
        fn.cleanup_resume_sites,
        fn.unwind_cleanup_normalized_sites,
        fn.unwind_cleanup_fail_closed_sites,
        fn.unwind_cleanup_contract_violation_sites);
  }

  void FinalizeUnwindCleanupProfile(Objc3MethodDecl &method) {
    const Objc3UnwindCleanupProfile profile = BuildUnwindCleanupProfileFromOpaqueBody(method);
    method.unwind_cleanup_sites = profile.unwind_cleanup_sites;
    method.exceptional_exit_sites = profile.exceptional_exit_sites;
    method.cleanup_action_sites = profile.cleanup_action_sites;
    method.cleanup_scope_sites = profile.cleanup_scope_sites;
    method.cleanup_resume_sites = profile.cleanup_resume_sites;
    method.unwind_cleanup_normalized_sites = profile.normalized_sites;
    method.unwind_cleanup_fail_closed_sites = profile.fail_closed_sites;
    method.unwind_cleanup_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_unwind_cleanup_handoff =
        profile.deterministic_unwind_cleanup_handoff;
    method.unwind_cleanup_profile = BuildUnwindCleanupProfile(
        method.unwind_cleanup_sites,
        method.exceptional_exit_sites,
        method.cleanup_action_sites,
        method.cleanup_scope_sites,
        method.cleanup_resume_sites,
        method.unwind_cleanup_normalized_sites,
        method.unwind_cleanup_fail_closed_sites,
        method.unwind_cleanup_contract_violation_sites,
        method.deterministic_unwind_cleanup_handoff);
    method.unwind_cleanup_profile_is_normalized = IsUnwindCleanupProfileNormalized(
        method.unwind_cleanup_sites,
        method.exceptional_exit_sites,
        method.cleanup_action_sites,
        method.cleanup_scope_sites,
        method.cleanup_resume_sites,
        method.unwind_cleanup_normalized_sites,
        method.unwind_cleanup_fail_closed_sites,
        method.unwind_cleanup_contract_violation_sites);
  }

  void FinalizeErrorDiagnosticsRecoveryProfile(FunctionDecl &fn) {
    const Objc3ErrorDiagnosticsRecoveryProfile profile =
        BuildErrorDiagnosticsRecoveryProfileFromFunction(fn);
    fn.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;
    fn.diagnostic_emit_sites = profile.diagnostic_emit_sites;
    fn.recovery_anchor_sites = profile.recovery_anchor_sites;
    fn.recovery_boundary_sites = profile.recovery_boundary_sites;
    fn.fail_closed_diagnostic_sites = profile.fail_closed_diagnostic_sites;
    fn.error_diagnostics_recovery_normalized_sites = profile.normalized_sites;
    fn.error_diagnostics_recovery_gate_blocked_sites = profile.gate_blocked_sites;
    fn.error_diagnostics_recovery_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_error_diagnostics_recovery_handoff =
        profile.deterministic_error_diagnostics_recovery_handoff;
    fn.error_diagnostics_recovery_profile = BuildErrorDiagnosticsRecoveryProfile(
        fn.error_diagnostics_recovery_sites,
        fn.diagnostic_emit_sites,
        fn.recovery_anchor_sites,
        fn.recovery_boundary_sites,
        fn.fail_closed_diagnostic_sites,
        fn.error_diagnostics_recovery_normalized_sites,
        fn.error_diagnostics_recovery_gate_blocked_sites,
        fn.error_diagnostics_recovery_contract_violation_sites,
        fn.deterministic_error_diagnostics_recovery_handoff);
    fn.error_diagnostics_recovery_profile_is_normalized =
        IsErrorDiagnosticsRecoveryProfileNormalized(
            fn.error_diagnostics_recovery_sites,
            fn.diagnostic_emit_sites,
            fn.recovery_anchor_sites,
            fn.recovery_boundary_sites,
            fn.fail_closed_diagnostic_sites,
            fn.error_diagnostics_recovery_normalized_sites,
            fn.error_diagnostics_recovery_gate_blocked_sites,
            fn.error_diagnostics_recovery_contract_violation_sites);
  }

  void FinalizeErrorDiagnosticsRecoveryProfile(Objc3MethodDecl &method) {
    const Objc3ErrorDiagnosticsRecoveryProfile profile =
        BuildErrorDiagnosticsRecoveryProfileFromOpaqueBody(method);
    method.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;
    method.diagnostic_emit_sites = profile.diagnostic_emit_sites;
    method.recovery_anchor_sites = profile.recovery_anchor_sites;
    method.recovery_boundary_sites = profile.recovery_boundary_sites;
    method.fail_closed_diagnostic_sites = profile.fail_closed_diagnostic_sites;
    method.error_diagnostics_recovery_normalized_sites = profile.normalized_sites;
    method.error_diagnostics_recovery_gate_blocked_sites = profile.gate_blocked_sites;
    method.error_diagnostics_recovery_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_error_diagnostics_recovery_handoff =
        profile.deterministic_error_diagnostics_recovery_handoff;
    method.error_diagnostics_recovery_profile = BuildErrorDiagnosticsRecoveryProfile(
        method.error_diagnostics_recovery_sites,
        method.diagnostic_emit_sites,
        method.recovery_anchor_sites,
        method.recovery_boundary_sites,
        method.fail_closed_diagnostic_sites,
        method.error_diagnostics_recovery_normalized_sites,
        method.error_diagnostics_recovery_gate_blocked_sites,
        method.error_diagnostics_recovery_contract_violation_sites,
        method.deterministic_error_diagnostics_recovery_handoff);
    method.error_diagnostics_recovery_profile_is_normalized =
        IsErrorDiagnosticsRecoveryProfileNormalized(
            method.error_diagnostics_recovery_sites,
            method.diagnostic_emit_sites,
            method.recovery_anchor_sites,
            method.recovery_boundary_sites,
            method.fail_closed_diagnostic_sites,
            method.error_diagnostics_recovery_normalized_sites,
            method.error_diagnostics_recovery_gate_blocked_sites,
            method.error_diagnostics_recovery_contract_violation_sites);
  }

  void FinalizeAsyncContinuationProfile(FunctionDecl &fn) {
    const Objc3AsyncContinuationProfile profile =
        BuildAsyncContinuationProfileFromFunction(fn);
    fn.async_continuation_sites = profile.async_continuation_sites;
    fn.async_keyword_sites = profile.async_keyword_sites;
    fn.async_function_sites = profile.async_function_sites;
    fn.continuation_allocation_sites = profile.continuation_allocation_sites;
    fn.continuation_resume_sites = profile.continuation_resume_sites;
    fn.continuation_suspend_sites = profile.continuation_suspend_sites;
    fn.async_state_machine_sites = profile.async_state_machine_sites;
    fn.async_continuation_normalized_sites = profile.normalized_sites;
    fn.async_continuation_gate_blocked_sites = profile.gate_blocked_sites;
    fn.async_continuation_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_async_continuation_handoff =
        profile.deterministic_async_continuation_handoff;
    fn.async_continuation_profile = BuildAsyncContinuationProfile(
        fn.async_continuation_sites,
        fn.async_keyword_sites,
        fn.async_function_sites,
        fn.continuation_allocation_sites,
        fn.continuation_resume_sites,
        fn.continuation_suspend_sites,
        fn.async_state_machine_sites,
        fn.async_continuation_normalized_sites,
        fn.async_continuation_gate_blocked_sites,
        fn.async_continuation_contract_violation_sites,
        fn.deterministic_async_continuation_handoff);
    fn.async_continuation_profile_is_normalized =
        IsAsyncContinuationProfileNormalized(
            fn.async_continuation_sites,
            fn.async_keyword_sites,
            fn.async_function_sites,
            fn.continuation_allocation_sites,
            fn.continuation_resume_sites,
            fn.continuation_suspend_sites,
            fn.async_state_machine_sites,
            fn.async_continuation_normalized_sites,
            fn.async_continuation_gate_blocked_sites,
            fn.async_continuation_contract_violation_sites);
  }

  void FinalizeAsyncContinuationProfile(Objc3MethodDecl &method) {
    const Objc3AsyncContinuationProfile profile =
        BuildAsyncContinuationProfileFromOpaqueBody(method);
    method.async_continuation_sites = profile.async_continuation_sites;
    method.async_keyword_sites = profile.async_keyword_sites;
    method.async_function_sites = profile.async_function_sites;
    method.continuation_allocation_sites = profile.continuation_allocation_sites;
    method.continuation_resume_sites = profile.continuation_resume_sites;
    method.continuation_suspend_sites = profile.continuation_suspend_sites;
    method.async_state_machine_sites = profile.async_state_machine_sites;
    method.async_continuation_normalized_sites = profile.normalized_sites;
    method.async_continuation_gate_blocked_sites = profile.gate_blocked_sites;
    method.async_continuation_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_async_continuation_handoff =
        profile.deterministic_async_continuation_handoff;
    method.async_continuation_profile = BuildAsyncContinuationProfile(
        method.async_continuation_sites,
        method.async_keyword_sites,
        method.async_function_sites,
        method.continuation_allocation_sites,
        method.continuation_resume_sites,
        method.continuation_suspend_sites,
        method.async_state_machine_sites,
        method.async_continuation_normalized_sites,
        method.async_continuation_gate_blocked_sites,
        method.async_continuation_contract_violation_sites,
        method.deterministic_async_continuation_handoff);
    method.async_continuation_profile_is_normalized =
        IsAsyncContinuationProfileNormalized(
            method.async_continuation_sites,
            method.async_keyword_sites,
            method.async_function_sites,
            method.continuation_allocation_sites,
            method.continuation_resume_sites,
            method.continuation_suspend_sites,
            method.async_state_machine_sites,
            method.async_continuation_normalized_sites,
            method.async_continuation_gate_blocked_sites,
            method.async_continuation_contract_violation_sites);
  }

  void FinalizeAwaitSuspensionProfile(FunctionDecl &fn) {
    const Objc3AwaitSuspensionProfile profile =
        BuildAwaitSuspensionProfileFromFunction(fn);
    fn.await_suspension_sites = profile.await_suspension_sites;
    fn.await_keyword_sites = profile.await_keyword_sites;
    fn.await_suspension_point_sites = profile.await_suspension_point_sites;
    fn.await_resume_sites = profile.await_resume_sites;
    fn.await_state_machine_sites = profile.await_state_machine_sites;
    fn.await_continuation_sites = profile.await_continuation_sites;
    fn.await_suspension_normalized_sites = profile.normalized_sites;
    fn.await_suspension_gate_blocked_sites = profile.gate_blocked_sites;
    fn.await_suspension_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_await_suspension_handoff =
        profile.deterministic_await_suspension_handoff;
    fn.await_suspension_profile = BuildAwaitSuspensionProfile(
        fn.await_suspension_sites,
        fn.await_keyword_sites,
        fn.await_suspension_point_sites,
        fn.await_resume_sites,
        fn.await_state_machine_sites,
        fn.await_continuation_sites,
        fn.await_suspension_normalized_sites,
        fn.await_suspension_gate_blocked_sites,
        fn.await_suspension_contract_violation_sites,
        fn.deterministic_await_suspension_handoff);
    fn.await_suspension_profile_is_normalized = IsAwaitSuspensionProfileNormalized(
        fn.await_suspension_sites,
        fn.await_keyword_sites,
        fn.await_suspension_point_sites,
        fn.await_resume_sites,
        fn.await_state_machine_sites,
        fn.await_continuation_sites,
        fn.await_suspension_normalized_sites,
        fn.await_suspension_gate_blocked_sites,
        fn.await_suspension_contract_violation_sites);
  }

  void FinalizeAwaitSuspensionProfile(Objc3MethodDecl &method) {
    const Objc3AwaitSuspensionProfile profile =
        BuildAwaitSuspensionProfileFromOpaqueBody(method);
    method.await_suspension_sites = profile.await_suspension_sites;
    method.await_keyword_sites = profile.await_keyword_sites;
    method.await_suspension_point_sites = profile.await_suspension_point_sites;
    method.await_resume_sites = profile.await_resume_sites;
    method.await_state_machine_sites = profile.await_state_machine_sites;
    method.await_continuation_sites = profile.await_continuation_sites;
    method.await_suspension_normalized_sites = profile.normalized_sites;
    method.await_suspension_gate_blocked_sites = profile.gate_blocked_sites;
    method.await_suspension_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_await_suspension_handoff =
        profile.deterministic_await_suspension_handoff;
    method.await_suspension_profile = BuildAwaitSuspensionProfile(
        method.await_suspension_sites,
        method.await_keyword_sites,
        method.await_suspension_point_sites,
        method.await_resume_sites,
        method.await_state_machine_sites,
        method.await_continuation_sites,
        method.await_suspension_normalized_sites,
        method.await_suspension_gate_blocked_sites,
        method.await_suspension_contract_violation_sites,
        method.deterministic_await_suspension_handoff);
    method.await_suspension_profile_is_normalized = IsAwaitSuspensionProfileNormalized(
        method.await_suspension_sites,
        method.await_keyword_sites,
        method.await_suspension_point_sites,
        method.await_resume_sites,
        method.await_state_machine_sites,
        method.await_continuation_sites,
        method.await_suspension_normalized_sites,
        method.await_suspension_gate_blocked_sites,
        method.await_suspension_contract_violation_sites);
  }

  void FinalizeActorIsolationSendabilityProfile(FunctionDecl &fn) {
    const Objc3ActorIsolationSendabilityProfile profile =
        BuildActorIsolationSendabilityProfileFromFunction(fn);
    fn.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;
    fn.actor_isolation_decl_sites = profile.actor_isolation_decl_sites;
    fn.actor_hop_sites = profile.actor_hop_sites;
    fn.sendable_annotation_sites = profile.sendable_annotation_sites;
    fn.non_sendable_crossing_sites = profile.non_sendable_crossing_sites;
    fn.isolation_boundary_sites = profile.isolation_boundary_sites;
    fn.actor_isolation_sendability_normalized_sites = profile.normalized_sites;
    fn.actor_isolation_sendability_gate_blocked_sites = profile.gate_blocked_sites;
    fn.actor_isolation_sendability_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_actor_isolation_sendability_handoff =
        profile.deterministic_actor_isolation_sendability_handoff;
    fn.actor_isolation_sendability_profile = BuildActorIsolationSendabilityProfile(
        fn.actor_isolation_sendability_sites,
        fn.actor_isolation_decl_sites,
        fn.actor_hop_sites,
        fn.sendable_annotation_sites,
        fn.non_sendable_crossing_sites,
        fn.isolation_boundary_sites,
        fn.actor_isolation_sendability_normalized_sites,
        fn.actor_isolation_sendability_gate_blocked_sites,
        fn.actor_isolation_sendability_contract_violation_sites,
        fn.deterministic_actor_isolation_sendability_handoff);
    fn.actor_isolation_sendability_profile_is_normalized =
        IsActorIsolationSendabilityProfileNormalized(
            fn.actor_isolation_sendability_sites,
            fn.actor_isolation_decl_sites,
            fn.actor_hop_sites,
            fn.sendable_annotation_sites,
            fn.non_sendable_crossing_sites,
            fn.isolation_boundary_sites,
            fn.actor_isolation_sendability_normalized_sites,
            fn.actor_isolation_sendability_gate_blocked_sites,
            fn.actor_isolation_sendability_contract_violation_sites);
  }

  void FinalizeActorIsolationSendabilityProfile(Objc3MethodDecl &method) {
    const Objc3ActorIsolationSendabilityProfile profile =
        BuildActorIsolationSendabilityProfileFromOpaqueBody(method);
    method.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;
    method.actor_isolation_decl_sites = profile.actor_isolation_decl_sites;
    method.actor_hop_sites = profile.actor_hop_sites;
    method.sendable_annotation_sites = profile.sendable_annotation_sites;
    method.non_sendable_crossing_sites = profile.non_sendable_crossing_sites;
    method.isolation_boundary_sites = profile.isolation_boundary_sites;
    method.actor_isolation_sendability_normalized_sites = profile.normalized_sites;
    method.actor_isolation_sendability_gate_blocked_sites = profile.gate_blocked_sites;
    method.actor_isolation_sendability_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_actor_isolation_sendability_handoff =
        profile.deterministic_actor_isolation_sendability_handoff;
    method.actor_isolation_sendability_profile = BuildActorIsolationSendabilityProfile(
        method.actor_isolation_sendability_sites,
        method.actor_isolation_decl_sites,
        method.actor_hop_sites,
        method.sendable_annotation_sites,
        method.non_sendable_crossing_sites,
        method.isolation_boundary_sites,
        method.actor_isolation_sendability_normalized_sites,
        method.actor_isolation_sendability_gate_blocked_sites,
        method.actor_isolation_sendability_contract_violation_sites,
        method.deterministic_actor_isolation_sendability_handoff);
    method.actor_isolation_sendability_profile_is_normalized =
        IsActorIsolationSendabilityProfileNormalized(
            method.actor_isolation_sendability_sites,
            method.actor_isolation_decl_sites,
            method.actor_hop_sites,
            method.sendable_annotation_sites,
            method.non_sendable_crossing_sites,
            method.isolation_boundary_sites,
            method.actor_isolation_sendability_normalized_sites,
            method.actor_isolation_sendability_gate_blocked_sites,
            method.actor_isolation_sendability_contract_violation_sites);
  }

  void FinalizeTaskRuntimeCancellationProfile(FunctionDecl &fn) {
    const Objc3TaskRuntimeCancellationProfile profile =
        BuildTaskRuntimeCancellationProfileFromFunction(fn);
    fn.task_runtime_interop_sites = profile.task_runtime_interop_sites;
    fn.runtime_hook_sites = profile.runtime_hook_sites;
    fn.cancellation_check_sites = profile.cancellation_check_sites;
    fn.cancellation_handler_sites = profile.cancellation_handler_sites;
    fn.suspension_point_sites = profile.suspension_point_sites;
    fn.cancellation_propagation_sites = profile.cancellation_propagation_sites;
    fn.task_runtime_normalized_sites = profile.normalized_sites;
    fn.task_runtime_gate_blocked_sites = profile.gate_blocked_sites;
    fn.task_runtime_contract_violation_sites = profile.contract_violation_sites;
    fn.task_runtime_cancellation_normalized_sites = profile.normalized_sites;
    fn.task_runtime_cancellation_gate_blocked_sites =
        profile.gate_blocked_sites;
    fn.task_runtime_cancellation_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_task_runtime_cancellation_handoff =
        profile.deterministic_task_runtime_cancellation_handoff;
    fn.task_runtime_cancellation_profile = BuildTaskRuntimeCancellationProfile(
        fn.task_runtime_interop_sites,
        fn.runtime_hook_sites,
        fn.cancellation_check_sites,
        fn.cancellation_handler_sites,
        fn.suspension_point_sites,
        fn.cancellation_propagation_sites,
        fn.task_runtime_normalized_sites,
        fn.task_runtime_gate_blocked_sites,
        fn.task_runtime_contract_violation_sites,
        fn.deterministic_task_runtime_cancellation_handoff);
    fn.task_runtime_cancellation_profile_is_normalized =
        IsTaskRuntimeCancellationProfileNormalized(
            fn.task_runtime_interop_sites,
            fn.runtime_hook_sites,
            fn.cancellation_check_sites,
            fn.cancellation_handler_sites,
            fn.suspension_point_sites,
            fn.cancellation_propagation_sites,
            fn.task_runtime_normalized_sites,
            fn.task_runtime_gate_blocked_sites,
            fn.task_runtime_contract_violation_sites);
  }

  void FinalizeTaskRuntimeCancellationProfile(Objc3MethodDecl &method) {
    const Objc3TaskRuntimeCancellationProfile profile =
        BuildTaskRuntimeCancellationProfileFromOpaqueBody(method);
    method.task_runtime_interop_sites = profile.task_runtime_interop_sites;
    method.runtime_hook_sites = profile.runtime_hook_sites;
    method.cancellation_check_sites = profile.cancellation_check_sites;
    method.cancellation_handler_sites = profile.cancellation_handler_sites;
    method.suspension_point_sites = profile.suspension_point_sites;
    method.cancellation_propagation_sites = profile.cancellation_propagation_sites;
    method.task_runtime_normalized_sites = profile.normalized_sites;
    method.task_runtime_gate_blocked_sites = profile.gate_blocked_sites;
    method.task_runtime_contract_violation_sites = profile.contract_violation_sites;
    method.task_runtime_cancellation_normalized_sites = profile.normalized_sites;
    method.task_runtime_cancellation_gate_blocked_sites =
        profile.gate_blocked_sites;
    method.task_runtime_cancellation_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_task_runtime_cancellation_handoff =
        profile.deterministic_task_runtime_cancellation_handoff;
    method.task_runtime_cancellation_profile = BuildTaskRuntimeCancellationProfile(
        method.task_runtime_interop_sites,
        method.runtime_hook_sites,
        method.cancellation_check_sites,
        method.cancellation_handler_sites,
        method.suspension_point_sites,
        method.cancellation_propagation_sites,
        method.task_runtime_normalized_sites,
        method.task_runtime_gate_blocked_sites,
        method.task_runtime_contract_violation_sites,
        method.deterministic_task_runtime_cancellation_handoff);
    method.task_runtime_cancellation_profile_is_normalized =
        IsTaskRuntimeCancellationProfileNormalized(
            method.task_runtime_interop_sites,
            method.runtime_hook_sites,
            method.cancellation_check_sites,
            method.cancellation_handler_sites,
            method.suspension_point_sites,
            method.cancellation_propagation_sites,
            method.task_runtime_normalized_sites,
            method.task_runtime_gate_blocked_sites,
            method.task_runtime_contract_violation_sites);
  }

  void FinalizeConcurrencyReplayRaceGuardProfile(FunctionDecl &fn) {
    const Objc3ConcurrencyReplayRaceGuardProfile profile =
        BuildConcurrencyReplayRaceGuardProfileFromFunction(fn);
    fn.concurrency_replay_race_guard_sites = profile.concurrency_replay_race_guard_sites;
    fn.concurrency_replay_sites = profile.concurrency_replay_sites;
    fn.replay_proof_sites = profile.replay_proof_sites;
    fn.race_guard_sites = profile.race_guard_sites;
    fn.task_handoff_sites = profile.task_handoff_sites;
    fn.actor_isolation_sites = profile.actor_isolation_sites;
    fn.deterministic_schedule_sites = profile.deterministic_schedule_sites;
    fn.concurrency_replay_guard_blocked_sites = profile.guard_blocked_sites;
    fn.concurrency_replay_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_concurrency_replay_race_guard_handoff =
        profile.deterministic_concurrency_replay_race_guard_handoff;
    fn.concurrency_replay_race_guard_profile =
        BuildConcurrencyReplayRaceGuardProfile(
            fn.concurrency_replay_race_guard_sites,
            fn.concurrency_replay_sites,
            fn.replay_proof_sites,
            fn.race_guard_sites,
            fn.task_handoff_sites,
            fn.actor_isolation_sites,
            fn.deterministic_schedule_sites,
            fn.concurrency_replay_guard_blocked_sites,
            fn.concurrency_replay_contract_violation_sites,
            fn.deterministic_concurrency_replay_race_guard_handoff);
    fn.concurrency_replay_race_guard_profile_is_normalized =
        IsConcurrencyReplayRaceGuardProfileNormalized(
            fn.concurrency_replay_race_guard_sites,
            fn.concurrency_replay_sites,
            fn.replay_proof_sites,
            fn.race_guard_sites,
            fn.task_handoff_sites,
            fn.actor_isolation_sites,
            fn.deterministic_schedule_sites,
            fn.concurrency_replay_guard_blocked_sites,
            fn.concurrency_replay_contract_violation_sites);
  }

  void FinalizeConcurrencyReplayRaceGuardProfile(Objc3MethodDecl &method) {
    const Objc3ConcurrencyReplayRaceGuardProfile profile =
        BuildConcurrencyReplayRaceGuardProfileFromOpaqueBody(method);
    method.concurrency_replay_race_guard_sites =
        profile.concurrency_replay_race_guard_sites;
    method.concurrency_replay_sites = profile.concurrency_replay_sites;
    method.replay_proof_sites = profile.replay_proof_sites;
    method.race_guard_sites = profile.race_guard_sites;
    method.task_handoff_sites = profile.task_handoff_sites;
    method.actor_isolation_sites = profile.actor_isolation_sites;
    method.deterministic_schedule_sites = profile.deterministic_schedule_sites;
    method.concurrency_replay_guard_blocked_sites = profile.guard_blocked_sites;
    method.concurrency_replay_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_concurrency_replay_race_guard_handoff =
        profile.deterministic_concurrency_replay_race_guard_handoff;
    method.concurrency_replay_race_guard_profile =
        BuildConcurrencyReplayRaceGuardProfile(
            method.concurrency_replay_race_guard_sites,
            method.concurrency_replay_sites,
            method.replay_proof_sites,
            method.race_guard_sites,
            method.task_handoff_sites,
            method.actor_isolation_sites,
            method.deterministic_schedule_sites,
            method.concurrency_replay_guard_blocked_sites,
            method.concurrency_replay_contract_violation_sites,
            method.deterministic_concurrency_replay_race_guard_handoff);
    method.concurrency_replay_race_guard_profile_is_normalized =
        IsConcurrencyReplayRaceGuardProfileNormalized(
            method.concurrency_replay_race_guard_sites,
            method.concurrency_replay_sites,
            method.replay_proof_sites,
            method.race_guard_sites,
            method.task_handoff_sites,
            method.actor_isolation_sites,
            method.deterministic_schedule_sites,
            method.concurrency_replay_guard_blocked_sites,
            method.concurrency_replay_contract_violation_sites);
  }

  void FinalizeUnsafePointerExtensionProfile(FunctionDecl &fn) {
    const Objc3UnsafePointerExtensionProfile profile =
        BuildUnsafePointerExtensionProfileFromFunction(fn);
    fn.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;
    fn.unsafe_keyword_sites = profile.unsafe_keyword_sites;
    fn.pointer_arithmetic_sites = profile.pointer_arithmetic_sites;
    fn.raw_pointer_type_sites = profile.raw_pointer_type_sites;
    fn.unsafe_operation_sites = profile.unsafe_operation_sites;
    fn.unsafe_pointer_extension_normalized_sites = profile.normalized_sites;
    fn.unsafe_pointer_extension_gate_blocked_sites = profile.gate_blocked_sites;
    fn.unsafe_pointer_extension_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_unsafe_pointer_extension_handoff =
        profile.deterministic_unsafe_pointer_extension_handoff;
    fn.unsafe_pointer_extension_profile = BuildUnsafePointerExtensionProfile(
        fn.unsafe_pointer_extension_sites,
        fn.unsafe_keyword_sites,
        fn.pointer_arithmetic_sites,
        fn.raw_pointer_type_sites,
        fn.unsafe_operation_sites,
        fn.unsafe_pointer_extension_normalized_sites,
        fn.unsafe_pointer_extension_gate_blocked_sites,
        fn.unsafe_pointer_extension_contract_violation_sites,
        fn.deterministic_unsafe_pointer_extension_handoff);
    fn.unsafe_pointer_extension_profile_is_normalized =
        IsUnsafePointerExtensionProfileNormalized(
            fn.unsafe_pointer_extension_sites,
            fn.unsafe_keyword_sites,
            fn.pointer_arithmetic_sites,
            fn.raw_pointer_type_sites,
            fn.unsafe_operation_sites,
            fn.unsafe_pointer_extension_normalized_sites,
            fn.unsafe_pointer_extension_gate_blocked_sites,
            fn.unsafe_pointer_extension_contract_violation_sites);
  }

  void FinalizeUnsafePointerExtensionProfile(Objc3MethodDecl &method) {
    const Objc3UnsafePointerExtensionProfile profile =
        BuildUnsafePointerExtensionProfileFromOpaqueBody(method);
    method.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;

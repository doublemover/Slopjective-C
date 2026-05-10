  template <typename TCallableDecl>
  bool ParseRetainableCFamilyCallableAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    const auto record_no_payload = [this, &decl, &attribute_name]() {
      decl.retainable_c_family_callable_attributes.push_back(attribute_name.text);
      decl.retainable_c_family_profile_is_normalized = true;
      decl.retainable_c_family_profile = BuildRetainableCFamilyCallableProfile(
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
          decl.retainable_c_family_profile = BuildRetainableCFamilyCallableProfile(
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
  bool ParseDispatchIntentCallableAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    auto record_duplicate = [&](const char *code, const std::string &message) {
      diagnostics_.push_back(MakeDiag(
          attribute_name.line, attribute_name.column, code, message));
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

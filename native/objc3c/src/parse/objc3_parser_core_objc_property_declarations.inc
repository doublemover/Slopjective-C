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

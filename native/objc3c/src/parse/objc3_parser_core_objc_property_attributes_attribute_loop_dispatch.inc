  bool ParseObjcPropertyAttribute(Objc3PropertyAttributeDecl &attribute) {
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      return ReportInvalidObjcPropertyAttribute(name_token);
    }

    attribute.name = Previous().text;
    attribute.line = Previous().line;
    attribute.column = Previous().column;
    if (Match(TokenKind::Equal)) {
      attribute.has_value = true;
      attribute.value = ParseObjcPropertyAttributeValueText();
      if (attribute.value.empty()) {
        const Token &token = Peek();
        return ReportMissingObjcPropertyAttributeValue(token);
      }
    }
    return ValidateObjcPropertyAttributePayload(attribute);
  }

  bool ParseObjcPropertyAttributes(std::vector<Objc3PropertyAttributeDecl> &attributes) {
    // export-legality anchor: property attribute spelling must stay
    // canonical here so later property/ivar export preconditions can trust the
    // parser-owned source shape without reparsing attributes.
    if (!Match(TokenKind::LParen)) {
      return true;
    }

    while (true) {
      Objc3PropertyAttributeDecl attribute;
      if (!ParseObjcPropertyAttribute(attribute)) {
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
      return ReportMissingObjcPropertyAttributeListClose(token);
    }
  }

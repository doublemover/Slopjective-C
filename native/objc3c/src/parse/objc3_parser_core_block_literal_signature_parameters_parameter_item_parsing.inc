  void ParseBlockLiteralParameterExplicitType(
      Objc3BlockParameterSourceModel &parameter) {
    if (At(TokenKind::KwI32) || At(TokenKind::KwBool) || At(TokenKind::KwVoid)) {
      parameter.explicit_type = true;
      parameter.type_spelling = Advance().text;
    }
  }

  bool ParseBlockLiteralParameterIdentifier(
      Objc3BlockParameterSourceModel &parameter) {
    if (!At(TokenKind::Identifier)) {
      return ReportExpectedBlockLiteralParameterIdentifier();
    }

    const Token name_token = Advance();
    parameter.name = name_token.text;
    return true;
  }

  bool ParseBlockLiteralParameterItem(
      std::vector<Objc3BlockParameterSourceModel> &parameters) {
    Objc3BlockParameterSourceModel parameter;
    ParseBlockLiteralParameterExplicitType(parameter);
    if (!ParseBlockLiteralParameterIdentifier(parameter)) {
      return false;
    }

    parameters.push_back(std::move(parameter));
    return true;
  }

  bool ParseBlockLiteralParameterItems(
      std::vector<Objc3BlockParameterSourceModel> &parameters) {
    while (true) {
      if (!ParseBlockLiteralParameterItem(parameters)) {
        return false;
      }
      if (!Match(TokenKind::Comma)) {
        break;
      }
    }
    return true;
  }

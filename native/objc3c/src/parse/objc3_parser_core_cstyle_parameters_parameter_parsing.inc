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
      param.value_optional = parsed_type.value_optional;
      param.has_generic_suffix = parsed_type.has_generic_suffix;
      param.generic_suffix_terminated = parsed_type.generic_suffix_terminated;
      param.generic_suffix_text = parsed_type.generic_suffix_text;
      param.generic_line = parsed_type.generic_line;
      param.generic_column = parsed_type.generic_column;
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

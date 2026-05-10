    bool parameter_type_spelling_matched = false;
    if (Match(TokenKind::KwI32)) {
      param.type = ValueType::I32;
      parameter_type_spelling_matched = true;
    } else if (Match(TokenKind::KwBool)) {
      param.type = ValueType::Bool;
      parameter_type_spelling_matched = true;
    } else if (Match(TokenKind::KwBOOL)) {
      param.type = ValueType::Bool;
      parameter_type_spelling_matched = true;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      param.type = ValueType::I32;
      parameter_type_spelling_matched = true;
    }

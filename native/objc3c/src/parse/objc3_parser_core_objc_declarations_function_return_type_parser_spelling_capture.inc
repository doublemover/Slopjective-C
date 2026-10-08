  void CaptureFunctionReturnIdentifierSpelling(
      FunctionDecl &fn,
      const Token &type_token) {
    ValueType vector_type = ValueType::Unknown;
    std::string vector_base_spelling;
    unsigned vector_lane_count = 1;
    if (TryParseVectorTypeSpelling(
            type_token,
            vector_type,
            vector_base_spelling,
            vector_lane_count)) {
      fn.return_type = vector_type;
      fn.return_vector_spelling = true;
      fn.return_vector_base_spelling = vector_base_spelling;
      fn.return_vector_lane_count = vector_lane_count;
      return;
    }
    if (type_token.text == "Text" || type_token.text == "Objc3Text") {
      fn.return_type = ValueType::TextHandle;
      return;
    }

    fn.return_type = ValueType::ObjCObjectPtr;
    fn.return_object_pointer_type_spelling = true;
    fn.return_object_pointer_type_name = type_token.text;
  }

  bool ParseFunctionReturnTypeSpelling(FunctionDecl &fn) {
    if (Match(TokenKind::KwI32)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) ||
               Match(TokenKind::KwNSUInteger)) {
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
    } else if (AtReservedValueOptionalType()) {
      const Token type_token = Advance();
      fn.return_type = ValueType::Optional;
      fn.return_value_optional.present = true;
      fn.return_value_optional.source_type_admitted = true;
      fn.return_value_optional.semantic_carrier_modeled = true;
      fn.return_value_optional.line = type_token.line;
      fn.return_value_optional.column = type_token.column;
    } else if (At(TokenKind::Identifier)) {
      const Token type_token = Advance();
      CaptureFunctionReturnIdentifierSpelling(fn, type_token);
    } else {
      return ReportExpectedFunctionReturnType();
    }

    return true;
  }

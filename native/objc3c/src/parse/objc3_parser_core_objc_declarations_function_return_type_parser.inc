  bool ParseFunctionReturnType(FunctionDecl &fn) {
    ResetFunctionReturnTypeState(fn);

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) ||
        At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in function return type annotation"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in function return type annotation"
                                                  : "unexpected qualifier 'async' in function return type annotation";
      diagnostics_.push_back(MakeDiag(
          qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) &&
           IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      fn.has_return_ownership_qualifier = true;
      fn.return_ownership_qualifier_spelling = qualifier.text;
      fn.return_ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(
              Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }
    if (At(TokenKind::Identifier) &&
        IsBorrowedQualifierSpelling(Peek().text)) {
      Advance();
      fn.return_borrowed_pointer_qualified = true;
    }

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
    } else {
      if (At(TokenKind::Identifier)) {
        const Token type_token = Advance();
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
        } else {
          fn.return_type = ValueType::ObjCObjectPtr;
          fn.return_object_pointer_type_spelling = true;
          fn.return_object_pointer_type_name = type_token.text;
        }
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P114",
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

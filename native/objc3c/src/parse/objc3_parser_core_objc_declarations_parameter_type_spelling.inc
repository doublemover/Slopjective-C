    if (Match(TokenKind::KwI32)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      param.type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      param.type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwId)) {
      param.type = ValueType::ObjCId;
      param.id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      param.type = ValueType::ObjCClass;
      param.class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      param.type = ValueType::ObjCSel;
      param.sel_spelling = true;
    } else if (Match(TokenKind::KwProtocol)) {
      param.type = ValueType::ObjCProtocol;
    } else if (Match(TokenKind::KwInstancetype)) {
      param.type = ValueType::ObjCInstancetype;
      param.instancetype_spelling = true;
    } else if (AtRemovedOptionalTemplateAlias()) {
      return RejectRemovedOptionalTemplateAlias(Peek());
    } else if (At(TokenKind::Identifier)) {
      const Token type_token = Advance();
      ValueType vector_type = ValueType::Unknown;
      std::string vector_base_spelling;
      unsigned vector_lane_count = 1;
      if (TryParseVectorTypeSpelling(
              type_token, vector_type, vector_base_spelling,
              vector_lane_count)) {
        param.type = vector_type;
        param.vector_spelling = true;
        param.vector_base_spelling = vector_base_spelling;
        param.vector_lane_count = vector_lane_count;
        ParseParameterTypeSuffix(param);
        if (!param.generic_suffix_terminated) {
          return false;
        }
        param.cross_module_conformance_profile =
            BuildCrossModuleConformanceProfile(
                param.object_pointer_type_spelling,
                param.has_generic_suffix,
                param.generic_suffix_terminated,
                param.has_pointer_declarator,
                param.generic_suffix_text,
                param.object_pointer_type_name);
        param.cross_module_conformance_profile_is_normalized =
            IsCrossModuleConformanceProfileNormalized(
                param.object_pointer_type_spelling,
                param.has_generic_suffix,
                param.generic_suffix_terminated,
                param.has_pointer_declarator,
                param.generic_suffix_text,
                param.object_pointer_type_name);
        return true;
      }
      param.type = ValueType::ObjCObjectPtr;
      param.object_pointer_type_spelling = true;
      param.object_pointer_type_name = type_token.text;
    } else {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P108",
          "expected parameter type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', or "
          "'id', 'Class', 'SEL', 'Protocol', 'instancetype', object pointer spelling, "
          "ownership qualifiers '__strong/__weak/__autoreleasing/__unsafe_unretained', "
          "or vector forms "
          "'i32x2/i32x4/i32x8/i32x16' and 'boolx2/boolx4/boolx8/boolx16'"));
      return false;
    }

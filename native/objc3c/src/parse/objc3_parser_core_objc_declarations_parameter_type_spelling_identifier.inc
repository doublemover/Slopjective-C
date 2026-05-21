    if (!parameter_type_spelling_matched && At(TokenKind::Identifier)) {
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
      if (type_token.text == "Text" || type_token.text == "Objc3Text") {
        param.type = ValueType::TextHandle;
        parameter_type_spelling_matched = true;
        return true;
      }
      param.type = ValueType::ObjCObjectPtr;
      param.object_pointer_type_spelling = true;
      param.object_pointer_type_name = type_token.text;
      parameter_type_spelling_matched = true;
    }

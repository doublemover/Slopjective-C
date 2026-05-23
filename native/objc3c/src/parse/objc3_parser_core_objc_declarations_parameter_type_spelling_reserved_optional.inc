    if (!parameter_type_spelling_matched && AtReservedValueOptionalType()) {
      const Token type_token = Advance();
      param.type = ValueType::Optional;
      param.value_optional.present = true;
      param.value_optional.source_type_admitted = true;
      param.value_optional.semantic_carrier_modeled = true;
      param.value_optional.line = type_token.line;
      param.value_optional.column = type_token.column;
      parameter_type_spelling_matched = true;
    }

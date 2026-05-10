    if (!parameter_type_spelling_matched) {
      if (Match(TokenKind::KwId)) {
        param.type = ValueType::ObjCId;
        param.id_spelling = true;
        parameter_type_spelling_matched = true;
      } else if (Match(TokenKind::KwClass)) {
        param.type = ValueType::ObjCClass;
        param.class_spelling = true;
        parameter_type_spelling_matched = true;
      } else if (Match(TokenKind::KwSEL)) {
        param.type = ValueType::ObjCSel;
        param.sel_spelling = true;
        parameter_type_spelling_matched = true;
      } else if (Match(TokenKind::KwProtocol)) {
        param.type = ValueType::ObjCProtocol;
        parameter_type_spelling_matched = true;
      } else if (Match(TokenKind::KwInstancetype)) {
        param.type = ValueType::ObjCInstancetype;
        param.instancetype_spelling = true;
        parameter_type_spelling_matched = true;
      }
    }

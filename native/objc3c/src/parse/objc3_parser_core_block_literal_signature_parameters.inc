  bool ParseBlockLiteralParameterList(
      std::vector<Objc3BlockParameterSourceModel> &parameters) {
    if (!Match(TokenKind::LParen)) {
      return true;
    }

    if (!At(TokenKind::RParen)) {
      while (true) {
        Objc3BlockParameterSourceModel parameter;
        if (At(TokenKind::KwI32) || At(TokenKind::KwBool) || At(TokenKind::KwVoid)) {
          parameter.explicit_type = true;
          parameter.type_spelling = Advance().text;
        }
        if (!At(TokenKind::Identifier)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(
              token.line, token.column, "O3P166",
              "expected parameter identifier in block literal"));
          return false;
        }
        const Token name_token = Advance();
        parameter.name = name_token.text;
        parameters.push_back(std::move(parameter));
        if (!Match(TokenKind::Comma)) {
          break;
        }
      }
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109",
                                      "missing ')' after block literal parameter list"));
      return false;
    }
    return true;
  }

  void ApplyBlockLiteralSignatureModel(
      Expr &block,
      const std::vector<Objc3BlockParameterSourceModel> &parameters,
      std::vector<std::string> &parameter_names) {
    parameter_names.reserve(parameters.size());
    for (const auto &parameter : parameters) {
      parameter_names.push_back(parameter.name);
    }
    block.block_parameter_count = parameter_names.size();
    block.block_parameter_names_lexicographic =
        BuildSortedUniqueStrings(parameter_names);
    // block-source-model-completion anchor: parser now publishes the
    // canonical parameter-signature, capture-inventory, and invoke-surface
    // source model directly on the AST so source-only frontend runs can carry
    // that closure forward before runnable block lowering still fails closed.
    // executable-block-object/invoke-thunk anchor: lane-C consumes
    // this retained parameter/body ordering directly when it emits one stack-resident block object plus one internal invoke thunk for the current readonly-scalar capture slice.
    block.block_parameter_signature_entries_lexicographic =
        BuildBlockParameterSignatureEntriesLexicographic(parameters);
    block.block_parameter_types_source_order =
        BuildBlockParameterTypesSourceOrder(parameters);
    block.block_parameters_source_order.reserve(parameters.size());
    for (const auto &parameter : parameters) {
      Expr::BlockParameter lowered_parameter;
      lowered_parameter.name = parameter.name;
      if (!parameter.explicit_type) {
        lowered_parameter.type = ValueType::Unknown;
      } else if (parameter.type_spelling == "i32") {
        lowered_parameter.type = ValueType::I32;
      } else if (parameter.type_spelling == "bool") {
        lowered_parameter.type = ValueType::Bool;
      } else if (parameter.type_spelling == "void") {
        lowered_parameter.type = ValueType::Void;
      } else {
        lowered_parameter.type = ValueType::Unknown;
      }
      block.block_parameters_source_order.push_back(std::move(lowered_parameter));
    }
    block.block_explicit_typed_parameter_count = static_cast<std::size_t>(
        std::count_if(parameters.begin(), parameters.end(),
                      [](const Objc3BlockParameterSourceModel &parameter) {
                        return parameter.explicit_type;
                      }));
    block.block_implicit_parameter_count =
        block.block_parameter_count - block.block_explicit_typed_parameter_count;
    block.block_signature_profile = BuildBlockSignatureProfile(parameters);
  }

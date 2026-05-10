  BlockLiteralSourceUseKind CurrentBlockLiteralSourceUseKind() const {
    if (block_literal_source_use_stack_.empty()) {
      return BlockLiteralSourceUseKind::ExpressionSite;
    }
    return block_literal_source_use_stack_.back();
  }

  std::unique_ptr<Expr> ParseExpressionWithBlockLiteralSourceUse(
      BlockLiteralSourceUseKind kind) {
    ScopedBlockLiteralSourceUse scope(block_literal_source_use_stack_, kind);
    return ParseConditional();
  }

  std::unique_ptr<Expr> ParseExpression() { return ParseConditional(); }

  struct ParsedOptionalBindingClause {
    std::string name;
    bool is_mutable = false;
    std::unique_ptr<Expr> value;
    unsigned line = 1;
    unsigned column = 1;
  };

  struct ParsedGuardConditionClause {
    enum class Kind {
      OptionalBinding,
      BooleanExpr,
    };

    Kind kind = Kind::BooleanExpr;
    ParsedOptionalBindingClause binding;
    std::unique_ptr<Expr> condition;
    unsigned line = 1;
    unsigned column = 1;
  };

  ParsedOptionalBindingClause ParseOptionalBindingClauseWithMutability(
      bool binding_is_mutable) {
    ParsedOptionalBindingClause clause;
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P103", "expected identifier in optional binding"));
      return clause;
    }
    const Token name_token = Advance();
    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "missing '=' in optional binding"));
      return clause;
    }
    clause.name = name_token.text;
    clause.is_mutable = binding_is_mutable;
    clause.line = name_token.line;
    clause.column = name_token.column;
    clause.value = ParseExpressionWithBlockLiteralSourceUse(
        BlockLiteralSourceUseKind::ExpressionSite);
    return clause;
  }

  ParsedOptionalBindingClause ParseOptionalBindingClause() {
    bool binding_is_mutable = false;
    if (Match(TokenKind::KwLet)) {
      binding_is_mutable = false;
    } else if (Match(TokenKind::KwVar)) {
      binding_is_mutable = true;
    } else {
      ParsedOptionalBindingClause clause;
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P103", "expected 'let' or 'var' in optional binding"));
      return clause;
    }
    return ParseOptionalBindingClauseWithMutability(binding_is_mutable);
  }

  std::vector<ParsedOptionalBindingClause> ParseOptionalBindingClauses() {
    std::vector<ParsedOptionalBindingClause> clauses;
    bool binding_is_mutable = false;
    if (Match(TokenKind::KwLet)) {
      binding_is_mutable = false;
    } else if (Match(TokenKind::KwVar)) {
      binding_is_mutable = true;
    } else {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P103", "expected 'let' or 'var' in optional binding"));
      return {};
    }
    while (true) {
      ParsedOptionalBindingClause clause =
          ParseOptionalBindingClauseWithMutability(binding_is_mutable);
      if (clause.value == nullptr || clause.name.empty()) {
        return {};
      }
      clauses.push_back(std::move(clause));
      if (!Match(TokenKind::Comma)) {
        break;
      }
      if (At(TokenKind::KwLet)) {
        Advance();
        binding_is_mutable = false;
      } else if (At(TokenKind::KwVar)) {
        Advance();
        binding_is_mutable = true;
      }
    }
    return clauses;
  }

  std::vector<ParsedGuardConditionClause> ParseGuardConditionClauses() {
    std::vector<ParsedGuardConditionClause> clauses;
    while (true) {
      ParsedGuardConditionClause clause;
      clause.line = Peek().line;
      clause.column = Peek().column;
      if (At(TokenKind::KwLet) || At(TokenKind::KwVar)) {
        clause.kind = ParsedGuardConditionClause::Kind::OptionalBinding;
        clause.binding = ParseOptionalBindingClause();
        if (clause.binding.value == nullptr || clause.binding.name.empty()) {
          return {};
        }
      } else {
        clause.kind = ParsedGuardConditionClause::Kind::BooleanExpr;
        clause.condition = ParseExpressionWithBlockLiteralSourceUse(
            BlockLiteralSourceUseKind::ExpressionSite);
        if (clause.condition == nullptr) {
          return {};
        }
      }
      clauses.push_back(std::move(clause));
      if (!Match(TokenKind::Comma)) {
        break;
      }
    }
    return clauses;
  }

  bool ParseMatchPattern(SwitchCase &case_stmt) {
    case_stmt.match_pattern_enabled = true;
    if (At(TokenKind::Identifier) && Peek().text == "_") {
      const Token wildcard = Advance();
      case_stmt.match_pattern_kind = MatchPatternKind::Wildcard;
      case_stmt.value_line = wildcard.line;
      case_stmt.value_column = wildcard.column;
      return true;
    }

    if (Match(TokenKind::Number)) {
      case_stmt.match_pattern_kind = MatchPatternKind::LiteralInteger;
      case_stmt.value_line = Previous().line;
      case_stmt.value_column = Previous().column;
      case_stmt.value = std::atoi(Previous().text.c_str());
      return true;
    }
    if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
      case_stmt.match_pattern_kind = MatchPatternKind::LiteralBool;
      case_stmt.value_line = Previous().line;
      case_stmt.value_column = Previous().column;
      case_stmt.value = Previous().kind == TokenKind::KwTrue ? 1 : 0;
      return true;
    }
    if (Match(TokenKind::KwNil)) {
      case_stmt.match_pattern_kind = MatchPatternKind::LiteralNil;
      case_stmt.value_line = Previous().line;
      case_stmt.value_column = Previous().column;
      case_stmt.value = 0;
      return true;
    }
    if (Match(TokenKind::KwLet) || Match(TokenKind::KwVar)) {
      const Token binding_token = Previous();
      const bool is_mutable = binding_token.kind == TokenKind::KwVar;
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P103", "expected identifier in match binding pattern"));
        return false;
      }
      const Token name_token = Advance();
      case_stmt.match_pattern_kind = MatchPatternKind::Binding;
      case_stmt.match_binding_mutable = is_mutable;
      case_stmt.match_binding_name = name_token.text;
      case_stmt.value_line = name_token.line;
      case_stmt.value_column = name_token.column;
      return true;
    }
    if (At(TokenKind::Identifier) && Peek().text == "is") {
      const Token token = Advance();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P158", "unsupported type-test match pattern"));
      return false;
    }
    if (Match(TokenKind::Dot)) {
      const Token &result_case = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            MakeDiag(result_case.line, result_case.column, "O3P103", "invalid result-case match pattern"));
        return false;
      }
      if (Previous().text != "Ok" && Previous().text != "Err") {
        diagnostics_.push_back(
            MakeDiag(Previous().line, Previous().column, "O3P103", "invalid result-case match pattern"));
        return false;
      }
      const Token result_name = Previous();
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P106", "missing '(' after result-case pattern"));
        return false;
      }
      if (!(Match(TokenKind::KwLet) || Match(TokenKind::KwVar))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P103", "expected 'let' or 'var' in result-case pattern"));
        return false;
      }
      const bool is_mutable = Previous().kind == TokenKind::KwVar;
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P103", "expected identifier in result-case pattern"));
        return false;
      }
      const Token name_token = Advance();
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P109", "missing ')' after result-case pattern"));
        return false;
      }
      case_stmt.match_pattern_kind = MatchPatternKind::ResultCase;
      case_stmt.match_result_case_name = result_name.text;
      case_stmt.match_binding_mutable = is_mutable;
      case_stmt.match_binding_name = name_token.text;
      case_stmt.value_line = result_name.line;
      case_stmt.value_column = result_name.column;
      return true;
    }

    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid match pattern"));
    return false;
  }

  std::unique_ptr<Expr> ParseConditional() {
    auto expr = ParseNilCoalescing();
    if (expr == nullptr) {
      return nullptr;
    }
    // Part 3 source-closure anchor: Question currently feeds only
    // conditional expressions here and nullability suffix parsing elsewhere.
    // Optional chaining and nil-coalescing are still fail-closed until A002/C002.
    if (!Match(TokenKind::Question)) {
      return expr;
    }

    const Token question = Previous();
    auto when_true = ParseExpression();
    if (when_true == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Colon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' in conditional expression"));
      return nullptr;
    }
    auto when_false = ParseConditional();
    if (when_false == nullptr) {
      return nullptr;
    }

    return BuildObjc3ConditionalExpr(
        question, std::move(expr), std::move(when_true), std::move(when_false));
  }

  std::unique_ptr<Expr> ParseNilCoalescing() {
    auto expr = ParseLogicalOr();
    if (expr == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::QuestionQuestion)) {
      return expr;
    }
    const Token op = Previous();
    auto rhs = ParseNilCoalescing();
    if (rhs == nullptr) {
      return nullptr;
    }
    return BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
  }

  std::unique_ptr<Expr> ParseLogicalOr() {
    auto expr = ParseLogicalAnd();
    while (expr != nullptr && Match(TokenKind::OrOr)) {
      const Token op = Previous();
      auto rhs = ParseLogicalAnd();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseLogicalAnd() {
    auto expr = ParseBitwiseOr();
    while (expr != nullptr && Match(TokenKind::AndAnd)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseOr();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseOr() {
    auto expr = ParseBitwiseXor();
    while (expr != nullptr && Match(TokenKind::Pipe)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseXor();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseXor() {
    auto expr = ParseBitwiseAnd();
    while (expr != nullptr && Match(TokenKind::Caret)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseAnd();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseAnd() {
    auto expr = ParseEquality();
    while (expr != nullptr && Match(TokenKind::Ampersand)) {
      const Token op = Previous();
      auto rhs = ParseEquality();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseEquality() {
    auto expr = ParseRelational();
    while (expr != nullptr && (Match(TokenKind::EqualEqual) || Match(TokenKind::BangEqual))) {
      const Token op = Previous();
      auto rhs = ParseRelational();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseRelational() {
    auto expr = ParseShift();
    while (expr != nullptr &&
           (Match(TokenKind::Less) || Match(TokenKind::LessEqual) || Match(TokenKind::Greater) ||
            Match(TokenKind::GreaterEqual))) {
      const Token op = Previous();
      auto rhs = ParseShift();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseShift() {
    auto expr = ParseAdditive();
    while (expr != nullptr && (Match(TokenKind::LessLess) || Match(TokenKind::GreaterGreater))) {
      const Token op = Previous();
      auto rhs = ParseAdditive();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseAdditive() {
    auto expr = ParseMultiplicative();
    while (expr != nullptr && (At(TokenKind::Plus) || At(TokenKind::Minus))) {
      const Token op = Advance();
      auto rhs = ParseMultiplicative();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseMultiplicative() {
    auto expr = ParseUnary();
    while (expr != nullptr && (At(TokenKind::Star) || At(TokenKind::Slash) || At(TokenKind::Percent))) {
      const Token op = Advance();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseUnary() {
    if (Match(TokenKind::KwTry)) {
      const Token try_token = Previous();
      Expr::TryOperatorKind try_kind = Expr::TryOperatorKind::Propagate;
      bool requires_throwing_context = true;
      if (Match(TokenKind::Question)) {
        try_kind = Expr::TryOperatorKind::Optional;
        requires_throwing_context = false;
      } else if (Match(TokenKind::Bang)) {
        try_kind = Expr::TryOperatorKind::Forced;
        requires_throwing_context = false;
      }
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3TryExpression(
          try_token, try_kind, requires_throwing_context, std::move(rhs));
    }
    if (Match(TokenKind::KwAwait)) {
      const Token await_token = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      rhs->await_expression_enabled = true;
      if (rhs->line == 0u) {
        rhs->line = await_token.line;
      }
      if (rhs->column == 0u) {
        rhs->column = await_token.column;
      }
      return rhs;
    }
    if (Match(TokenKind::Bang)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3LogicalNotExpr(op, std::move(rhs));
    }
    if (Match(TokenKind::Plus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3UnaryLoweringBinaryExpr(op, "+", 0, std::move(rhs));
    }
    if (Match(TokenKind::Minus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3UnaryLoweringBinaryExpr(op, "-", 0, std::move(rhs));
    }
    if (Match(TokenKind::Tilde)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3BitwiseNotExpr(op, std::move(rhs));
    }
    return ParsePostfix();
  }

  std::unique_ptr<Expr> ParsePostfix() {
    auto expr = ParsePrimary();
    // Part 3 source-closure anchor: postfix parsing now admits calls,
    // message-send receivers, and optional-member access lowering sugar while
    // broader typed key-path execution still remains deferred.
    while (expr != nullptr) {
      if (Match(TokenKind::LParen)) {
        const unsigned callee_line = expr->line;
        const unsigned callee_column = expr->column;
        auto call = std::make_unique<Expr>();
        call->kind = Expr::Kind::Call;
        call->line = callee_line;
        call->column = callee_column;
        if (expr->kind != Expr::Kind::Identifier) {
          diagnostics_.push_back(MakeDiag(expr->line, expr->column, "O3P112", "call target must be identifier"));
          return nullptr;
        }
        call->ident = expr->ident;
        if (!At(TokenKind::RParen)) {
          while (true) {
            auto arg = ParseExpressionWithBlockLiteralSourceUse(
                BlockLiteralSourceUseKind::CallArgument);
            if (arg == nullptr) {
              return nullptr;
            }
            call->args.push_back(std::move(arg));
            if (!Match(TokenKind::Comma)) {
              break;
            }
          }
        }
        if (!Match(TokenKind::RParen)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after call"));
          return nullptr;
        }
        expr = std::move(call);
        continue;
      }

      if (Match(TokenKind::QuestionDot)) {
        const Token access = Previous();
        if (!At(TokenKind::Identifier)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected identifier after '?.'"));
          return nullptr;
        }
        const Token member = Advance();
        auto message = std::make_unique<Expr>();
        message->kind = Expr::Kind::MessageSend;
        message->line = access.line;
        message->column = access.column;
        message->receiver = std::move(expr);
        message->optional_send_enabled = true;
        message->optional_member_access_enabled = true;
        message->optional_send_symbol = BuildOptionalSendSymbol(true);
        message->optional_send_is_normalized = true;
        message->selector = member.text;
        message->message_send_form = Expr::MessageSendForm::Unary;
        Expr::MessageSendSelectorPiece head_piece;
        head_piece.keyword = member.text;
        head_piece.has_argument = false;
        head_piece.line = member.line;
        head_piece.column = member.column;
        message->selector_lowering_pieces.push_back(head_piece);
        message->message_send_form_symbol = BuildMessageSendFormSymbol(message->message_send_form);
        message->selector_lowering_symbol = BuildMessageSendSelectorLoweringSymbol(message->selector_lowering_pieces);
        message->selector_lowering_is_normalized = true;
        message->dispatch_abi_receiver_slots_marshaled = 1u;
        message->dispatch_abi_selector_slots_marshaled = 1u;
        message->dispatch_abi_argument_value_slots_marshaled = 0u;
        message->dispatch_abi_runtime_arg_slots = kDispatchAbiMarshallingRuntimeArgSlots;
        message->dispatch_abi_argument_padding_slots_marshaled = ComputeDispatchAbiArgumentPaddingSlots(
            0u, message->dispatch_abi_runtime_arg_slots);
        message->dispatch_abi_argument_total_slots_marshaled = message->dispatch_abi_argument_padding_slots_marshaled;
        message->dispatch_abi_total_slots_marshaled = message->dispatch_abi_receiver_slots_marshaled +
                                                      message->dispatch_abi_selector_slots_marshaled +
                                                      message->dispatch_abi_argument_total_slots_marshaled;
        message->dispatch_abi_marshalling_symbol = BuildDispatchAbiMarshallingSymbol(
            message->dispatch_abi_receiver_slots_marshaled, message->dispatch_abi_selector_slots_marshaled,
            message->dispatch_abi_argument_value_slots_marshaled, message->dispatch_abi_argument_padding_slots_marshaled,
            message->dispatch_abi_argument_total_slots_marshaled, message->dispatch_abi_total_slots_marshaled,
            message->dispatch_abi_runtime_arg_slots);
        message->dispatch_abi_marshalling_is_normalized = true;
        message->nil_receiver_semantics_enabled = message->receiver->kind == Expr::Kind::NilLiteral;
        message->nil_receiver_foldable = message->nil_receiver_semantics_enabled;
        message->nil_receiver_requires_runtime_dispatch = !message->nil_receiver_foldable;
        message->nil_receiver_folding_symbol = BuildNilReceiverFoldingSymbol(
            message->nil_receiver_foldable, message->nil_receiver_requires_runtime_dispatch, message->message_send_form);
        message->nil_receiver_semantics_is_normalized = true;
        expr = std::move(message);
        continue;
      }

      break;
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBlockLiteralExpression() {
    const Token caret = Previous();
    auto block = std::make_unique<Expr>();
    block->kind = Expr::Kind::BlockLiteral;
    block->line = caret.line;
    block->column = caret.column;

    if (Match(TokenKind::LBracket)) {
      block->block_has_explicit_capture_list = true;
      while (!At(TokenKind::Eof) && !At(TokenKind::RBracket)) {
        Expr::ExplicitBlockCaptureItem item;
        item.mode = "plain";
        if (At(TokenKind::Identifier) &&
            (Peek().text == "weak" || Peek().text == "unowned" ||
             Peek().text == "move" || Peek().text == "byref" ||
             Peek().text == "__block")) {
          item.mode = Advance().text;
          if (item.mode == "__block") {
            item.mode = "byref";
          }
        }
        if (!At(TokenKind::Identifier)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(
              token.line, token.column, "O3P313",
              "expected capture identifier in explicit block capture list"));
          return nullptr;
        }
        item.name = Advance().text;
        block->block_explicit_capture_items_source_order.push_back(std::move(item));
        if (!Match(TokenKind::Comma)) {
          break;
        }
      }
      if (!Match(TokenKind::RBracket)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P314",
            "missing ']' after explicit block capture list"));
        return nullptr;
      }
    }

    std::vector<Objc3BlockParameterSourceModel> parameters;
    if (Match(TokenKind::LParen)) {
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
                token.line, token.column, "O3P166", "expected parameter identifier in block literal"));
            return nullptr;
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
        return nullptr;
      }
    }

    // executable-block-source-closure anchor: block literals enter
    // the parser-owned source surface at the brace-owned ParseBlock() helper,
    // so the freeze boundary must only probe for '{' here and leave actual
    // block consumption to the shared block parser.
    // block-lowering-ABI/artifact-boundary freeze anchor: parser
    // owns only the source spelling, parameter list, capture inventory, and
    // body shape that later lowering consumes. It does not assign emitted
    // block-object layout slots, invoke-thunk bodies, byref cells, or helper
    // symbols.
    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P166", "expected '{' before block literal body"));
      return nullptr;
    }

    auto body = ParseBlock();
    bool deterministic_capture_set = true;
    std::vector<std::string> parameter_names;
    parameter_names.reserve(parameters.size());
    for (const auto &parameter : parameters) {
      parameter_names.push_back(parameter.name);
    }
    block->block_parameter_count = parameter_names.size();
    block->block_parameter_names_lexicographic = BuildSortedUniqueStrings(parameter_names);
    // block-source-model-completion anchor: parser now publishes the
    // canonical parameter-signature, capture-inventory, and invoke-surface
    // source model directly on the AST so source-only frontend runs can carry
    // that closure forward before runnable block lowering still fails closed.
    // executable-block-object/invoke-thunk anchor: lane-C consumes
    // this retained parameter/body ordering directly when it emits one stack-resident block object plus one internal invoke thunk for the current readonly-scalar capture slice.
    block->block_parameter_signature_entries_lexicographic =
        BuildBlockParameterSignatureEntriesLexicographic(parameters);
    block->block_parameter_types_source_order =
        BuildBlockParameterTypesSourceOrder(parameters);
    block->block_parameters_source_order.reserve(parameters.size());
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
      block->block_parameters_source_order.push_back(std::move(lowered_parameter));
    }
    block->block_explicit_typed_parameter_count = static_cast<std::size_t>(
        std::count_if(parameters.begin(), parameters.end(),
                      [](const Objc3BlockParameterSourceModel &parameter) {
                        return parameter.explicit_type;
                      }));
    block->block_implicit_parameter_count =
        block->block_parameter_count - block->block_explicit_typed_parameter_count;
    block->block_signature_profile = BuildBlockSignatureProfile(parameters);
    block->block_capture_names_lexicographic =
        BuildObjc3BlockLiteralCaptureSet(body, parameter_names, deterministic_capture_set);
    block->block_capture_count = block->block_capture_names_lexicographic.size();
    block->block_capture_inventory_entries_lexicographic =
        BuildBlockCaptureInventoryEntriesLexicographic(
            block->block_capture_names_lexicographic);
    for (const auto &item : block->block_explicit_capture_items_source_order) {
      block->block_explicit_capture_names_lexicographic.push_back(item.name);
      if (item.mode == "weak") {
        ++block->block_explicit_capture_weak_count;
      } else if (item.mode == "unowned") {
        ++block->block_explicit_capture_unowned_count;
      } else if (item.mode == "move") {
        ++block->block_explicit_capture_move_count;
      } else if (item.mode == "byref") {
        ++block->block_explicit_capture_byref_count;
      } else {
        ++block->block_explicit_capture_plain_count;
      }
    }
    block->block_explicit_capture_names_lexicographic =
        BuildSortedUniqueStrings(block->block_explicit_capture_names_lexicographic);
    block->block_explicit_capture_count =
        block->block_explicit_capture_items_source_order.size();
    {
      std::ostringstream out;
      out << "explicit-captures:present="
          << (block->block_has_explicit_capture_list ? "true" : "false")
          << ";count=" << block->block_explicit_capture_count
          << ";weak=" << block->block_explicit_capture_weak_count
          << ";unowned=" << block->block_explicit_capture_unowned_count
          << ";move=" << block->block_explicit_capture_move_count
          << ";byref=" << block->block_explicit_capture_byref_count
          << ";plain=" << block->block_explicit_capture_plain_count;
      block->block_explicit_capture_profile = out.str();
    }
    block->block_byvalue_readonly_capture_count =
        block->block_capture_count;
    block->block_mutated_capture_names_lexicographic =
        BuildObjc3BlockLiteralMutatedCaptureSet(
            body, block->block_capture_names_lexicographic);
    block->block_mutated_capture_count =
        block->block_mutated_capture_names_lexicographic.size();
    block->block_byref_capture_names_lexicographic =
        block->block_mutated_capture_names_lexicographic;
    for (const auto &item : block->block_explicit_capture_items_source_order) {
      if (item.mode == "byref") {
        block->block_byref_capture_names_lexicographic.push_back(item.name);
      }
    }
    block->block_byref_capture_names_lexicographic =
        BuildSortedUniqueStrings(block->block_byref_capture_names_lexicographic);
    block->block_byref_capture_count =
        block->block_byref_capture_names_lexicographic.size();
    block->block_capture_inventory_profile =
        BuildBlockCaptureInventoryProfile(
            block->block_capture_count,
            block->block_byvalue_readonly_capture_count);
    block->block_body_statement_count = body.size();
    block->block_capture_set_deterministic = deterministic_capture_set;
    block->block_capture_profile =
        BuildBlockLiteralCaptureProfile(block->block_capture_names_lexicographic);
    block->block_literal_is_normalized = true;
    block->block_abi_invoke_argument_slots = block->block_parameter_count;
    block->block_abi_capture_word_count = block->block_capture_count;
    block->block_abi_layout_profile = BuildBlockLiteralAbiLayoutProfile(
        block->block_parameter_count,
        block->block_capture_count,
        block->block_body_statement_count);
    block->block_abi_descriptor_symbol = BuildBlockLiteralAbiDescriptorSymbol(
        block->line,
        block->column,
        block->block_parameter_count,
        block->block_capture_count);
    block->block_invoke_trampoline_symbol = BuildBlockLiteralInvokeTrampolineSymbol(
        block->line,
        block->column,
        block->block_parameter_count,
        block->block_capture_count);
    block->block_invoke_surface_entries_lexicographic =
        BuildBlockInvokeSurfaceEntriesLexicographic(*block);
    block->block_invoke_surface_profile =

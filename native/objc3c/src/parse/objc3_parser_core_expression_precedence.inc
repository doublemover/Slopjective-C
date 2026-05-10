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


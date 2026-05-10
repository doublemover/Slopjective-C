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
          diagnostics_.push_back(MakeDiag(
              expr->line, expr->column, "O3P112",
              "call target must be identifier"));
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
          diagnostics_.push_back(MakeDiag(
              token.line, token.column, "O3P109", "missing ')' after call"));
          return nullptr;
        }
        expr = std::move(call);
        continue;
      }

      if (Match(TokenKind::QuestionDot)) {
        const Token access = Previous();
        if (!At(TokenKind::Identifier)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(
              token.line, token.column, "O3P103",
              "expected identifier after '?.'"));
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
        message->message_send_form_symbol =
            BuildMessageSendFormSymbol(message->message_send_form);
        message->selector_lowering_symbol =
            BuildMessageSendSelectorLoweringSymbol(
                message->selector_lowering_pieces);
        message->selector_lowering_is_normalized = true;
        message->dispatch_abi_receiver_slots_marshaled = 1u;
        message->dispatch_abi_selector_slots_marshaled = 1u;
        message->dispatch_abi_argument_value_slots_marshaled = 0u;
        message->dispatch_abi_runtime_arg_slots =
            kDispatchAbiMarshallingRuntimeArgSlots;
        message->dispatch_abi_argument_padding_slots_marshaled =
            ComputeDispatchAbiArgumentPaddingSlots(
                0u, message->dispatch_abi_runtime_arg_slots);
        message->dispatch_abi_argument_total_slots_marshaled =
            message->dispatch_abi_argument_padding_slots_marshaled;
        message->dispatch_abi_total_slots_marshaled =
            message->dispatch_abi_receiver_slots_marshaled +
            message->dispatch_abi_selector_slots_marshaled +
            message->dispatch_abi_argument_total_slots_marshaled;
        message->dispatch_abi_marshalling_symbol =
            BuildDispatchAbiMarshallingSymbol(
                message->dispatch_abi_receiver_slots_marshaled,
                message->dispatch_abi_selector_slots_marshaled,
                message->dispatch_abi_argument_value_slots_marshaled,
                message->dispatch_abi_argument_padding_slots_marshaled,
                message->dispatch_abi_argument_total_slots_marshaled,
                message->dispatch_abi_total_slots_marshaled,
                message->dispatch_abi_runtime_arg_slots);
        message->dispatch_abi_marshalling_is_normalized = true;
        message->nil_receiver_semantics_enabled =
            message->receiver->kind == Expr::Kind::NilLiteral;
        message->nil_receiver_foldable =
            message->nil_receiver_semantics_enabled;
        message->nil_receiver_requires_runtime_dispatch =
            !message->nil_receiver_foldable;
        message->nil_receiver_folding_symbol =
            BuildNilReceiverFoldingSymbol(
                message->nil_receiver_foldable,
                message->nil_receiver_requires_runtime_dispatch,
                message->message_send_form);
        message->nil_receiver_semantics_is_normalized = true;
        expr = std::move(message);
        continue;
      }

      break;
    }
    return expr;
  }

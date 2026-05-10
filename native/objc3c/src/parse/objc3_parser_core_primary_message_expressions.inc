  std::unique_ptr<Expr> ParsePrimary() {
    if (Match(TokenKind::Number)) {
      int value = 0;
      const Token &number_token = Previous();
      if (!ParseIntegerLiteralValue(number_token.text, value)) {
        diagnostics_.push_back(MakeDiag(
            number_token.line, number_token.column, "O3P103",
            "invalid numeric literal '" + number_token.text + "'"));
        return nullptr;
      }
      return BuildObjc3NumericLiteralExpr(value, number_token);
    }
    if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
      const Token &bool_token = Previous();
      return BuildObjc3BoolLiteralExpr(
          bool_token.kind == TokenKind::KwTrue, bool_token);
    }
    if (Match(TokenKind::KwNil)) {
      return BuildObjc3NilLiteralExpr(Previous());
    }
    if (Match(TokenKind::Caret)) {
      return ParseBlockLiteralExpression();
    }
    if (Match(TokenKind::KwAtKeypath)) {
      const Token keypath_token = Previous();
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after @keypath"));
        return nullptr;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected key-path root identifier"));
        return nullptr;
      }
      const Token root = Advance();
      if (!Match(TokenKind::Comma)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "missing ',' after key-path root"));
        return nullptr;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected key-path component"));
        return nullptr;
      }
      std::vector<std::string> components;
      components.push_back(Advance().text);
      while (Match(TokenKind::Dot)) {
        if (!At(TokenKind::Identifier)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected identifier after '.' in key path"));
          return nullptr;
        }
        components.push_back(Advance().text);
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after @keypath"));
        return nullptr;
      }
      return BuildObjc3TypedKeyPathLiteralExpr(
          keypath_token, root, std::move(components));
    }
    if (Match(TokenKind::Identifier)) {
      const Token &identifier = Previous();
      return BuildObjc3IdentifierExpr(identifier.text, identifier);
    }
    if (Match(TokenKind::LParen)) {
      auto expr = ParseExpression();
      if (expr == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after expression"));
        return nullptr;
      }
      return expr;
    }

    if (Match(TokenKind::LBracket)) {
      return ParseMessageSendExpression();
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in expression position"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in expression position"
                                                  : "unexpected qualifier 'async' in expression position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return nullptr;
    }

    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid expression"));
    return nullptr;
  }

  std::unique_ptr<Expr> ParseMessageSendExpression() {
    const Token open = Previous();
    auto message = std::make_unique<Expr>();
    message->kind = Expr::Kind::MessageSend;
    message->line = open.line;
    message->column = open.column;

    const std::size_t receiver_diag_count = diagnostics_.size();
    message->receiver = ParsePostfix();
    if (message->receiver == nullptr) {
      if (diagnostics_.size() == receiver_diag_count) {
        diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P113",
                                        "invalid receiver expression in message send"));
      }
      return nullptr;
    }
    if (Match(TokenKind::Question)) {
      message->optional_send_enabled = true;
      message->optional_send_symbol = BuildOptionalSendSymbol(true);
      message->optional_send_is_normalized = true;
    } else {
      message->optional_send_symbol = BuildOptionalSendSymbol(false);
      message->optional_send_is_normalized = true;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message_text =
          qualifier.kind == TokenKind::KwPure
              ? "unexpected qualifier 'pure' in message selector position"
              : qualifier.kind == TokenKind::KwExtern
                    ? "unexpected qualifier 'extern' in message selector position"
                    : "unexpected qualifier 'async' in message selector position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message_text));
      return nullptr;
    }

    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "expected selector identifier in message send"));
      return nullptr;
    }

    const Token selector_head = Advance();
    message->selector = selector_head.text;
    Expr::MessageSendSelectorPiece head_piece;
    head_piece.keyword = selector_head.text;
    head_piece.line = selector_head.line;
    head_piece.column = selector_head.column;
    if (Match(TokenKind::Colon)) {
      message->message_send_form = Expr::MessageSendForm::Keyword;
      head_piece.has_argument = true;
      message->selector_lowering_pieces.push_back(head_piece);
      message->selector += ":";
      auto first_arg = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::MessageArgument);
      if (first_arg == nullptr) {
        return nullptr;
      }
      message->args.push_back(std::move(first_arg));

      while (true) {
        if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
          const Token qualifier = Advance();
          const std::string message_text =
              qualifier.kind == TokenKind::KwPure
                  ? "unexpected qualifier 'pure' in keyword selector segment position"
                  : qualifier.kind == TokenKind::KwExtern
                        ? "unexpected qualifier 'extern' in keyword selector segment position"
                        : "unexpected qualifier 'async' in keyword selector segment position";
          diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message_text));
          return nullptr;
        }
        if (!At(TokenKind::Identifier)) {
          break;
        }
        const Token keyword = Advance();
        if (!Match(TokenKind::Colon)) {
          diagnostics_.push_back(MakeDiag(keyword.line, keyword.column, "O3P113",
                                          "missing ':' in keyword selector segment"));
          return nullptr;
        }
        Expr::MessageSendSelectorPiece keyword_piece;
        keyword_piece.keyword = keyword.text;
        keyword_piece.has_argument = true;
        keyword_piece.line = keyword.line;
        keyword_piece.column = keyword.column;
        message->selector_lowering_pieces.push_back(std::move(keyword_piece));
        message->selector += keyword.text;
        message->selector += ":";
        auto arg = ParseExpressionWithBlockLiteralSourceUse(
            BlockLiteralSourceUseKind::MessageArgument);
        if (arg == nullptr) {
          return nullptr;
        }
        message->args.push_back(std::move(arg));
      }
    } else {
      message->message_send_form = Expr::MessageSendForm::Unary;
      message->selector_lowering_pieces.push_back(head_piece);
    }
    message->message_send_form_symbol = BuildMessageSendFormSymbol(message->message_send_form);
    message->selector_lowering_symbol = BuildMessageSendSelectorLoweringSymbol(message->selector_lowering_pieces);
    message->selector_lowering_is_normalized = true;
    message->dispatch_abi_receiver_slots_marshaled = 1u;
    message->dispatch_abi_selector_slots_marshaled = 1u;
    message->dispatch_abi_argument_value_slots_marshaled = static_cast<unsigned>(message->args.size());
    message->dispatch_abi_runtime_arg_slots = kDispatchAbiMarshallingRuntimeArgSlots;
    message->dispatch_abi_argument_padding_slots_marshaled = ComputeDispatchAbiArgumentPaddingSlots(
        message->args.size(), message->dispatch_abi_runtime_arg_slots);
    message->dispatch_abi_argument_total_slots_marshaled = message->dispatch_abi_argument_value_slots_marshaled +
                                                           message->dispatch_abi_argument_padding_slots_marshaled;
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
    // dispatch-surface classification anchor: super receivers stay explicit while direct dispatch remains reserved.
    // dispatch-site modeling anchor: parsing preserves raw receiver
    // spelling only; whole-program frontend normalization classifies
    // instance/class/super/direct/dynamic families before sema consumes the
    // message send.
    // dispatch legality/selector-resolution freeze anchor: message sends remain legal only for normalized unary/keyword selectors with an
    // explicit receiver form; unresolved or ambiguous selector resolution stays
    // fail-closed and direct dispatch remains reserved.
    // selector-resolution implementation anchor: parser still keeps
    // one normalized selector spelling plus explicit receiver syntax only;
    // lane-B sema is now responsible for exact concrete self/super/known-class
    // resolution and fail-closed ambiguity diagnostics.
    // super/direct/dynamic legality expansion anchor: parser still
    // does not invent direct-dispatch syntax or alternate receiver forms;
    // lane-B sema now owns rejecting illegal `super` sites while preserving the
    // same raw dynamic receiver spellings for runtime method-family accounting.
    // dispatch lowering ABI freeze anchor: parser continues to hand
    // selector text through as lowered cstring material only. Selector-handle
    // lookup and canonical runtime-dispatch cutover stay in lane-C lowering,
    // with the compatibility bridge remaining the default call target until
    // 
    // runtime call ABI generation anchor: parser still does not pick
    // the runtime entrypoint itself. Lowering now cuts normalized instance and
    // runtime ARC helper API surface anchor: parser-owned ARC and
    // ownership spelling remains purely descriptive; the private ARC helper
    // ABI stays a lowering/runtime concern rather than a source-level promise.
    // runtime ARC helper implementation anchor: parser-owned ARC and
    // ownership spelling remains purely descriptive, but the supported
    // property/weak/autorelease-return slice now continues into a live linked
    // runtime-helper path rather than stopping at a freeze-only boundary.
    // ownership-debug/runtime-validation anchor: parser-owned ARC
    // and ownership spelling still does not promise any source-level debug
    // surface; lane-D owns the private runtime snapshot hooks above these same
    // source packets.
    // class sends over to objc3_runtime_dispatch_i32 while deferred
    // super/dynamic/direct handling stays on the compatibility bridge until
    // 
    // live-dispatch cutover anchor: lowering now moves normalized
    // dynamic sends onto objc3_runtime_dispatch_i32 too, removing the last
    // live-path compatibility-bridge dependency while keeping reserved direct
    // dispatch fail closed in IR lowering.
    // lookup/dispatch runtime freeze anchor: parser still does not
    // own selector interning, metadata-backed lookup tables, method caches, or
    // slow-path runtime resolution. It hands normalized selector text and
    // receiver form through to the canonical runtime-owned lookup/dispatch
    // boundary for lane-D to extend later.
    // selector-table anchor: metadata-backed selector lookup tables
    // are now runtime-owned behind that preserved boundary.
    // method-cache / slow-path anchor: parser still does not resolve
    // methods from class metadata or own cache behavior. It preserves the
    // normalized selector text plus receiver-family classification that the
    // runtime now consumes for deterministic class/metaclass slow-path lookup.
    // protocol/category-aware resolution anchor: parser still does
    // not resolve category tiers or protocol declaration graphs. It preserves
    // the normalized selector text, receiver family, and protocol/category
    // declaration surface that the runtime now consumes for deterministic
    // category-backed live lookup and protocol-backed negative lookup evidence.
    // live-dispatch gate anchor: parser still only publishes the
    // normalized selector/receiver classification surface. Proof that sends
    // execute through the live runtime path rather than the non-authoritative test surface
    // remains a lane-E evidence responsibility rooted in C004/D004 and handed
    // off to E002 for smoke/closeout replacement.
    // live-dispatch smoke/replay closeout anchor: execution-smoke
    // proof consumes the canonical runtime dispatch symbol from the published
    // parser/sema handoff rather than treating the non-authoritative test surface as the
    // primary proof surface.
    message->super_dispatch_enabled = IsSuperDispatchReceiver(*message->receiver);
    message->super_dispatch_requires_class_context = message->super_dispatch_enabled;
    message->super_dispatch_symbol = BuildSuperDispatchSymbol(
        message->super_dispatch_enabled, message->super_dispatch_requires_class_context, message->message_send_form);
    message->super_dispatch_semantics_is_normalized = true;
    message->method_family_name = objc3c::support::ClassifyMethodFamilyFromSelector(message->selector);
    message->method_family_returns_retained_result = message->method_family_name == "init" ||
                                                     message->method_family_name == "copy" ||
                                                     message->method_family_name == "mutableCopy" ||
                                                     message->method_family_name == "new";
    message->method_family_returns_related_result = message->method_family_name == "init";
    message->method_family_semantics_symbol = BuildMethodFamilySemanticsSymbol(
        message->method_family_name, message->method_family_returns_retained_result,
        message->method_family_returns_related_result);
    message->method_family_semantics_is_normalized = true;
    message->runtime_link_host_link_required = message->nil_receiver_requires_runtime_dispatch;
    message->runtime_link_host_link_elided = !message->runtime_link_host_link_required;
    message->runtime_link_host_link_declaration_parameter_count = message->dispatch_abi_runtime_arg_slots + 2u;
    message->runtime_dispatch_bridge_symbol = kRuntimeLinkHostLinkDispatchSymbol;
    message->runtime_link_host_link_symbol = BuildRuntimeLinkHostLinkSymbol(
        message->runtime_link_host_link_required, message->runtime_link_host_link_elided,
        message->dispatch_abi_runtime_arg_slots, message->runtime_link_host_link_declaration_parameter_count,
        message->runtime_dispatch_bridge_symbol, message->message_send_form);
    message->runtime_link_host_link_is_normalized = true;
    message->dispatch_surface_kind = Expr::DispatchSurfaceKind::Unclassified;
    message->dispatch_surface_family_symbol.clear();
    message->dispatch_surface_entrypoint_family_symbol.clear();
    message->dispatch_surface_is_normalized = false;

    if (!Match(TokenKind::RBracket)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "missing ']' after message send expression"));
      return nullptr;
    }
    return message;
  }


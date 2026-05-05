            return false;
          }
        }
        return true;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return true;
        }
        if (!ValidateMessageSendArityExpr(stmt->switch_stmt->condition.get(), error)) {
          return false;
        }
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &case_body_stmt : case_stmt.body) {
            if (!ValidateMessageSendArityStmt(case_body_stmt.get(), error)) {
              return false;
            }
          }
        }
        return true;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return true;
        }
        if (!ValidateMessageSendArityExpr(stmt->while_stmt->condition.get(), error)) {
          return false;
        }
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          if (!ValidateMessageSendArityStmt(loop_stmt.get(), error)) {
            return false;
          }
        }
        return true;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt == nullptr) {
          return true;
        }
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          if (!ValidateMessageSendArityStmt(nested_stmt.get(), error)) {
            return false;
          }
        }
        return true;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return true;
    }
    return true;
  }

  bool ValidateMessageSendArityContract(std::string &error) const {
    for (const auto &global : program_.globals) {
      if (!ValidateMessageSendArityExpr(global.value.get(), error)) {
        return false;
      }
    }
    for (const auto &fn : program_.functions) {
      for (const auto &stmt : fn.body) {
        if (!ValidateMessageSendArityStmt(stmt.get(), error)) {
          return false;
        }
      }
    }
    for (const auto &implementation : program_.implementations) {
      for (const auto &method : implementation.methods) {
        if (!method.has_body) {
          continue;
        }
        for (const auto &stmt : method.body) {
          if (!ValidateMessageSendArityStmt(stmt.get(), error)) {
            return false;
          }
        }
      }
    }
    return true;
  }

  LoweredMessageSend LowerMessageSendHeader(const Expr *expr,
                                           FunctionContext &ctx) const {
    LoweredMessageSend lowered;
    lowered.args.assign(lowering_ir_boundary_.runtime_dispatch_arg_slots, "0");
    if (expr == nullptr) {
      return lowered;
    }

    lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);
    lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);
    lowered.receiver = EmitExpr(expr->receiver.get(), ctx);
    lowered.selector = expr->selector;
    lowered.dispatch_surface_family = expr->dispatch_surface_family_symbol;
    lowered.dispatch_surface_entrypoint_family =
        expr->dispatch_surface_entrypoint_family_symbol;
    lowered.dispatch_symbol =
        UsesCanonicalObjc3RuntimeDispatchEntrypoint(
            lowered.dispatch_surface_family)
            ? lowering_ir_boundary_.runtime_dispatch_symbol
            : Objc3DispatchSurfaceRuntimeEntrypointSymbol(
                  lowered.dispatch_surface_family);
    lowered.direct_call_symbol = TryResolveDirectDispatchSymbol(expr, ctx);
    return lowered;
  }

  void MaterializeMessageSendArgs(const Expr *expr, LoweredMessageSend &lowered,
                                  FunctionContext &ctx) const {
    if (expr == nullptr) {
      return;
    }
    lowered.explicit_arg_count = expr->args.size();
    for (std::size_t i = 0; i < expr->args.size() && i < lowered.args.size(); ++i) {
      lowered.args[i] = EmitExpr(expr->args[i].get(), ctx);
    }
  }

  std::string TryResolveDirectDispatchSymbol(const Expr *expr,
                                             const FunctionContext &ctx) const {
    if (expr == nullptr || expr->receiver == nullptr || expr->selector.empty()) {
      return {};
    }

    std::string owner_name;
    bool is_class_method = false;
    if (expr->receiver->kind == Expr::Kind::Identifier &&
        expr->receiver->ident == "self" &&
        !ctx.current_implementation_name.empty()) {
      owner_name = ctx.current_implementation_name;
      is_class_method = ctx.current_method_is_class_method;
    } else if (expr->receiver->kind == Expr::Kind::Identifier &&
               class_receiver_constants_.find(expr->receiver->ident) !=
                   class_receiver_constants_.end()) {
      owner_name = expr->receiver->ident;
      is_class_method = true;
    } else {
      return {};
    }

    const auto symbol_it = direct_dispatch_symbols_by_key_.find(
        BuildDirectDispatchMethodKey(owner_name, expr->selector,
                                     is_class_method));
    if (symbol_it == direct_dispatch_symbols_by_key_.end()) {
      return {};
    }
    return symbol_it->second;
  }

  LoweredMessageSend LowerMessageSendExpr(const Expr *expr, FunctionContext &ctx) const {
    LoweredMessageSend lowered = LowerMessageSendHeader(expr, ctx);
    MaterializeMessageSendArgs(expr, lowered, ctx);
    return lowered;
  }

  std::string EmitRuntimeDispatch(const LoweredMessageSend &lowered, FunctionContext &ctx) const {
    if (!lowered.direct_call_symbol.empty()) {
      // dispatch-control lowering anchor: concrete self/known-class
      // sends that target effective objc_direct methods now lower as exact LLVM
      // direct calls instead of routing through the runtime dispatch entrypoint.
      const std::string direct_value = NewTemp(ctx);
      std::ostringstream call;
      call << "  " << direct_value << " = call i32 "
           << lowered.direct_call_symbol << "(";
      for (std::size_t i = 0; i < lowered.explicit_arg_count; ++i) {
        if (i != 0) {
          call << ", ";
        }
        call << "i32 " << lowered.args[i];
      }
      call << ")";
      ctx.code_lines.push_back(call.str());
      ++direct_dispatch_call_sites_emitted_;
      InvalidateGlobalProofState(ctx);
      return direct_value;
    }

    if (RequiresFailClosedObjc3RuntimeDispatchError(
            lowered.dispatch_surface_family)) {
      return EmitUnsupportedI32Value(
          "direct dispatch lowering remains unsupported on the live runtime path");
    }

    const bool uses_canonical_runtime_entrypoint =
        UsesCanonicalObjc3RuntimeDispatchEntrypoint(
            lowered.dispatch_surface_family);
    if (lowered.receiver_is_compile_time_zero) {
      return "0";
    }

    auto selector_it = selector_pool_globals_.find(lowered.selector);
    if (selector_it == selector_pool_globals_.end()) {
      return EmitUnsupportedI32Value("missing selector global for message send selector '" + lowered.selector + "'");
    }

    const std::size_t selector_len = lowered.selector.size() + 1;
    const std::string selector_ptr = NewTemp(ctx);
    ctx.code_lines.push_back("  " + selector_ptr + " = getelementptr inbounds [" + std::to_string(selector_len) +
                             " x i8], ptr " + selector_it->second + ", i32 0, i32 0");
    ++selector_pool_gep_sites_emitted_;

    const auto emit_dispatch_call = [&](const std::string &dispatch_value) {
      // dispatch-surface classification anchor: instance/class/super/dynamic
      // message sends that survive folding all route through the live runtime family;
      // direct dispatch remains an explicit non-goal for this freeze.
      // dispatch legality/selector-resolution freeze anchor: lowering
      // consumes normalized selector text only, preserves explicit receiver
      // legality, and does not attempt overload or ambiguity recovery beyond the
      // fail-closed frontend contract.
      // selector-resolution implementation anchor: once lane-B sema
      // resolves concrete self/super/known-class receivers, lowering still
      // emits the same live runtime entrypoint family and relies on the
      // fail-closed exact-signature result instead of performing its own
      // overload recovery.
      // super/direct/dynamic legality expansion anchor: lowering
      // continues to route admitted super/dynamic sites through the live
      // runtime family, preserves their normalized method-family metadata, and
      // never synthesizes a reserved direct-dispatch entrypoint.
      // dispatch lowering ABI anchor: lowering emits the canonical runtime
      // entrypoint directly. The compatibility symbol stays exported only as a
      // non-emitted alias while selector lookup, receiver/result ABI, and the
      // fixed four-slot argument vector remain stable.
      // runtime call ABI generation anchor: normalized instance/class/super
      // and dynamic sends all call objc3_runtime_dispatch_i32 directly.
      // live-dispatch cutover anchor: supported dynamic sends now
      // join instance/class/super on objc3_runtime_dispatch_i32, nil semantics
      // for canonical surfaces stay runtime-owned, the compatibility symbol is
      // retained only as a non-emitted alias/test surface, and reserved direct
      // dispatch surfaces still fail closed before IR emission.
      // live-dispatch gate anchor: supported live sends must
      // continue to emit only objc3_runtime_dispatch_i32 calls here. The
      // non-authoritative test surface remains exported only as evidence/test surface, and
      // E002 is the first issue allowed to replace non-authoritative smoke/closeout
      // assumptions with the integrated live-dispatch gate.
      // live-dispatch smoke/replay closeout anchor: smoke and replay
      // now treat canonical runtime dispatch evidence as authoritative, so
      // emitted live sends must continue to surface objc3_runtime_dispatch_i32
      // even for nil-result paths that return 0 through the runtime.
      // lookup/dispatch runtime freeze anchor: emitted IR still
      // targets only the canonical lookup/dispatch boundary and does not
      // materialize runtime selector-table, method-cache, or slow-path helper
      // symbols. Those runtime-owned details stay behind the frozen
      // objc3_runtime_lookup_selector / objc3_runtime_dispatch_i32 surface
      // until later lane-D issues extend them explicitly.
      std::ostringstream call;
      call << "  " << dispatch_value << " = call i32 @" << lowered.dispatch_symbol << "(i32 "
           << lowered.receiver << ", ptr " << selector_ptr;
      for (const std::string &arg : lowered.args) {
        call << ", i32 " << arg;
      }
      call << ")";
      runtime_dispatch_call_emitted_ = true;
      ++runtime_dispatch_call_sites_emitted_;
      runtime_dispatch_symbols_used_.insert(lowered.dispatch_symbol);
      ctx.code_lines.push_back(call.str());
    };

    if (uses_canonical_runtime_entrypoint ||
        lowered.receiver_is_compile_time_nonzero) {
      const std::string dispatch_value = NewTemp(ctx);
      emit_dispatch_call(dispatch_value);
      InvalidateGlobalProofState(ctx);
      return dispatch_value;
    }

    const std::string is_nil = NewTemp(ctx);
    const std::string nil_label = NewLabel(ctx, "msg_nil_");
    const std::string dispatch_label = NewLabel(ctx, "msg_dispatch_");
    const std::string merge_label = NewLabel(ctx, "msg_merge_");
    const std::string dispatch_value = NewTemp(ctx);
    const std::string out = NewTemp(ctx);
    ctx.code_lines.push_back("  " + is_nil + " = icmp eq i32 " + lowered.receiver + ", 0");
    ctx.code_lines.push_back("  br i1 " + is_nil + ", label %" + nil_label + ", label %" + dispatch_label);
    ctx.code_lines.push_back(nil_label + ":");
    ctx.code_lines.push_back("  br label %" + merge_label);
    ctx.code_lines.push_back(dispatch_label + ":");
    emit_dispatch_call(dispatch_value);
    ctx.code_lines.push_back("  br label %" + merge_label);
    ctx.code_lines.push_back(merge_label + ":");
    ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label + "], [" + dispatch_value + ", %" +
                             dispatch_label + "]");
    InvalidateGlobalProofState(ctx);
    return out;
  }

  std::string EmitMessageSendExpr(const Expr *expr, FunctionContext &ctx) const {
    if (expr != nullptr && expr->optional_send_enabled) {
      LoweredMessageSend lowered = LowerMessageSendHeader(expr, ctx);
      if (lowered.receiver_is_compile_time_zero) {
        return "0";
      }
      if (lowered.receiver_is_compile_time_nonzero) {
        MaterializeMessageSendArgs(expr, lowered, ctx);
        return EmitRuntimeDispatch(lowered, ctx);
      }

      const std::string is_nil = NewTemp(ctx);
      const std::string nil_label = NewLabel(ctx, "opt_send_nil_");
      const std::string dispatch_label = NewLabel(ctx, "opt_send_dispatch_");
      const std::string merge_label = NewLabel(ctx, "opt_send_merge_");
      const std::string out = NewTemp(ctx);

      ctx.code_lines.push_back("  " + is_nil + " = icmp eq i32 " +
                               lowered.receiver + ", 0");
      ctx.code_lines.push_back("  br i1 " + is_nil + ", label %" + nil_label +
                               ", label %" + dispatch_label);
      ctx.code_lines.push_back(nil_label + ":");
      ctx.code_lines.push_back("  br label %" + merge_label);
      ctx.code_lines.push_back(dispatch_label + ":");
      lowered.receiver_is_compile_time_zero = false;
      lowered.receiver_is_compile_time_nonzero = true;
      MaterializeMessageSendArgs(expr, lowered, ctx);
      const std::string dispatch_value = EmitRuntimeDispatch(lowered, ctx);
      ctx.code_lines.push_back("  br label %" + merge_label);
      ctx.code_lines.push_back(merge_label + ":");
      ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label +
                               "], [" + dispatch_value + ", %" +
                               dispatch_label + "]");
      return out;
    }
    const LoweredMessageSend lowered = LowerMessageSendExpr(expr, ctx);
    return EmitRuntimeDispatch(lowered, ctx);
  }

  std::string EmitExpr(const Expr *expr, FunctionContext &ctx) const {
    if (expr == nullptr) {
      return EmitUnsupportedI32Value("null expression reached IR lowering");
    }
    switch (expr->kind) {
      case Expr::Kind::Number:
        return std::to_string(expr->number);
      case Expr::Kind::BoolLiteral:
        return expr->bool_value ? "1" : "0";
      case Expr::Kind::NilLiteral:
        return "0";
      case Expr::Kind::BlockLiteral:
        if (BlockLiteralSupportsEscapingRuntimeHookLowering(*expr)) {
          const std::string storage_ptr = EmitBlockLiteralStorage(*expr, ctx);
          if (storage_ptr == "poison") {
            return storage_ptr;
          }
          return EmitPromotedBlockHandle(*expr, storage_ptr, ctx);
        }
        return EmitUnsupportedI32Value(
            "block literal values must be bound to a local name before use");
      case Expr::Kind::Identifier: {
        if (expr->typed_keypath_literal_enabled) {
          return EmitTypedKeyPathLiteralValue(*expr);
        }
        return EmitIdentifierValue(expr->ident, ctx);
      }
      case Expr::Kind::Binary: {
        if (expr->op == "&&" || expr->op == "||") {
          const std::string lhs = EmitExpr(expr->left.get(), ctx);
          const std::string lhs_i1 = NewTemp(ctx);
          const std::string rhs_label = NewLabel(ctx, expr->op == "&&" ? "and_rhs_" : "or_rhs_");
          const std::string rhs_done_label = NewLabel(ctx, expr->op == "&&" ? "and_rhs_done_" : "or_rhs_done_");
          const std::string short_label = NewLabel(ctx, expr->op == "&&" ? "and_short_" : "or_short_");
          const std::string merge_label = NewLabel(ctx, expr->op == "&&" ? "and_merge_" : "or_merge_");
          const std::string rhs_i1 = NewTemp(ctx);
          const std::string logical_i1 = NewTemp(ctx);
          const std::string out_i32 = NewTemp(ctx);
          const std::string short_value = expr->op == "&&" ? "0" : "1";

          ctx.code_lines.push_back("  " + lhs_i1 + " = icmp ne i32 " + lhs + ", 0");
          if (expr->op == "&&") {
            ctx.code_lines.push_back(
                "  br i1 " + lhs_i1 + ", label %" + rhs_label + ", label %" + short_label);
          } else {
            ctx.code_lines.push_back(
                "  br i1 " + lhs_i1 + ", label %" + short_label + ", label %" + rhs_label);
          }

          ctx.code_lines.push_back(rhs_label + ":");
          const std::string rhs = EmitExpr(expr->right.get(), ctx);
          ctx.code_lines.push_back("  br label %" + rhs_done_label);
          ctx.code_lines.push_back(rhs_done_label + ":");
          ctx.code_lines.push_back("  " + rhs_i1 + " = icmp ne i32 " + rhs + ", 0");
          ctx.code_lines.push_back("  br label %" + merge_label);

          ctx.code_lines.push_back(short_label + ":");
          ctx.code_lines.push_back("  br label %" + merge_label);

          ctx.code_lines.push_back(merge_label + ":");
          ctx.code_lines.push_back("  " + logical_i1 + " = phi i1 [" + short_value + ", %" + short_label +
                                   "], [" + rhs_i1 + ", %" + rhs_done_label + "]");
          ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " + logical_i1 + " to i32");
          return out_i32;
        }
        if (expr->op == "??") {
          const std::string lhs = EmitExpr(expr->left.get(), ctx);
          const std::string lhs_i1 = NewTemp(ctx);
          const std::string rhs_label = NewLabel(ctx, "coalesce_rhs_");
          const std::string lhs_label = NewLabel(ctx, "coalesce_lhs_");
          const std::string merge_label = NewLabel(ctx, "coalesce_merge_");
          const std::string rhs_value_name = NewTemp(ctx);
          const std::string out_value = NewTemp(ctx);
          ctx.code_lines.push_back("  " + lhs_i1 + " = icmp ne i32 " + lhs +
                                   ", 0");
          ctx.code_lines.push_back("  br i1 " + lhs_i1 + ", label %" +
                                   lhs_label + ", label %" + rhs_label);
          ctx.code_lines.push_back(rhs_label + ":");
          const std::string rhs = EmitExpr(expr->right.get(), ctx);
          ctx.code_lines.push_back("  " + rhs_value_name + " = add i32 " + rhs +
                                   ", 0");
          ctx.code_lines.push_back("  br label %" + merge_label);
          ctx.code_lines.push_back(lhs_label + ":");
          ctx.code_lines.push_back("  br label %" + merge_label);
          ctx.code_lines.push_back(merge_label + ":");
          ctx.code_lines.push_back("  " + out_value + " = phi i32 [" + lhs +
                                   ", %" + lhs_label + "], [" + rhs_value_name +
                                   ", %" + rhs_label + "]");
          return out_value;
        }

        const std::string lhs = EmitExpr(expr->left.get(), ctx);
        const std::string rhs = EmitExpr(expr->right.get(), ctx);
        if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%") {
          const std::string tmp = NewTemp(ctx);
          std::string op = "add";
          if (expr->op == "+") {
            op = "add";
          } else if (expr->op == "-") {
            op = "sub";
          } else if (expr->op == "*") {
            op = "mul";
          } else if (expr->op == "/") {
            op = "sdiv";
          } else if (expr->op == "%") {
            op = "srem";
          }
          ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs + ", " + rhs);
          return tmp;
        }

        if (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>") {
          const std::string tmp = NewTemp(ctx);
          std::string op = "and";
          if (expr->op == "&") {
            op = "and";
          } else if (expr->op == "|") {
            op = "or";
          } else if (expr->op == "^") {
            op = "xor";
          } else if (expr->op == "<<") {
            op = "shl";
          } else if (expr->op == ">>") {
            op = "ashr";
          }
          ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs + ", " + rhs);
          return tmp;
        }

        std::string pred;
        if (expr->op == "==") {
          pred = "eq";
        } else if (expr->op == "!=") {
          pred = "ne";
        } else if (expr->op == "<") {
          pred = "slt";
        } else if (expr->op == "<=") {
          pred = "sle";
        } else if (expr->op == ">") {
          pred = "sgt";
        } else if (expr->op == ">=") {
          pred = "sge";
        } else {
          return EmitUnsupportedI32Value("unsupported binary operator '" + expr->op + "'");
        }
        const std::string cmp_i1 = NewTemp(ctx);
        const std::string out_i32 = NewTemp(ctx);
        ctx.code_lines.push_back("  " + cmp_i1 + " = icmp " + pred + " i32 " + lhs + ", " + rhs);
        ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " + cmp_i1 + " to i32");
        return out_i32;
      }
      case Expr::Kind::Conditional: {
        const std::string cond_value = EmitExpr(expr->left.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        const std::string true_label = NewLabel(ctx, "cond_true_");
        const std::string false_label = NewLabel(ctx, "cond_false_");
        const std::string merge_label = NewLabel(ctx, "cond_merge_");
        const std::string result_ptr = "%cond.addr." + std::to_string(ctx.temp_counter++);
        ctx.entry_lines.push_back("  " + result_ptr + " = alloca i32, align 4");
        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond_value + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + true_label + ", label %" + false_label);

        ctx.code_lines.push_back(true_label + ":");
        const std::string true_value = EmitExpr(expr->right.get(), ctx);
        ctx.code_lines.push_back("  store i32 " + true_value + ", ptr " + result_ptr + ", align 4");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(false_label + ":");
        const std::string false_value = EmitExpr(expr->third.get(), ctx);
        ctx.code_lines.push_back("  store i32 " + false_value + ", ptr " + result_ptr + ", align 4");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(merge_label + ":");
        const std::string out_value = NewTemp(ctx);
        ctx.code_lines.push_back("  " + out_value + " = load i32, ptr " + result_ptr + ", align 4");
        return out_value;
      }
      case Expr::Kind::Call: {
        if (expr->ident == "__objc3_throw_stmt") {
          const std::string error_value =
              expr->args.empty() ? "1" : EmitExpr(expr->args.front().get(), ctx);
          EmitPropagateThrownError(error_value, ctx);
          return "0";
        }
        if (expr->ident == "__objc3_try_expr") {
          const Expr *operand =
              !expr->args.empty() ? expr->args.front().get() : expr->left.get();
          if (operand == nullptr || operand->kind != Expr::Kind::Call) {
            return EmitUnsupportedI32Value(
                "try lowering expected callable operand");
          }
          const LoweredFunctionSignature *operand_signature =
              LookupFunctionSignature(operand->ident);
          if (operand_signature == nullptr) {
            return EmitUnsupportedI32Value(
                "try lowering requires declared callable signature");
          }
          const std::string result_ptr =
              "%try.result.addr." + std::to_string(ctx.temp_counter++);
          const std::string error_slot =
              BuildThrowsErrorSlotAlloca(ctx, "try");
          ctx.entry_lines.push_back("  " + result_ptr + " = alloca i32, align 4");
          const std::string merged_label = NewLabel(ctx, "try_merge_");
          const std::string failure_label = NewLabel(ctx, "try_fail_");
          const std::string success_label = NewLabel(ctx, "try_success_");
          ctx.code_lines.push_back("  store i32 0, ptr " + error_slot + ", align 4");
          bool bridge_failed = false;
          std::string bridge_error_value = "0";
          std::string result = EmitDirectFunctionCall(
              operand, operand_signature, ctx, error_slot,
              &bridge_failed, &bridge_error_value);
          std::string actual_result = result;
          std::string failure_cond;
          if (bridge_failed) {
            const std::size_t marker = result.rfind('|');
            actual_result = result.substr(0, marker);
            failure_cond = result.substr(marker + 1);
          } else if (operand_signature->throws_declared) {
            const std::string loaded_error = NewTemp(ctx);
            const std::string has_error = NewTemp(ctx);
            ctx.code_lines.push_back("  " + loaded_error + " = load i32, ptr " +
                                     error_slot + ", align 4");
            ctx.code_lines.push_back("  " + has_error + " = icmp ne i32 " +
                                     loaded_error + ", 0");
            failure_cond = has_error;
            bridge_error_value = loaded_error;
          } else {
            return EmitUnsupportedI32Value(
                "try lowering requires throwing or bridged operand");
          }
          ctx.code_lines.push_back("  br i1 " + failure_cond + ", label %" +
                                   failure_label + ", label %" + success_label);
          ctx.code_lines.push_back(success_label + ":");
          ctx.code_lines.push_back("  store i32 " + actual_result + ", ptr " +
                                   result_ptr + ", align 4");
          ctx.code_lines.push_back("  br label %" + merged_label);
          ctx.code_lines.push_back(failure_label + ":");
          switch (expr->try_operator_kind) {
          case Expr::TryOperatorKind::Optional:
            ctx.code_lines.push_back("  store i32 0, ptr " + result_ptr +
                                     ", align 4");
            ctx.code_lines.push_back("  br label %" + merged_label);
            break;
          case Expr::TryOperatorKind::Forced:
            ctx.code_lines.push_back("  call void @abort()");
            ctx.code_lines.push_back("  unreachable");
            break;
          case Expr::TryOperatorKind::Propagate:
            EmitPropagateThrownError(bridge_error_value, ctx);
            break;
          case Expr::TryOperatorKind::None:
            ctx.code_lines.push_back("  store i32 " + actual_result + ", ptr " +
                                     result_ptr + ", align 4");
            ctx.code_lines.push_back("  br label %" + merged_label);
            break;
          }
          if (expr->try_operator_kind == Expr::TryOperatorKind::Forced ||
              expr->try_operator_kind == Expr::TryOperatorKind::Propagate) {
            // The failure arm terminates, but the success arm still flows
            // through the merged result block.
            ctx.terminated = false;
          }
          ctx.code_lines.push_back(merged_label + ":");
          const std::string loaded = NewTemp(ctx);
          ctx.code_lines.push_back("  " + loaded + " = load i32, ptr " +
                                   result_ptr + ", align 4");
          return loaded;
        }
        const auto local_block_it = ctx.block_bindings.find(expr->ident);
        if (local_block_it != ctx.block_bindings.end()) {
          return EmitBlockInvokeCall(local_block_it->second, expr, ctx);
        }
        const LoweredFunctionSignature *signature = LookupFunctionSignature(expr->ident);
        if (signature != nullptr && signature->throws_declared) {
          const std::string ignored_error_slot =
              BuildThrowsErrorSlotAlloca(ctx, "ignored");
          ctx.code_lines.push_back("  store i32 0, ptr " + ignored_error_slot +
                                   ", align 4");
          return EmitDirectFunctionCall(expr, signature, ctx, ignored_error_slot);
        }
        // implementation anchor: supported await-marked expressions
        // currently reach native IR through the operand's direct-call lowering
        // path. This emits runnable IR/object code for the non-suspending happy
        // slice without materializing continuation allocation or a state
        // machine; those surfaces remain later work.
        return EmitDirectFunctionCall(expr, signature, ctx, "");
      }
      case Expr::Kind::MessageSend: {
        return EmitMessageSendExpr(expr, ctx);
      }
    }
    return EmitUnsupportedI32Value("unsupported expression kind reached IR lowering");
  }

  std::string EmitUnsupportedI32Value(const std::string &reason) const {
    if (!unsupported_fail_closed_path_triggered_) {
      unsupported_fail_closed_path_triggered_ = true;
      unsupported_fail_closed_path_reason_ = reason;
    }
    return "poison";
  }

  void EmitStatement(const Stmt *stmt, FunctionContext &ctx) const {
    if (stmt == nullptr || ctx.terminated) {
      return;
    }

    switch (stmt->kind) {
      case Stmt::Kind::Let: {
        const LetStmt *let = stmt->let_stmt.get();
        if (let == nullptr || ctx.scopes.empty()) {
          return;
        }
        if (let->value != nullptr && let->value->kind == Expr::Kind::BlockLiteral) {
          const std::string storage_ptr = EmitBlockLiteralStorage(*let->value, ctx);
          if (storage_ptr == "poison") {
            return;
          }
          ctx.block_bindings[let->name] =
              BlockBinding{storage_ptr, let->value.get(), ""};
          return;
        }
        if (let->value != nullptr && let->value->kind == Expr::Kind::Identifier) {
          const auto existing_block_it =
              ctx.block_bindings.find(let->value->ident);
          if (existing_block_it != ctx.block_bindings.end()) {
            ctx.block_bindings[let->name] = existing_block_it->second;
            return;
          }
        }
        // Evaluate the initializer against the currently visible scope first so
        // shadowing declarations can read the previous binding deterministically.
        const std::string value = EmitExpr(let->value.get(), ctx);
        if (ctx.terminated) {
          return;
        }
        int let_const_value = 0;
        const bool has_let_const_value = TryGetCompileTimeI32ExprInContext(let->value.get(), ctx, let_const_value);
        const bool has_let_nil_value = IsCompileTimeNilReceiverExprInContext(let->value.get(), ctx);
        const std::string ptr = "%" + let->name + ".addr." + std::to_string(ctx.temp_counter++);
        ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
        ctx.scopes.back()[let->name] = ptr;
        if (has_let_nil_value) {
          ctx.nil_bound_ptrs.insert(ptr);
        }
        if (has_let_const_value) {
          ctx.const_value_ptrs[ptr] = let_const_value;
        }
        if (has_let_const_value && let_const_value != 0) {
          ctx.nonzero_bound_ptrs.insert(ptr);
        }
        ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
        if (let->cleanup_attribute_declared || let->cleanup_sugar_declared ||
            let->resource_attribute_declared || let->resource_sugar_declared) {
          PendingOwnershipCleanupCall cleanup_call;
          cleanup_call.binding_name = let->name;
          cleanup_call.storage_ptr = ptr;
          cleanup_call.cleanup_function_symbol = let->cleanup_function_symbol;
          cleanup_call.resource_close_symbol = let->resource_close_symbol;
          if (!let->resource_close_symbol.empty()) {
            int invalid_value = 0;
            if (!let->resource_invalid_expression.empty() &&
                !ParseOwnershipResourceInvalidLiteral(
                    let->resource_invalid_expression, invalid_value)) {
              (void)EmitUnsupportedI32Value(
                  "resource invalid expression for '" + let->name +
                  "' must stay an integer literal in the current Part 8 lowering slice");
              return;
            }
            cleanup_call.has_resource_invalid_value =
                !let->resource_invalid_expression.empty();
            cleanup_call.resource_invalid_value = invalid_value;
          }
          ctx.ownership_cleanup_call_indices[let->name] =
              ctx.pending_ownership_cleanup_calls.size();
          ctx.pending_ownership_cleanup_calls.push_back(std::move(cleanup_call));
        }
        return;
      }
      case Stmt::Kind::Return: {
        const ReturnStmt *ret = stmt->return_stmt.get();
        if (ret == nullptr) {
          return;
        }
        if (ret->value == nullptr) {
          EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
          EmitTypedReturn("0", ctx);
        } else {
          const std::string value = EmitExpr(ret->value.get(), ctx);
          if (ctx.terminated) {
            return;
          }
          EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
          EmitTypedReturn(value, ctx);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Assign: {
        const AssignStmt *assign = stmt->assign_stmt.get();
        if (assign == nullptr) {
          return;
        }
        const std::string ptr = LookupVarPtr(ctx, assign->name);
        if (ptr.empty() && ctx.block_bindings.find(assign->name) != ctx.block_bindings.end()) {
          (void)EmitUnsupportedI32Value(
              "reassigning block values is not yet runnable in Objective-C 3 native mode");
          return;
        }
        EmitAssignmentStore(ptr, assign->op, assign->value.get(), ctx);
        return;
      }
      case Stmt::Kind::Break: {
        if (ctx.control_stack.empty()) {
          EmitTerminalCleanupToDepth(ctx, 0u, 0u, 0u, 0u, 0u);
          ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
        } else {
          EmitTerminalCleanupToDepth(
              ctx, ctx.control_stack.back().scope_depth,
              ctx.control_stack.back().autoreleasepool_depth,
              ctx.control_stack.back().pending_block_dispose_depth,
              ctx.control_stack.back().ownership_cleanup_depth,
              ctx.control_stack.back().arc_cleanup_depth);
          ctx.code_lines.push_back("  br label %" + ctx.control_stack.back().break_label);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Continue: {
        std::string continue_label;
        std::size_t continue_autoreleasepool_depth =
            ctx.autoreleasepool_scope_symbols.size();
        for (auto it = ctx.control_stack.rbegin(); it != ctx.control_stack.rend(); ++it) {
          if (it->continue_allowed) {
            continue_label = it->continue_label;
            continue_autoreleasepool_depth = it->autoreleasepool_depth;
            break;
          }
        }
        if (continue_label.empty()) {
          EmitTerminalCleanupToDepth(ctx, 0u, 0u, 0u, 0u, 0u);
          ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
        } else {
          const ControlLabels &target = *std::find_if(
              ctx.control_stack.rbegin(), ctx.control_stack.rend(),
              [&continue_label](const ControlLabels &labels) {
                return labels.continue_allowed &&
                       labels.continue_label == continue_label;
              });
          EmitTerminalCleanupToDepth(
              ctx, target.scope_depth, continue_autoreleasepool_depth,
              target.pending_block_dispose_depth, target.ownership_cleanup_depth,
              target.arc_cleanup_depth);
          ctx.code_lines.push_back("  br label %" + continue_label);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Empty:
        return;
      case Stmt::Kind::Block: {
        const BlockStmt *block_stmt = stmt->block_stmt.get();
        if (block_stmt == nullptr) {
          return;
        }
        if (block_stmt->is_do_catch_scope) {
          const std::size_t autoreleasepool_depth =
              ctx.autoreleasepool_scope_symbols.size();
          const std::string dispatch_label =
              NewLabel(ctx, "do_catch_dispatch_");
          const std::string merge_label = NewLabel(ctx, "do_catch_end_");
          const std::string error_slot =
              BuildThrowsErrorSlotAlloca(ctx, "do_catch");
          ctx.code_lines.push_back("  store i32 0, ptr " + error_slot +
                                   ", align 4");
          PushScope(ctx);
          ctx.error_handler_stack.push_back({error_slot,
                                             dispatch_label,
                                             ctx.scopes.size() - 1u,
                                             ctx.autoreleasepool_scope_symbols.size(),
                                             ctx.pending_block_dispose_calls.size(),
                                             ctx.pending_ownership_cleanup_calls.size(),
                                             ctx.arc_owned_cleanup_ptrs.size()});
          for (const auto &nested_stmt : block_stmt->body) {
            EmitStatement(nested_stmt.get(), ctx);
            if (ctx.terminated) {
              break;
            }
          }
          const bool body_terminated = ctx.terminated;
          ctx.error_handler_stack.pop_back();
          PopScope(ctx, !body_terminated);
          if (!body_terminated) {
            EmitAutoreleasepoolUnwindToDepth(ctx, autoreleasepool_depth);
            ctx.code_lines.push_back("  br label %" + merge_label);
          }
          ctx.code_lines.push_back(dispatch_label + ":");
          const std::string loaded_error = EmitLoadThrownError(error_slot, ctx);
          const std::string no_match_label =
              NewLabel(ctx, "do_catch_no_match_");
          for (std::size_t clause_index = 0;
               clause_index < block_stmt->catch_clauses.size(); ++clause_index) {
            const auto &clause = block_stmt->catch_clauses[clause_index];
            const std::string catch_label =
                NewLabel(ctx, clause.catch_all ? "catch_all_" : "catch_");
            const std::string next_label =
                clause_index + 1u < block_stmt->catch_clauses.size()
                    ? NewLabel(ctx, "catch_next_")
                    : no_match_label;
            const std::string catches = NewTemp(ctx);
            const std::string catches_bool = NewTemp(ctx);
            ctx.code_lines.push_back(
                "  " + catches + " = call i32 @" +
                std::string(kObjc3RuntimeCatchMatchesErrorI32Symbol) + "(i32 " +
                loaded_error + ", i32 " +
                std::to_string(ErrorHandlingCatchKindForTypeSpelling(
                    clause.binding_type_spelling)) +
                ", i32 " + (clause.catch_all ? "1" : "0") + ")");
            ctx.code_lines.push_back("  " + catches_bool + " = icmp ne i32 " +
                                     catches + ", 0");
            ctx.code_lines.push_back("  br i1 " + catches_bool + ", label %" +
                                     catch_label + ", label %" + next_label);
            ctx.code_lines.push_back(catch_label + ":");
            PushScope(ctx);
            if (clause.has_binding && !clause.binding_name.empty()) {
              const std::string ptr =
                  "%" + clause.binding_name + ".addr." +
                  std::to_string(ctx.temp_counter++);
              ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
              ctx.code_lines.push_back("  store i32 " + loaded_error + ", ptr " +
                                       ptr + ", align 4");
              ctx.scopes.back()[clause.binding_name] = ptr;
            }
            ctx.terminated = false;
            for (const auto &catch_stmt : clause.body) {
              EmitStatement(catch_stmt.get(), ctx);
              if (ctx.terminated) {
                break;
              }
            }
            const bool catch_terminated = ctx.terminated;
            PopScope(ctx, !catch_terminated);
            if (!catch_terminated) {
              ctx.code_lines.push_back("  br label %" + merge_label);
            }
            ctx.code_lines.push_back(next_label + ":");
          }
          EmitPropagateThrownError(loaded_error, ctx);
          ctx.code_lines.push_back(merge_label + ":");
          ctx.terminated = false;
          return;
        }
        const std::size_t autoreleasepool_depth =
            ctx.autoreleasepool_scope_symbols.size();
        if (block_stmt->is_autoreleasepool_scope) {
          // integration anchor: the current non-suspending async
          // slice reuses the existing autoreleasepool scope hooks and later
          // composes them with deferred cleanup emission on scope exit. There
          // is still no separate suspension-frame cleanup runtime here.
          ctx.code_lines.push_back(
              "  call void @" +
              std::string(kObjc3RuntimePushAutoreleasepoolScopeSymbol) + "()");
          ctx.autoreleasepool_scope_symbols.push_back(
              block_stmt->autoreleasepool_scope_symbol);
        }
        PushScope(ctx);
        for (const auto &nested_stmt : block_stmt->body) {
          EmitStatement(nested_stmt.get(), ctx);
        }
        PopScope(ctx, !ctx.terminated);
        if (!ctx.terminated) {
          EmitAutoreleasepoolUnwindToDepth(ctx, autoreleasepool_depth);
        }
        return;
      }
      case Stmt::Kind::Defer: {
        const BlockStmt *block_stmt = stmt->block_stmt.get();
        if (block_stmt == nullptr || ctx.pending_defer_scope_blocks.empty()) {
          return;
        }
        ctx.pending_defer_scope_blocks.back().push_back(block_stmt);
        return;
      }
      case Stmt::Kind::Expr: {
        const ExprStmt *expr_stmt = stmt->expr_stmt.get();
        if (expr_stmt != nullptr) {
          (void)EmitExpr(expr_stmt->value.get(), ctx);
        }
        return;
      }
      case Stmt::Kind::While: {
        const WhileStmt *while_stmt = stmt->while_stmt.get();
        if (while_stmt == nullptr) {
          return;
        }

        const std::string cond_label = NewLabel(ctx, "while_cond_");
        const std::string body_label = NewLabel(ctx, "while_body_");
        const std::string end_label = NewLabel(ctx, "while_end_");
        ctx.code_lines.push_back("  br label %" + cond_label);

        ctx.code_lines.push_back(cond_label + ":");
        const std::string cond = EmitExpr(while_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + body_label + ", label %" + end_label);

        ctx.code_lines.push_back(body_label + ":");
        PushScope(ctx);
        ctx.control_stack.push_back(
            {cond_label, end_label, true, ctx.scopes.size() - 1u,
             ctx.autoreleasepool_scope_symbols.size(),
             ctx.pending_block_dispose_calls.size(),
             ctx.pending_ownership_cleanup_calls.size(),
             ctx.arc_owned_cleanup_ptrs.size()});
        ctx.terminated = false;
        for (const auto &s : while_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        PopScope(ctx, !body_terminated);
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + cond_label);
        }
        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::DoWhile: {
        const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
        if (do_while_stmt == nullptr) {
          return;
        }

        const std::string body_label = NewLabel(ctx, "do_body_");
        const std::string cond_label = NewLabel(ctx, "do_cond_");
        const std::string end_label = NewLabel(ctx, "do_end_");
        ctx.code_lines.push_back("  br label %" + body_label);

        ctx.code_lines.push_back(body_label + ":");
        PushScope(ctx);
        ctx.control_stack.push_back(
            {cond_label, end_label, true, ctx.scopes.size() - 1u,
             ctx.autoreleasepool_scope_symbols.size(),
             ctx.pending_block_dispose_calls.size(),
             ctx.pending_ownership_cleanup_calls.size(),
             ctx.arc_owned_cleanup_ptrs.size()});
        ctx.terminated = false;
        for (const auto &s : do_while_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        PopScope(ctx, !body_terminated);
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + cond_label);
        }

        ctx.code_lines.push_back(cond_label + ":");
        const std::string cond = EmitExpr(do_while_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + body_label + ", label %" + end_label);

        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::For: {
        const ForStmt *for_stmt = stmt->for_stmt.get();
        if (for_stmt == nullptr) {
          return;
        }

        PushScope(ctx);
        EmitForClause(for_stmt->init, ctx);

        const std::string cond_label = NewLabel(ctx, "for_cond_");
        const std::string body_label = NewLabel(ctx, "for_body_");
        const std::string step_label = NewLabel(ctx, "for_step_");
        const std::string end_label = NewLabel(ctx, "for_end_");

        ctx.code_lines.push_back("  br label %" + cond_label);
        ctx.code_lines.push_back(cond_label + ":");
        if (for_stmt->condition == nullptr) {
          ctx.code_lines.push_back("  br label %" + body_label);
        } else {
          const std::string cond = EmitExpr(for_stmt->condition.get(), ctx);
          const std::string cond_i1 = NewTemp(ctx);
          ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
          ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + body_label + ", label %" + end_label);
        }

        ctx.code_lines.push_back(body_label + ":");
        PushScope(ctx);
        ctx.control_stack.push_back(
            {step_label, end_label, true, ctx.scopes.size() - 1u,
             ctx.autoreleasepool_scope_symbols.size(),
             ctx.pending_block_dispose_calls.size(),
             ctx.pending_ownership_cleanup_calls.size(),
             ctx.arc_owned_cleanup_ptrs.size()});
        ctx.terminated = false;
        for (const auto &s : for_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        PopScope(ctx, !body_terminated);
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + step_label);
        }

        ctx.code_lines.push_back(step_label + ":");
        EmitForClause(for_stmt->step, ctx);
        ctx.code_lines.push_back("  br label %" + cond_label);

        ctx.code_lines.push_back(end_label + ":");
        PopScope(ctx, true);
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::Switch: {
        const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
        if (switch_stmt == nullptr) {
          return;
        }

        const std::string condition_value = EmitExpr(switch_stmt->condition.get(), ctx);
        const std::string end_label = NewLabel(ctx, "switch_end_");

        if (switch_stmt->match_surface_enabled) {
          // match lowering anchor: statement-form match now lowers
          // literal/default/wildcard/binding arms as a distinct control-flow
          // carrier with case-local binding storage and no switch-style
          // fallthrough, while result-case payload matching remains fail-closed
          // until a runtime Result ABI exists.
          std::vector<std::string> arm_labels;
          arm_labels.reserve(switch_stmt->cases.size());
          for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
            const SwitchCase &case_stmt = switch_stmt->cases[i];
            arm_labels.push_back(case_stmt.is_default
                                     ? NewLabel(ctx, "match_default_")
                                     : NewLabel(ctx, "match_case_"));
          }

          if (!switch_stmt->cases.empty()) {
            std::vector<std::string> test_labels;
            test_labels.reserve(switch_stmt->cases.size());
            for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
              test_labels.push_back(NewLabel(ctx, "match_test_"));
            }
            ctx.code_lines.push_back("  br label %" + test_labels.front());
            for (std::size_t case_index = 0; case_index < switch_stmt->cases.size(); ++case_index) {
              const SwitchCase &case_stmt = switch_stmt->cases[case_index];
              const std::string next_label =
                  case_index + 1u < switch_stmt->cases.size() ? test_labels[case_index + 1u] : end_label;
              ctx.code_lines.push_back(test_labels[case_index] + ":");
              if (case_stmt.is_default ||
                  case_stmt.match_pattern_kind == MatchPatternKind::Wildcard ||
                  case_stmt.match_pattern_kind == MatchPatternKind::Binding) {
                ctx.code_lines.push_back("  br label %" + arm_labels[case_index]);
                continue;
              }
              if (case_stmt.match_pattern_kind == MatchPatternKind::LiteralInteger ||
                  case_stmt.match_pattern_kind == MatchPatternKind::LiteralBool ||
                  case_stmt.match_pattern_kind == MatchPatternKind::LiteralNil) {
                const std::string cmp = NewTemp(ctx);
                ctx.code_lines.push_back("  " + cmp + " = icmp eq i32 " + condition_value + ", " +
                                         std::to_string(case_stmt.value));
                ctx.code_lines.push_back("  br i1 " + cmp + ", label %" + arm_labels[case_index] +
                                         ", label %" + next_label);
                continue;
              }
              if (case_stmt.match_pattern_kind == MatchPatternKind::ResultCase) {
                EmitUnsupportedI32Value(
                    "result-case match lowering remains fail-closed until a runtime Result payload ABI lands");
                return;
              }
              EmitUnsupportedI32Value("unsupported match pattern reached IR lowering");
              return;
            }
          } else {
            ctx.code_lines.push_back("  br label %" + end_label);
          }

          for (std::size_t arm_index = 0; arm_index < switch_stmt->cases.size(); ++arm_index) {
            const SwitchCase &case_stmt = switch_stmt->cases[arm_index];
            ctx.code_lines.push_back(arm_labels[arm_index] + ":");
            PushScope(ctx);
            if (!case_stmt.match_binding_name.empty() &&
                (case_stmt.match_pattern_kind == MatchPatternKind::Binding)) {
              const std::string ptr =
                  "%" + case_stmt.match_binding_name + ".addr." + std::to_string(ctx.temp_counter++);
              ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
              ctx.code_lines.push_back("  store i32 " + condition_value + ", ptr " + ptr + ", align 4");
              ctx.scopes.back()[case_stmt.match_binding_name] = ptr;
            }
            ctx.control_stack.push_back(
                {"", end_label, false, ctx.scopes.size() - 1u,
                 ctx.autoreleasepool_scope_symbols.size(),
                 ctx.pending_block_dispose_calls.size(),
                 ctx.pending_ownership_cleanup_calls.size(),
                 ctx.arc_owned_cleanup_ptrs.size()});
            ctx.terminated = false;
            for (const auto &case_body_stmt : case_stmt.body) {
              EmitStatement(case_body_stmt.get(), ctx);
            }
            const bool arm_terminated = ctx.terminated;
            ctx.control_stack.pop_back();
            PopScope(ctx, !arm_terminated);

            if (!arm_terminated) {
              ctx.code_lines.push_back("  br label %" + end_label);
            }
          }

          ctx.code_lines.push_back(end_label + ":");
          ctx.terminated = false;
          return;
        }

        std::vector<std::string> arm_labels;
        arm_labels.reserve(switch_stmt->cases.size());
        std::vector<std::size_t> case_clause_indices;
        case_clause_indices.reserve(switch_stmt->cases.size());
        std::size_t default_index = switch_stmt->cases.size();

        for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
          const SwitchCase &case_stmt = switch_stmt->cases[i];
          if (case_stmt.is_default) {
            arm_labels.push_back(NewLabel(ctx, "switch_default_"));
            if (default_index == switch_stmt->cases.size()) {
              default_index = i;
            }
          } else {
            arm_labels.push_back(NewLabel(ctx, "switch_case_"));
            case_clause_indices.push_back(i);
          }
        }

        const std::string default_label =
            default_index < switch_stmt->cases.size() ? arm_labels[default_index] : end_label;

        if (!case_clause_indices.empty()) {
          std::vector<std::string> test_labels;
          test_labels.reserve(case_clause_indices.size());
          for (std::size_t i = 0; i < case_clause_indices.size(); ++i) {
            test_labels.push_back(NewLabel(ctx, "switch_test_"));
          }

          ctx.code_lines.push_back("  br label %" + test_labels[0]);
          for (std::size_t test_index = 0; test_index < case_clause_indices.size(); ++test_index) {
            const std::size_t case_index = case_clause_indices[test_index];
            const std::string next_label =
                (test_index + 1 < case_clause_indices.size()) ? test_labels[test_index + 1] : default_label;

            ctx.code_lines.push_back(test_labels[test_index] + ":");
            const std::string cmp = NewTemp(ctx);
            ctx.code_lines.push_back("  " + cmp + " = icmp eq i32 " + condition_value + ", " +
                                     std::to_string(switch_stmt->cases[case_index].value));
            ctx.code_lines.push_back("  br i1 " + cmp + ", label %" + arm_labels[case_index] + ", label %" +
                                     next_label);
          }
        } else {
          ctx.code_lines.push_back("  br label %" + default_label);
        }

        for (std::size_t arm_index = 0; arm_index < switch_stmt->cases.size(); ++arm_index) {
          const SwitchCase &case_stmt = switch_stmt->cases[arm_index];
          ctx.code_lines.push_back(arm_labels[arm_index] + ":");
        PushScope(ctx);
        ctx.control_stack.push_back(
            {"", end_label, false, ctx.scopes.size() - 1u,
             ctx.autoreleasepool_scope_symbols.size(),
             ctx.pending_block_dispose_calls.size(),
             ctx.pending_ownership_cleanup_calls.size(),
             ctx.arc_owned_cleanup_ptrs.size()});
          ctx.terminated = false;
          for (const auto &case_body_stmt : case_stmt.body) {
            EmitStatement(case_body_stmt.get(), ctx);
          }
          const bool arm_terminated = ctx.terminated;
          ctx.control_stack.pop_back();
          PopScope(ctx, !arm_terminated);

          if (!arm_terminated) {
            if (arm_index + 1 < switch_stmt->cases.size()) {
              ctx.code_lines.push_back("  br label %" + arm_labels[arm_index + 1]);
            } else {
              ctx.code_lines.push_back("  br label %" + end_label);
            }
          }
        }

        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::If: {
        const IfStmt *if_stmt = stmt->if_stmt.get();
        if (if_stmt == nullptr) {
          return;
        }
        if (if_stmt->optional_binding_surface_enabled ||
            if_stmt->guard_condition_list_surface_enabled) {
          EmitOptionalBindingIfStatement(if_stmt, ctx);
          return;
        }

        const std::string cond = EmitExpr(if_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        const std::string then_label = NewLabel(ctx, "if_then_");
        const std::string else_label = NewLabel(ctx, "if_else_");
        const std::string merge_label = NewLabel(ctx, "if_end_");

        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + then_label + ", label %" + else_label);

        ctx.code_lines.push_back(then_label + ":");
        PushScope(ctx);
        ctx.terminated = false;
        for (const auto &s : if_stmt->then_body) {
          EmitStatement(s.get(), ctx);
        }
        const bool then_terminated = ctx.terminated;
        PopScope(ctx, !then_terminated);
        if (!then_terminated) {
          ctx.code_lines.push_back("  br label %" + merge_label);
        }

        ctx.code_lines.push_back(else_label + ":");
        PushScope(ctx);
        ctx.terminated = false;
        for (const auto &s : if_stmt->else_body) {
          EmitStatement(s.get(), ctx);
        }
        const bool else_terminated = ctx.terminated;
        PopScope(ctx, !else_terminated);
        if (!else_terminated) {
          ctx.code_lines.push_back("  br label %" + merge_label);
        }

        if (then_terminated && else_terminated) {
          ctx.terminated = true;
        } else {
          ctx.code_lines.push_back(merge_label + ":");
          ctx.terminated = false;
        }
        return;
      }
    }
  }

  void InvalidateGlobalProofState(FunctionContext &ctx) const {
    ctx.global_proofs_invalidated = true;
    for (auto it = ctx.nil_bound_ptrs.begin(); it != ctx.nil_bound_ptrs.end();) {
      if (it->rfind("@", 0) == 0) {
        it = ctx.nil_bound_ptrs.erase(it);
      } else {
        ++it;
      }
    }
    for (auto it = ctx.nonzero_bound_ptrs.begin(); it != ctx.nonzero_bound_ptrs.end();) {
      if (it->rfind("@", 0) == 0) {
        it = ctx.nonzero_bound_ptrs.erase(it);
      } else {
        ++it;
      }
    }
    for (auto it = ctx.const_value_ptrs.begin(); it != ctx.const_value_ptrs.end();) {
      if (it->first.rfind("@", 0) == 0) {
        it = ctx.const_value_ptrs.erase(it);
      } else {
        ++it;
      }
    }
  }

  void EmitPrototypeDeclarations(std::ostringstream &out) const {
    bool emitted = false;
    std::unordered_set<std::string> declared_symbols;
    const auto requires_arc_helper_declarations = [&]() {
      for (const auto &fn : program_.functions) {
        if (EffectiveArcReturnInsertRetain(
                fn, frontend_metadata_.arc_mode_enabled) ||
            fn.return_ownership_insert_release ||
            EffectiveArcReturnInsertAutorelease(fn)) {
          return true;
        }
        for (const auto &param : fn.params) {
          if (EffectiveArcParamInsertRetain(
                  param, frontend_metadata_.arc_mode_enabled) ||
              EffectiveArcParamInsertRelease(
                  param, frontend_metadata_.arc_mode_enabled) ||
              param.ownership_insert_autorelease) {
            return true;
          }
        }
      }
      for (const auto &method_def : method_definitions_) {
        if (method_def.method == nullptr) {
          continue;
        }
        const Objc3MethodDecl &method = *method_def.method;
        if (EffectiveArcReturnInsertRetain(
                method, frontend_metadata_.arc_mode_enabled) ||
            method.return_ownership_insert_release ||
            EffectiveArcReturnInsertAutorelease(method)) {
          return true;
        }
        for (const auto &param : method.params) {
          if (EffectiveArcParamInsertRetain(
                  param, frontend_metadata_.arc_mode_enabled) ||
              EffectiveArcParamInsertRelease(
                  param, frontend_metadata_.arc_mode_enabled) ||
              param.ownership_insert_autorelease) {
            return true;
          }
        }
      }
      return false;
    };
    const auto emit_declaration_once =
        [&](const std::string &symbol, const std::string &declaration) {
          if (symbol.empty() || !declared_symbols.insert(symbol).second) {
            return false;
          }
          out << declaration;
          emitted = true;
          return true;
        };
    emit_declaration_once("abort", "declare void @abort()\n");
    if (ShouldEmitRuntimeBootstrapLowering()) {
      emit_declaration_once(
          kObjc3RuntimeBootstrapStageRegistrationTableSymbol,
          "declare void @" +
              std::string(kObjc3RuntimeBootstrapStageRegistrationTableSymbol) +
              "(ptr)\n");
      emit_declaration_once(
          frontend_metadata_
              .runtime_bootstrap_lowering_registration_entrypoint_symbol,
          "declare i32 @" +
              frontend_metadata_
                  .runtime_bootstrap_lowering_registration_entrypoint_symbol +
              "(ptr)\n");
    }
    if (synthesized_property_accessor_count_ > 0u ||
        requires_arc_helper_declarations() ||
        !frontend_metadata_.lowering_error_handling_throws_abi_propagation_replay_key
             .empty() ||
        !frontend_metadata_.lowering_async_continuation_replay_key.empty() ||
        !frontend_metadata_
             .lowering_await_lowering_suspension_state_replay_key.empty() ||
        frontend_metadata_.autoreleasepool_scope_lowering_scope_sites > 0u ||
        frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites >
            0u ||
        frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites > 0u ||
        frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites > 0u) {
      emit_declaration_once(kObjc3RuntimeReadCurrentPropertyI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeReadCurrentPropertyI32Symbol) +
                                "()\n");
      emit_declaration_once(kObjc3RuntimeWriteCurrentPropertyI32Symbol,
                            "declare void @" +
                                std::string(
                                    kObjc3RuntimeWriteCurrentPropertyI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeExchangeCurrentPropertyI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeExchangeCurrentPropertyI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeStoreThrownErrorI32Symbol,
                            "declare void @" +
                                std::string(
                                    kObjc3RuntimeStoreThrownErrorI32Symbol) +
                                "(ptr, i32)\n");

    }
    const std::string promoted =
        EmitPromotedBlockHandle(*binding.literal, binding.storage_ptr, ctx,
                                true);
    if (promoted == "poison") {
      return promoted;
    }
    binding.promoted_handle_ptr =
        "%block.promoted.addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + binding.promoted_handle_ptr +
                              " = alloca i32, align 4");
    ctx.code_lines.push_back("  store i32 " + promoted + ", ptr " +
                             binding.promoted_handle_ptr + ", align 4");
    return promoted;
  }

  std::string EmitIdentifierValue(const std::string &name,
                                  FunctionContext &ctx) const {
    auto block_it = ctx.block_bindings.find(name);
    if (block_it != ctx.block_bindings.end()) {
      return EmitPromotedBlockHandleLoad(block_it->second, ctx);
    }
    const std::string ptr = LookupVarPtr(ctx, name);
    if (!ptr.empty()) {
      const std::string tmp = NewTemp(ctx);
      ctx.code_lines.push_back("  " + tmp + " = load i32, ptr " + ptr + ", align 4");
      return tmp;
    }
    if (globals_.find(name) != globals_.end()) {
      const std::string tmp = NewTemp(ctx);
      ctx.code_lines.push_back("  " + tmp + " = load i32, ptr @" + name + ", align 4");
      return tmp;
    }
    const auto immediate_it = ctx.immediate_identifiers.find(name);
    if (immediate_it != ctx.immediate_identifiers.end()) {
      return std::to_string(immediate_it->second);
    }
    // Match bindings are now materialized by the executable Part 5 lowering
    // path when a live match arm captures the condition value. Remaining
    // unresolved identifiers still fail closed here.
    return EmitUnsupportedI32Value("unresolved identifier '" + name + "' during IR lowering");
  }

  std::string EmitTypedKeyPathLiteralValue(const Expr &expr) const {
    const std::string profile =
        expr.typed_keypath_literal_profile.empty()
            ? std::string("typed-keypath:root=") + expr.typed_keypath_root_name
            : expr.typed_keypath_literal_profile;
    const auto artifact_it = typed_keypath_artifacts_.find(profile);
    if (artifact_it == typed_keypath_artifacts_.end()) {
      return EmitUnsupportedI32Value(
          "typed key-path artifact '" + profile +
          "' was not registered before IR lowering");
    }
    return std::to_string(
        static_cast<unsigned long long>(artifact_it->second.ordinal + 1u));
  }

  void EmitBlockInvokeThunk(const Expr &expr) const {
    // byref-cell/copy-helper/dispose-helper anchor: each runnable
    // local block literal now receives one internal invoke thunk definition
    // that rehydrates readonly captures from snapshot cells and mutated captures
    // from stack byref-cell references.
    const std::string symbol = BuildBlockInvokeSymbol(expr);
    if (symbol.empty() || !emitted_block_invoke_symbols_.insert(symbol).second) {
      return;
    }

    std::ostringstream out;
    out << "define internal i32 @" << symbol
        << "(ptr %block, i32 %arg0, i32 %arg1, i32 %arg2, i32 %arg3) {\n";
    out << "entry:\n";

    FunctionContext ctx;
    ctx.return_type = ValueType::I32;
    PushScope(ctx);

    const std::string block_storage_type = BuildBlockStorageType(expr);
    const bool pointer_capture_storage =
        BlockLiteralUsesPointerCaptureStorage(expr);
    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size(); ++i) {
      const std::string &capture_name = expr.block_capture_names_lexicographic[i];
      const std::string slot_ptr = NewTemp(ctx);
      const std::size_t capture_field_index =
          pointer_capture_storage ? 3u : 1u;
      ctx.entry_lines.push_back("  " + slot_ptr + " = getelementptr inbounds " +
                                block_storage_type +
                                ", ptr %block, i32 0, i32 " +
                                std::to_string(capture_field_index) +
                                ", i32 " + std::to_string(i));
      if (pointer_capture_storage) {
        const std::string capture_ptr = NewTemp(ctx);
        ctx.entry_lines.push_back("  " + capture_ptr + " = load ptr, ptr " +
                                  slot_ptr + ", align 8");
        ctx.scopes.back()[capture_name] = capture_ptr;
        continue;
      }
      const std::string ptr =
          "%" + capture_name + ".addr." + std::to_string(ctx.temp_counter++);
      const std::string value = NewTemp(ctx);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      ctx.scopes.back()[capture_name] = ptr;
      ctx.entry_lines.push_back("  " + value + " = load i32, ptr " + slot_ptr +
                                ", align 4");
      ctx.entry_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                                ", align 4");
    }

    for (std::size_t i = 0; i < expr.block_parameters_source_order.size() && i < 4u; ++i) {
      const auto &parameter = expr.block_parameters_source_order[i];
      const std::string ptr =
          "%" + parameter.name + ".addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      ctx.entry_lines.push_back("  store i32 %arg" + std::to_string(i) + ", ptr " + ptr + ", align 4");
      ctx.scopes.back()[parameter.name] = ptr;
    }

    for (const auto &stmt : expr.block_body) {
      EmitStatement(stmt.get(), ctx);
      if (ctx.terminated) {
        break;
      }
    }

    if (!ctx.terminated) {
      EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
      EmitOwnershipCleanupUnwindToDepth(ctx, 0u);
      EmitPendingBlockDisposeHelpers(ctx);
      EmitArcOwnedCleanupReleases(ctx);
      ctx.code_lines.push_back("  ret i32 0");
    }

    for (const auto &line : ctx.entry_lines) {
      out << line << "\n";
    }
    for (const auto &line : ctx.code_lines) {
      out << line << "\n";
    }
    out << "}\n";
    block_function_definitions_.push_back(out.str());
  }

  std::string EmitBlockLiteralStorage(const Expr &expr,
                                      FunctionContext &ctx) const {
    // byref-cell/copy-helper/dispose-helper anchor: the current live
    // lowering slice now supports non-escaping byref and owned-capture block
    // objects through stack snapshot/byref cells plus emitted helper bodies.
    // lowering-implementation anchor: cleanup/resource-backed move
    // captures now lower through the same stack/local helper path. The
    // unsupported boundary is promotion, not local helper materialization.
    if (BlockLiteralRequiresFutureRuntimeLanes(expr)) {
      return EmitUnsupportedI32Value(
          "block literal requires escaping heap-promotion or runtime-managed copy/dispose lowering that lands in later runtime work");
    }
    if (expr.block_parameter_count > 4u) {
      return EmitUnsupportedI32Value(
          "block literal exceeds current runnable invoke-thunk arity limit of 4");
    }

    EmitBlockInvokeThunk(expr);
    EmitBlockCopyHelper(expr);
    EmitBlockDisposeHelper(expr, ctx);

    const std::string storage_type = BuildBlockStorageType(expr);
    const bool pointer_capture_storage =
        BlockLiteralUsesPointerCaptureStorage(expr);
    const std::string storage_ptr =
        "%block.literal.addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + storage_ptr + " = alloca " + storage_type + ", align 8");

    const std::string invoke_ptr_slot = NewTemp(ctx);
    ctx.code_lines.push_back("  " + invoke_ptr_slot + " = getelementptr inbounds " +
                             storage_type + ", ptr " + storage_ptr + ", i32 0, i32 0");
    ctx.code_lines.push_back("  store ptr @" + BuildBlockInvokeSymbol(expr) + ", ptr " +
                             invoke_ptr_slot + ", align 8");

    if (pointer_capture_storage) {
      const std::string copy_helper_slot = NewTemp(ctx);
      const std::string dispose_helper_slot = NewTemp(ctx);
      ctx.code_lines.push_back("  " + copy_helper_slot +
                               " = getelementptr inbounds " + storage_type +
                               ", ptr " + storage_ptr + ", i32 0, i32 1");
      ctx.code_lines.push_back("  store ptr " +
                               std::string(expr.block_runtime_copy_helper_required
                                               ? "@" + BuildBlockCopyHelperSymbol(expr)
                                               : "null") +
                               ", ptr " + copy_helper_slot + ", align 8");
      ctx.code_lines.push_back("  " + dispose_helper_slot +
                               " = getelementptr inbounds " + storage_type +
                               ", ptr " + storage_ptr + ", i32 0, i32 2");
      ctx.code_lines.push_back("  store ptr " +
                               std::string(expr.block_runtime_dispose_helper_required
                                               ? "@" + BuildBlockDisposeHelperSymbol(expr)
                                               : "null") +
                               ", ptr " + dispose_helper_slot + ", align 8");
    }

    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size(); ++i) {
      const std::string &capture_name = expr.block_capture_names_lexicographic[i];
      const std::string capture_slot = NewTemp(ctx);
      if (pointer_capture_storage) {
        std::string capture_cell_ptr;
        const auto moved_capture_it = std::find_if(
            expr.block_explicit_capture_items_source_order.begin(),
            expr.block_explicit_capture_items_source_order.end(),
            [&capture_name](const Expr::ExplicitBlockCaptureItem &item) {
              return item.name == capture_name && item.mode == "move";
            });
        if (moved_capture_it != expr.block_explicit_capture_items_source_order.end()) {
          const auto cleanup_it = ctx.ownership_cleanup_call_indices.find(capture_name);
          if (cleanup_it == ctx.ownership_cleanup_call_indices.end()) {
            return EmitUnsupportedI32Value(
                "move capture '" + capture_name +
                "' was not registered as a cleanup/resource-backed local");
          }
          PendingOwnershipCleanupCall &cleanup_call =
              ctx.pending_ownership_cleanup_calls[cleanup_it->second];
          cleanup_call.active = false;
          capture_cell_ptr = cleanup_call.storage_ptr;
        } else if (SortedStringListContains(
                expr.block_byref_capture_names_lexicographic, capture_name)) {
          capture_cell_ptr = LookupVarPtr(ctx, capture_name);
          if (capture_cell_ptr.empty()) {
            return EmitUnsupportedI32Value(
                "block literal byref capture '" + capture_name +
                "' could not be resolved during IR lowering");
          }
        } else {
          capture_cell_ptr = "%" + capture_name + ".capture.addr." +
                             std::to_string(ctx.temp_counter++);
          ctx.entry_lines.push_back("  " + capture_cell_ptr +
                                    " = alloca i32, align 4");
          const std::string capture_value = EmitIdentifierValue(capture_name, ctx);
          ctx.code_lines.push_back("  store i32 " + capture_value + ", ptr " +
                                   capture_cell_ptr + ", align 4");
        }
        ctx.code_lines.push_back("  " + capture_slot +
                                 " = getelementptr inbounds " + storage_type +
                                 ", ptr " + storage_ptr +
                                 ", i32 0, i32 3, i32 " + std::to_string(i));
        ctx.code_lines.push_back("  store ptr " + capture_cell_ptr + ", ptr " +
                                 capture_slot + ", align 8");
        continue;
      }
      const std::string capture_value = EmitIdentifierValue(capture_name, ctx);
      ctx.code_lines.push_back("  " + capture_slot + " = getelementptr inbounds " +
                               storage_type + ", ptr " + storage_ptr + ", i32 0, i32 1, i32 " +
                               std::to_string(i));
      ctx.code_lines.push_back("  store i32 " + capture_value + ", ptr " + capture_slot +
                               ", align 4");
    }

    if (pointer_capture_storage && expr.block_runtime_copy_helper_required) {
      ctx.code_lines.push_back("  call void @" + BuildBlockCopyHelperSymbol(expr) +
                               "(ptr " + storage_ptr + ")");
    }
    if (pointer_capture_storage && expr.block_runtime_dispose_helper_required) {
      ctx.pending_block_dispose_calls.push_back(
          PendingBlockDisposeCall{BuildBlockDisposeHelperSymbol(expr),
                                  storage_ptr});
    }

    return storage_ptr;
  }

  std::string EmitBlockInvokeCall(const BlockBinding &binding,
                                  const Expr *call_expr,
                                  FunctionContext &ctx) const {
    if (binding.literal == nullptr) {
      return EmitUnsupportedI32Value(
          "missing block literal metadata for local callable invocation");
    }

    if (!binding.promoted_handle_ptr.empty()) {
      std::array<std::string, 4> args{"0", "0", "0", "0"};
      for (std::size_t i = 0; i < call_expr->args.size() && i < args.size();
           ++i) {
        args[i] = EmitExpr(call_expr->args[i].get(), ctx);
      }
      const std::string handle = NewTemp(ctx);
      ctx.code_lines.push_back("  " + handle + " = load i32, ptr " +
                               binding.promoted_handle_ptr + ", align 4");
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back(
          "  " + out + " = call i32 @" +
          std::string(kObjc3RuntimeInvokeBlockI32Symbol) + "(i32 " + handle +
          ", i32 " + args[0] + ", i32 " + args[1] + ", i32 " + args[2] +
          ", i32 " + args[3] + ")");
      return out;
    }

    const std::string storage_type = BuildBlockStorageType(*binding.literal);
    const std::string invoke_ptr_slot = NewTemp(ctx);
    const std::string invoke_ptr = NewTemp(ctx);
    ctx.code_lines.push_back("  " + invoke_ptr_slot + " = getelementptr inbounds " +
                             storage_type + ", ptr " + binding.storage_ptr + ", i32 0, i32 0");
    ctx.code_lines.push_back("  " + invoke_ptr + " = load ptr, ptr " + invoke_ptr_slot +
                             ", align 8");

    std::array<std::string, 4> args{"0", "0", "0", "0"};
    for (std::size_t i = 0; i < call_expr->args.size() && i < args.size(); ++i) {
      args[i] = EmitExpr(call_expr->args[i].get(), ctx);
    }

    const std::string out = NewTemp(ctx);
    ctx.code_lines.push_back("  " + out + " = call i32 " + invoke_ptr + "(ptr " +
                             binding.storage_ptr + ", i32 " + args[0] + ", i32 " + args[1] +
                             ", i32 " + args[2] + ", i32 " + args[3] + ")");
    return out;
  }

  static int NextNonZeroReceiverIdentityValue(std::size_t ordinal, int salt) {
    constexpr int kBase = 1024;
    constexpr int kStride = 17;
    return kBase + static_cast<int>(ordinal * kStride) + salt;
  }

  void CollectKnownClassReceiverConstants() {
    std::set<std::string> class_names;
    for (const auto &interface_decl : program_.interfaces) {
      if (!interface_decl.has_category && !interface_decl.name.empty()) {
        class_names.insert(interface_decl.name);
      }
    }
    for (const auto &implementation : program_.implementations) {
      if (!implementation.has_category && !implementation.name.empty()) {
        class_names.insert(implementation.name);
      }
    }
    std::size_t ordinal = 0;
    for (const std::string &class_name : class_names) {
      class_receiver_constants_[class_name] =
          NextNonZeroReceiverIdentityValue(ordinal++, 0);
    }
  }

  int LookupClassReceiverIdentityValue(const std::string &class_name) const {
    const auto value_it = class_receiver_constants_.find(class_name);
    if (value_it == class_receiver_constants_.end()) {
      return 0;
    }
    return value_it->second;
  }

  static int BuildInstanceReceiverIdentityValue(int class_identity) {
    return class_identity == 0 ? 0 : class_identity + 1;
  }

  static int BuildClassReceiverIdentityValue(int class_identity) {
    return class_identity == 0 ? 0 : class_identity + 2;
  }

  void SeedKnownClassReceiverBindings(FunctionContext &ctx) const {
    for (const auto &entry : class_receiver_constants_) {
      ctx.immediate_identifiers.emplace(entry.first, entry.second);
    }
  }

  std::string CoerceI32ToBoolI1(const std::string &i32_value, FunctionContext &ctx) const {
    const std::string bool_i1 = NewTemp(ctx);
    ctx.code_lines.push_back("  " + bool_i1 + " = icmp ne i32 " + i32_value + ", 0");
    return bool_i1;
  }

  std::string CoerceValueToI32(const std::string &value, ValueType value_type, FunctionContext &ctx) const {
    if (value_type != ValueType::Bool) {
      return value;
    }
    const std::string widened = NewTemp(ctx);
    ctx.code_lines.push_back("  " + widened + " = zext i1 " + value + " to i32");
    return widened;
  }

  const LoweredFunctionSignature *LookupFunctionSignature(const std::string &name) const {
    auto signature_it = function_signatures_.find(name);
    if (signature_it == function_signatures_.end()) {
      return nullptr;
    }
    return &signature_it->second;
  }

  std::string BuildThrowsErrorSlotAlloca(FunctionContext &ctx,
                                         const std::string &prefix) const {
    const std::string slot = "%" + prefix + ".error.addr." +
                             std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + slot + " = alloca i32, align 4");
    return slot;
  }

  void EmitStoreThrownError(const std::string &error_value,
                            const std::string &slot,
                            FunctionContext &ctx) const {
    if (slot.empty()) {
      return;
    }
    ctx.code_lines.push_back("  call void @" +
                             std::string(kObjc3RuntimeStoreThrownErrorI32Symbol) +
                             "(ptr " + slot + ", i32 " + error_value + ")");
  }

  std::string EmitLoadThrownError(const std::string &slot,
                                  FunctionContext &ctx) const {
    if (slot.empty()) {
      return "0";
    }
    const std::string loaded = NewTemp(ctx);
    ctx.code_lines.push_back("  " + loaded + " = call i32 @" +
                             std::string(kObjc3RuntimeLoadThrownErrorI32Symbol) +
                             "(ptr " + slot + ")");
    return loaded;
  }

  void EmitPropagateThrownError(const std::string &error_value,
                                FunctionContext &ctx) const {
    if (!ctx.error_handler_stack.empty()) {
      const auto &handler = ctx.error_handler_stack.back();
      EmitStoreThrownError(error_value, handler.error_slot_ptr, ctx);
      EmitTerminalCleanupToDepth(ctx, handler.scope_depth,
                                 handler.autoreleasepool_depth,
                                 handler.pending_block_dispose_depth,
                                 handler.ownership_cleanup_depth,
                                 handler.arc_cleanup_depth);
      ctx.code_lines.push_back("  br label %" + handler.dispatch_label);
      ctx.terminated = true;
      return;
    }
    if (!ctx.function_error_out_param.empty()) {
      EmitStoreThrownError(error_value, ctx.function_error_out_param, ctx);
      EmitTypedReturn("0", ctx);
      ctx.terminated = true;
      return;
    }
    ctx.code_lines.push_back("  call void @abort()");
    ctx.code_lines.push_back("  unreachable");
    ctx.terminated = true;
  }

  static int ErrorHandlingCatchKindForTypeSpelling(const std::string &spelling) {
    std::string normalized;
    normalized.reserve(spelling.size());
    for (unsigned char ch : spelling) {
      if (!std::isspace(ch)) {
        normalized.push_back(
            static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
      }
    }
    if (normalized == "nserror" || normalized == "nserror*" ||
        normalized == "nserror**") {
      return 1;
    }
    if (normalized == "id<error>" || normalized == "id<error>*") {
      return 2;
    }
    return 0;
  }

  std::string EmitDirectFunctionCall(const Expr *expr,
                                     const LoweredFunctionSignature *signature,
                                     FunctionContext &ctx,
                                     const std::string &throws_error_slot_ptr,
                                     bool *bridge_failed_out = nullptr,
                                     std::string *bridge_error_value_out = nullptr) const {
    std::vector<std::string> args;
    std::vector<std::string> post_call_release_values;
    args.reserve(expr->args.size() + (signature != nullptr && signature->throws_declared ? 1u : 0u));
    post_call_release_values.reserve(expr->args.size());
    for (std::size_t i = 0; i < expr->args.size(); ++i) {
      std::string arg_i32 = EmitExpr(expr->args[i].get(), ctx);
      if (signature != nullptr && i < signature->param_insert_retain.size()) {
        if (signature->param_insert_retain[i]) {
          const std::string retained_value = NewTemp(ctx);
          ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                                   std::string(kObjc3RuntimeRetainI32Symbol) +
                                   "(i32 " + arg_i32 + ")");
          arg_i32 = retained_value;
        }
        if (signature->param_insert_autorelease[i]) {
          const std::string autoreleased_value = NewTemp(ctx);
          ctx.code_lines.push_back("  " + autoreleased_value +
                                   " = call i32 @" +
                                   std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
                                   "(i32 " + arg_i32 + ")");
          arg_i32 = autoreleased_value;
        }
        if (signature->param_insert_release[i]) {
          post_call_release_values.push_back(arg_i32);
        }
      }
      const ValueType expected_type =
          signature != nullptr && i < signature->param_types.size() ? signature->param_types[i] : ValueType::I32;
      AppendLoweredCallArg(args, arg_i32, expected_type, ctx);
    }
    if (signature != nullptr && signature->throws_declared) {
      args.push_back("ptr " + throws_error_slot_ptr);
    }
    std::ostringstream arglist;
    for (std::size_t i = 0; i < args.size(); ++i) {
      if (i != 0) {
        arglist << ", ";
      }
      arglist << args[i];
    }
    const ValueType return_type =
        signature != nullptr ? signature->return_type : ValueType::I32;
    const std::string llvm_return_type = LLVMScalarType(return_type);
    const bool call_may_have_global_side_effects =
        FunctionMayHaveGlobalSideEffects(expr->ident);
    std::string out = "0";
    if (TryEmitConcurrencyActorLoweringCall(expr, ctx, out)) {
      // lowering anchor: actor helper spellings inside actor methods
      // now lower through the private runtime helper slice rather than staying
      // as ordinary direct-call placeholders.
    } else if (TryEmitConcurrencyTaskRuntimeLoweringCall(expr, ctx, out)) {
      // lowering anchor: supported task/executor/cancellation
      // symbols now route through the private Part 7 runtime helper cluster
      // rather than remaining ordinary extern-call placeholders.
    } else if (return_type == ValueType::Void) {
      ctx.code_lines.push_back("  call " + llvm_return_type + " @" + expr->ident +
                               "(" + arglist.str() + ")");
    } else {
      const std::string tmp = NewTemp(ctx);
      ctx.code_lines.push_back("  " + tmp + " = call " + llvm_return_type + " @" +
                               expr->ident + "(" + arglist.str() + ")");
      out = CoerceValueToI32(tmp, return_type, ctx);
    }
    if (call_may_have_global_side_effects) {
      InvalidateGlobalProofState(ctx);
    }
    for (const auto &release_value : post_call_release_values) {
      const std::string released_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + released_value + " = call i32 @" +
                               std::string(kObjc3RuntimeReleaseI32Symbol) +
                               "(i32 " + release_value + ")");
      (void)released_value;
    }

    if (bridge_failed_out != nullptr) {
      *bridge_failed_out = false;
    }
    if (bridge_error_value_out != nullptr) {
      *bridge_error_value_out = "0";
    }

    if (signature != nullptr && signature->objc_status_code_declared) {
      const std::string is_success = NewTemp(ctx);
      ctx.code_lines.push_back("  " + is_success + " = icmp eq i32 " + out +
                               ", " +
                               std::to_string(signature->objc_status_code_success_literal));
      const std::string bridge_failed = NewTemp(ctx);
      ctx.code_lines.push_back("  " + bridge_failed + " = xor i1 " + is_success +
                               ", true");
      if (bridge_failed_out != nullptr) {
        *bridge_failed_out = true;
      }
      if (bridge_error_value_out != nullptr) {
        std::string raw_bridge_error = out;
        if (!signature->objc_status_code_mapping_symbol.empty()) {
          const LoweredFunctionSignature *mapping_signature =
              LookupFunctionSignature(signature->objc_status_code_mapping_symbol);
          if (mapping_signature != nullptr && mapping_signature->return_type == ValueType::Void) {
            ctx.code_lines.push_back("  call void @" +
                                     signature->objc_status_code_mapping_symbol +
                                     "(i32 " + out + ")");
          } else {
            const std::string mapped = NewTemp(ctx);
            ctx.code_lines.push_back("  " + mapped + " = call i32 @" +
                                     signature->objc_status_code_mapping_symbol +
                                     "(i32 " + out + ")");
            raw_bridge_error = mapped;
          }
        }
        const std::string bridged_error = NewTemp(ctx);
        ctx.code_lines.push_back("  " + bridged_error + " = call i32 @" +
                                 std::string(kObjc3RuntimeBridgeStatusErrorI32Symbol) +
                                 "(i32 " + out + ", i32 " + raw_bridge_error + ")");
        *bridge_error_value_out = bridged_error;
      }
      return out + "|" + bridge_failed;
    }

    if (signature != nullptr && signature->objc_nserror_declared) {
      const std::string is_success = NewTemp(ctx);
      ctx.code_lines.push_back("  " + is_success + " = icmp ne i32 " + out + ", 0");
      const std::string bridge_failed = NewTemp(ctx);
      ctx.code_lines.push_back("  " + bridge_failed + " = xor i1 " + is_success + ", true");
      if (bridge_failed_out != nullptr) {
        *bridge_failed_out = true;
      }
      if (bridge_error_value_out != nullptr) {
        std::string raw_bridge_error = "1";
        if (signature->ns_error_out_param_index < expr->args.size()) {
          raw_bridge_error =
              EmitExpr(expr->args[signature->ns_error_out_param_index].get(), ctx);
        }
        const std::string bridged_error = NewTemp(ctx);
        ctx.code_lines.push_back("  " + bridged_error + " = call i32 @" +
                                 std::string(kObjc3RuntimeBridgeNSErrorErrorI32Symbol) +
                                 "(i32 " + raw_bridge_error + ")");
        *bridge_error_value_out = bridged_error;
      }
      return out + "|" + bridge_failed;
    }

    if (expr != nullptr && expr->await_expression_enabled &&
        ctx.async_runtime_helper_enabled) {
      const std::string continuation_handle = NewTemp(ctx);
      ctx.code_lines.push_back(
          "  " + continuation_handle + " = call i32 @" +
          std::string(kObjc3RuntimeAllocateAsyncContinuationI32Symbol) +
          "(i32 " + std::to_string(ctx.async_resume_entry_tag) + ", i32 " +
          std::to_string(ctx.async_executor_tag) + ")");
      const std::string handed_off_handle = NewTemp(ctx);
      ctx.code_lines.push_back(
          "  " + handed_off_handle + " = call i32 @" +
          std::string(kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol) +
          "(i32 " + continuation_handle + ", i32 " +
          std::to_string(ctx.async_executor_tag) + ")");
      const std::string resumed_value = NewTemp(ctx);
      ctx.code_lines.push_back(
          "  " + resumed_value + " = call i32 @" +
          std::string(kObjc3RuntimeResumeAsyncContinuationI32Symbol) +
          "(i32 " + handed_off_handle + ", i32 " + out + ")");
      InvalidateGlobalProofState(ctx);
      return resumed_value;
    }
    if (signature != nullptr && signature->return_insert_retain) {
      const std::string retained_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                               std::string(kObjc3RuntimeRetainI32Symbol) +
                               "(i32 " + out + ")");
      out = retained_value;
    }
    if (signature != nullptr && signature->return_insert_autorelease) {
      const std::string autoreleased_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + autoreleased_value + " = call i32 @" +
                               std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
                               "(i32 " + out + ")");
      out = autoreleased_value;
    }
    if (signature != nullptr && signature->return_insert_release) {
      const std::string released_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + released_value + " = call i32 @" +
                               std::string(kObjc3RuntimeReleaseI32Symbol) +
                               "(i32 " + out + ")");
      (void)released_value;
    }
    return out;
  }

  void AppendLoweredCallArg(std::vector<std::string> &args, const std::string &arg_i32, ValueType expected_type,
                            FunctionContext &ctx) const {
    if (expected_type == ValueType::Bool) {
      const std::string arg_i1 = CoerceI32ToBoolI1(arg_i32, ctx);
      args.push_back("i1 " + arg_i1);
      return;
    }
    args.push_back("i32 " + arg_i32);
  }

  void RegisterArcOwnedCleanupPtr(const std::string &ptr,
                                  FunctionContext &ctx) const {
    if (ptr.empty() || !ctx.arc_owned_cleanup_ptr_set.insert(ptr).second) {
      return;
    }
    ctx.arc_owned_cleanup_ptrs.push_back(ptr);
    ctx.arc_owned_storage_ptrs.insert(ptr);
  }

  void EmitArcOwnedCleanupReleases(FunctionContext &ctx) const {
    EmitArcOwnedCleanupUnwindToDepth(ctx, 0u);
  }

  void EmitTerminalCleanupToDepth(FunctionContext &ctx, std::size_t scope_depth,
                                  std::size_t autoreleasepool_depth,
                                  std::size_t pending_block_dispose_depth,
                                  std::size_t ownership_cleanup_depth,
                                  std::size_t arc_cleanup_depth) const {
    EmitDeferredCleanupTerminalToDepth(ctx, scope_depth);
    EmitAutoreleasepoolUnwindToDepth(ctx, autoreleasepool_depth);
    EmitOwnershipCleanupTerminalCleanupToDepth(
        ctx, ownership_cleanup_depth, ctx.code_lines, ctx.temp_counter);
    EmitPendingBlockDisposeTerminalCleanupToDepth(
        ctx, pending_block_dispose_depth, ctx.code_lines);
    EmitArcOwnedTerminalCleanupToDepth(
        ctx, arc_cleanup_depth, ctx.code_lines, ctx.temp_counter);
  }

  void EmitTypedReturn(const std::string &i32_value, FunctionContext &ctx) const {
    if (ctx.return_type == ValueType::Void) {
      EmitDeferredCleanupTerminalToDepth(ctx, 0u);
      EmitOwnershipCleanupTerminalCleanupToDepth(
          ctx, 0u, ctx.code_lines, ctx.temp_counter);
      EmitPendingBlockDisposeTerminalCleanupToDepth(ctx, 0u, ctx.code_lines);
      EmitArcOwnedTerminalCleanupToDepth(
          ctx, 0u, ctx.code_lines, ctx.temp_counter);
      ctx.code_lines.push_back("  ret void");
      return;
    }
    std::string returned_value = i32_value;
    if (ctx.arc_return_insert_retain) {
      const std::string retained_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                               std::string(kObjc3RuntimeRetainI32Symbol) +
                               "(i32 " + returned_value + ")");
      returned_value = retained_value;
    }
    if (ctx.arc_return_insert_autorelease) {
      const std::string autoreleased_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + autoreleased_value + " = call i32 @" +
                               std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
                               "(i32 " + returned_value + ")");
      returned_value = autoreleased_value;
    }
    EmitDeferredCleanupTerminalToDepth(ctx, 0u);
    EmitOwnershipCleanupTerminalCleanupToDepth(
        ctx, 0u, ctx.code_lines, ctx.temp_counter);
    EmitPendingBlockDisposeTerminalCleanupToDepth(ctx, 0u, ctx.code_lines);
    EmitArcOwnedTerminalCleanupToDepth(
        ctx, 0u, ctx.code_lines, ctx.temp_counter);
    if (ctx.return_type == ValueType::Bool) {
      const std::string bool_i1 = CoerceI32ToBoolI1(returned_value, ctx);
      ctx.code_lines.push_back("  ret i1 " + bool_i1);
      return;
    }
    ctx.code_lines.push_back("  ret i32 " + returned_value);
  }

  void EmitTypedParamStore(const FuncParam &param, std::size_t index, const std::string &ptr, FunctionContext &ctx) const {
    if (param.type == ValueType::Bool) {
      const std::string widened = "%arg" + std::to_string(index) + ".zext." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + widened + " = zext i1 %arg" + std::to_string(index) + " to i32");
      ctx.entry_lines.push_back("  store i32 " + widened + ", ptr " + ptr + ", align 4");
      return;
    }
    std::string stored_value = "%arg" + std::to_string(index);
    if (EffectiveArcParamInsertRetain(param, frontend_metadata_.arc_mode_enabled)) {
      const std::string retained_value =
          "%arg" + std::to_string(index) + ".retained." +
          std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + retained_value + " = call i32 @" +
                                std::string(kObjc3RuntimeRetainI32Symbol) +
                                "(i32 " + stored_value + ")");
      stored_value = retained_value;
    }
    ctx.entry_lines.push_back("  store i32 " + stored_value + ", ptr " + ptr +
                              ", align 4");
    if (EffectiveArcParamInsertRelease(param, frontend_metadata_.arc_mode_enabled)) {
      RegisterArcOwnedCleanupPtr(ptr, ctx);
    }
  }

  void EmitAssignmentStore(const std::string &ptr, const std::string &op, const Expr *value_expr,
                           FunctionContext &ctx) const {
    if (ptr.empty()) {
      return;
    }
    int assigned_const_value = 0;
    const bool has_assigned_const_value =
        op == "=" && value_expr != nullptr && TryGetCompileTimeI32ExprInContext(value_expr, ctx, assigned_const_value);
    const bool has_assigned_nil_value = op == "=" && value_expr != nullptr && IsCompileTimeNilReceiverExprInContext(value_expr, ctx);
    // Any explicit write invalidates compile-time nil binding for this storage slot.
    ctx.nil_bound_ptrs.erase(ptr);
    // Any explicit write invalidates compile-time known non-zero binding for this storage slot.
    ctx.nonzero_bound_ptrs.erase(ptr);
    // Any explicit write invalidates tracked compile-time constant value for this storage slot.
    ctx.const_value_ptrs.erase(ptr);
    if (op == "++" || op == "--") {
      const std::string lhs = NewTemp(ctx);
      ctx.code_lines.push_back("  " + lhs + " = load i32, ptr " + ptr + ", align 4");
      const std::string out = NewTemp(ctx);
      const std::string opcode = op == "++" ? "add" : "sub";
      ctx.code_lines.push_back("  " + out + " = " + opcode + " i32 " + lhs + ", 1");
      ctx.code_lines.push_back("  store i32 " + out + ", ptr " + ptr + ", align 4");
      return;
    }
    if (op == "=") {
      if (value_expr == nullptr) {
        return;
      }
      const std::string value = EmitExpr(value_expr, ctx);
      if (ctx.terminated) {
        return;
      }
      if (ctx.arc_owned_storage_ptrs.find(ptr) != ctx.arc_owned_storage_ptrs.end()) {
        const std::string retained_value = NewTemp(ctx);
        const std::string previous_value = NewTemp(ctx);
        const std::string released_value = NewTemp(ctx);
        ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                                 std::string(kObjc3RuntimeRetainI32Symbol) +
                                 "(i32 " + value + ")");
        ctx.code_lines.push_back("  " + previous_value +
                                 " = load i32, ptr " + ptr + ", align 4");
        ctx.code_lines.push_back("  store i32 " + retained_value + ", ptr " +
                                 ptr + ", align 4");
        ctx.code_lines.push_back("  " + released_value + " = call i32 @" +
                                 std::string(kObjc3RuntimeReleaseI32Symbol) +
                                 "(i32 " + previous_value + ")");
        (void)released_value;
      } else {
        ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                                 ", align 4");
      }
      if (has_assigned_nil_value && ptr.rfind("@", 0) != 0) {
        ctx.nil_bound_ptrs.insert(ptr);
      }
      if (has_assigned_const_value) {
        ctx.const_value_ptrs[ptr] = assigned_const_value;
        if (assigned_const_value != 0) {
          ctx.nonzero_bound_ptrs.insert(ptr);
        }
      }
      return;
    }

    if (value_expr == nullptr) {
      return;
    }
    std::string binary_opcode;
    if (!TryGetCompoundAssignmentBinaryOpcode(op, binary_opcode)) {
      const std::string value = EmitExpr(value_expr, ctx);
      if (ctx.terminated) {
        return;
      }
      ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
      return;
    }

    const std::string lhs = NewTemp(ctx);
    ctx.code_lines.push_back("  " + lhs + " = load i32, ptr " + ptr + ", align 4");
    const std::string rhs = EmitExpr(value_expr, ctx);
    if (ctx.terminated) {
      return;
    }
    const std::string out = NewTemp(ctx);
    ctx.code_lines.push_back("  " + out + " = " + binary_opcode + " i32 " + lhs + ", " + rhs);
    ctx.code_lines.push_back("  store i32 " + out + ", ptr " + ptr + ", align 4");
  }

  void EmitForClause(const ForClause &clause, FunctionContext &ctx) const {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return;
      case ForClause::Kind::Expr:
        if (clause.value != nullptr) {
          (void)EmitExpr(clause.value.get(), ctx);
        }
        return;
      case ForClause::Kind::Assign: {
        const std::string ptr = LookupVarPtr(ctx, clause.name);
        if (ptr.empty() && ctx.block_bindings.find(clause.name) != ctx.block_bindings.end()) {
          (void)EmitUnsupportedI32Value(
              "reassigning block values is not yet runnable in Objective-C 3 native mode");
          return;
        }
        EmitAssignmentStore(ptr, clause.op, clause.value.get(), ctx);
        return;
      }
      case ForClause::Kind::Let: {
        if (ctx.scopes.empty() || clause.value == nullptr) {
          return;
        }
        const std::string value = EmitExpr(clause.value.get(), ctx);
        if (ctx.terminated) {
          return;
        }
        const std::string ptr = "%" + clause.name + ".addr." + std::to_string(ctx.temp_counter++);
        int clause_const_value = 0;
        const bool has_clause_const_value = TryGetCompileTimeI32ExprInContext(clause.value.get(), ctx, clause_const_value);
        const bool has_clause_nil_value = IsCompileTimeNilReceiverExprInContext(clause.value.get(), ctx);
        ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
        ctx.scopes.back()[clause.name] = ptr;
        if (has_clause_nil_value) {
          ctx.nil_bound_ptrs.insert(ptr);
        }
        if (has_clause_const_value) {
          ctx.const_value_ptrs[ptr] = clause_const_value;
        }
        if (has_clause_const_value && clause_const_value != 0) {
          ctx.nonzero_bound_ptrs.insert(ptr);
        }
        ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
        return;
      }
    }
  }

  std::string EmitLocalI32Binding(const std::string &name, const Expr *value_expr,
                                  FunctionContext &ctx,
                                  bool mark_runtime_nonnull) const {
    const std::string value = EmitExpr(value_expr, ctx);
    if (ctx.terminated) {
      return value;
    }
    int const_value = 0;
    const bool has_const_value =
        TryGetCompileTimeI32ExprInContext(value_expr, ctx, const_value);
    const bool has_nil_value =
        IsCompileTimeNilReceiverExprInContext(value_expr, ctx);
    const std::string ptr =
        "%" + name + ".addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
    ctx.scopes.back()[name] = ptr;
    if (has_nil_value) {
      ctx.nil_bound_ptrs.insert(ptr);
    }
    if (has_const_value) {
      ctx.const_value_ptrs[ptr] = const_value;
    }
    if ((has_const_value && const_value != 0) || mark_runtime_nonnull) {
      ctx.nonzero_bound_ptrs.insert(ptr);
    }
    ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                             ", align 4");
    return value;
  }

  void EmitOptionalBindingIfStatement(const IfStmt *if_stmt,
                                      FunctionContext &ctx) const {
    if (if_stmt == nullptr) {
      return;
    }

    const bool is_guard = if_stmt->guard_binding_surface_enabled ||
                          if_stmt->guard_condition_list_surface_enabled;
    const std::size_t binding_count =
        std::min(if_stmt->optional_binding_clause_count, if_stmt->then_body.size());
    if (binding_count == 0u && !is_guard) {
      return;
    }
    const std::string success_label =
        NewLabel(ctx, is_guard ? "guard_success_" : "if_bind_success_");
    const std::string failure_label =
        NewLabel(ctx, is_guard ? "guard_else_" : "if_bind_else_");
    const std::string merge_label =
        NewLabel(ctx, is_guard ? "guard_end_" : "if_bind_end_");

    PushScope(ctx);
    for (std::size_t index = 0; index < binding_count; ++index) {
      const Stmt *binding_stmt = if_stmt->then_body[index].get();
      if (binding_stmt == nullptr || binding_stmt->kind != Stmt::Kind::Let ||
          binding_stmt->let_stmt == nullptr) {
        (void)EmitUnsupportedI32Value(
            "optional binding lowering expected synthetic let clauses");
        PopScope(ctx, false);
        return;
      }
      const LetStmt *binding = binding_stmt->let_stmt.get();
      const std::string value =
          EmitLocalI32Binding(binding->name, binding->value.get(), ctx, true);
      const std::string is_present = NewTemp(ctx);
      const std::string next_label =
          (index + 1u == binding_count) ? success_label
                                        : NewLabel(ctx, "if_bind_clause_");
      ctx.code_lines.push_back("  " + is_present + " = icmp ne i32 " + value +
                               ", 0");
      ctx.code_lines.push_back("  br i1 " + is_present + ", label %" +
                               next_label + ", label %" + failure_label);
      if (next_label != success_label) {
        ctx.code_lines.push_back(next_label + ":");
      }
    }

    if (is_guard) {
      const std::string guard_ready_label =
          if_stmt->guard_condition_exprs.empty()
              ? success_label
              : NewLabel(ctx, "guard_ready_");
      if (binding_count == 0u) {
        ctx.code_lines.push_back("  br label %" + success_label);
      }
      ctx.code_lines.push_back(success_label + ":");
      for (const auto &guard_condition : if_stmt->guard_condition_exprs) {
        const std::string condition_value = EmitExpr(guard_condition.get(), ctx);
        const std::string condition_i1 = NewTemp(ctx);
        const bool is_last_condition =
            guard_condition.get() == if_stmt->guard_condition_exprs.back().get();
        const std::string next_label =
            is_last_condition ? guard_ready_label
                              : NewLabel(ctx, "guard_clause_");
        ctx.code_lines.push_back("  " + condition_i1 + " = icmp ne i32 " +
                                 condition_value + ", 0");
        ctx.code_lines.push_back("  br i1 " + condition_i1 + ", label %" +
                                 next_label + ", label %" + failure_label);
        if (!is_last_condition) {
          ctx.code_lines.push_back(next_label + ":");
        }
      }
      if (guard_ready_label != success_label) {
        ctx.code_lines.push_back(guard_ready_label + ":");
      }
      const auto promoted_bindings = ctx.scopes.back();
      PopScope(ctx, false);
      for (const auto &binding : promoted_bindings) {
        ctx.scopes.back()[binding.first] = binding.second;
      }
      ctx.code_lines.push_back("  br label %" + merge_label);

      ctx.code_lines.push_back(failure_label + ":");
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

      ctx.code_lines.push_back(merge_label + ":");
      ctx.terminated = false;
      return;
    }

    ctx.code_lines.push_back(success_label + ":");
    ctx.terminated = false;
    for (std::size_t index = binding_count; index < if_stmt->then_body.size();
         ++index) {
      EmitStatement(if_stmt->then_body[index].get(), ctx);
    }
    const bool then_terminated = ctx.terminated;
    PopScope(ctx, !then_terminated);
    if (!then_terminated) {
      ctx.code_lines.push_back("  br label %" + merge_label);
    }

    ctx.code_lines.push_back(failure_label + ":");
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
      return;
    }
    ctx.code_lines.push_back(merge_label + ":");
    ctx.terminated = false;
  }

  bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      return true;
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return IsCompileTimeNilReceiverExprInContext(expr->right.get(), ctx);
      }
      return IsCompileTimeNilReceiverExprInContext(expr->third.get(), ctx);
    }
    if (expr->kind != Expr::Kind::Identifier) {
      return false;
    }
    const std::string ptr = LookupVarPtr(ctx, expr->ident);
    if (ptr.empty()) {
      return LookupImmediateIdentifierValue(ctx, expr->ident) == 0;
    }
    if (ctx.nil_bound_ptrs.find(ptr) != ctx.nil_bound_ptrs.end()) {
      return true;
    }
    if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
      return global_nil_proven_symbols_.find(expr->ident) != global_nil_proven_symbols_.end();
    }
    return false;
  }

  bool IsCompileTimeGlobalNilExpr(const Expr *expr) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      return true;
    }
    if (expr->kind == Expr::Kind::Identifier) {
      return global_nil_proven_symbols_.find(expr->ident) != global_nil_proven_symbols_.end();
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      const FunctionContext global_eval_ctx;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), global_eval_ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return IsCompileTimeGlobalNilExpr(expr->right.get());
      }
      return IsCompileTimeGlobalNilExpr(expr->third.get());
    }
    return false;
  }

  bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::Number) {
      value = expr->number;
      return true;
    }
    if (expr->kind == Expr::Kind::BoolLiteral) {
      value = expr->bool_value ? 1 : 0;
      return true;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      value = 0;
      return true;
    }
    if (expr->kind == Expr::Kind::Identifier) {
      const std::string ptr = LookupVarPtr(ctx, expr->ident);
      if (ptr.empty()) {
        const int immediate_value =
            LookupImmediateIdentifierValue(ctx, expr->ident);
        if (immediate_value != 0) {
          value = immediate_value;
          return true;
        }
        return false;
      }
      auto value_it = ctx.const_value_ptrs.find(ptr);
      if (value_it != ctx.const_value_ptrs.end()) {
        value = value_it->second;
        return true;
      }
      if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
        auto global_it = global_const_values_.find(expr->ident);
        if (global_it != global_const_values_.end()) {
          value = global_it->second;
          return true;
        }
      }
      return false;
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, value);
      }
      return TryGetCompileTimeI32ExprInContext(expr->third.get(), ctx, value);
    }
    if (expr->kind != Expr::Kind::Binary || expr->left == nullptr || expr->right == nullptr) {
      return false;
    }
    if (expr->op == "&&" || expr->op == "||") {
      int lhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs)) {
        return false;
      }
      if (expr->op == "&&") {
        if (lhs == 0) {
          value = 0;
          return true;
        }
        int rhs = 0;
        if (!TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
          return false;
        }
        value = rhs != 0 ? 1 : 0;
        return true;
      }
      if (lhs != 0) {
        value = 1;
        return true;
      }
      int rhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
        return false;
      }
      value = rhs != 0 ? 1 : 0;
      return true;
    }
    if (expr->op == "??") {
      int lhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs)) {
        return false;
      }
      if (lhs != 0) {
        value = lhs;
        return true;
      }
      return TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, value);
    }
    int lhs = 0;
    int rhs = 0;
    if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs) ||
        !TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
      return false;
    }
    if (expr->op == "+") {
      value = lhs + rhs;
      return true;
    }
    if (expr->op == "-") {
      value = lhs - rhs;
      return true;
    }
    if (expr->op == "*") {
      value = lhs * rhs;
      return true;
    }
    if (expr->op == "/") {
      if (rhs == 0) {
        return false;
      }
      value = lhs / rhs;
      return true;
    }
    if (expr->op == "%") {
      if (rhs == 0) {
        return false;
      }
      value = lhs % rhs;
      return true;
    }
    if (expr->op == "&") {
      value = lhs & rhs;
      return true;
    }
    if (expr->op == "|") {
      value = lhs | rhs;
      return true;
    }
    if (expr->op == "^") {
      value = lhs ^ rhs;
      return true;
    }
    if (expr->op == "<<" || expr->op == ">>") {
      if (rhs < 0 || rhs > 31) {
        return false;
      }
      value = expr->op == "<<" ? (lhs << rhs) : (lhs >> rhs);
      return true;
    }
    if (expr->op == "==") {
      value = lhs == rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "!=") {
      value = lhs != rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "<") {
      value = lhs < rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "<=") {
      value = lhs <= rhs ? 1 : 0;
      return true;
    }
    if (expr->op == ">") {
      value = lhs > rhs ? 1 : 0;
      return true;
    }
    if (expr->op == ">=") {
      value = lhs >= rhs ? 1 : 0;
      return true;
    }
    return false;
  }

  bool IsCompileTimeKnownNonNilExprInContext(const Expr *expr, const FunctionContext &ctx) const {
    int const_value = 0;
    if (!TryGetCompileTimeI32ExprInContext(expr, ctx, const_value)) {
      return false;
    }
    return const_value != 0;
  }

  bool ValidateMessageSendArityExpr(const Expr *expr, std::string &error) const {
    if (expr == nullptr) {
      return true;
    }
    switch (expr->kind) {
      case Expr::Kind::Number:
      case Expr::Kind::BoolLiteral:
      case Expr::Kind::NilLiteral:
      case Expr::Kind::Identifier:
      case Expr::Kind::BlockLiteral:
        return true;
      case Expr::Kind::Binary:
        return ValidateMessageSendArityExpr(expr->left.get(), error) &&
               ValidateMessageSendArityExpr(expr->right.get(), error);
      case Expr::Kind::Conditional:
        return ValidateMessageSendArityExpr(expr->left.get(), error) &&
               ValidateMessageSendArityExpr(expr->right.get(), error) &&
               ValidateMessageSendArityExpr(expr->third.get(), error);
      case Expr::Kind::Call:
        for (const auto &arg : expr->args) {
          if (!ValidateMessageSendArityExpr(arg.get(), error)) {
            return false;
          }
        }
        return true;
      case Expr::Kind::MessageSend:
        if (expr->args.size() > lowering_ir_boundary_.runtime_dispatch_arg_slots) {
          error = "message send exceeds runtime dispatch arg slots: got " + std::to_string(expr->args.size()) +
                  ", max " + std::to_string(lowering_ir_boundary_.runtime_dispatch_arg_slots) + " at " +
                  std::to_string(expr->line) + ":" + std::to_string(expr->column);
          return false;
        }
        if (!ValidateMessageSendArityExpr(expr->receiver.get(), error)) {
          return false;
        }
        for (const auto &arg : expr->args) {
          if (!ValidateMessageSendArityExpr(arg.get(), error)) {
            return false;
          }
        }
        return true;
    }
    return true;
  }

  bool ValidateMessageSendArityForClause(const ForClause &clause, std::string &error) const {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return true;
      case ForClause::Kind::Expr:
      case ForClause::Kind::Let:
      case ForClause::Kind::Assign:
        return ValidateMessageSendArityExpr(clause.value.get(), error);
    }
    return true;
  }

  bool ValidateMessageSendArityStmt(const Stmt *stmt, std::string &error) const {
    if (stmt == nullptr) {
      return true;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        return stmt->let_stmt == nullptr || ValidateMessageSendArityExpr(stmt->let_stmt->value.get(), error);
      case Stmt::Kind::Assign:
        return stmt->assign_stmt == nullptr || ValidateMessageSendArityExpr(stmt->assign_stmt->value.get(), error);
      case Stmt::Kind::Return:
        return stmt->return_stmt == nullptr || ValidateMessageSendArityExpr(stmt->return_stmt->value.get(), error);
      case Stmt::Kind::Expr:
        return stmt->expr_stmt == nullptr || ValidateMessageSendArityExpr(stmt->expr_stmt->value.get(), error);
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return true;
        }
        if (!ValidateMessageSendArityExpr(stmt->if_stmt->condition.get(), error)) {
          return false;
        }
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          if (!ValidateMessageSendArityStmt(then_stmt.get(), error)) {
            return false;
          }
        }
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          if (!ValidateMessageSendArityStmt(else_stmt.get(), error)) {
            return false;
          }
        }
        return true;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return true;
        }
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          if (!ValidateMessageSendArityStmt(loop_stmt.get(), error)) {
            return false;
          }
        }
        return ValidateMessageSendArityExpr(stmt->do_while_stmt->condition.get(), error);
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return true;
        }
        if (!ValidateMessageSendArityForClause(stmt->for_stmt->init, error) ||
            !ValidateMessageSendArityExpr(stmt->for_stmt->condition.get(), error) ||
            !ValidateMessageSendArityForClause(stmt->for_stmt->step, error)) {
          return false;
        }
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          if (!ValidateMessageSendArityStmt(loop_stmt.get(), error)) {

      emit_declaration_once(kObjc3RuntimeLoadThrownErrorI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeLoadThrownErrorI32Symbol) +
                                "(ptr)\n");
      emit_declaration_once(kObjc3RuntimeBridgeStatusErrorI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeBridgeStatusErrorI32Symbol) +
                                "(i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeBridgeNSErrorErrorI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeBridgeNSErrorErrorI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeCatchMatchesErrorI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeCatchMatchesErrorI32Symbol) +
                                "(i32, i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeAllocateAsyncContinuationI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeAllocateAsyncContinuationI32Symbol) +
                                "(i32, i32)\n");
      emit_declaration_once(
          kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol,
          "declare i32 @" +
              std::string(
                  kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol) +
              "(i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeResumeAsyncContinuationI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeResumeAsyncContinuationI32Symbol) +
                                "(i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeSpawnTaskI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeSpawnTaskI32Symbol) +
                                "(i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeEnterTaskGroupScopeI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeEnterTaskGroupScopeI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeAddTaskGroupTaskI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeAddTaskGroupTaskI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeWaitTaskGroupNextI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeWaitTaskGroupNextI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeCancelTaskGroupI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeCancelTaskGroupI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeTaskIsCancelledI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeTaskIsCancelledI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeTaskOnCancelI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeTaskOnCancelI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeExecutorHopI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeExecutorHopI32Symbol) +
                                "(i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeActorEnterIsolationThunkI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeActorEnterIsolationThunkI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeActorEnterNonisolatedI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeActorEnterNonisolatedI32Symbol) +
                                "(i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeActorHopToExecutorI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeActorHopToExecutorI32Symbol) +
                                "(i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeActorRecordReplayProofI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeActorRecordReplayProofI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeActorRecordRaceGuardI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeActorRecordRaceGuardI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeActorBindExecutorI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeActorBindExecutorI32Symbol) +
                                "(i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeActorMailboxEnqueueI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeActorMailboxEnqueueI32Symbol) +
                                "(i32, i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeActorMailboxDrainNextI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeActorMailboxDrainNextI32Symbol) +
                                "(i32, i32)\n");
      emit_declaration_once(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol) +
                                "()\n");
      emit_declaration_once(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol,
                            "declare void @" +
                                std::string(
                                    kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeRetainI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeRetainI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeReleaseI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeReleaseI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeAutoreleaseI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimePromoteBlockI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimePromoteBlockI32Symbol) +
                                "(ptr, i64, i32)\n");
      emit_declaration_once(kObjc3RuntimeInvokeBlockI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeInvokeBlockI32Symbol) +
                                "(i32, i32, i32, i32, i32)\n");
      emit_declaration_once(kObjc3RuntimePushAutoreleasepoolScopeSymbol,
                            "declare void @" +
                                std::string(
                                    kObjc3RuntimePushAutoreleasepoolScopeSymbol) +
                                "()\n");
      emit_declaration_once(kObjc3RuntimePopAutoreleasepoolScopeSymbol,
                            "declare void @" +
                                std::string(
                                    kObjc3RuntimePopAutoreleasepoolScopeSymbol) +
                                "()\n");
    }
    for (const auto &entry : function_signatures_) {
      if (defined_functions_.find(entry.first) != defined_functions_.end()) {
        continue;
      }
      if (!declared_symbols.insert(entry.first).second) {
        continue;
      }
      const LoweredFunctionSignature &signature = entry.second;
      std::ostringstream params;
      for (std::size_t i = 0; i < signature.param_types.size(); ++i) {
        if (i != 0) {
          params << ", ";
        }
        params << LLVMScalarType(signature.param_types[i]);
      }
      if (signature.throws_declared) {
        if (!signature.param_types.empty()) {
          params << ", ";
        }
        params << "ptr";
      }
      out << "declare " << LLVMScalarType(signature.return_type) << " @" << entry.first << "(" << params.str()
          << ")\n";
      emitted = true;
    }
    if (emitted) {
      out << "\n";
    }
  }

  void EmitRuntimeBootstrapLoweringFunctions(std::ostringstream &out) const {
    if (!ShouldEmitRuntimeBootstrapLowering()) {
      return;
    }

    const std::string init_stub_symbol =
        "@" + RuntimeBootstrapInitStubSymbol();
    const std::string ctor_root_symbol =
        "@" +
        frontend_metadata_.runtime_bootstrap_lowering_constructor_root_symbol;
    const std::string registration_table_symbol =
        "@" + RuntimeBootstrapRegistrationTableSymbol();
    const std::string register_image_symbol =
        "@" +
        frontend_metadata_.runtime_bootstrap_lowering_registration_entrypoint_symbol;

    out << "define internal void " << init_stub_symbol << "() {\n";
    out << "entry:\n";
    out << "  %bootstrap_state_slot = getelementptr inbounds "
        << RuntimeBootstrapRegistrationTableType() << ", ptr "
        << registration_table_symbol << ", i32 0, i32 13\n";
    out << "  %bootstrap_state_cell = load ptr, ptr %bootstrap_state_slot, align 8\n";
    out << "  %bootstrap_state = load i8, ptr %bootstrap_state_cell, align 1\n";
    out << "  %bootstrap_already_initialized = icmp ne i8 %bootstrap_state, 0\n";
    out << "  br i1 %bootstrap_already_initialized, label %bootstrap_success, label %bootstrap_register\n";
    out << "bootstrap_register:\n";
    out << "  call void @" << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
        << "(ptr " << registration_table_symbol << ")\n";
    out << "  %bootstrap_image_slot = getelementptr inbounds "
        << RuntimeBootstrapRegistrationTableType() << ", ptr "
        << registration_table_symbol << ", i32 0, i32 2\n";
    out << "  %bootstrap_image = load ptr, ptr %bootstrap_image_slot, align 8\n";
    out << "  %bootstrap_status = call i32 " << register_image_symbol
        << "(ptr %bootstrap_image)\n";
    out << "  %bootstrap_ok = icmp eq i32 %bootstrap_status, 0\n";
    out << "  br i1 %bootstrap_ok, label %bootstrap_success, label %bootstrap_fail\n";
    out << "bootstrap_fail:\n";
    out << "  call void @abort()\n";
    out << "  unreachable\n";
    out << "bootstrap_success:\n";
    out << "  store i8 1, ptr %bootstrap_state_cell, align 1\n";
    out << "  ret void\n";
    out << "}\n\n";

    out << "define internal void " << ctor_root_symbol << "() {\n";
    out << "entry:\n";
    out << "  call void " << init_stub_symbol << "()\n";
    out << "  ret void\n";
    out << "}\n\n";
  }

  void EmitRuntimeDispatchDeclarations(std::ostringstream &out) const {
    EmitObjc3RuntimeDispatchDeclarations(lowering_ir_boundary_,
                                         runtime_call_lowering_state_, out);
  }

  void EmitFunction(const FunctionDecl &fn, std::ostringstream &out) const {
    std::ostringstream signature;
    for (std::size_t i = 0; i < fn.params.size(); ++i) {
      if (i != 0) {
        signature << ", ";
      }
      signature << LLVMScalarType(fn.params[i].type) << " %arg" << i;
    }
    if (fn.throws_declared) {
      if (!fn.params.empty()) {
        signature << ", ";
      }
      signature << "ptr %error_out";
    }

    out << "define " << LLVMScalarType(fn.return_type) << " @" << fn.name << "(" << signature.str() << ") {\n";
    out << "entry:\n";

    FunctionContext ctx;
    ctx.return_type = fn.return_type;
    ctx.async_runtime_helper_enabled =
        fn.async_declared && ExecutorAffinityTag(fn) != 0;
    ctx.async_resume_entry_tag = AsyncResumeEntryTag(fn);
    ctx.async_executor_tag = ExecutorAffinityTag(fn);
    if (fn.throws_declared) {
      ctx.function_error_out_param = "%error_out";
    }
    ctx.arc_return_insert_retain =
        EffectiveArcReturnInsertRetain(fn, frontend_metadata_.arc_mode_enabled);
    ctx.arc_return_insert_autorelease =
        EffectiveArcReturnInsertAutorelease(fn);
    PushScope(ctx);
    SeedKnownClassReceiverBindings(ctx);

    for (std::size_t i = 0; i < fn.params.size(); ++i) {
      const auto &param = fn.params[i];
      const std::string ptr = "%" + param.name + ".addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      EmitTypedParamStore(param, i, ptr, ctx);
      ctx.scopes.back()[param.name] = ptr;
    }

    for (const auto &stmt : fn.body) {
      EmitStatement(stmt.get(), ctx);
      if (ctx.terminated) {
        break;
      }
    }

    if (!ctx.terminated) {
      EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
      EmitTypedReturn("0", ctx);
    }

    for (const auto &line : ctx.entry_lines) {
      out << line << "\n";
    }
    for (const auto &line : ctx.code_lines) {
      out << line << "\n";
    }

    out << "}\n";
  }

  void EmitMethod(const MethodDefinition &method_def,
                  std::ostringstream &out) const {
    if (method_def.method == nullptr) {
      EmitSyntheticMethod(method_def, out);
      return;
    }
    const Objc3MethodDecl &method = *method_def.method;
    std::ostringstream signature;
    for (std::size_t i = 0; i < method.params.size(); ++i) {
      if (i != 0) {
        signature << ", ";
      }
      signature << LLVMScalarType(method.params[i].type) << " %arg" << i;
    }
    if (method.throws_declared) {
      if (!method.params.empty()) {
        signature << ", ";
      }
      signature << "ptr %error_out";
    }

    out << "define " << LLVMScalarType(method.return_type) << " @"
        << method_def.symbol << "(" << signature.str() << ") {\n";
    out << "entry:\n";

    FunctionContext ctx;
    ctx.return_type = method.return_type;
    ctx.async_runtime_helper_enabled =
        method.async_declared && ExecutorAffinityTag(method) != 0;
    ctx.actor_runtime_helper_enabled =
        IsActorImplementation(method_def.implementation_name);
    ctx.actor_nonisolated_entry_enabled =
        ctx.actor_runtime_helper_enabled && method.objc_nonisolated_declared;
    ctx.current_implementation_name = method_def.implementation_name;
    ctx.current_superclass_name = method_def.superclass_name;
    ctx.current_method_is_class_method = method.is_class_method;
    ctx.async_resume_entry_tag = AsyncResumeEntryTag(method_def);
    ctx.async_executor_tag = ExecutorAffinityTag(method);
    if (method.throws_declared) {
      ctx.function_error_out_param = "%error_out";
    }
    ctx.arc_return_insert_retain = EffectiveArcReturnInsertRetain(
        method, frontend_metadata_.arc_mode_enabled);
    ctx.arc_return_insert_autorelease =
        EffectiveArcReturnInsertAutorelease(method);
    PushScope(ctx);
    SeedKnownClassReceiverBindings(ctx);
    const int implementation_class_identity =
        LookupClassReceiverIdentityValue(method_def.implementation_name);
    const int self_identity =
        method.is_class_method
            ? BuildClassReceiverIdentityValue(implementation_class_identity)
            : BuildInstanceReceiverIdentityValue(implementation_class_identity);
    if (self_identity != 0) {
      ctx.immediate_identifiers["self"] = self_identity;
    }
    const int super_class_identity =
        LookupClassReceiverIdentityValue(method_def.superclass_name);
    const int super_identity =
        method.is_class_method
            ? BuildClassReceiverIdentityValue(super_class_identity)
            : BuildInstanceReceiverIdentityValue(super_class_identity);
    if (super_identity != 0) {
      ctx.immediate_identifiers["super"] = super_identity;
    }

    for (std::size_t i = 0; i < method.params.size(); ++i) {
      const auto &param = method.params[i];
      const std::string ptr = "%" + param.name + ".addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      EmitTypedParamStore(param, i, ptr, ctx);
      ctx.scopes.back()[param.name] = ptr;
    }

    for (const auto &stmt : method.body) {
      EmitStatement(stmt.get(), ctx);
      if (ctx.terminated) {
        break;
      }
    }

    if (!ctx.terminated) {
      EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
      EmitTypedReturn("0", ctx);
    }

    for (const auto &line : ctx.entry_lines) {
      out << line << "\n";
    }
    for (const auto &line : ctx.code_lines) {
      out << line << "\n";
    }

    out << "}\n";
  }

  void EmitSyntheticMethod(const MethodDefinition &method_def,
                           std::ostringstream &out) const {
    if (method_def.synthetic_method_kind ==
        SyntheticMethodKind::MetaprogrammingDerivedEquality) {
      out << "define i32 @" << method_def.symbol << "(i32 %arg0) {\n";
      out << "entry:\n";
      out << "  ret i32 1\n";
      out << "}\n";
      return;
    }
    if (method_def.synthetic_method_kind ==
            SyntheticMethodKind::MetaprogrammingDerivedHash ||
        method_def.synthetic_method_kind ==
            SyntheticMethodKind::MetaprogrammingDerivedDebugDescription) {
      std::uint32_t seed = 2166136261u;
      for (unsigned char ch : method_def.symbol) {
        seed ^= static_cast<std::uint32_t>(ch);
        seed *= 16777619u;
      }
      out << "define i32 @" << method_def.symbol << "() {\n";
      out << "entry:\n";
      out << "  ret i32 " << static_cast<unsigned long long>(seed & 0x7fffffffu)
          << "\n";
      out << "}\n";
      return;
    }
    if ((method_def.synthetic_method_kind !=
         SyntheticMethodKind::PropertyGetter &&
         method_def.synthetic_method_kind !=
             SyntheticMethodKind::PropertySetter)) {
      return;
    }
    const bool uses_weak_runtime_hooks =
        objc3c::support::UsesWeakCurrentPropertyRuntimeHelper(
            method_def.synthesized_ownership_runtime_hook_profile);
    const bool uses_strong_runtime_hooks =
        objc3c::support::UsesStrongOwnedCurrentPropertyExchange(
            method_def.synthesized_ownership_lifetime_profile,
            method_def.synthesized_accessor_ownership_profile);
    const char *llvm_value_type = LLVMScalarType(method_def.synthesized_value_type);
    if (method_def.synthetic_method_kind ==
        SyntheticMethodKind::PropertyGetter) {
      ++synthesized_getter_definition_count_;
      out << "define " << llvm_value_type << " @" << method_def.symbol << "() {\n";
      out << "entry:\n";
      const std::string loaded_value = "%objc3_property_slot";
      out << "  " << loaded_value << " = call i32 @"
          << (uses_weak_runtime_hooks ? kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
                                      : kObjc3RuntimeReadCurrentPropertyI32Symbol)
          << "()\n";
      if (uses_weak_runtime_hooks) {
        ++weak_current_property_load_helper_call_count_;
      } else {
        ++current_property_read_helper_call_count_;
      }
      std::string returned_value = loaded_value;
      if (uses_strong_runtime_hooks) {
        const std::string retained_value = "%objc3_property_retained";
        const std::string autoreleased_value = "%objc3_property_autoreleased";
        out << "  " << retained_value << " = call i32 @"
            << kObjc3RuntimeRetainI32Symbol << "(i32 " << loaded_value << ")\n";
        out << "  " << autoreleased_value << " = call i32 @"
            << kObjc3RuntimeAutoreleaseI32Symbol << "(i32 " << retained_value
            << ")\n";
        ++retain_helper_call_count_;
        ++autorelease_helper_call_count_;
        returned_value = autoreleased_value;
      }
      if (method_def.synthesized_value_type == ValueType::Bool) {
        out << "  %objc3_property_value = icmp ne i32 %objc3_property_slot, 0\n";
        out << "  ret i1 %objc3_property_value\n";
      } else {
        out << "  ret i32 " << returned_value << "\n";
      }
      out << "}\n";
      return;
    }

    ++synthesized_setter_definition_count_;
    out << "define void @" << method_def.symbol << "(" << llvm_value_type
        << " %arg0) {\n";
    out << "entry:\n";
    std::string stored_value = "%arg0";
    if (method_def.synthesized_value_type == ValueType::Bool) {
      out << "  %objc3_property_value = zext i1 %arg0 to i32\n";
      stored_value = "%objc3_property_value";
    }
    if (uses_weak_runtime_hooks) {
      out << "  call void @" << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
          << "(i32 " << stored_value << ")\n";
      ++weak_current_property_store_helper_call_count_;
    } else if (uses_strong_runtime_hooks) {
      out << "  %objc3_property_retained = call i32 @"
          << kObjc3RuntimeRetainI32Symbol << "(i32 " << stored_value << ")\n";
      out << "  %objc3_property_previous = call i32 @"
          << kObjc3RuntimeExchangeCurrentPropertyI32Symbol << "(i32 %objc3_property_retained)\n";
      out << "  %objc3_property_release = call i32 @"
          << kObjc3RuntimeReleaseI32Symbol
          << "(i32 %objc3_property_previous)\n";
      ++retain_helper_call_count_;
      ++current_property_exchange_helper_call_count_;
      ++release_helper_call_count_;
    } else {
      out << "  call void @" << kObjc3RuntimeWriteCurrentPropertyI32Symbol
          << "(i32 " << stored_value << ")\n";
      ++current_property_write_helper_call_count_;
    }
    out << "  ret void\n";
    out << "}\n";
  }

  void EmitEntryPoint(std::ostringstream &out) const {
    auto main_it = function_arity_.find("main");
    const bool has_zero_arity_main =
        main_it != function_arity_.end() && main_it->second == 0;
    if (has_zero_arity_main) {
      out << "define i32 @objc3c_entry() {\n";
      out << "entry:\n";
      ValueType main_return_type = ValueType::I32;
      const LoweredFunctionSignature *main_signature = LookupFunctionSignature("main");
      if (main_signature != nullptr) {
        main_return_type = main_signature->return_type;
      }
      const bool main_throws =
          main_signature != nullptr && main_signature->throws_declared;
      if (main_throws) {
        out << "  %main.error.addr = alloca i32, align 4\n";
        out << "  store i32 0, ptr %main.error.addr, align 4\n";
      }
      if (main_return_type == ValueType::Void) {
        out << "  call void @main(";
        if (main_throws) {
          out << "ptr %main.error.addr";
        }
        out << ")\n";
        out << "  ret i32 0\n";
      } else {
        out << "  %call_main = call " << LLVMScalarType(main_return_type)
            << " @main(";
        if (main_throws) {
          out << "ptr %main.error.addr";
        }
        out << ")\n";
        if (main_return_type == ValueType::Bool) {
          out << "  %call_main_i32 = zext i1 %call_main to i32\n";
          out << "  ret i32 %call_main_i32\n";
        } else {
          out << "  ret i32 %call_main\n";
        }
      }
      out << "}\n";
      return;
    }

    out << "define internal i32 @objc3c_entry() {\n";
    out << "entry:\n";
    std::string previous = "0";
    for (std::size_t i = 0; i < program_.globals.size(); ++i) {
      const auto &global = program_.globals[i];
      const std::string load_name = "%entry_load_" + std::to_string(i);
      const std::string sum_name = "%entry_sum_" + std::to_string(i);
      out << "  " << load_name << " = load i32, ptr @" << global.name << ", align 4\n";
      out << "  " << sum_name << " = add i32 " << previous << ", " << load_name << "\n";
      previous = sum_name;
    }
    out << "  ret i32 " << previous << "\n";
    out << "}\n";
  }

  const Objc3Program &program_;
  Objc3IRFrontendMetadata frontend_metadata_;
  Objc3LoweringIRBoundary lowering_ir_boundary_;
  std::string boundary_error_;
  std::unordered_set<std::string> globals_;
  std::unordered_set<std::string> mutable_global_symbols_;
  std::unordered_map<std::string, int> global_const_values_;
  std::unordered_set<std::string> global_nil_proven_symbols_;
  std::unordered_set<std::string> defined_functions_;
  std::unordered_set<std::string> declared_pure_functions_;
  std::vector<const FunctionDecl *> function_definitions_;
  std::vector<MethodDefinition> method_definitions_;
  std::size_t synthesized_property_accessor_count_ = 0;
  std::vector<MetaprogrammingGlobalArtifact> metaprogramming_global_artifacts_;
  std::size_t metaprogramming_derived_method_count_ = 0;
  std::unordered_map<std::string, FunctionEffectInfo> function_effects_;
  std::unordered_set<std::string> impure_functions_;
  std::unordered_map<std::string, std::size_t> function_arity_;
  std::map<std::string, LoweredFunctionSignature> function_signatures_;
  std::unordered_map<std::string, std::string> direct_dispatch_symbols_by_key_;
  std::map<std::string, std::string> selector_pool_globals_;
  std::map<std::string, std::string> runtime_string_pool_globals_;
  std::map<std::string, TypedKeyPathArtifact> typed_keypath_artifacts_;
  std::unordered_map<std::string, int> class_receiver_constants_;
  std::size_t vector_signature_function_count_ = 0;
  mutable std::vector<std::string> block_function_definitions_;
  mutable std::unordered_set<std::string> emitted_block_invoke_symbols_;
  mutable std::unordered_set<std::string> emitted_block_copy_helper_symbols_;
  mutable std::unordered_set<std::string> emitted_block_dispose_helper_symbols_;
  mutable Objc3RuntimeCallLoweringState runtime_call_lowering_state_;
  mutable std::size_t synthesized_getter_definition_count_ = 0;
  mutable std::size_t synthesized_setter_definition_count_ = 0;
  mutable std::size_t current_property_read_helper_call_count_ = 0;
  mutable std::size_t current_property_write_helper_call_count_ = 0;
  mutable std::size_t current_property_exchange_helper_call_count_ = 0;
  mutable std::size_t weak_current_property_load_helper_call_count_ = 0;
  mutable std::size_t weak_current_property_store_helper_call_count_ = 0;
  mutable std::size_t retain_helper_call_count_ = 0;
  mutable std::size_t release_helper_call_count_ = 0;
  mutable std::size_t autorelease_helper_call_count_ = 0;
  mutable bool unsupported_fail_closed_path_triggered_ = false;
  mutable std::string unsupported_fail_closed_path_reason_;
};

bool EmitObjc3IRText(const Objc3Program &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error) {
  Objc3IREmitter emitter(program, lowering_contract, frontend_metadata);
  return emitter.Emit(ir, error);
}

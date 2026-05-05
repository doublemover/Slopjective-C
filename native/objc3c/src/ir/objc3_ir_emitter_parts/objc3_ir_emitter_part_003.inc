               .task_runtime_interop_cancellation_lowering_normalized_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_contract_violation_sites
        << ", deterministic_task_runtime_interop_cancellation_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_task_runtime_interop_cancellation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_concurrency_replay_race_guard_lowering_profile = concurrency_replay_sites="
        << frontend_metadata_.concurrency_replay_race_guard_lowering_sites
        << ", replay_proof_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_replay_proof_sites
        << ", race_guard_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_race_guard_sites
        << ", task_handoff_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_task_handoff_sites
        << ", actor_isolation_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_actor_isolation_sites
        << ", deterministic_schedule_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_deterministic_schedule_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_contract_violation_sites
        << ", deterministic_concurrency_replay_race_guard_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_concurrency_replay_race_guard_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_unsafe_pointer_extension_lowering_profile = unsafe_pointer_extension_sites="
        << frontend_metadata_.unsafe_pointer_extension_lowering_sites
        << ", unsafe_keyword_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_unsafe_keyword_sites
        << ", pointer_arithmetic_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_pointer_arithmetic_sites
        << ", raw_pointer_type_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_raw_pointer_type_sites
        << ", unsafe_operation_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_unsafe_operation_sites
        << ", normalized_sites="
        << frontend_metadata_.unsafe_pointer_extension_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_contract_violation_sites
        << ", deterministic_unsafe_pointer_extension_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_unsafe_pointer_extension_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_inline_asm_intrinsic_governance_lowering_profile = inline_asm_intrinsic_sites="
        << frontend_metadata_.inline_asm_intrinsic_governance_lowering_sites
        << ", inline_asm_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_inline_asm_sites
        << ", intrinsic_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_intrinsic_sites
        << ", governed_intrinsic_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites
        << ", privileged_intrinsic_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites
        << ", normalized_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_contract_violation_sites
        << ", deterministic_inline_asm_intrinsic_governance_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_inline_asm_intrinsic_governance_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_object_pointer_nullability_generics_profile = object_pointer_type_spellings="
        << frontend_metadata_.object_pointer_type_spellings
        << ", pointer_declarator_entries=" << frontend_metadata_.pointer_declarator_entries
        << ", pointer_declarator_depth_total=" << frontend_metadata_.pointer_declarator_depth_total
        << ", pointer_declarator_token_entries=" << frontend_metadata_.pointer_declarator_token_entries
        << ", nullability_suffix_entries=" << frontend_metadata_.nullability_suffix_entries
        << ", generic_suffix_entries=" << frontend_metadata_.generic_suffix_entries
        << ", terminated_generic_suffix_entries=" << frontend_metadata_.terminated_generic_suffix_entries
        << ", unterminated_generic_suffix_entries=" << frontend_metadata_.unterminated_generic_suffix_entries
        << ", deterministic_object_pointer_nullability_generics_handoff="
        << (frontend_metadata_.deterministic_object_pointer_nullability_generics_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_symbol_graph_scope_resolution_profile = global_symbol_nodes="
        << frontend_metadata_.global_symbol_nodes
        << ", function_symbol_nodes=" << frontend_metadata_.function_symbol_nodes
        << ", interface_symbol_nodes=" << frontend_metadata_.interface_symbol_nodes
        << ", implementation_symbol_nodes=" << frontend_metadata_.implementation_symbol_nodes
        << ", interface_property_symbol_nodes=" << frontend_metadata_.interface_property_symbol_nodes
        << ", implementation_property_symbol_nodes=" << frontend_metadata_.implementation_property_symbol_nodes
        << ", interface_method_symbol_nodes=" << frontend_metadata_.interface_method_symbol_nodes
        << ", implementation_method_symbol_nodes=" << frontend_metadata_.implementation_method_symbol_nodes
        << ", top_level_scope_symbols=" << frontend_metadata_.top_level_scope_symbols
        << ", nested_scope_symbols=" << frontend_metadata_.nested_scope_symbols
        << ", scope_frames_total=" << frontend_metadata_.scope_frames_total
        << ", implementation_interface_resolution_sites="
        << frontend_metadata_.implementation_interface_resolution_sites
        << ", implementation_interface_resolution_hits="
        << frontend_metadata_.implementation_interface_resolution_hits
        << ", implementation_interface_resolution_misses="
        << frontend_metadata_.implementation_interface_resolution_misses
        << ", method_resolution_sites=" << frontend_metadata_.method_resolution_sites
        << ", method_resolution_hits=" << frontend_metadata_.method_resolution_hits
        << ", method_resolution_misses=" << frontend_metadata_.method_resolution_misses
        << ", deterministic_symbol_graph_handoff="
        << (frontend_metadata_.deterministic_symbol_graph_handoff ? "true" : "false")
        << ", deterministic_scope_resolution_handoff="
        << (frontend_metadata_.deterministic_scope_resolution_handoff ? "true" : "false")
        << ", deterministic_symbol_graph_scope_resolution_handoff_key="
        << frontend_metadata_.deterministic_symbol_graph_scope_resolution_handoff_key << "\n";
    out << "source_filename = \"" << program_.module_name << ".objc3\"\n\n";
    EmitFrontendMetadata(out);
    // Historical extraction contract markers retained for fail-closed tooling:
    // out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";
    // for (std::size_t i = 0; i < lowering_ir_boundary_.runtime_dispatch_arg_slots; ++i) {
    //   out << ", i32";
    // }
    // out << ")\n\n";
    if (runtime_dispatch_call_emitted_) {
      out << "; runtime_dispatch_call_decl = "
          << Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_)
          << "\n\n";
    }
    if (synthesized_getter_definition_count_ > 0 ||
        synthesized_setter_definition_count_ > 0) {
      out << "; synthesized_getter_setter_llvm_ir_generation_surface = "
          << "contract_id=objc3c.synthesized.getter.setter.llvm.ir.generation.v1"
          << ";getter_definitions=" << synthesized_getter_definition_count_
          << ";setter_definitions=" << synthesized_setter_definition_count_
          << ";read_current_property_calls="
          << current_property_read_helper_call_count_
          << ";write_current_property_calls="
          << current_property_write_helper_call_count_
          << ";exchange_current_property_calls="
          << current_property_exchange_helper_call_count_
          << ";weak_load_current_property_calls="
          << weak_current_property_load_helper_call_count_
          << ";weak_store_current_property_calls="
          << weak_current_property_store_helper_call_count_
          << ";retain_calls=" << retain_helper_call_count_
          << ";release_calls=" << release_helper_call_count_
          << ";autorelease_calls=" << autorelease_helper_call_count_
          << "\n";
    }
    if (runtime_dispatch_call_emitted_ || direct_dispatch_call_sites_emitted_ > 0) {
      out << "; method_dispatch_and_selector_thunk_lowering_surface = "
          << "contract_id=objc3c.method.dispatch.selector.thunk.lowering.v1"
          << ";runtime_dispatch_symbol=" << lowering_ir_boundary_.runtime_dispatch_symbol
          << ";runtime_dispatch_call_emitted="
          << (runtime_dispatch_call_emitted_ ? "true" : "false")
          << ";runtime_dispatch_call_sites=" << runtime_dispatch_call_sites_emitted_
          << ";direct_dispatch_call_sites=" << direct_dispatch_call_sites_emitted_
          << ";selector_pool_gep_sites=" << selector_pool_gep_sites_emitted_
          << ";selector_pool_count=" << selector_pool_globals_.size()
          << ";direct_dispatch_candidate_sites="
          << frontend_metadata_
                 .dispatch_dispatch_control_lowering_direct_call_candidate_sites
          << ";dynamic_opt_out_sites="
          << frontend_metadata_.dispatch_dispatch_control_lowering_dynamic_opt_out_sites
          << ";selector_pool_symbol="
          << (selector_pool_globals_.empty() ? "null"
                                             : "@__objc3_sec_selector_pool")
          << "\n";
    }
    out << body.str();
    ir = out.str();
    return true;
  }

 private:
  struct LoweredFunctionSignature {
    ValueType return_type = ValueType::I32;
    std::vector<ValueType> param_types;
    std::vector<bool> param_insert_retain;
    std::vector<bool> param_insert_release;
    std::vector<bool> param_insert_autorelease;
    std::vector<bool> param_has_lifetime_bridge;
    bool throws_declared = false;
    bool objc_nserror_declared = false;
    bool objc_status_code_declared = false;
    bool return_insert_retain = false;
    bool return_insert_release = false;
    bool return_insert_autorelease = false;
    bool return_has_lifetime_bridge = false;
    bool interop_foreign_surface = false;
    bool interop_c_foreign_callable = false;
    bool interop_metadata_preservation = false;
    std::size_t ns_error_out_param_index = std::numeric_limits<std::size_t>::max();
    int objc_status_code_success_literal = 0;
    std::string objc_status_code_mapping_symbol;
  };

  struct FunctionEffectInfo {
    bool has_global_write = false;
    bool has_message_send = false;
    std::unordered_set<std::string> called_functions;
  };

  struct LoweredMessageSend {
    std::string receiver = "0";
    bool receiver_is_compile_time_zero = false;
    bool receiver_is_compile_time_nonzero = false;
    std::vector<std::string> args;
    std::size_t explicit_arg_count = 0;
    std::string selector;
    std::string dispatch_surface_family;
    std::string dispatch_surface_entrypoint_family;
    std::string dispatch_symbol = kObjc3RuntimeDispatchSymbol;
    std::string direct_call_symbol;
  };

  struct ControlLabels {
    std::string continue_label;
    std::string break_label;
    bool continue_allowed = false;
    std::size_t scope_depth = 0;
    std::size_t autoreleasepool_depth = 0;
    std::size_t pending_block_dispose_depth = 0;
    std::size_t ownership_cleanup_depth = 0;
    std::size_t arc_cleanup_depth = 0;
  };

  struct BlockBinding {
    std::string storage_ptr;
    const Expr *literal = nullptr;
    std::string promoted_handle_ptr;
  };

  struct TypedKeyPathArtifact {
    std::size_t ordinal = 0;
    bool root_is_self = false;
    std::string root_name;
    std::string component_path;
    std::string profile;
    std::string descriptor_symbol;
  };

  struct PendingBlockDisposeCall {
    std::string helper_symbol;
    std::string storage_ptr;
  };

  struct PendingOwnershipCleanupCall {
    std::string binding_name;
    std::string storage_ptr;
    std::string cleanup_function_symbol;
    std::string resource_close_symbol;
    bool has_resource_invalid_value = false;
    int resource_invalid_value = 0;
    bool active = true;
  };

  struct FunctionContext {
    std::vector<std::string> entry_lines;
    std::vector<std::string> code_lines;
    std::vector<std::unordered_map<std::string, std::string>> scopes;
    std::unordered_map<std::string, BlockBinding> block_bindings;
    std::vector<PendingBlockDisposeCall> pending_block_dispose_calls;
    std::vector<PendingOwnershipCleanupCall> pending_ownership_cleanup_calls;
    std::vector<ControlLabels> control_stack;
    std::vector<std::string> autoreleasepool_scope_symbols;
    std::vector<std::vector<const BlockStmt *>> pending_defer_scope_blocks;
    std::vector<std::size_t> pending_block_dispose_scope_depths;
    std::vector<std::size_t> pending_ownership_cleanup_scope_depths;
    std::vector<std::size_t> arc_cleanup_scope_depths;
    std::unordered_set<std::string> nil_bound_ptrs;
    std::unordered_set<std::string> nonzero_bound_ptrs;
    std::unordered_map<std::string, int> const_value_ptrs;
    std::unordered_map<std::string, int> immediate_identifiers;
    std::vector<std::string> arc_owned_cleanup_ptrs;
    std::unordered_set<std::string> arc_owned_cleanup_ptr_set;
    std::unordered_set<std::string> arc_owned_storage_ptrs;
    std::unordered_map<std::string, std::size_t> ownership_cleanup_call_indices;
    struct ErrorHandlerFrame {
      std::string error_slot_ptr;
      std::string dispatch_label;
      std::size_t scope_depth = 0;
      std::size_t autoreleasepool_depth = 0;
      std::size_t pending_block_dispose_depth = 0;
      std::size_t ownership_cleanup_depth = 0;
      std::size_t arc_cleanup_depth = 0;
    };
    std::vector<ErrorHandlerFrame> error_handler_stack;
    std::string function_error_out_param;
    ValueType return_type = ValueType::I32;
    bool async_runtime_helper_enabled = false;
    bool actor_runtime_helper_enabled = false;
    bool actor_nonisolated_entry_enabled = false;
    std::string current_implementation_name;
    std::string current_superclass_name;
    bool current_method_is_class_method = false;
    int async_resume_entry_tag = 0;
    int async_executor_tag = 0;
    int temp_counter = 0;
    int label_counter = 0;
    bool terminated = false;
    bool global_proofs_invalidated = false;
    bool arc_return_insert_retain = false;
    bool arc_return_insert_autorelease = false;
  };

  static const char *LLVMScalarType(ValueType type) {
    if (type == ValueType::Bool) {
      return "i1";
    }
    if (type == ValueType::Void) {
      return "void";
    }
    return "i32";
  }

  static bool IsArcExecutableObjectParam(const FuncParam &param) {
    return param.id_spelling || param.instancetype_spelling ||
           param.object_pointer_type_spelling;
  }

  static bool IsArcExecutableObjectReturn(const FunctionDecl &fn) {
    return fn.return_id_spelling || fn.return_instancetype_spelling ||
           fn.return_object_pointer_type_spelling;
  }

  static bool IsArcExecutableObjectReturn(const Objc3MethodDecl &method) {
    return method.return_id_spelling || method.return_instancetype_spelling ||
           method.return_object_pointer_type_spelling;
  }

  static std::string LowercaseAscii(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char ch) {
                     return static_cast<char>(std::tolower(ch));
                   });
    return text;
  }

  static int StablePositiveAsyncTag(const std::string &text) {
    std::uint32_t hash = 2166136261u;
    for (unsigned char ch : text) {
      hash ^= static_cast<std::uint32_t>(ch);
      hash *= 16777619u;
    }
    return static_cast<int>((hash % 2147483646u) + 1u);
  }

  static int ExecutorAffinityTag(const FunctionDecl &fn) {
    if (!fn.executor_affinity_declared) {
      return 0;
    }
    if (fn.executor_affinity_kind == "main") {
      return 1;
    }
    if (fn.executor_affinity_named && !fn.executor_affinity_name.empty()) {
      return StablePositiveAsyncTag("executor:" + fn.executor_affinity_name);
    }
    return StablePositiveAsyncTag("executor-kind:" + fn.executor_affinity_kind);
  }

  static int ExecutorAffinityTag(const Objc3MethodDecl &method) {
    if (!method.executor_affinity_declared) {
      return 0;
    }
    if (method.executor_affinity_kind == "main") {
      return 1;
    }
    if (method.executor_affinity_named && !method.executor_affinity_name.empty()) {
      return StablePositiveAsyncTag("executor:" + method.executor_affinity_name);
    }
    return StablePositiveAsyncTag("executor-kind:" + method.executor_affinity_kind);
  }

  static int AsyncResumeEntryTag(const FunctionDecl &fn) {
    return StablePositiveAsyncTag("resume-entry:function:" + fn.name);
  }

  static int AsyncResumeEntryTag(const MethodDefinition &method_def) {
    return StablePositiveAsyncTag("resume-entry:method:" + method_def.symbol);
  }

  bool IsActorImplementation(const std::string &name) const {
    if (name.empty()) {
      return false;
    }
    for (const auto &interface_decl : program_.interfaces) {
      if (!interface_decl.has_category && interface_decl.name == name) {
        return interface_decl.is_actor;
      }
    }
    return false;
  }

  bool TryEmitConcurrencyTaskRuntimeLoweringCall(const Expr *expr, FunctionContext &ctx,
                                           std::string &result_out) const {
    if (expr == nullptr || !ctx.async_runtime_helper_enabled) {
      return false;
    }

    const std::string lowered = LowercaseAscii(expr->ident);
    const auto emit_unary_runtime_call = [&](const char *symbol) {
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(symbol) + "(i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
    };

    if (lowered == "task_spawn_child" || lowered == "spawn_task") {
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(kObjc3RuntimeSpawnTaskI32Symbol) +
                               "(i32 1, i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }
    if (lowered == "detached_task_create") {
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(kObjc3RuntimeSpawnTaskI32Symbol) +
                               "(i32 2, i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }
    if (lowered == "with_task_group_scope") {
      emit_unary_runtime_call(kObjc3RuntimeEnterTaskGroupScopeI32Symbol);
      return true;
    }
    if (lowered == "task_group_add_task") {
      emit_unary_runtime_call(kObjc3RuntimeAddTaskGroupTaskI32Symbol);
      return true;
    }
    if (lowered == "task_group_cancel_all") {
      emit_unary_runtime_call(kObjc3RuntimeCancelTaskGroupI32Symbol);
      return true;
    }
    if (lowered == "task_runtime_cancelled_value") {
      emit_unary_runtime_call(kObjc3RuntimeTaskIsCancelledI32Symbol);
      return true;
    }
    if (lowered == "task_runtime_on_cancel") {
      emit_unary_runtime_call(kObjc3RuntimeTaskOnCancelI32Symbol);
      return true;
    }
    if (lowered == "task_group_wait_next" || lowered == "wait_next") {
      const std::string waited = NewTemp(ctx);
      ctx.code_lines.push_back("  " + waited + " = call i32 @" +
                               std::string(kObjc3RuntimeWaitTaskGroupNextI32Symbol) +
                               "(i32 " + std::to_string(ctx.async_executor_tag) +
                               ")");
      const std::string hopped = NewTemp(ctx);
      ctx.code_lines.push_back("  " + hopped + " = call i32 @" +
                               std::string(kObjc3RuntimeExecutorHopI32Symbol) +
                               "(i32 " + waited + ", i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = hopped;
      return true;
    }
    return false;
  }

  bool TryEmitConcurrencyActorLoweringCall(const Expr *expr, FunctionContext &ctx,
                                     std::string &result_out) const {
    if (expr == nullptr || !ctx.actor_runtime_helper_enabled) {
      return false;
    }

    const std::string lowered = LowercaseAscii(expr->ident);
    if (lowered == "actor_enter_isolation_thunk") {
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(
                                   kObjc3RuntimeActorEnterIsolationThunkI32Symbol) +
                               "(i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }
    if (lowered == "actor_nonisolated_entry" &&
        ctx.actor_nonisolated_entry_enabled) {
      const std::string value =
          expr->args.empty() ? "0" : EmitExpr(expr->args.front().get(), ctx);
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(
                                   kObjc3RuntimeActorEnterNonisolatedI32Symbol) +
                               "(i32 " + value + ", i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }
    if (lowered == "actor_hop_to_executor") {
      const std::string value =
          expr->args.empty() ? "0" : EmitExpr(expr->args.front().get(), ctx);
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(kObjc3RuntimeActorHopToExecutorI32Symbol) +
                               "(i32 " + value + ", i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }
    if (lowered == "actor_bind_executor") {
      const std::string actor_handle =
          expr->args.empty() ? "0" : EmitExpr(expr->args.front().get(), ctx);
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(kObjc3RuntimeActorBindExecutorI32Symbol) +
                               "(i32 " + actor_handle + ", i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }
    if (lowered == "actor_mailbox_enqueue") {
      const std::string actor_handle =
          expr->args.empty() ? "0" : EmitExpr(expr->args.front().get(), ctx);
      const std::string value = expr->args.size() < 2u
                                    ? "0"
                                    : EmitExpr(expr->args[1].get(), ctx);
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(
                                   kObjc3RuntimeActorMailboxEnqueueI32Symbol) +
                               "(i32 " + actor_handle + ", i32 " + value +
                               ", i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }
    if (lowered == "actor_mailbox_drain_next") {
      const std::string actor_handle =
          expr->args.empty() ? "0" : EmitExpr(expr->args.front().get(), ctx);
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(
                                   kObjc3RuntimeActorMailboxDrainNextI32Symbol) +
                               "(i32 " + actor_handle + ", i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }
    if (lowered == "replay_proof_step") {
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(
                                   kObjc3RuntimeActorRecordReplayProofI32Symbol) +
                               "(i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }
    if (lowered == "race_guard_lock") {
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back("  " + out + " = call i32 @" +
                               std::string(
                                   kObjc3RuntimeActorRecordRaceGuardI32Symbol) +
                               "(i32 " +
                               std::to_string(ctx.async_executor_tag) + ")");
      InvalidateGlobalProofState(ctx);
      result_out = out;
      return true;
    }

    return false;
  }

  static bool IsNSErrorOutParameterSite(const FuncParam &param) {
    if (!param.object_pointer_type_spelling) {
      return false;
    }
    const std::string lowered_type = LowercaseAscii(param.object_pointer_type_name);
    if (lowered_type != "nserror") {
      return false;
    }
    const std::string lowered_name = LowercaseAscii(param.name);
    return param.has_pointer_declarator ||
           lowered_name.find("error") != std::string::npos;
  }

  static bool EffectiveArcParamInsertRetain(const FuncParam &param,
                                            bool arc_mode_enabled) {
    return param.ownership_insert_retain ||
           (arc_mode_enabled && !param.has_ownership_qualifier &&
            IsArcExecutableObjectParam(param));
  }

  static bool EffectiveArcParamInsertRelease(const FuncParam &param,
                                             bool arc_mode_enabled) {
    return param.ownership_insert_release ||
           (arc_mode_enabled && !param.has_ownership_qualifier &&
            IsArcExecutableObjectParam(param));
  }

  static bool EffectiveArcReturnInsertRetain(const FunctionDecl &fn,
                                             bool arc_mode_enabled) {
    return fn.return_ownership_insert_retain ||
           (arc_mode_enabled && !fn.has_return_ownership_qualifier &&
            IsArcExecutableObjectReturn(fn));
  }

  static bool EffectiveArcReturnInsertRetain(const Objc3MethodDecl &method,
                                             bool arc_mode_enabled) {
    return method.return_ownership_insert_retain ||
           (arc_mode_enabled && !method.has_return_ownership_qualifier &&
            IsArcExecutableObjectReturn(method));
  }

  static bool EffectiveArcReturnInsertAutorelease(const FunctionDecl &fn) {
    return fn.return_ownership_insert_autorelease;
  }

  static bool EffectiveArcReturnInsertAutorelease(
      const Objc3MethodDecl &method) {
    return method.return_ownership_insert_autorelease;
  }

  static bool HasInteropInteropSurface(const FunctionDecl &fn) {
    return fn.objc_foreign_declared || fn.objc_import_module_declared ||
           fn.objc_cxx_name_declared || fn.objc_header_name_declared ||
           fn.objc_swift_name_declared || fn.objc_swift_private_declared ||
           fn.objc_export_header_declared || fn.objc_abi_align_declared ||
           fn.objc_foreign_type_declared || fn.objc_mixed_image_declared ||
           fn.objc_package_entry_declared;
  }

  static bool HasInteropLifetimeBridge(const FuncParam &param) {
    return !param.ownership_lifetime_profile.empty();
  }

  static bool HasInteropLifetimeBridge(const FunctionDecl &fn) {
    if (!fn.return_ownership_lifetime_profile.empty()) {
      return true;
    }
    return std::any_of(fn.params.begin(), fn.params.end(),
                       [](const FuncParam &param) {
                         return HasInteropLifetimeBridge(param);
                       });
  }

  static std::map<std::string, LoweredFunctionSignature> BuildLoweredFunctionSignatures(const Objc3Program &program) {
    std::map<std::string, LoweredFunctionSignature> signatures;
    for (const auto &fn : program.functions) {
      LoweredFunctionSignature signature;
      signature.return_type = fn.return_type;
      signature.throws_declared = fn.throws_declared;
      signature.objc_nserror_declared = fn.objc_nserror_declared;
      signature.objc_status_code_declared = fn.objc_status_code_declared;
      signature.objc_status_code_mapping_symbol = fn.objc_status_code_mapping_symbol;
      signature.return_insert_retain = EffectiveArcReturnInsertRetain(fn, false);
      signature.return_insert_release = fn.return_ownership_insert_release;
      signature.return_insert_autorelease =
          EffectiveArcReturnInsertAutorelease(fn);
      signature.return_has_lifetime_bridge =
          !fn.return_ownership_lifetime_profile.empty();
      signature.interop_foreign_surface = HasInteropInteropSurface(fn);
      signature.interop_c_foreign_callable = fn.objc_foreign_declared;
      signature.interop_metadata_preservation = HasInteropInteropSurface(fn);
      if (!fn.objc_status_code_success_literal.empty()) {
        try {
          signature.objc_status_code_success_literal =
              std::stoi(fn.objc_status_code_success_literal);
        } catch (...) {
          signature.objc_status_code_success_literal = 0;
        }
      }
      signature.param_types.reserve(fn.params.size());
      signature.param_insert_retain.reserve(fn.params.size());
      signature.param_insert_release.reserve(fn.params.size());
      signature.param_insert_autorelease.reserve(fn.params.size());
      signature.param_has_lifetime_bridge.reserve(fn.params.size());
      for (std::size_t param_index = 0; param_index < fn.params.size(); ++param_index) {
        const auto &param = fn.params[param_index];
        signature.param_types.push_back(param.type);
        signature.param_insert_retain.push_back(
            EffectiveArcParamInsertRetain(param, false));
        signature.param_insert_release.push_back(
            EffectiveArcParamInsertRelease(param, false));
        signature.param_insert_autorelease.push_back(
            param.ownership_insert_autorelease);
        signature.param_has_lifetime_bridge.push_back(
            HasInteropLifetimeBridge(param));
        if (signature.ns_error_out_param_index ==
                std::numeric_limits<std::size_t>::max() &&
            IsNSErrorOutParameterSite(param)) {
          signature.ns_error_out_param_index = param_index;
        }
      }
      auto existing = signatures.find(fn.name);
      if (existing == signatures.end()) {
        signatures.emplace(fn.name, std::move(signature));
      }
    }
    return signatures;
  }

  static std::size_t CountVectorSignatureFunctions(const Objc3Program &program) {
    std::unordered_set<std::string> vector_function_names;
    for (const auto &fn : program.functions) {
      bool has_vector_signature = fn.return_vector_spelling;
      if (!has_vector_signature) {
        for (const auto &param : fn.params) {
          if (param.vector_spelling) {
            has_vector_signature = true;
            break;
          }
        }
      }
      if (has_vector_signature) {
        vector_function_names.insert(fn.name);
      }
    }
    return vector_function_names.size();
  }

  static std::string EscapeCStringLiteral(const std::string &text) {
    std::ostringstream out;
    for (unsigned char c : text) {
      std::ostringstream byte;
      byte << std::hex << std::uppercase << static_cast<int>(c);
      std::string value = byte.str();
      if (value.size() < 2) {
        value = "0" + value;
      }
      if (c == '\\' || c == '"' || c < 32 || c > 126) {
        out << "\\" << value;
        continue;
      }
      out << static_cast<char>(c);
    }
    return out.str();
  }

  static std::string JoinStringParts(const std::vector<std::string> &parts,
                                     const std::string &delimiter) {
    std::ostringstream out;
    for (std::size_t i = 0; i < parts.size(); ++i) {
      if (i != 0) {
        out << delimiter;
      }
      out << parts[i];
    }
    return out.str();
  }

  static std::uint64_t StableRuntimeMetadataLinkerAnchorHash(
      const std::string &text) {
    std::uint64_t hash = 1469598103934665603ull;
    for (unsigned char c : text) {
      hash ^= static_cast<std::uint64_t>(c);
      hash *= 1099511628211ull;
    }
    return hash;
  }

  static std::string LowerHex64(std::uint64_t value) {
    std::ostringstream out;
    out << std::hex << std::nouppercase << value;
    return out.str();
  }

  std::string RuntimeMetadataLinkerAnchorSuffix() const {
    std::string seed = program_.module_name;
    seed += "|";
    seed += frontend_metadata_.runtime_metadata_class_metaclass_typed_handoff_replay_key;
    seed += "|";
    seed += frontend_metadata_.runtime_metadata_protocol_category_typed_handoff_replay_key;
    seed += "|";
    seed += frontend_metadata_.runtime_metadata_member_table_typed_handoff_replay_key;
    seed += "|";
    seed +=
        frontend_metadata_
            .runtime_metadata_archive_static_link_translation_unit_identity_key;
    return LowerHex64(StableRuntimeMetadataLinkerAnchorHash(seed));
  }

  std::string RuntimeMetadataLinkerAnchorSymbol() const {
    return "objc3_runtime_metadata_link_anchor_" +
           RuntimeMetadataLinkerAnchorSuffix();
  }

  std::string RuntimeMetadataDiscoveryRootSymbol() const {
    return "objc3_runtime_metadata_discovery_root_" +
           RuntimeMetadataLinkerAnchorSuffix();
  }

  static std::string MakeModuleIdentifierSafeSuffix(const std::string &text) {
    return objc3c::support::MakeIdentifierSafeSuffix(text, "module");
  }

  static std::string EncodeBoundaryTokenValueHex(const std::string &text) {
    static constexpr char kHex[] = "0123456789abcdef";
    std::string out;
    out.reserve(text.size() * 2u);
    for (unsigned char ch : text) {
      out.push_back(kHex[(ch >> 4) & 0x0f]);
      out.push_back(kHex[ch & 0x0f]);
    }
    return out;
  }

  bool ShouldEmitRuntimeBootstrapLowering() const {
    return ShouldEmitRuntimeMetadataSectionScaffold() &&
           frontend_metadata_.runtime_bootstrap_lowering_ready &&
           frontend_metadata_.runtime_bootstrap_lowering_fail_closed &&
           !frontend_metadata_.runtime_bootstrap_lowering_contract_id.empty() &&
           !frontend_metadata_.runtime_bootstrap_lowering_constructor_root_symbol
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_init_stub_symbol_prefix.empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_registration_table_symbol_prefix
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_registration_entrypoint_symbol
                .empty() &&
           !frontend_metadata_.runtime_bootstrap_lowering_global_ctor_list_model
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_registration_table_layout_model
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_image_local_initialization_model
                .empty() &&
           frontend_metadata_
                   .runtime_bootstrap_lowering_registration_table_abi_version >
               0 &&
           frontend_metadata_
                   .runtime_bootstrap_lowering_registration_table_pointer_field_count >
               0 &&
           frontend_metadata_
               .runtime_bootstrap_lowering_bootstrap_ir_materialization_landed &&
           frontend_metadata_
               .runtime_bootstrap_lowering_image_local_initialization_landed &&
           !frontend_metadata_
                .runtime_metadata_archive_static_link_translation_unit_identity_key
                .empty();
  }

  bool ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering() const {
    return ShouldEmitRuntimeBootstrapLowering() &&
           !frontend_metadata_
                .runtime_bootstrap_registration_descriptor_image_root_lowering_contract_id
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_registration_descriptor_identifier.empty() &&
           !frontend_metadata_.runtime_bootstrap_image_root_identifier.empty();
  }

  std::string RuntimeBootstrapSafeSuffix() const {
    return MakeModuleIdentifierSafeSuffix(
        frontend_metadata_
            .runtime_metadata_archive_static_link_translation_unit_identity_key);
  }

  std::string RuntimeBootstrapModuleNameGlobalSymbol() const {
    return ".objc3_runtime_module_name_" + RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapTranslationUnitIdentityGlobalSymbol() const {
    return ".objc3_runtime_translation_unit_identity_" +
           RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapImageDescriptorSymbol() const {
    return "__objc3_runtime_image_descriptor_" + RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapRegistrationDescriptorIdentifierSafeSuffix() const {
    return MakeModuleIdentifierSafeSuffix(
        frontend_metadata_.runtime_bootstrap_registration_descriptor_identifier);
  }

  std::string RuntimeBootstrapImageRootIdentifierSafeSuffix() const {
    return MakeModuleIdentifierSafeSuffix(
        frontend_metadata_.runtime_bootstrap_image_root_identifier);
  }

  std::string RuntimeBootstrapRegistrationDescriptorNameGlobalSymbol() const {
    return ".objc3_runtime_registration_descriptor_name_" +
           RuntimeBootstrapRegistrationDescriptorIdentifierSafeSuffix();
  }

  std::string RuntimeBootstrapImageRootNameGlobalSymbol() const {
    return ".objc3_runtime_image_root_name_" +
           RuntimeBootstrapImageRootIdentifierSafeSuffix();
  }

  std::string RuntimeBootstrapRegistrationDescriptorSymbol() const {
    return std::string(kObjc3RuntimeBootstrapRegistrationDescriptorSymbolPrefix) +
           RuntimeBootstrapRegistrationDescriptorIdentifierSafeSuffix();
  }

  std::string RuntimeBootstrapImageRootSymbol() const {
    return std::string(kObjc3RuntimeBootstrapImageRootSymbolPrefix) +
           RuntimeBootstrapImageRootIdentifierSafeSuffix();
  }

  std::string RuntimeBootstrapInitStubSymbol() const {
    return frontend_metadata_.runtime_bootstrap_lowering_init_stub_symbol_prefix +
           RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapRegistrationTableSymbol() const {
    return frontend_metadata_
               .runtime_bootstrap_lowering_registration_table_symbol_prefix +
           RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapImageLocalInitStateSymbol() const {
    return frontend_metadata_
               .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix +
           RuntimeBootstrapSafeSuffix();
  }

  static constexpr const char *RuntimeBootstrapImageDescriptorType() {
    return "{ ptr, ptr, i64, i64, i64, i64, i64, i64 }";
  }

  static constexpr const char *RuntimeBootstrapRegistrationTableType() {
    return "{ i64, i64, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr }";
  }

  void EmitFrontendMetadata(std::ostringstream &out) const {
    // executable source-closure freeze anchor: IR currently
    // publishes interface/protocol/category/linking metadata as the canonical
    // source-closure proof surface only. Later realization work must
    // preserve these identities while adding runnable class/category/protocol
    // behavior.
    // executable class/metaclass source-closure anchor: the IR
    // handoff now carries declaration-owned parent identities, method-owner
    // identities, and class/metaclass object identities so later realization
    // work consumes the same fail-closed source model.
    if (frontend_metadata_.executable_class_metaclass_source_closure_ready) {
      out << "; executable_class_metaclass_source_closure = contract="
          << frontend_metadata_
                 .executable_class_metaclass_source_closure_contract_id
          << ";parent_identity_model="
          << frontend_metadata_
                 .executable_class_metaclass_parent_identity_model
          << ";method_owner_identity_model="
          << frontend_metadata_
                 .executable_class_metaclass_method_owner_identity_model
          << ";class_object_identity_model="
          << frontend_metadata_
                 .executable_class_metaclass_object_identity_model
          << ";declaration_node_count="
          << frontend_metadata_
                 .executable_class_metaclass_declaration_node_count
          << ";parent_identity_edge_count="
          << frontend_metadata_
                 .executable_class_metaclass_parent_identity_edge_count
          << ";method_owner_identity_edge_count="
          << frontend_metadata_
                 .executable_class_metaclass_method_owner_identity_edge_count
          << ";class_object_identity_edge_count="
          << frontend_metadata_
                 .executable_class_metaclass_object_identity_edge_count
          << "\n";
    }
    // executable protocol/category source-closure anchor: the IR
    // handoff now carries protocol inheritance, category attachment, and
    // adopted-protocol conformance identities so later object-model semantic
    // issues consume the same fail-closed source model.
    // object-model semantic-rule freeze anchor: IR stays evidence-only
    // for the frozen semantic boundary covering realization legality,
    // inheritance legality, override compatibility, protocol conformance, and
    // deterministic category merge behavior; executable enforcement begins in
    // later lane-B work.
    // protocol-conformance implementation anchor: IR remains a
    // evidence-only consumer of the sema-owned protocol conformance result while
    // publishing the same protocol/category source identities after sema
    // starts enforcing required-vs-optional protocol member coverage with
    // fail-closed diagnostics.
    // category-merge implementation anchor: IR remains downstream of
    // the sema-owned realized-class category merge/conflict decision and must
    // not reinterpret attachment legality or concrete message resolution.
    // inheritance/override legality anchor: IR remains downstream of
    // the sema-owned realized-class inheritance and override legality result
    // and must not reinterpret superclass cycles, missing realization closure,
    // or inherited member compatibility.
    if (frontend_metadata_.executable_protocol_category_source_closure_ready) {
      out << "; executable_protocol_category_source_closure = contract="
          << frontend_metadata_
                 .executable_protocol_category_source_closure_contract_id
          << ";protocol_inheritance_model="
          << frontend_metadata_
                 .executable_protocol_inheritance_identity_model
          << ";category_attachment_model="
          << frontend_metadata_
                 .executable_category_attachment_identity_model
          << ";protocol_category_conformance_model="
          << frontend_metadata_
                 .executable_protocol_category_conformance_identity_model
          << ";protocol_node_count="
          << frontend_metadata_
                 .executable_protocol_category_protocol_node_count
          << ";category_node_count="
          << frontend_metadata_
                 .executable_protocol_category_category_node_count
          << ";protocol_inheritance_edge_count="
          << frontend_metadata_
                 .executable_protocol_inheritance_identity_edge_count
          << ";category_attachment_edge_count="
          << frontend_metadata_
                 .executable_category_attachment_identity_edge_count
          << ";protocol_category_conformance_edge_count="
          << frontend_metadata_
                 .executable_protocol_category_conformance_identity_edge_count
          << "\n";
    }
    out << "!objc3.frontend = !{!0}\n";
    out << "!objc3.objc_interface_implementation = !{!1}\n";
    out << "!objc3.objc_protocol_category = !{!2}\n";
    out << "!objc3.objc_class_protocol_category_linking = !{!7}\n";
    out << "!objc3.objc_selector_normalization = !{!3}\n";
    out << "!objc3.objc_property_attribute = !{!4}\n";
    out << "!objc3.objc_runtime_metadata_source_ownership = !{!45}\n";
    out << "!objc3.objc_runtime_export_legality = !{!46}\n";
    out << "!objc3.objc_runtime_export_enforcement = !{!47}\n";
    out << "!objc3.objc_runtime_metadata_section_abi = !{!48}\n";
    out << "!objc3.objc_runtime_metadata_section_publication = !{!49}\n";
    out << "!objc3.objc_runtime_metadata_layout_policy = !{!55}\n";
    out << "!objc3.objc_runtime_class_metaclass_emission = !{!56}\n";
    out << "!objc3.objc_runtime_protocol_category_emission = !{!57}\n";
    out << "!objc3.objc_runtime_member_table_emission = !{!58}\n";
    out << "!objc3.objc_runtime_selector_string_pool_emission = !{!59}\n";
    out << "!objc3.objc_runtime_binary_inspection_harness = !{!60}\n";
    out << "!objc3.objc_runtime_object_packaging_retention = !{!61}\n";
    out << "!objc3.objc_runtime_linker_retention = !{!62}\n";
    out << "!objc3.objc_runtime_archive_static_link_discovery = !{!63}\n";
    out << "!objc3.objc_runtime_metadata_emission_gate = !{!64}\n";
    out << "!objc3.objc_runtime_metadata_object_emission_closeout = !{!65}\n";
    out << "!objc3.objc_runtime_metadata_object_inspection = !{!50}\n";
    out << "!objc3.objc_runtime_support_library = !{!51}\n";
    out << "!objc3.objc_runtime_support_library_core_feature = !{!52}\n";
    out << "!objc3.objc_runtime_support_library_link_wiring = !{!53}\n";
    out << "!objc3.objc_executable_metadata_debug_projection = !{!54}\n";
    out << "!objc3.objc_object_pointer_nullability_generics = !{!5}\n";
    out << "!objc3.objc_symbol_graph_scope_resolution = !{!6}\n";
    out << "!objc3.objc_id_class_sel_object_pointer_typecheck = !{!8}\n";
    out << "!objc3.objc_dispatch_surface_classification = !{!66}\n";
    out << "!objc3.objc_executable_ivar_layout_emission = !{!67}\n";
    out << "!objc3.objc_executable_synthesized_accessor_property_lowering = !{!68}\n";
    out << "!objc3.objc_runtime_ownership_hook_emission = !{!69}\n";
    out << "!objc3.objc_runtime_memory_management_api = !{!70}\n";
    out << "!objc3.objc_runtime_memory_management_implementation = !{!71}\n";
    out << "!objc3.objc_ownership_runtime_gate = !{!72}\n";
    out << "!objc3.objc_runnable_block_runtime_gate = !{!73}\n";
    out << "!objc3.objc_runnable_block_execution_matrix = !{!74}\n";
    out << "!objc3.objc_arc_source_mode_boundary = !{!75}\n";
    out << "!objc3.objc_arc_mode_handling = !{!76}\n";
    out << "!objc3.objc_arc_semantic_rules = !{!77}\n";
    out << "!objc3.objc_arc_inference_lifetime = !{!78}\n";
    out << "!objc3.objc_arc_interaction_semantics = !{!79}\n";
    out << "!objc3.objc_arc_automatic_insertions = !{!80}\n";
    out << "!objc3.objc_arc_cleanup_weak_lifetime_hooks = !{!81}\n";
    out << "!objc3.objc_arc_block_autorelease_return_lowering = !{!82}\n";
    out << "!objc3.objc_runtime_arc_helper_api_surface = !{!83}\n";
    out << "!objc3.objc_runtime_arc_helper_runtime_support = !{!84}\n";
    out << "!objc3.objc_runtime_arc_debug_instrumentation = !{!85}\n";
    out << "!objc3.objc_runnable_arc_runtime_gate = !{!86}\n";
    out << "!objc3.objc_message_send_selector_lowering = !{!9}\n";
    out << "!objc3.objc_dispatch_abi_marshalling = !{!10}\n";
    out << "!objc3.objc_nil_receiver_semantics_foldability = !{!11}\n";
    out << "!objc3.objc_super_dispatch_method_family = !{!12}\n";
    out << "!objc3.objc_runtime_link_host_link = !{!13}\n";
    out << "!objc3.objc_ownership_qualifier_lowering = !{!14}\n";
    out << "!objc3.objc_retain_release_operation_lowering = !{!15}\n";
    out << "!objc3.objc_autoreleasepool_scope_lowering = !{!16}\n";
    out << "!objc3.objc_weak_unowned_semantics_lowering = !{!17}\n";
    out << "!objc3.objc_arc_diagnostics_fixit_lowering = !{!18}\n";
    out << "!objc3.objc_block_literal_capture_lowering = !{!19}\n";
    out << "!objc3.objc_block_abi_invoke_trampoline_lowering = !{!20}\n";
    out << "!objc3.objc_block_storage_escape_lowering = !{!21}\n";
    out << "!objc3.objc_block_copy_dispose_lowering = !{!22}\n";
    out << "!objc3.objc_block_determinism_perf_baseline_lowering = !{!23}\n";
    out << "!objc3.objc_lightweight_generic_constraint_lowering = !{!24}\n";
    out << "!objc3.objc_nullability_flow_warning_precision_lowering = !{!25}\n";
    out << "!objc3.objc_protocol_qualified_object_type_lowering = !{!26}\n";
    out << "!objc3.objc_variance_bridge_cast_lowering = !{!27}\n";
    out << "!objc3.objc_generic_metadata_abi_lowering = !{!28}\n";
    out << "!objc3.objc_module_import_graph_lowering = !{!29}\n";
    out << "!objc3.objc_namespace_collision_shadowing_lowering = !{!30}\n";
    out << "!objc3.objc_public_private_api_partition_lowering = !{!31}\n";
    out << "!objc3.objc_incremental_module_cache_invalidation_lowering = !{!32}\n";
    out << "!objc3.objc_cross_module_conformance_lowering = !{!33}\n";
    out << "!objc3.objc_error_handling_throws_abi_propagation_lowering = !{!87}\n";
    out << "!objc3.objc_error_handling_result_and_bridging_artifact_replay = !{!88}\n";
    out << "!objc3.objc_error_handling_error_runtime_bridge_helper = !{!89}\n";
    out << "!objc3.objc_error_handling_live_error_runtime_integration = !{!90}\n";
    out << "!objc3.objc_concurrency_continuation_runtime_helper = !{!91}\n";
    out << "!objc3.objc_concurrency_live_continuation_runtime_integration = !{!92}\n";
    out << "!objc3.objc_concurrency_task_runtime_abi_completion = !{!93}\n";
    out << "!objc3.objc_concurrency_scheduler_executor_runtime_contract = !{!94}\n";
    out << "!objc3.objc_concurrency_live_task_runtime_integration = !{!95}\n";
    out << "!objc3.objc_concurrency_task_runtime_hardening = !{!96}\n";
    out << "!objc3.objc_concurrency_actor_lowering_and_metadata = !{!97}\n";
    out << "!objc3.objc_dispatch_dispatch_control_lowering_contract = !{!102}\n";
    out << "!objc3.objc_interop_interop_lowering_and_abi_contract = !{!108}\n";
    out << "!objc3.objc_interop_foreign_call_and_lifetime_lowering = !{!109}\n";
    out << "!objc3.objc_interop_ffi_metadata_and_interface_preservation = !{!110}\n";
    out << "!objc3.objc_interop_bridge_packaging_and_toolchain_contract = !{!111}\n";
    out << "!objc3.objc_interop_header_module_and_bridge_generation = !{!112}\n";
    out << "!objc3.objc_metaprogramming_expansion_and_lowering_contract = !{!104}\n";
    out << "!objc3.objc_metaprogramming_synthesized_ast_and_ir_emission = !{!105}\n";
    out << "!objc3.objc_metaprogramming_module_interface_and_replay_preservation = !{!106}\n";
    out << "!objc3.objc_metaprogramming_expansion_host_and_runtime_boundary = !{!107}\n";
    out << "!objc3.objc_dispatch_dispatch_metadata_and_interface_preservation = !{!103}\n";
    out << "!objc3.objc_ownership_system_extension_lowering_contract = !{!98}\n";
    out << "!objc3.objc_ownership_borrowed_pointer_and_retainable_family_abi_completion = !{!99}\n";
    out << "!objc3.objc_ownership_system_helper_runtime_contract = !{!100}\n";
    out << "!objc3.objc_ownership_live_cleanup_retainable_runtime_integration = !{!101}\n";
    out << "!objc3.objc_throws_propagation_lowering = !{!34}\n";
    out << "!objc3.objc_unwind_cleanup_lowering = !{!35}\n";
    out << "!objc3.objc_ns_error_bridging_lowering = !{!36}\n";
    out << "!objc3.objc_unsafe_pointer_extension_lowering = !{!37}\n";
    out << "!objc3.objc_inline_asm_intrinsic_governance_lowering = !{!38}\n";
    out << "!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}\n";
    out << "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}\n";
    out << "!objc3.objc_actor_isolation_sendability_lowering = !{!41}\n";
    out << "!objc3.objc_await_lowering_suspension_state_lowering = !{!42}\n";
    out << "!objc3.objc_async_continuation_lowering = !{!43}\n";
    out << "!objc3.objc_error_diagnostics_recovery_lowering = !{!44}\n";
    out << "!0 = !{i32 " << static_cast<unsigned>(frontend_metadata_.language_version) << ", !\""
        << EscapeCStringLiteral(frontend_metadata_.language_profile) << "\", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.migration_legacy_yes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.migration_legacy_no) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.migration_legacy_null) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.migration_legacy_total()) << "}\n";
    out << "!1 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.declared_interfaces) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.declared_implementations) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_interface_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_implementation_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.interface_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.implementation_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.linked_implementation_symbols) << ", i1 "
        << (frontend_metadata_.deterministic_interface_implementation_handoff ? 1 : 0) << "}\n";
    out << "!2 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.declared_protocols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.declared_categories) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_protocol_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_category_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.protocol_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.category_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.linked_category_symbols) << ", i1 "
        << (frontend_metadata_.deterministic_protocol_category_handoff ? 1 : 0) << "}\n";
    out << "!3 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.selector_method_declaration_entries)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.selector_normalized_method_declarations)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.selector_piece_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.selector_piece_parameter_links) << ", i1 "
        << (frontend_metadata_.deterministic_selector_normalization_handoff ? 1 : 0) << "}\n";
    out << "!4 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.property_declaration_entries)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.property_attribute_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.property_attribute_value_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.property_accessor_modifier_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.property_getter_selector_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.property_setter_selector_entries) << ", i1 "
        << (frontend_metadata_.deterministic_property_attribute_handoff ? 1 : 0) << "}\n\n";
    out << "!45 = !{!\"" << EscapeCStringLiteral(frontend_metadata_.runtime_metadata_source_ownership_contract_id)
        << "\", !\"" << EscapeCStringLiteral(frontend_metadata_.runtime_metadata_source_schema) << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.runtime_metadata_ivar_source_model) << "\", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_class_record_count) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_protocol_record_count) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_category_interface_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_category_implementation_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_property_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_method_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_ivar_record_count)
        << ", i1 " << (frontend_metadata_.frontend_owns_runtime_metadata_source_records ? 1 : 0) << ", i1 "
        << (frontend_metadata_.runtime_metadata_source_records_ready_for_lowering ? 1 : 0) << ", i1 "
        << (frontend_metadata_.native_runtime_library_present ? 1 : 0) << ", i1 "
        << (frontend_metadata_.runtime_metadata_source_boundary_fail_closed ? 1 : 0) << ", i1 "
        << (frontend_metadata_.runtime_link_test_only ? 1 : 0) << ", i1 "
        << (frontend_metadata_.deterministic_runtime_metadata_source_schema ? 1 : 0) << "}\n";
    out << "!46 = !{!\"" << EscapeCStringLiteral(frontend_metadata_.runtime_export_legality_contract_id)
        << "\", i1 " << (frontend_metadata_.runtime_export_semantic_boundary_frozen ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_metadata_export_enforcement_ready ? 1 : 0)
        << ", i1 " << (frontend_metadata_.runtime_export_fail_closed ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_duplicate_runtime_identity_enforcement_pending ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_incomplete_declaration_export_blocking_pending ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_illegal_redeclaration_mix_export_blocking_pending ? 1 : 0)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_class_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_protocol_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_category_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_method_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_ivar_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_invalid_protocol_composition_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_attribute_invalid_entries)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_attribute_contract_violations)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_invalid_type_annotation_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_ivar_binding_missing)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_ivar_binding_conflicts)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_implementation_resolution_misses)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_method_resolution_misses)
        << ", i1 " << (frontend_metadata_.runtime_export_boundary_ready ? 1 : 0) << "}\n";
    out << "!47 = !{!\"" << EscapeCStringLiteral(frontend_metadata_.runtime_export_enforcement_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_export_metadata_completeness_enforced ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_duplicate_runtime_identity_suppression_enforced ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_illegal_redeclaration_mix_blocking_enforced ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_metadata_shape_drift_blocking_enforced ? 1 : 0)
        << ", i1 " << (frontend_metadata_.runtime_export_enforcement_fail_closed ? 1 : 0)
        << ", i1 " << (frontend_metadata_.runtime_export_ready_for_runtime_export ? 1 : 0)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_duplicate_runtime_identity_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_incomplete_declaration_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_illegal_redeclaration_mix_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_metadata_shape_drift_sites)
        << "}\n";
    out << "!48 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_abi_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_section_boundary_frozen ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_fail_closed ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_metadata_section_object_file_inventory_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_symbol_policy_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_visibility_model_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_retention_policy_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_ready_for_scaffold ? 1
                                                                           : 0)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_image_info_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_class_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_protocol_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_category_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_property_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_ivar_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_descriptor_symbol_prefix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_aggregate_symbol_prefix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_image_info_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_descriptor_linkage)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_aggregate_linkage)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_visibility)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_retention_root)
        << "\"}\n";
    out << "!49 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_publication_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_publication_abi_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_section_publication_emitted ? 1
                                                                         : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_publication_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_publication_uses_llvm_used
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_metadata_section_publication_image_info_emitted
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_publication_class_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_publication_protocol_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_publication_category_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_publication_property_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_publication_ivar_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_publication_total_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_publication_total_retained_global_count)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_publication_image_info_symbol)
        << "\", !\""
        << EscapeCStringLiteral(

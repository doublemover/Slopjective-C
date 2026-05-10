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
        BuildBlockInvokeSurfaceProfile(*block);
    block->block_abi_has_invoke_trampoline = true;
    block->block_abi_layout_is_normalized =
        block->block_literal_is_normalized && block->block_capture_set_deterministic;
    const BlockLiteralSourceUseKind source_use_kind =
        CurrentBlockLiteralSourceUseKind();
    // block-source-storage-annotation anchor: parser now classifies
    // mutated/byref/helper/escape-shape source annotations directly from the
    // block body and surrounding expression context so later runnable block
    // lanes consume deterministic source truth instead of reconstructing it.
    // block-runtime-semantic-rules freeze anchor: these parser-owned
    // annotations are the complete current legality input for block runtime
    // semantics; no runnable byref/helper/heap-promotion behavior is implied
    // by their presence yet.
    // capture-legality/escape/invocation implementation anchor:
    // lane-B now consumes these exact parser-owned source facts as the live
    // sema input for capture resolution, truthful escape classification, and
    // local block invocation typing.
    // byref/copy-dispose/object-ownership anchor: parser still owns
    // only the deterministic source inventory here; sema layers
    // ownership-sensitive helper eligibility on top without widening this
    // source-only parser contract.
    // escaping-block runtime-hook anchor: parser continues to own
    // the exact escape-shape source classification, including which block
    // literal sites are truthful heap-promotion candidates for later lane-C
    // runtime-hook lowering.
    // block-runtime API/object-layout freeze anchor: parser does not
    // widen the runtime ABI here; it only preserves the source-truth escape
    // inventory that the frozen private runtime boundary consumes downstream.
    // block-runtime allocation/copy-dispose/invoke anchor: parser
    // still does not widen the source surface; runtime helper realization
    // remains an internal lowering/runtime concern layered on this same source
    // inventory.
    // byref-forwarding/heap-promotion/ownership-interop anchor:
    // parser still only publishes the truthful capture and escape inventory;
    // runtime-owned forwarding cells for escaping pointer-capture blocks are
    // layered later without widening this source contract.
    // runnable-block-runtime gate anchor: lane-E consumes this exact
    // parser-owned capture and escape inventory as the source side of the
    // runnable block proof chain and must fail closed if the source truth
    // drifts behind the integrated gate.
    // runnable-block execution-matrix anchor: lane-E closes the block-runtime tranche on
    // the same parser-owned capture and escape inventory together with live executable block programs,
    // without widening the supported source surface.
    block->block_copy_helper_intent_required =
        block->block_byref_capture_count > 0u;
    block->block_dispose_helper_intent_required =
        block->block_byref_capture_count > 0u;
    block->block_escape_shape_symbol =
        BuildObjc3BlockEscapeShapeSymbol(source_use_kind);
    block->block_escape_shape_promotes_to_heap_candidate =
        Objc3BlockEscapeShapePromotesToHeapCandidate(source_use_kind);
    block->block_helper_intent_profile =
        BuildObjc3BlockHelperIntentProfile(
            block->block_mutated_capture_count,
            block->block_byref_capture_count,
            block->block_copy_helper_intent_required,
            block->block_dispose_helper_intent_required,
            source_use_kind);
    block->block_escape_shape_profile =
        BuildObjc3BlockEscapeShapeProfile(
            source_use_kind,
            block->block_escape_shape_promotes_to_heap_candidate,
            block->block_capture_count,
            block->block_byref_capture_count);
    block->block_source_storage_annotations_are_normalized =
        block->block_literal_is_normalized &&
        block->block_capture_set_deterministic &&
        IsSortedUniqueStrings(
            block->block_mutated_capture_names_lexicographic) &&
        IsSortedUniqueStrings(
            block->block_byref_capture_names_lexicographic) &&
        !block->block_helper_intent_profile.empty() &&
        !block->block_escape_shape_symbol.empty() &&
        !block->block_escape_shape_profile.empty();
    block->block_storage_mutable_capture_count =
        block->block_mutated_capture_count;
    block->block_storage_byref_slot_count =
        block->block_byref_capture_count;
    block->block_storage_requires_byref_cells =
        block->block_storage_byref_slot_count > 0u;
    block->block_storage_escape_analysis_enabled = true;
    block->block_storage_escape_to_heap =
        block->block_escape_shape_promotes_to_heap_candidate;
    block->block_storage_escape_profile =
        BuildBlockStorageEscapeProfile(
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap,
            block->block_body_statement_count);
    block->block_storage_byref_layout_symbol =
        BuildBlockStorageByrefLayoutSymbol(
            block->line,
            block->column,
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap);
    block->block_storage_escape_profile_is_normalized =
        block->block_literal_is_normalized && block->block_capture_set_deterministic;
    block->block_copy_helper_required = block->block_storage_byref_slot_count > 0u;
    block->block_dispose_helper_required = block->block_storage_byref_slot_count > 0u;
    block->block_copy_dispose_profile =
        BuildBlockCopyDisposeProfile(
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap,
            block->block_body_statement_count);
    block->block_copy_helper_symbol =
        BuildBlockCopyHelperSymbol(
            block->line,
            block->column,
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap);
    block->block_dispose_helper_symbol =
        BuildBlockDisposeHelperSymbol(
            block->line,
            block->column,
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap);
    block->block_copy_dispose_profile_is_normalized =
        block->block_storage_escape_profile_is_normalized &&
        block->block_copy_helper_required == block->block_dispose_helper_required;
    block->block_determinism_perf_baseline_weight =
        BuildBlockDeterminismPerfBaselineWeight(
            block->block_parameter_count,
            block->block_capture_count,
            block->block_body_statement_count,
            block->block_copy_helper_required,
            block->block_dispose_helper_required);
    block->block_determinism_perf_baseline_profile =
        BuildBlockDeterminismPerfBaselineProfile(
            block->block_parameter_count,
            block->block_capture_count,
            block->block_body_statement_count,
            block->block_copy_helper_required,
            block->block_dispose_helper_required,
            block->block_capture_set_deterministic,
            block->block_copy_dispose_profile_is_normalized,
            block->block_determinism_perf_baseline_weight);
    block->block_determinism_perf_baseline_profile_is_normalized =
        block->block_copy_dispose_profile_is_normalized &&
        block->block_determinism_perf_baseline_weight >= block->block_capture_count;
    block->block_source_model_is_normalized =
        block->block_literal_is_normalized &&
        block->block_capture_set_deterministic &&
        block->block_abi_layout_is_normalized &&
        block->block_storage_escape_profile_is_normalized &&
        block->block_copy_dispose_profile_is_normalized &&
        block->block_determinism_perf_baseline_profile_is_normalized;
    block->block_source_model_replay_key =
        BuildBlockSourceModelReplayKey(*block);
    block->block_body = std::move(body);
    return block;
  }


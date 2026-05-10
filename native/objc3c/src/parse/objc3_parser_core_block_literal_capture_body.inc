  bool RequireBlockLiteralBodyStart() {
    // executable-block-source-closure anchor: block literals enter
    // the parser-owned source surface at the brace-owned ParseBlock() helper,
    // so the freeze boundary must only probe for '{' here and leave actual
    // block consumption to the shared block parser.
    // block-lowering-ABI/artifact-boundary freeze anchor: parser
    // owns only the source spelling, parameter list, capture inventory, and
    // body shape that later lowering consumes. It does not assign emitted
    // block-object layout slots, invoke-thunk bodies, byref cells, or helper
    // symbols.
    if (At(TokenKind::LBrace)) {
      return true;
    }
    const Token &token = Peek();
    diagnostics_.push_back(
        MakeDiag(token.line, token.column, "O3P166",
                 "expected '{' before block literal body"));
    return false;
  }

  void ApplyBlockLiteralCaptureBodyModel(
      Expr &block,
      const std::vector<std::unique_ptr<Stmt>> &body,
      const std::vector<std::string> &parameter_names,
      bool &deterministic_capture_set) {
    block.block_capture_names_lexicographic =
        BuildObjc3BlockLiteralCaptureSet(body, parameter_names, deterministic_capture_set);
    block.block_capture_count = block.block_capture_names_lexicographic.size();
    block.block_capture_inventory_entries_lexicographic =
        BuildBlockCaptureInventoryEntriesLexicographic(
            block.block_capture_names_lexicographic);
    for (const auto &item : block.block_explicit_capture_items_source_order) {
      block.block_explicit_capture_names_lexicographic.push_back(item.name);
      if (item.mode == "weak") {
        ++block.block_explicit_capture_weak_count;
      } else if (item.mode == "unowned") {
        ++block.block_explicit_capture_unowned_count;
      } else if (item.mode == "move") {
        ++block.block_explicit_capture_move_count;
      } else if (item.mode == "byref") {
        ++block.block_explicit_capture_byref_count;
      } else {
        ++block.block_explicit_capture_plain_count;
      }
    }
    block.block_explicit_capture_names_lexicographic =
        BuildSortedUniqueStrings(block.block_explicit_capture_names_lexicographic);
    block.block_explicit_capture_count =
        block.block_explicit_capture_items_source_order.size();
    {
      std::ostringstream out;
      out << "explicit-captures:present="
          << (block.block_has_explicit_capture_list ? "true" : "false")
          << ";count=" << block.block_explicit_capture_count
          << ";weak=" << block.block_explicit_capture_weak_count
          << ";unowned=" << block.block_explicit_capture_unowned_count
          << ";move=" << block.block_explicit_capture_move_count
          << ";byref=" << block.block_explicit_capture_byref_count
          << ";plain=" << block.block_explicit_capture_plain_count;
      block.block_explicit_capture_profile = out.str();
    }
    block.block_byvalue_readonly_capture_count =
        block.block_capture_count;
    block.block_mutated_capture_names_lexicographic =
        BuildObjc3BlockLiteralMutatedCaptureSet(
            body, block.block_capture_names_lexicographic);
    block.block_mutated_capture_count =
        block.block_mutated_capture_names_lexicographic.size();
    block.block_byref_capture_names_lexicographic =
        block.block_mutated_capture_names_lexicographic;
    for (const auto &item : block.block_explicit_capture_items_source_order) {
      if (item.mode == "byref") {
        block.block_byref_capture_names_lexicographic.push_back(item.name);
      }
    }
    block.block_byref_capture_names_lexicographic =
        BuildSortedUniqueStrings(block.block_byref_capture_names_lexicographic);
    block.block_byref_capture_count =
        block.block_byref_capture_names_lexicographic.size();
    block.block_capture_inventory_profile =
        BuildBlockCaptureInventoryProfile(
            block.block_capture_count,
            block.block_byvalue_readonly_capture_count);
    block.block_body_statement_count = body.size();
    block.block_capture_set_deterministic = deterministic_capture_set;
    block.block_capture_profile =
        BuildBlockLiteralCaptureProfile(block.block_capture_names_lexicographic);
    block.block_literal_is_normalized = true;
  }

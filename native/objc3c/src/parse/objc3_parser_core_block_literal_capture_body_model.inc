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
    ApplyBlockLiteralExplicitCaptureModel(block);
    ApplyBlockLiteralInferredCaptureModel(block, body);
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

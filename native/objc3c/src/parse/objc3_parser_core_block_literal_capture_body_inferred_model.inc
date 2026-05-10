  void ApplyBlockLiteralInferredCaptureModel(
      Expr &block,
      const std::vector<std::unique_ptr<Stmt>> &body) {
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
  }

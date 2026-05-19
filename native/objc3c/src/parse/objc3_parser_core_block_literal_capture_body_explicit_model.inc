  void ApplyBlockLiteralExplicitCaptureModel(Expr &block) {
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
  }

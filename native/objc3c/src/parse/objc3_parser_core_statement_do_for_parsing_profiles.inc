    auto collect_do_catch_profile = [this](BlockStmt &block_stmt) {
      block_stmt.do_catch_is_normalized = !block_stmt.catch_clauses.empty();
      block_stmt.do_catch_profile = BuildDoCatchProfile(block_stmt);
    };

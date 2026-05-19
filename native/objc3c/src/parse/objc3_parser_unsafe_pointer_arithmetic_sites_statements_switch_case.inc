void CollectPointerArithmeticSwitchStmtSites(const Stmt *stmt,
                                            std::size_t &sites) {
  if (stmt->switch_stmt == nullptr) {
    return;
  }

  CollectPointerArithmeticExprSites(stmt->switch_stmt->condition.get(), sites);
  for (const auto &switch_case : stmt->switch_stmt->cases) {
    CollectPointerArithmeticStmtBodySites(switch_case.body, sites);
  }
}

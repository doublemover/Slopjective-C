std::size_t CountPointerArithmeticSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  std::size_t sites = 0;
  for (const auto &stmt : body) {
    CollectPointerArithmeticStmtSites(stmt.get(), sites);
  }
  return sites;
}

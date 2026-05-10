std::size_t CountFailableCallSitesInForClause(const ForClause &clause) {
  return CountFailableCallSitesInExpr(clause.value.get());
}

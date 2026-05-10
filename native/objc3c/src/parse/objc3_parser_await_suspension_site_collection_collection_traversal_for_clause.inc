void CollectAwaitSuspensionForClauseSites(
    const ForClause &clause,
    Objc3AwaitSuspensionSiteCounts &counts) {
  CollectAwaitSuspensionExprSites(clause.value.get(), counts);
}

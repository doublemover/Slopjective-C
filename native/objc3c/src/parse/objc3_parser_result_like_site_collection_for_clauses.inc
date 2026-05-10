void CollectResultLikeForClauseProfile(const ForClause &clause,
                                       Objc3ResultLikeProfile &profile) {
  CollectResultLikeExprProfile(clause.value.get(), profile);
}

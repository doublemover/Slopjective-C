static std::vector<OwnershipBorrowedScope>
BuildOwnershipBorrowedPointerEscapeScopes(
    const std::vector<SemanticScope> &semantic_scopes,
    const std::unordered_set<std::string> &borrowed_parameter_names) {
  std::vector<OwnershipBorrowedScope> scopes;
  scopes.reserve(semantic_scopes.size());
  for (const auto &semantic_scope : semantic_scopes) {
    OwnershipBorrowedScope scope;
    for (const auto &[name, _] : semantic_scope) {
      OwnershipBorrowedBindingState state;
      state.borrowed = borrowed_parameter_names.find(name) !=
                       borrowed_parameter_names.end();
      scope[name] = state;
    }
    scopes.push_back(std::move(scope));
  }
  return scopes;
}

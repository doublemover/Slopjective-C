static void DiagnoseOwnershipBorrowedPointerEscapeSemantics(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    const std::vector<SemanticScope> &semantic_scopes,
    const std::unordered_set<std::string> &borrowed_parameter_names,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    const OwnershipBorrowedReturnContract &return_contract,
    std::vector<std::string> &diagnostics) {
  auto scopes = BuildOwnershipBorrowedPointerEscapeScopes(
      semantic_scopes, borrowed_parameter_names);
  DiagnoseOwnershipBorrowedEscapeStatements(statements, scopes, callable_contracts,
                                            return_contract, diagnostics);
}

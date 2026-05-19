std::size_t CountRawPointerTypeSites(
    const std::vector<FuncParam> &params,
    bool has_return_pointer_declarator) {
  std::size_t sites = has_return_pointer_declarator ? 1u : 0u;
  for (const auto &param : params) {
    if (param.has_pointer_declarator) {
      sites += 1u;
    }
  }
  return sites;
}

std::size_t CountUnsafeKeywordSites(
    const std::vector<FuncParam> &params,
    const std::string &return_ownership_qualifier_spelling) {
  std::size_t sites =
      IsUnsafeOwnershipQualifierSpelling(return_ownership_qualifier_spelling)
          ? 1u
          : 0u;
  for (const auto &param : params) {
    if (IsUnsafeOwnershipQualifierSpelling(
            param.ownership_qualifier_spelling)) {
      sites += 1u;
    }
  }
  return sites;
}

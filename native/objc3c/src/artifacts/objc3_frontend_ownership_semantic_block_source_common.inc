bool IsStrictlySortedUniqueStrings(const std::vector<std::string> &entries) {
  if (!std::is_sorted(entries.begin(), entries.end())) {
    return false;
  }
  return std::adjacent_find(entries.begin(), entries.end()) == entries.end();
}

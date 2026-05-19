std::vector<std::string> BuildObjc3BlockLiteralCaptureSet(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const std::vector<std::string> &parameter_names,
    bool &deterministic) {
  deterministic = true;
  std::unordered_set<std::string> parameter_name_set;
  for (const auto &name : parameter_names) {
    if (!parameter_name_set.insert(name).second) {
      deterministic = false;
    }
  }

  std::vector<std::string> used_identifiers;
  std::vector<std::string> declared_identifiers = parameter_names;
  for (const auto &stmt : body) {
    CollectBlockLiteralStmtIdentifiers(
        stmt.get(), used_identifiers, declared_identifiers);
  }

  std::unordered_set<std::string> declared_name_set;
  for (const auto &name : declared_identifiers) {
    if (!name.empty()) {
      declared_name_set.insert(name);
    }
  }

  std::vector<std::string> capture_names;
  capture_names.reserve(used_identifiers.size());
  for (const auto &used_name : used_identifiers) {
    if (used_name.empty() || declared_name_set.count(used_name) != 0u) {
      continue;
    }
    capture_names.push_back(used_name);
  }
  return BuildSortedUniqueStrings(std::move(capture_names));
}

std::vector<std::string> BuildObjc3BlockLiteralMutatedCaptureSet(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const std::vector<std::string> &capture_names) {
  std::vector<std::string> mutated_identifiers;
  for (const auto &stmt : body) {
    CollectBlockLiteralMutatedIdentifiersFromStatement(
        stmt.get(), mutated_identifiers);
  }

  std::unordered_set<std::string> capture_name_set;
  for (const auto &capture_name : capture_names) {
    if (!capture_name.empty()) {
      capture_name_set.insert(capture_name);
    }
  }

  std::vector<std::string> mutated_capture_names;
  mutated_capture_names.reserve(mutated_identifiers.size());
  for (const auto &mutated_identifier : mutated_identifiers) {
    if (!mutated_identifier.empty() &&
        capture_name_set.count(mutated_identifier) != 0u) {
      mutated_capture_names.push_back(mutated_identifier);
    }
  }
  return BuildSortedUniqueStrings(std::move(mutated_capture_names));
}

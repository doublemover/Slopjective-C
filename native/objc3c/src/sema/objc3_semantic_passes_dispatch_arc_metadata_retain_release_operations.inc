static Objc3RetainReleaseOperationSummary
BuildRetainReleaseOperationSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3RetainReleaseOperationSummary summary;
  std::size_t operation_sites = 0;

  const auto accumulate_function = [&summary, &operation_sites](const FunctionInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_insert_retain.size() != arity ||
        info.param_ownership_insert_release.size() != arity ||
        info.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = info.param_has_ownership_qualifier[i];
      const bool insert_retain = info.param_ownership_insert_retain[i];
      const bool insert_release = info.param_ownership_insert_release[i];
      const bool insert_autorelease = info.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = info.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (info.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (info.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (info.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (info.return_ownership_insert_retain || info.return_ownership_insert_release ||
                               info.return_ownership_insert_autorelease)) ||
        (info.return_ownership_insert_autorelease &&
         (info.return_ownership_insert_retain || info.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_method = [&summary, &operation_sites](const Objc3MethodInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_insert_retain.size() != arity ||
        info.param_ownership_insert_release.size() != arity ||
        info.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = info.param_has_ownership_qualifier[i];
      const bool insert_retain = info.param_ownership_insert_retain[i];
      const bool insert_release = info.param_ownership_insert_release[i];
      const bool insert_autorelease = info.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = info.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (info.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (info.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (info.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (info.return_ownership_insert_retain || info.return_ownership_insert_release ||
                               info.return_ownership_insert_autorelease)) ||
        (info.return_ownership_insert_autorelease &&
         (info.return_ownership_insert_retain || info.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_property = [&summary, &operation_sites](const Objc3PropertyInfo &info) {
    ++operation_sites;
    const bool qualified = info.has_ownership_qualifier;
    if (qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (info.ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (info.ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (info.ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!qualified &&
         (info.ownership_insert_retain || info.ownership_insert_release || info.ownership_insert_autorelease)) ||
        (info.ownership_insert_autorelease && (info.ownership_insert_retain || info.ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  for (const auto &entry : surface.functions) {
    accumulate_function(entry.second);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }

  const std::size_t qualified_or_violation = summary.ownership_qualified_sites + summary.contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      summary.contract_violation_sites <= operation_sites &&
      summary.retain_insertion_sites <= qualified_or_violation &&
      summary.release_insertion_sites <= qualified_or_violation &&
      summary.autorelease_insertion_sites <= qualified_or_violation;
  return summary;
}

static Objc3RetainReleaseOperationSummary
BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3RetainReleaseOperationSummary summary;
  std::size_t operation_sites = 0;

  const auto accumulate_function = [&summary, &operation_sites](const Objc3SemanticFunctionTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_insert_retain.size() != arity ||
        metadata.param_ownership_insert_release.size() != arity ||
        metadata.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = metadata.param_has_ownership_qualifier[i];
      const bool insert_retain = metadata.param_ownership_insert_retain[i];
      const bool insert_release = metadata.param_ownership_insert_release[i];
      const bool insert_autorelease = metadata.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = metadata.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (metadata.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (metadata.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (metadata.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
                               metadata.return_ownership_insert_autorelease)) ||
        (metadata.return_ownership_insert_autorelease &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_method = [&summary, &operation_sites](const Objc3SemanticMethodTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_insert_retain.size() != arity ||
        metadata.param_ownership_insert_release.size() != arity ||
        metadata.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = metadata.param_has_ownership_qualifier[i];
      const bool insert_retain = metadata.param_ownership_insert_retain[i];
      const bool insert_release = metadata.param_ownership_insert_release[i];
      const bool insert_autorelease = metadata.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = metadata.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (metadata.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (metadata.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (metadata.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
                               metadata.return_ownership_insert_autorelease)) ||
        (metadata.return_ownership_insert_autorelease &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_property = [&summary, &operation_sites](const Objc3SemanticPropertyTypeMetadata &metadata) {
    ++operation_sites;
    const bool qualified = metadata.has_ownership_qualifier;
    if (qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (metadata.ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (metadata.ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (metadata.ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!qualified && (metadata.ownership_insert_retain || metadata.ownership_insert_release ||
                        metadata.ownership_insert_autorelease)) ||
        (metadata.ownership_insert_autorelease &&
         (metadata.ownership_insert_retain || metadata.ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function(metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }

  const std::size_t qualified_or_violation = summary.ownership_qualified_sites + summary.contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      summary.contract_violation_sites <= operation_sites &&
      summary.retain_insertion_sites <= qualified_or_violation &&
      summary.release_insertion_sites <= qualified_or_violation &&
      summary.autorelease_insertion_sites <= qualified_or_violation;
  return summary;
}

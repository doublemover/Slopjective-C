Objc3MetaprogrammingDeriveExpansionInventorySummary
BuildMetaprogrammingDeriveExpansionInventorySummary(
    const Objc3Program &program,
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3MetaprogrammingDeriveExpansionInventorySummary summary;

  const auto count_diagnostic = [&diagnostics](std::string_view code,
                                               std::string_view needle) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code, needle](const std::string &entry) {
          return entry.find(code) != std::string::npos &&
                 entry.find(needle) != std::string::npos;
        }));
  };

  for (const auto &interface_decl : program.interfaces) {
    if (!interface_decl.objc_derive_declared) {
      continue;
    }
    ++summary.derive_request_sites;

    const std::string canonical_derive_name =
        NormalizeMetaprogrammingSupportedDeriveName(
            interface_decl.objc_derive_name);
    if (interface_decl.has_category) {
      ++summary.unsupported_topology_sites;
      continue;
    }
    if (canonical_derive_name.empty()) {
      ++summary.unsupported_derive_request_sites;
      continue;
    }

    ++summary.supported_derive_request_sites;
    const std::string lowered = objc3c::support::LowercaseAscii(
        NormalizeMetaprogrammingRawDeriveToken(interface_decl.objc_derive_name));
    if (lowered == "equatable") {
      ++summary.equatable_alias_sites;
    }
    if (canonical_derive_name == "Equality") {
      ++summary.equality_derive_sites;
    } else if (canonical_derive_name == "Hash") {
      ++summary.hash_derive_sites;
    } else if (canonical_derive_name == "DebugDescription") {
      ++summary.debug_description_derive_sites;
    }

    const std::string derived_selector =
        BuildMetaprogrammingDerivedSelectorForInventoryRow(
            canonical_derive_name);
    const bool selector_conflict =
        !derived_selector.empty() &&
        std::any_of(interface_decl.methods.begin(),
                    interface_decl.methods.end(),
                    [&derived_selector](const Objc3MethodDecl &method_decl) {
                      return BuildMethodSelectorNormalizationContractInfo(
                                 method_decl)
                                 .normalized_selector == derived_selector;
                    });
    if (selector_conflict) {
      ++summary.selector_conflict_sites;
      continue;
    }

    summary.expansion_inventory_rows_lexicographic.push_back(
        interface_decl.name + ":" + canonical_derive_name + ":" +
        derived_selector);
    ++summary.generated_method_entry_count;
  }

  std::sort(summary.expansion_inventory_rows_lexicographic.begin(),
            summary.expansion_inventory_rows_lexicographic.end());

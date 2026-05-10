std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
BuildMetaprogrammingDerivedMethodBundles(const Objc3Program &program) {
  std::unordered_map<std::string, const Objc3ImplementationDecl *> implementations;
  implementations.reserve(program.implementations.size());
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      implementations.emplace(implementation_decl.name, &implementation_decl);
    }
  }

  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle> bundles;
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      continue;
    }
    const bool has_derive_marker =
        interface_decl.objc_derive_declared ||
        !interface_decl.objc_derive_name.empty();
    if (!has_derive_marker) {
      continue;
    }
    const auto impl_it = implementations.find(interface_decl.name);
    if (impl_it == implementations.end()) {
      continue;
    }
    const std::string derive_name =
        NormalizeMetaprogrammingDeriveNameForEmission(
            interface_decl.objc_derive_name);
    if (derive_name.empty()) {
      continue;
    }

    std::string selector;
    std::size_t parameter_count = 0u;
    if (derive_name == "Equality") {
      selector = "isEqual:";
      parameter_count = 1u;
    } else if (derive_name == "Hash") {
      selector = "hash";
    } else if (derive_name == "DebugDescription") {
      selector = "debugDescription";
    }
    const auto selector_exists = [&selector](const auto &methods) {
      return std::any_of(methods.begin(), methods.end(),
                         [&selector](const Objc3MethodDecl &method_decl) {
                           return !method_decl.is_class_method &&
                                  method_decl.selector == selector;
                         });
    };
    if (selector.empty() || selector_exists(interface_decl.methods) ||
        selector_exists(impl_it->second->methods)) {
      continue;
    }

    Objc3IRMetaprogrammingDerivedMethodBundle bundle;
    bundle.implementation_name = impl_it->second->name;
    bundle.declaration_owner_identity = impl_it->second->semantic_link_symbol;
    bundle.export_owner_identity =
        BuildMetaprogrammingRuntimeClassOwnerIdentity(interface_decl.name);
    bundle.derive_name = derive_name;
    bundle.selector = selector;
    bundle.emitted_symbol =
        BuildMetaprogrammingDerivedMethodFunctionSymbol(
            bundle.implementation_name, bundle.selector);
    bundle.parameter_count = parameter_count;
    bundle.line = interface_decl.line;
    bundle.column = interface_decl.column;
    bundles.push_back(std::move(bundle));
  }

  std::sort(bundles.begin(), bundles.end(),
            [](const Objc3IRMetaprogrammingDerivedMethodBundle &lhs,
               const Objc3IRMetaprogrammingDerivedMethodBundle &rhs) {
              return std::tie(lhs.implementation_name, lhs.selector,
                              lhs.derive_name) <
                     std::tie(rhs.implementation_name, rhs.selector,
                              rhs.derive_name);
            });
  return bundles;
}

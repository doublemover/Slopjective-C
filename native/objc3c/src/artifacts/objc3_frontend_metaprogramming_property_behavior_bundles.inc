std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
BuildMetaprogrammingPropertyBehaviorArtifactBundles(const Objc3Program &program) {
  std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle> bundles;
  const auto append_properties =
      [&bundles](const auto &properties, const std::string &owner_kind,
                 const std::string &owner_name,
                 const std::string &declaration_owner_identity,
                 const std::string &export_owner_identity) {
        for (const auto &property : properties) {
          if (!property.property_behavior_declared) {
            continue;
          }
          const std::string behavior_name =
              NormalizeMetaprogrammingPropertyBehaviorNameForEmission(
                  property.property_behavior_name);
          if (behavior_name.empty() ||
              !IsMetaprogrammingPropertyBehaviorObjectLike(property)) {
            continue;
          }
          if (behavior_name == "Observed" &&
              (owner_kind == "protocol" || property.is_readonly ||
               !property.effective_setter_available)) {
            continue;
          }
          if (behavior_name == "Projected" &&
              (!property.is_readonly || property.has_setter ||
               property.effective_setter_available)) {
            continue;
          }

          Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle bundle;
          bundle.owner_kind = owner_kind;
          bundle.owner_name = owner_name;
          bundle.declaration_owner_identity = declaration_owner_identity;
          bundle.export_owner_identity = export_owner_identity;
          bundle.property_name = property.name;
          bundle.behavior_name = behavior_name;
          bundle.binding_symbol = property.executable_synthesized_binding_symbol;
          if (bundle.binding_symbol.empty()) {
            bundle.binding_symbol = property.ivar_binding_symbol;
          }
          bundle.emitted_symbol =
              BuildMetaprogrammingPropertyBehaviorArtifactSymbol(
                  owner_kind, owner_name, property.name, behavior_name);
          bundle.line = property.line;
          bundle.column = property.column;
          bundles.push_back(std::move(bundle));
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    append_properties(
        interface_decl.properties,
        interface_decl.has_category ? "category-interface" : "class-interface",
        interface_decl.has_category
            ? BuildMetaprogrammingCategoryOwnerName(interface_decl.name,
                                                    interface_decl.category_name)
            : interface_decl.name,
        interface_decl.semantic_link_symbol,
        interface_decl.has_category
            ? BuildMetaprogrammingRuntimeCategoryOwnerIdentity(
                  interface_decl.name, interface_decl.category_name)
            : BuildMetaprogrammingRuntimeClassOwnerIdentity(interface_decl.name));
  }
  for (const auto &protocol_decl : program.protocols) {
    append_properties(protocol_decl.properties, "protocol", protocol_decl.name,
                      protocol_decl.semantic_link_symbol,
                      protocol_decl.semantic_link_symbol);
  }
  for (const auto &implementation_decl : program.implementations) {
    append_properties(
        implementation_decl.properties,
        implementation_decl.has_category ? "category-implementation"
                                         : "class-implementation",
        implementation_decl.has_category
            ? BuildMetaprogrammingCategoryOwnerName(
                  implementation_decl.name, implementation_decl.category_name)
            : implementation_decl.name,
        implementation_decl.semantic_link_symbol,
        implementation_decl.has_category
            ? BuildMetaprogrammingRuntimeCategoryOwnerIdentity(
                  implementation_decl.name,
                  implementation_decl.category_name)
            : BuildMetaprogrammingRuntimeClassOwnerIdentity(
                  implementation_decl.name));
  }

  std::sort(bundles.begin(), bundles.end(),
            [](const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &lhs,
               const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &rhs) {
              return std::tie(lhs.owner_kind, lhs.owner_name,
                              lhs.property_name, lhs.behavior_name) <
                     std::tie(rhs.owner_kind, rhs.owner_name,
                              rhs.property_name, rhs.behavior_name);
            });
  return bundles;
}

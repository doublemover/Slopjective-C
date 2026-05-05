      aggregate.interface_method_count = interface_decl.methods.size();
      aggregate.interface_class_method_count =
          CountClassMethods(interface_decl.methods);

      add_property_nodes(interface_decl.properties, "category-interface",
                         category_owner_name, interface_decl.semantic_link_symbol,
                         BuildRuntimeCategoryOwnerIdentity(
                             interface_decl.name, interface_decl.category_name));
      add_method_nodes(interface_decl.methods, "category-interface",
                       category_owner_name, interface_decl.semantic_link_symbol,
                       BuildRuntimeCategoryOwnerIdentity(
                           interface_decl.name, interface_decl.category_name),
                       BuildRuntimeCategoryOwnerIdentity(
                           interface_decl.name, interface_decl.category_name),
                       false);
      continue;
    }

    Objc3ExecutableMetadataInterfaceGraphNode node;
    node.class_name = interface_decl.name;
    node.owner_identity = interface_decl.semantic_link_symbol;
    node.class_owner_identity = BuildRuntimeClassOwnerIdentity(interface_decl.name);
    node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(interface_decl.name);
    node.super_class_owner_identity =
        interface_decl.super_name.empty()
            ? std::string{}
            : BuildRuntimeClassOwnerIdentity(interface_decl.super_name);
    node.super_metaclass_owner_identity =
        interface_decl.super_name.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(interface_decl.super_name);
    node.instance_method_owner_identity = node.class_owner_identity;
    node.class_method_owner_identity = node.metaclass_owner_identity;
    node.has_super = !interface_decl.super_name.empty();
    node.declaration_complete =
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        !node.instance_method_owner_identity.empty() &&
        !node.class_method_owner_identity.empty() &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty()));
    node.property_count = interface_decl.properties.size();
    node.method_count = interface_decl.methods.size();
    node.class_method_count = CountClassMethods(interface_decl.methods);
    node.instance_method_count = node.method_count - node.class_method_count;
    node.line = interface_decl.line;
    node.column = interface_decl.column;
    graph.interface_nodes_lexicographic.push_back(node);

    AggregatedClassSurface &aggregate = aggregated_classes[interface_decl.name];
    if (!aggregate.has_interface && !aggregate.has_implementation) {
      aggregate.line = interface_decl.line;
      aggregate.column = interface_decl.column;
    }
    aggregate.has_interface = true;
    aggregate.interface_owner_identity = interface_decl.semantic_link_symbol;
    aggregate.super_class_owner_identity = node.super_class_owner_identity;
    aggregate.adopted_protocol_owner_identities_lexicographic =
        interface_decl.adopted_protocols_lexicographic;
    aggregate.objc_direct_members_declared =
        interface_decl.objc_direct_members_declared;
    aggregate.objc_final_declared = interface_decl.objc_final_declared;
    aggregate.objc_sealed_declared = interface_decl.objc_sealed_declared;
    aggregate.interface_property_count = node.property_count;
    aggregate.interface_method_count = node.method_count;
    aggregate.interface_class_method_count = node.class_method_count;

    add_owner_edge("interface-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-metaclass", node.owner_identity,
                   node.metaclass_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-instance-method-owner", node.owner_identity,
                   node.instance_method_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-class-method-owner", node.owner_identity,
                   node.class_method_owner_identity, node.line, node.column);
    add_owner_edge("class-to-superclass", node.class_owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-superclass", node.owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-super-metaclass", node.owner_identity,
                   node.super_metaclass_owner_identity, node.line, node.column);

    add_property_nodes(interface_decl.properties, "class-interface",
                       interface_decl.name, interface_decl.semantic_link_symbol,
                       node.class_owner_identity);
    add_method_nodes(interface_decl.methods, "class-interface", interface_decl.name,
                     interface_decl.semantic_link_symbol, node.class_owner_identity,
                     node.metaclass_owner_identity,
                     interface_decl.objc_direct_members_declared);
  }

  for (const auto &implementation_decl : program.implementations) {
    if (implementation_decl.has_category) {
      const std::string category_owner_name =
          BuildCategoryOwnerName(implementation_decl.name,
                                 implementation_decl.category_name);
      AggregatedCategorySurface &aggregate =
          aggregated_categories[category_owner_name];
      if (!aggregate.has_interface && !aggregate.has_implementation) {
        aggregate.line = implementation_decl.line;
        aggregate.column = implementation_decl.column;
      }
      aggregate.has_implementation = true;
      aggregate.implementation_owner_identity =
          implementation_decl.semantic_link_symbol;
      aggregate.class_owner_identity =
          BuildRuntimeClassOwnerIdentity(implementation_decl.name);
      aggregate.implementation_property_count =
          implementation_decl.properties.size();
      aggregate.implementation_method_count = implementation_decl.methods.size();
      aggregate.implementation_class_method_count =
          CountClassMethods(implementation_decl.methods);

      add_property_nodes(
          implementation_decl.properties, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name));
      add_method_nodes(
          implementation_decl.methods, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          false);
      continue;
    }

    AggregatedClassSurface &aggregate = aggregated_classes[implementation_decl.name];
    Objc3ExecutableMetadataImplementationGraphNode node;
    node.class_name = implementation_decl.name;
    node.owner_identity = implementation_decl.semantic_link_symbol;
    node.interface_owner_identity =
        implementation_decl.semantic_link_interface_symbol;
    node.class_owner_identity =
        BuildRuntimeClassOwnerIdentity(implementation_decl.name);
    node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(implementation_decl.name);
    node.super_class_owner_identity = aggregate.super_class_owner_identity;
    node.super_metaclass_owner_identity =
        aggregate.super_class_owner_identity.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(
                  aggregate.super_class_owner_identity.substr(6u));
    node.instance_method_owner_identity = node.class_owner_identity;
    node.class_method_owner_identity = node.metaclass_owner_identity;
    node.has_matching_interface = !node.interface_owner_identity.empty();
    node.has_super = !node.super_class_owner_identity.empty();
    node.declaration_complete =
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        !node.instance_method_owner_identity.empty() &&
        !node.class_method_owner_identity.empty() &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty()));
    node.property_count = implementation_decl.properties.size();
    node.method_count = implementation_decl.methods.size();
    node.class_method_count = CountClassMethods(implementation_decl.methods);
    node.instance_method_count = node.method_count - node.class_method_count;
    node.line = implementation_decl.line;
    node.column = implementation_decl.column;
    graph.implementation_nodes_lexicographic.push_back(node);

    if (!aggregate.has_interface && !aggregate.has_implementation) {
      aggregate.line = implementation_decl.line;
      aggregate.column = implementation_decl.column;
    }
    aggregate.has_implementation = true;
    aggregate.implementation_owner_identity = implementation_decl.semantic_link_symbol;
    aggregate.implementation_property_count = node.property_count;
    aggregate.implementation_method_count = node.method_count;
    aggregate.implementation_class_method_count = node.class_method_count;

    add_owner_edge("implementation-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-metaclass", node.owner_identity,
                   node.metaclass_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-interface", node.owner_identity,
                   node.interface_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-instance-method-owner",
                   node.owner_identity, node.instance_method_owner_identity,
                   node.line, node.column);
    add_owner_edge("implementation-to-class-method-owner", node.owner_identity,
                   node.class_method_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-superclass", node.owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-super-metaclass", node.owner_identity,
                   node.super_metaclass_owner_identity, node.line, node.column);

    add_property_nodes(implementation_decl.properties, "class-implementation",
                       implementation_decl.name,
                       implementation_decl.semantic_link_symbol,
                       node.class_owner_identity);
    add_method_nodes(implementation_decl.methods, "class-implementation",
                     implementation_decl.name,
                     implementation_decl.semantic_link_symbol,
                     node.class_owner_identity, node.metaclass_owner_identity,
                     aggregate.objc_direct_members_declared);
  }

  for (const auto &protocol_decl : program.protocols) {
    Objc3ExecutableMetadataProtocolGraphNode node;
    node.protocol_name = protocol_decl.name;
    node.owner_identity = protocol_decl.semantic_link_symbol;
    node.inherited_protocol_owner_identities_lexicographic =
        protocol_decl.inherited_protocols_lexicographic;
    node.property_count = protocol_decl.properties.size();
    node.method_count = protocol_decl.methods.size();
    node.is_forward_declaration = protocol_decl.is_forward_declaration;
    node.declaration_complete =
        !node.protocol_name.empty() && !node.owner_identity.empty();
    node.line = protocol_decl.line;
    node.column = protocol_decl.column;
    std::sort(node.inherited_protocol_owner_identities_lexicographic.begin(),
              node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_owner_identities_lexicographic.erase(
        std::unique(node.inherited_protocol_owner_identities_lexicographic.begin(),
                    node.inherited_protocol_owner_identities_lexicographic.end()),
        node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_identity_complete =
        std::all_of(node.inherited_protocol_owner_identities_lexicographic.begin(),
                    node.inherited_protocol_owner_identities_lexicographic.end(),
                    [](const std::string &owner_identity) {
                      return !owner_identity.empty();
                    });
    graph.protocol_nodes_lexicographic.push_back(node);

    for (const auto &target : node.inherited_protocol_owner_identities_lexicographic) {
      add_owner_edge("protocol-to-inherited-protocol", node.owner_identity,
                     target, node.line, node.column);
    }

    add_property_nodes(protocol_decl.properties, "protocol", protocol_decl.name,
                       protocol_decl.semantic_link_symbol,
                       protocol_decl.semantic_link_symbol);
    add_method_nodes(protocol_decl.methods, "protocol", protocol_decl.name,
                     protocol_decl.semantic_link_symbol,
                     protocol_decl.semantic_link_symbol,
                     protocol_decl.semantic_link_symbol, false);
  }

  std::vector<std::string> class_names;
  class_names.reserve(aggregated_classes.size());
  for (const auto &entry : aggregated_classes) {
    class_names.push_back(entry.first);
  }
  std::sort(class_names.begin(), class_names.end());

  graph.class_nodes_lexicographic.reserve(class_names.size());
  graph.metaclass_nodes_lexicographic.reserve(class_names.size());
  for (const std::string &class_name : class_names) {
    const AggregatedClassSurface &aggregate = aggregated_classes.at(class_name);

    Objc3ExecutableMetadataClassGraphNode class_node;
    class_node.class_name = class_name;
    class_node.owner_identity = BuildRuntimeClassOwnerIdentity(class_name);
    class_node.interface_owner_identity = aggregate.interface_owner_identity;
    class_node.implementation_owner_identity =
        aggregate.implementation_owner_identity;
    class_node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(class_name);
    class_node.super_class_owner_identity = aggregate.super_class_owner_identity;
    class_node.super_metaclass_owner_identity =
        aggregate.super_class_owner_identity.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(
                  aggregate.super_class_owner_identity.substr(6u));
    class_node.adopted_protocol_owner_identities_lexicographic =
        aggregate.adopted_protocol_owner_identities_lexicographic;
    class_node.instance_method_owner_identity = class_node.owner_identity;
    class_node.class_method_owner_identity = class_node.metaclass_owner_identity;
    class_node.has_interface = aggregate.has_interface;
    class_node.has_implementation = aggregate.has_implementation;
    class_node.has_super = !aggregate.super_class_owner_identity.empty();
    class_node.objc_final_declared = aggregate.objc_final_declared;
    class_node.objc_sealed_declared = aggregate.objc_sealed_declared;
    class_node.realization_identity_complete =
        !class_node.owner_identity.empty() &&
        !class_node.metaclass_owner_identity.empty() &&
        !class_node.instance_method_owner_identity.empty() &&
        !class_node.class_method_owner_identity.empty() &&
        (!class_node.has_super ||
         (!class_node.super_class_owner_identity.empty() &&
          !class_node.super_metaclass_owner_identity.empty()));
    class_node.interface_property_count = aggregate.interface_property_count;
    class_node.implementation_property_count =
        aggregate.implementation_property_count;
    class_node.interface_method_count = aggregate.interface_method_count;
    class_node.implementation_method_count = aggregate.implementation_method_count;
    class_node.interface_class_method_count =
        aggregate.interface_class_method_count;
    class_node.implementation_class_method_count =
        aggregate.implementation_class_method_count;
    class_node.interface_instance_method_count =
        aggregate.interface_method_count -
        aggregate.interface_class_method_count;
    class_node.implementation_instance_method_count =
        aggregate.implementation_method_count -
        aggregate.implementation_class_method_count;
    class_node.line = aggregate.line;
    class_node.column = aggregate.column;
    graph.class_nodes_lexicographic.push_back(class_node);

    if (aggregate.has_interface) {
      Objc3ExecutableMetadataMetaclassGraphNode metaclass_node;
      metaclass_node.class_name = class_name;
      metaclass_node.owner_identity =
          BuildRuntimeMetaclassOwnerIdentity(class_name);
      metaclass_node.class_owner_identity = class_node.owner_identity;
      metaclass_node.interface_owner_identity = aggregate.interface_owner_identity;
      metaclass_node.implementation_owner_identity =
          aggregate.implementation_owner_identity;
      metaclass_node.super_metaclass_owner_identity =
          class_node.has_super
              ? BuildRuntimeMetaclassOwnerIdentity(
                    class_node.super_class_owner_identity.substr(6u))
              : std::string{};
      metaclass_node.derived_from_interface = true;
      metaclass_node.has_implementation = aggregate.has_implementation;
      metaclass_node.has_super = class_node.has_super;
      metaclass_node.interface_class_method_count =
          aggregate.interface_class_method_count;
      metaclass_node.implementation_class_method_count =
          aggregate.implementation_class_method_count;
      metaclass_node.line = aggregate.line;
      metaclass_node.column = aggregate.column;
      graph.metaclass_nodes_lexicographic.push_back(metaclass_node);

      add_owner_edge("class-to-metaclass", class_node.owner_identity,
                     metaclass_node.owner_identity, class_node.line,
                     class_node.column);
      add_owner_edge("metaclass-to-super-metaclass",
                     metaclass_node.owner_identity,
                     metaclass_node.super_metaclass_owner_identity,
                     metaclass_node.line, metaclass_node.column);
    }
  }

  std::vector<std::string> category_names;
  category_names.reserve(aggregated_categories.size());
  for (const auto &entry : aggregated_categories) {
    category_names.push_back(entry.first);
  }
  std::sort(category_names.begin(), category_names.end());

  graph.category_nodes_lexicographic.reserve(category_names.size());
  for (const std::string &category_owner_name : category_names) {
    const AggregatedCategorySurface &aggregate =
        aggregated_categories.at(category_owner_name);
    const std::size_t open_paren = category_owner_name.find('(');
    const std::string class_name =
        open_paren == std::string::npos
            ? category_owner_name
            : category_owner_name.substr(0u, open_paren);
    const std::string category_name =
        open_paren == std::string::npos
            ? std::string{}
            : category_owner_name.substr(open_paren + 1u,
                                         category_owner_name.size() - open_paren - 2u);

    Objc3ExecutableMetadataCategoryGraphNode node;
    node.class_name = class_name;
    node.category_name = category_name;
    node.owner_identity =
        BuildRuntimeCategoryOwnerIdentity(class_name, category_name);
    node.interface_owner_identity = aggregate.interface_owner_identity;
    node.implementation_owner_identity = aggregate.implementation_owner_identity;
    node.class_owner_identity = aggregate.class_owner_identity;
    node.adopted_protocol_owner_identities_lexicographic =
        aggregate.adopted_protocol_owner_identities_lexicographic;
    node.has_interface = aggregate.has_interface;
    node.has_implementation = aggregate.has_implementation;
    node.declaration_complete =
        !node.class_name.empty() && !node.category_name.empty() &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        (!node.has_interface || !node.interface_owner_identity.empty()) &&
        (!node.has_implementation || !node.implementation_owner_identity.empty());
    node.attachment_identity_complete =
        !node.class_owner_identity.empty() &&
        (!node.has_interface || !node.interface_owner_identity.empty()) &&
        (!node.has_implementation || !node.implementation_owner_identity.empty());
    node.interface_property_count = aggregate.interface_property_count;
    node.implementation_property_count = aggregate.implementation_property_count;
    node.interface_method_count = aggregate.interface_method_count;
    node.implementation_method_count = aggregate.implementation_method_count;
    node.interface_class_method_count = aggregate.interface_class_method_count;
    node.implementation_class_method_count =
        aggregate.implementation_class_method_count;
    node.line = aggregate.line;
    node.column = aggregate.column;
    std::sort(node.adopted_protocol_owner_identities_lexicographic.begin(),
              node.adopted_protocol_owner_identities_lexicographic.end());
    node.adopted_protocol_owner_identities_lexicographic.erase(
        std::unique(node.adopted_protocol_owner_identities_lexicographic.begin(),
                    node.adopted_protocol_owner_identities_lexicographic.end()),
        node.adopted_protocol_owner_identities_lexicographic.end());
    node.conformance_identity_complete =
        std::all_of(node.adopted_protocol_owner_identities_lexicographic.begin(),
                    node.adopted_protocol_owner_identities_lexicographic.end(),
                    [](const std::string &owner_identity) {
                      return !owner_identity.empty();
                    });
    graph.category_nodes_lexicographic.push_back(node);

    add_owner_edge("category-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("category-to-interface", node.owner_identity,
                   node.interface_owner_identity, node.line, node.column);
    add_owner_edge("category-to-implementation", node.owner_identity,
                   node.implementation_owner_identity, node.line, node.column);
    for (const auto &target : node.adopted_protocol_owner_identities_lexicographic) {
      add_owner_edge("category-to-protocol", node.owner_identity, target,
                     node.line, node.column);
    }
  }

  for (const auto &property_node : graph.property_nodes_lexicographic) {
    if (property_node.has_getter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity == property_node.export_owner_identity &&
            method_record.selector == property_node.getter_selector &&
            !method_record.is_class_method) {
          add_owner_edge("property-to-getter-method", property_node.owner_identity,
                         method_record.owner_identity, property_node.line,
                         property_node.column);
        }
      }
    }

    if (property_node.has_setter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity == property_node.export_owner_identity &&
            method_record.selector == property_node.setter_selector &&
            !method_record.is_class_method) {
          add_owner_edge("property-to-setter-method", property_node.owner_identity,
                         method_record.owner_identity, property_node.line,
                         property_node.column);
        }
      }
    }
  }

  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity, &class_node);
  }

  std::unordered_map<std::string,
                     const Objc3ExecutableMetadataInterfaceGraphNode *>
      interface_nodes_by_owner_identity;
  interface_nodes_by_owner_identity.reserve(
      graph.interface_nodes_lexicographic.size());
  for (const auto &interface_node : graph.interface_nodes_lexicographic) {
    interface_nodes_by_owner_identity.emplace(interface_node.owner_identity,
                                              &interface_node);
  }

  std::unordered_map<std::string, std::string>
      class_interface_method_owner_identity_by_key;
  class_interface_method_owner_identity_by_key.reserve(
      graph.method_nodes_lexicographic.size());
  const auto build_interface_method_key =
      [](const std::string &declaration_owner_identity,
         const std::string &selector, bool is_class_method) {
        return declaration_owner_identity + "::" +
               (is_class_method ? "class" : "instance") + "::" + selector;
      };
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    class_interface_method_owner_identity_by_key.emplace(
        build_interface_method_key(method_node.declaration_owner_identity,
                                   method_node.selector,
                                   method_node.is_class_method),
        method_node.owner_identity);
  }

  const auto find_overridden_interface_method_owner_identity =
      [&](const Objc3ExecutableMetadataInterfaceGraphNode &interface_node,
          const Objc3ExecutableMetadataMethodGraphNode &method_node) {
        std::string next_super_owner_identity =
            interface_node.super_class_owner_identity;
        std::unordered_set<std::string> visited;
        while (!next_super_owner_identity.empty()) {
          if (!visited.insert(next_super_owner_identity).second) {
            return std::string{};
          }
          const auto class_it =
              class_nodes_by_owner_identity.find(next_super_owner_identity);
          if (class_it == class_nodes_by_owner_identity.end() ||
              class_it->second->interface_owner_identity.empty()) {
            return std::string{};
          }
          const std::string key = build_interface_method_key(
              class_it->second->interface_owner_identity, method_node.selector,
              method_node.is_class_method);
          const auto method_it =
              class_interface_method_owner_identity_by_key.find(key);
          if (method_it !=
              class_interface_method_owner_identity_by_key.end()) {
            return method_it->second;
          }
          next_super_owner_identity =
              class_it->second->super_class_owner_identity;
        }
        return std::string{};
      };

  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    const auto interface_it = interface_nodes_by_owner_identity.find(
        method_node.declaration_owner_identity);
    if (interface_it == interface_nodes_by_owner_identity.end() ||
        !interface_it->second->has_super) {
      continue;
    }
    const std::string overridden_method_owner_identity =
        find_overridden_interface_method_owner_identity(*interface_it->second,
                                                        method_node);
    if (!overridden_method_owner_identity.empty()) {
      add_owner_edge("method-to-overridden-method", method_node.owner_identity,
                     overridden_method_owner_identity, method_node.line,
                     method_node.column);
    }
  }

  std::sort(graph.interface_nodes_lexicographic.begin(),
            graph.interface_nodes_lexicographic.end(),
            IsExecutableMetadataInterfaceNodeLess);
  std::sort(graph.implementation_nodes_lexicographic.begin(),
            graph.implementation_nodes_lexicographic.end(),
            IsExecutableMetadataImplementationNodeLess);
  std::sort(graph.class_nodes_lexicographic.begin(),
            graph.class_nodes_lexicographic.end(),
            IsExecutableMetadataClassNodeLess);
  std::sort(graph.metaclass_nodes_lexicographic.begin(),
            graph.metaclass_nodes_lexicographic.end(),
            IsExecutableMetadataMetaclassNodeLess);
  std::sort(graph.protocol_nodes_lexicographic.begin(),
            graph.protocol_nodes_lexicographic.end(),
            IsExecutableMetadataProtocolNodeLess);
  std::sort(graph.category_nodes_lexicographic.begin(),
            graph.category_nodes_lexicographic.end(),
            IsExecutableMetadataCategoryNodeLess);
  std::sort(graph.property_nodes_lexicographic.begin(),
            graph.property_nodes_lexicographic.end(),
            IsExecutableMetadataPropertyNodeLess);
  std::sort(graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            IsExecutableMetadataMethodNodeLess);
  std::sort(graph.ivar_nodes_lexicographic.begin(),
            graph.ivar_nodes_lexicographic.end(),
            IsExecutableMetadataIvarNodeLess);
  std::sort(graph.owner_edges_lexicographic.begin(),
            graph.owner_edges_lexicographic.end(),
            IsExecutableMetadataGraphEdgeLess);

  const auto has_graph_edge = [&graph](const std::string &edge_kind,
                                       const std::string &source_owner_identity,
                                       const std::string &target_owner_identity) {
    return std::any_of(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&edge_kind, &source_owner_identity, &target_owner_identity](
            const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind &&
                 edge.source_owner_identity == source_owner_identity &&
                 edge.target_owner_identity == target_owner_identity;
        });
  };

  const std::size_t expected_class_interface_count = static_cast<std::size_t>(
      std::count_if(program.interfaces.begin(), program.interfaces.end(),
                    [](const Objc3InterfaceDecl &decl) {
                      return !decl.has_category;
                    }));
  const std::size_t expected_class_implementation_count = static_cast<std::size_t>(
      std::count_if(program.implementations.begin(), program.implementations.end(),
                    [](const Objc3ImplementationDecl &decl) {
                      return !decl.has_category;
                    }));
  const bool interface_count_aligned =
      graph.interface_nodes_lexicographic.size() == expected_class_interface_count;
  const bool implementation_count_aligned =
      graph.implementation_nodes_lexicographic.size() ==
      expected_class_implementation_count;
  const bool metaclass_count_aligned =
      graph.metaclass_nodes_lexicographic.size() ==
      graph.interface_nodes_lexicographic.size();
  const bool class_node_floor_satisfied =
      graph.class_nodes_lexicographic.size() >=
          graph.interface_nodes_lexicographic.size() &&
      graph.class_nodes_lexicographic.size() >=
          graph.implementation_nodes_lexicographic.size();
  const bool protocol_count_aligned =
      graph.protocol_nodes_lexicographic.size() ==
      runtime_metadata_source_records.protocols_lexicographic.size();
  const bool property_count_aligned =
      graph.property_nodes_lexicographic.size() ==
      runtime_metadata_source_records.properties_lexicographic.size();
  const bool method_count_aligned =
      graph.method_nodes_lexicographic.size() ==
      runtime_metadata_source_records.methods_lexicographic.size();
  const bool ivar_count_aligned =
      graph.ivar_nodes_lexicographic.size() ==
      runtime_metadata_source_records.ivars_lexicographic.size();
  std::vector<std::string> category_record_owner_names;
  category_record_owner_names.reserve(
      runtime_metadata_source_records.categories_lexicographic.size());
  for (const auto &record :
       runtime_metadata_source_records.categories_lexicographic) {
    category_record_owner_names.push_back(
        BuildCategoryOwnerName(record.class_name, record.category_name));
  }
  std::sort(category_record_owner_names.begin(),
            category_record_owner_names.end());
  category_record_owner_names.erase(
      std::unique(category_record_owner_names.begin(),
                  category_record_owner_names.end()),
      category_record_owner_names.end());
  const bool category_count_aligned =
      graph.category_nodes_lexicographic.size() ==
      category_record_owner_names.size();

  graph.class_metaclass_declaration_closure_complete = true;
  graph.class_metaclass_parent_identity_closure_complete = true;
  graph.class_metaclass_method_owner_identity_closure_complete = true;
  graph.class_metaclass_object_identity_closure_complete = true;
  graph.protocol_category_declaration_closure_complete = true;
  graph.protocol_inheritance_identity_closure_complete = true;
  graph.category_attachment_identity_closure_complete = true;
  graph.protocol_category_conformance_identity_closure_complete = true;

  for (const auto &node : graph.interface_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.declaration_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("interface-to-superclass", node.owner_identity,
                         node.super_class_owner_identity) &&
          has_graph_edge("interface-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        has_graph_edge("interface-to-instance-method-owner", node.owner_identity,
                       node.instance_method_owner_identity) &&
        has_graph_edge("interface-to-class-method-owner", node.owner_identity,
                       node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("interface-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        has_graph_edge("interface-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.implementation_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.declaration_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("implementation-to-superclass", node.owner_identity,
                         node.super_class_owner_identity) &&
          has_graph_edge("implementation-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        has_graph_edge("implementation-to-instance-method-owner",
                       node.owner_identity, node.instance_method_owner_identity) &&
        has_graph_edge("implementation-to-class-method-owner",
                       node.owner_identity, node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("implementation-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        has_graph_edge("implementation-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.class_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.realization_identity_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("class-to-superclass", node.owner_identity,
                         node.super_class_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity;
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("class-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.metaclass_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty();
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("metaclass-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        has_graph_edge("class-to-metaclass", node.class_owner_identity,
                       node.owner_identity);
  }

  for (const auto &node : graph.protocol_nodes_lexicographic) {
    graph.protocol_category_declaration_closure_complete =
        graph.protocol_category_declaration_closure_complete &&
        node.declaration_complete;
    graph.protocol_inheritance_identity_closure_complete =
        graph.protocol_inheritance_identity_closure_complete &&
        node.inherited_protocol_identity_complete &&
        std::all_of(
            node.inherited_protocol_owner_identities_lexicographic.begin(),
            node.inherited_protocol_owner_identities_lexicographic.end(),
            [&](const std::string &target_owner_identity) {
              return has_graph_edge("protocol-to-inherited-protocol",
                                    node.owner_identity,
                                    target_owner_identity);
            });
  }

  for (const auto &node : graph.category_nodes_lexicographic) {
    graph.protocol_category_declaration_closure_complete =
        graph.protocol_category_declaration_closure_complete &&
        node.declaration_complete;
    graph.category_attachment_identity_closure_complete =
        graph.category_attachment_identity_closure_complete &&
        node.attachment_identity_complete &&
        has_graph_edge("category-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        (!node.has_interface ||
         has_graph_edge("category-to-interface", node.owner_identity,
                        node.interface_owner_identity)) &&
        (!node.has_implementation ||
         has_graph_edge("category-to-implementation", node.owner_identity,
                        node.implementation_owner_identity));
    graph.protocol_category_conformance_identity_closure_complete =
        graph.protocol_category_conformance_identity_closure_complete &&
        node.conformance_identity_complete &&
        std::all_of(
            node.adopted_protocol_owner_identities_lexicographic.begin(),
            node.adopted_protocol_owner_identities_lexicographic.end(),
            [&](const std::string &target_owner_identity) {
              return has_graph_edge("category-to-protocol", node.owner_identity,
                                    target_owner_identity);
            });
  }

  graph.deterministic =
      std::is_sorted(graph.interface_nodes_lexicographic.begin(),
                     graph.interface_nodes_lexicographic.end(),
                     IsExecutableMetadataInterfaceNodeLess) &&
      std::is_sorted(graph.implementation_nodes_lexicographic.begin(),
                     graph.implementation_nodes_lexicographic.end(),
                     IsExecutableMetadataImplementationNodeLess) &&
      std::is_sorted(graph.class_nodes_lexicographic.begin(),
                     graph.class_nodes_lexicographic.end(),
                     IsExecutableMetadataClassNodeLess) &&
      std::is_sorted(graph.metaclass_nodes_lexicographic.begin(),
                     graph.metaclass_nodes_lexicographic.end(),
                     IsExecutableMetadataMetaclassNodeLess) &&
      std::is_sorted(graph.protocol_nodes_lexicographic.begin(),
                     graph.protocol_nodes_lexicographic.end(),
                     IsExecutableMetadataProtocolNodeLess) &&
      std::is_sorted(graph.category_nodes_lexicographic.begin(),
                     graph.category_nodes_lexicographic.end(),
                     IsExecutableMetadataCategoryNodeLess) &&
      std::is_sorted(graph.property_nodes_lexicographic.begin(),
                     graph.property_nodes_lexicographic.end(),
                     IsExecutableMetadataPropertyNodeLess) &&
      std::is_sorted(graph.method_nodes_lexicographic.begin(),
                     graph.method_nodes_lexicographic.end(),
                     IsExecutableMetadataMethodNodeLess) &&
      std::is_sorted(graph.ivar_nodes_lexicographic.begin(),
                     graph.ivar_nodes_lexicographic.end(),
                     IsExecutableMetadataIvarNodeLess) &&
      std::is_sorted(graph.owner_edges_lexicographic.begin(),
                     graph.owner_edges_lexicographic.end(),
                     IsExecutableMetadataGraphEdgeLess);
  graph.source_graph_complete =
      graph.deterministic && interface_count_aligned &&
      implementation_count_aligned && metaclass_count_aligned &&
      class_node_floor_satisfied && protocol_count_aligned &&
      category_count_aligned && property_count_aligned &&
      method_count_aligned && ivar_count_aligned &&
      graph.class_metaclass_declaration_closure_complete &&
      graph.class_metaclass_parent_identity_closure_complete &&
      graph.class_metaclass_method_owner_identity_closure_complete &&
      graph.class_metaclass_object_identity_closure_complete &&
      graph.protocol_category_declaration_closure_complete &&
      graph.protocol_inheritance_identity_closure_complete &&
      graph.category_attachment_identity_closure_complete &&
      graph.protocol_category_conformance_identity_closure_complete;
  graph.ready_for_semantic_closure = graph.source_graph_complete;
  graph.ready_for_lowering = false;
  return graph;
}

Objc3ExecutableMetadataSemanticConsistencyBoundary
BuildExecutableMetadataSemanticConsistencyBoundary(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary) {
  Objc3ExecutableMetadataSemanticConsistencyBoundary boundary;
  boundary.executable_metadata_source_graph_contract_id = graph.contract_id;
  boundary.source_graph_ready = IsReadyObjc3ExecutableMetadataSourceGraph(graph);
  boundary.protocol_category_handoff_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  boundary.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  boundary.selector_normalization_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  boundary.property_attribute_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  boundary.symbol_graph_scope_resolution_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary
          .deterministic_scope_resolution_handoff;
  boundary.protocol_node_count = graph.protocol_nodes_lexicographic.size();
  boundary.category_node_count = graph.category_nodes_lexicographic.size();
  boundary.property_node_count = graph.property_nodes_lexicographic.size();
  boundary.method_node_count = graph.method_nodes_lexicographic.size();
  boundary.ivar_node_count = graph.ivar_nodes_lexicographic.size();
  boundary.owner_edge_count = graph.owner_edges_lexicographic.size();

  const auto has_graph_edge =
      [&graph](const std::string &edge_kind, const std::string &source,
               const std::string &target) {
        return std::any_of(
            graph.owner_edges_lexicographic.begin(),
            graph.owner_edges_lexicographic.end(),
            [&](const Objc3ExecutableMetadataGraphEdge &edge) {
              return edge.edge_kind == edge_kind &&
                     edge.source_owner_identity == source &&
                     edge.target_owner_identity == target;
            });
      };

  boundary.protocol_inheritance_edges_complete = true;
  for (const auto &node : graph.protocol_nodes_lexicographic) {
    for (const auto &target :
         node.inherited_protocol_owner_identities_lexicographic) {
      if (!has_graph_edge("protocol-to-inherited-protocol", node.owner_identity,
                          target)) {
        boundary.protocol_inheritance_edges_complete = false;
      }
    }
  }

  boundary.category_attachment_edges_complete = true;
  for (const auto &node : graph.category_nodes_lexicographic) {
    if (!has_graph_edge("category-to-class", node.owner_identity,
                        node.class_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    if (node.has_interface &&
        !has_graph_edge("category-to-interface", node.owner_identity,
                        node.interface_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    if (node.has_implementation &&
        !has_graph_edge("category-to-implementation", node.owner_identity,
                        node.implementation_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    for (const auto &target :
         node.adopted_protocol_owner_identities_lexicographic) {
      if (!has_graph_edge("category-to-protocol", node.owner_identity, target)) {
        boundary.category_attachment_edges_complete = false;
      }
    }
  }

  boundary.declaration_export_owner_split_complete = true;
  for (const auto &node : graph.property_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.owner_kind.empty() ||
        node.owner_name.empty() || node.property_name.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }
  for (const auto &node : graph.method_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.owner_kind.empty() ||
        node.owner_name.empty() || node.selector.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }
  for (const auto &node : graph.ivar_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.property_owner_identity.empty() ||
        node.owner_kind.empty() || node.owner_name.empty() ||
        node.ivar_binding_symbol.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }

  boundary.property_method_ivar_owner_edges_complete = true;
  for (const auto &node : graph.property_nodes_lexicographic) {
    if (!has_graph_edge("property-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("property-to-export-owner", node.owner_identity,
                        node.export_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
    if (!node.ivar_binding_symbol.empty()) {
      const auto ivar_it = std::find_if(
          graph.ivar_nodes_lexicographic.begin(),
          graph.ivar_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataIvarGraphNode &ivar_node) {
            return ivar_node.property_owner_identity == node.owner_identity &&
                   ivar_node.ivar_binding_symbol == node.ivar_binding_symbol;
          });
      if (ivar_it == graph.ivar_nodes_lexicographic.end() ||
          !has_graph_edge("property-to-ivar", node.owner_identity,
                          ivar_it->owner_identity)) {
        boundary.property_method_ivar_owner_edges_complete = false;
      }
    }
    if (node.has_getter) {
      const bool getter_exists = std::any_of(
          graph.method_nodes_lexicographic.begin(),
          graph.method_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
            return method_node.export_owner_identity ==
                       node.export_owner_identity &&
                   !method_node.is_class_method &&
                   method_node.selector == node.getter_selector;
          });
      if (getter_exists) {
        const auto getter_it = std::find_if(
            graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
              return method_node.export_owner_identity ==
                         node.export_owner_identity &&
                     !method_node.is_class_method &&
                     method_node.selector == node.getter_selector &&
                     has_graph_edge("property-to-getter-method",
                                    node.owner_identity,
                                    method_node.owner_identity);
            });
        if (getter_it == graph.method_nodes_lexicographic.end()) {
          boundary.property_method_ivar_owner_edges_complete = false;
        }
      }
    }
    if (node.has_setter) {
      const bool setter_exists = std::any_of(
          graph.method_nodes_lexicographic.begin(),
          graph.method_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
            return method_node.export_owner_identity ==
                       node.export_owner_identity &&
                   !method_node.is_class_method &&
                   method_node.selector == node.setter_selector;
          });
      if (setter_exists) {
        const auto setter_it = std::find_if(
            graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
              return method_node.export_owner_identity ==
                         node.export_owner_identity &&
                     !method_node.is_class_method &&
                     method_node.selector == node.setter_selector &&
                     has_graph_edge("property-to-setter-method",
                                    node.owner_identity,
                                    method_node.owner_identity);
            });
        if (setter_it == graph.method_nodes_lexicographic.end()) {
          boundary.property_method_ivar_owner_edges_complete = false;
        }
      }
    }
  }

  for (const auto &node : graph.method_nodes_lexicographic) {
    if (!has_graph_edge("method-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("method-to-export-owner", node.owner_identity,
                        node.export_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
  }

  for (const auto &node : graph.ivar_nodes_lexicographic) {
    if (!has_graph_edge("ivar-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("ivar-to-export-owner", node.owner_identity,
                        node.export_owner_identity) ||
        !has_graph_edge("ivar-to-property", node.owner_identity,
                        node.property_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
  }

  if (boundary.contract_id.empty()) {
    boundary.failure_reason =
        "metadata semantic consistency contract id is empty";
  } else if (boundary.executable_metadata_source_graph_contract_id.empty()) {
    boundary.failure_reason =
        "metadata semantic consistency graph contract id is empty";
  } else if (!boundary.source_graph_ready) {
    boundary.failure_reason =
        "executable metadata source graph is not ready";
  } else if (!boundary.protocol_category_handoff_deterministic) {
    boundary.failure_reason =
        "protocol/category semantic handoff is not deterministic";
  } else if (!boundary.class_protocol_category_linking_deterministic) {
    boundary.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!boundary.selector_normalization_deterministic) {
    boundary.failure_reason =
        "selector normalization handoff is not deterministic";
  } else if (!boundary.property_attribute_deterministic) {
    boundary.failure_reason =
        "property attribute handoff is not deterministic";
  } else if (!boundary.symbol_graph_scope_resolution_deterministic) {
    boundary.failure_reason =
        "symbol-graph/scope-resolution handoff is not deterministic";
  } else if (!boundary.protocol_inheritance_edges_complete) {
    boundary.failure_reason =
        "protocol inheritance edges are incomplete";
  } else if (!boundary.category_attachment_edges_complete) {
    boundary.failure_reason =
        "category attachment edges are incomplete";
  } else if (!boundary.declaration_export_owner_split_complete) {
    boundary.failure_reason =
        "declaration/export owner identities are incomplete";
  } else if (!boundary.property_method_ivar_owner_edges_complete) {
    boundary.failure_reason =
        "property/method/ivar owner edges are incomplete";
  }

  boundary.semantic_boundary_frozen = boundary.failure_reason.empty();
  boundary.lowering_admission_ready = false;
  boundary.fail_closed =
      boundary.semantic_boundary_frozen &&
      !boundary.lowering_admission_ready &&
      boundary.semantic_conflict_diagnostics_enforcement_pending &&
      boundary.duplicate_export_owner_enforcement_pending &&
      boundary.lowering_admission_pending;
  if (boundary.failure_reason.empty() && !boundary.fail_closed) {
    boundary.failure_reason =
        "metadata semantic consistency freeze is not fail-closed";
  }
  return boundary;
}

Objc3ExecutableMetadataSemanticValidationSurface
BuildExecutableMetadataSemanticValidationSurface(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary) {
  Objc3ExecutableMetadataSemanticValidationSurface surface;
  surface.executable_metadata_semantic_consistency_contract_id =
      semantic_consistency_boundary.contract_id;
  surface.semantic_consistency_ready =
      IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
          semantic_consistency_boundary);

  const Objc3MethodLookupOverrideConflictSummary
      &method_lookup_override_conflict_summary =
          sema_type_metadata_handoff.method_lookup_override_conflict_summary;
  surface.method_lookup_override_conflict_handoff_deterministic =
      method_lookup_override_conflict_summary.deterministic;
  surface.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;

  surface.override_lookup_sites =
      method_lookup_override_conflict_summary.override_lookup_sites;
  surface.override_lookup_hits =
      method_lookup_override_conflict_summary.override_lookup_hits;
  surface.override_lookup_misses =
      method_lookup_override_conflict_summary.override_lookup_misses;
  surface.override_conflicts =
      method_lookup_override_conflict_summary.override_conflicts;
  surface.unresolved_base_interfaces =
      method_lookup_override_conflict_summary.unresolved_base_interfaces;
  surface.protocol_composition_sites =
      class_protocol_category_linking_summary.protocol_composition_sites;
  surface.protocol_composition_symbols =
      class_protocol_category_linking_summary.protocol_composition_symbols;
  surface.category_composition_sites =
      class_protocol_category_linking_summary.category_composition_sites;
  surface.category_composition_symbols =
      class_protocol_category_linking_summary.category_composition_symbols;
  surface.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;

  const auto count_edges = [&](const std::string &edge_kind) {
    return static_cast<std::size_t>(std::count_if(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&](const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind;
        }));
  };
  const auto has_edge = [&](const std::string &edge_kind,
                            const std::string &source_owner_identity,
                            const std::string &target_owner_identity) {
    return std::any_of(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&](const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind &&
                 edge.source_owner_identity == source_owner_identity &&
                 edge.target_owner_identity == target_owner_identity;
        });
  };

  surface.class_inheritance_edge_count = count_edges("class-to-superclass");
  surface.protocol_inheritance_edge_count =
      count_edges("protocol-to-inherited-protocol");
  surface.metaclass_super_edge_count =
      count_edges("metaclass-to-super-metaclass");
  surface.override_edge_count = count_edges("method-to-overridden-method");

  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity, &class_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataMetaclassGraphNode *>
      metaclass_nodes_by_owner_identity;
  metaclass_nodes_by_owner_identity.reserve(
      graph.metaclass_nodes_lexicographic.size());
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    metaclass_nodes_by_owner_identity.emplace(metaclass_node.owner_identity,
                                              &metaclass_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataProtocolGraphNode *>
      protocol_nodes_by_owner_identity;
  protocol_nodes_by_owner_identity.reserve(
      graph.protocol_nodes_lexicographic.size());
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    protocol_nodes_by_owner_identity.emplace(protocol_node.owner_identity,
                                             &protocol_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataInterfaceGraphNode *>
      interface_nodes_by_owner_identity;
  interface_nodes_by_owner_identity.reserve(
      graph.interface_nodes_lexicographic.size());
  for (const auto &interface_node : graph.interface_nodes_lexicographic) {
    interface_nodes_by_owner_identity.emplace(interface_node.owner_identity,
                                              &interface_node);
  }

  std::unordered_map<std::string, const Objc3ExecutableMetadataMethodGraphNode *>
      class_interface_methods_by_key;
  class_interface_methods_by_key.reserve(graph.method_nodes_lexicographic.size());
  const auto build_interface_method_key =
      [](const std::string &declaration_owner_identity,
         const std::string &selector, bool is_class_method) {
        return declaration_owner_identity + "::" +
               (is_class_method ? "class" : "instance") + "::" + selector;
      };
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    class_interface_methods_by_key.emplace(
        build_interface_method_key(method_node.declaration_owner_identity,
                                   method_node.selector,
                                   method_node.is_class_method),
        &method_node);
  }

  surface.class_inheritance_edges_complete = true;
  surface.inheritance_chain_cycle_free = true;
  surface.superclass_targets_resolved = true;
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    if (!class_node.has_super) {
      continue;
    }
    if (!has_edge("class-to-superclass", class_node.owner_identity,
                  class_node.super_class_owner_identity)) {
      surface.class_inheritance_edges_complete = false;
    }
    std::string next_super_owner_identity = class_node.super_class_owner_identity;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        surface.inheritance_chain_cycle_free = false;
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end()) {
        surface.superclass_targets_resolved = false;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
  }

  surface.protocol_inheritance_edges_complete = true;
  surface.protocol_inheritance_targets_resolved = true;
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    for (const auto &target_owner_identity :
         protocol_node.inherited_protocol_owner_identities_lexicographic) {
      if (!has_edge("protocol-to-inherited-protocol",
                    protocol_node.owner_identity, target_owner_identity)) {
        surface.protocol_inheritance_edges_complete = false;
      }
      if (protocol_nodes_by_owner_identity.find(target_owner_identity) ==
          protocol_nodes_by_owner_identity.end()) {
        surface.protocol_inheritance_targets_resolved = false;
      }
    }
  }

  surface.metaclass_edges_complete = true;
  surface.metaclass_targets_resolved = true;
  surface.metaclass_lineage_aligned = true;
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    const auto class_it =
        class_nodes_by_owner_identity.find(metaclass_node.class_owner_identity);
    if (class_it == class_nodes_by_owner_identity.end()) {
      surface.metaclass_targets_resolved = false;
      surface.metaclass_lineage_aligned = false;
      continue;
    }
    if (!has_edge("class-to-metaclass", metaclass_node.class_owner_identity,
                  metaclass_node.owner_identity)) {
      surface.metaclass_edges_complete = false;
    }
    if (class_it->second->metaclass_owner_identity != metaclass_node.owner_identity) {
      surface.metaclass_lineage_aligned = false;
    }
    if (metaclass_node.has_super) {
      if (!has_edge("metaclass-to-super-metaclass", metaclass_node.owner_identity,
                    metaclass_node.super_metaclass_owner_identity)) {
        surface.metaclass_edges_complete = false;
      }
      const auto super_metaclass_it =
          metaclass_nodes_by_owner_identity.find(
              metaclass_node.super_metaclass_owner_identity);
      if (super_metaclass_it == metaclass_nodes_by_owner_identity.end()) {
        surface.metaclass_targets_resolved = false;
        surface.metaclass_lineage_aligned = false;
      } else if (class_it->second->has_super &&
                 super_metaclass_it->second->class_owner_identity !=
                     class_it->second->super_class_owner_identity) {
        surface.metaclass_lineage_aligned = false;
      }
    } else if (!metaclass_node.super_metaclass_owner_identity.empty()) {
      surface.metaclass_lineage_aligned = false;
    }
  }

  surface.method_override_edges_complete = true;
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    const auto interface_it = interface_nodes_by_owner_identity.find(
        method_node.declaration_owner_identity);
    if (interface_it == interface_nodes_by_owner_identity.end() ||
        !interface_it->second->has_super) {
      continue;
    }

    std::string next_super_owner_identity =
        interface_it->second->super_class_owner_identity;
    const Objc3ExecutableMetadataMethodGraphNode *overridden_method = nullptr;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end() ||
          class_it->second->interface_owner_identity.empty()) {
        break;
      }
      const auto base_method_it = class_interface_methods_by_key.find(
          build_interface_method_key(class_it->second->interface_owner_identity,
                                     method_node.selector,
                                     method_node.is_class_method));
      if (base_method_it != class_interface_methods_by_key.end()) {
        overridden_method = base_method_it->second;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
    if (overridden_method == nullptr) {
      continue;
    }
    if (!has_edge("method-to-overridden-method", method_node.owner_identity,
                  overridden_method->owner_identity)) {
      surface.method_override_edges_complete = false;
    } else if (method_node.is_class_method) {
      ++surface.class_method_override_edge_count;
    } else {
      ++surface.instance_method_override_edge_count;
    }
  }

  surface.override_lookup_complete =
      method_lookup_override_conflict_summary.unresolved_base_interfaces == 0u;
  surface.override_conflicts_absent =
      method_lookup_override_conflict_summary.override_conflicts == 0u;
  surface.protocol_composition_valid =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          0u &&
      class_protocol_category_linking_summary.protocol_composition_symbols >=
          class_protocol_category_linking_summary
              .category_composition_symbols &&
      class_protocol_category_linking_summary.protocol_composition_sites >=
          class_protocol_category_linking_summary.category_composition_sites;

  surface.inheritance_validation_ready =
      surface.class_inheritance_edges_complete &&
      surface.protocol_inheritance_edges_complete &&
      surface.inheritance_chain_cycle_free &&
      surface.superclass_targets_resolved &&
      surface.protocol_inheritance_targets_resolved;
  surface.override_validation_ready =
      surface.method_lookup_override_conflict_handoff_deterministic &&
      surface.method_override_edges_complete &&
      surface.override_lookup_complete &&
      surface.override_conflicts_absent;
  surface.protocol_composition_validation_ready =
      surface.class_protocol_category_linking_deterministic &&
      surface.protocol_composition_valid;
  surface.metaclass_relationship_validation_ready =
      surface.metaclass_edges_complete &&
      surface.metaclass_targets_resolved &&
      surface.metaclass_lineage_aligned;

  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic validation contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic consistency dependency contract id is empty";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "executable metadata semantic consistency boundary is not ready";
  } else if (!surface.method_lookup_override_conflict_handoff_deterministic) {
    surface.failure_reason =
        "method lookup/override conflict handoff is not deterministic";

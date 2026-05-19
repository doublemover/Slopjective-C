static void ValidateAndBuildCategoryMergeSurfaces(
    const Objc3Program &ast, Objc3SemanticIntegrationSurface &surface,
    std::vector<std::string> &diagnostics) {
  // category-merge enforcement anchor: sema owns deterministic
  // realized-class category merge order, concrete conflict detection, and the
  // merged member surface used by later concrete resolution and conformance.
  std::unordered_map<std::string, std::vector<Objc3CategoryMergeOrderEntry>>
      merge_order_by_class;
  std::unordered_set<std::string> seen_realized_category_interfaces;

  for (const auto &interface_decl : ast.interfaces) {
    if (!interface_decl.has_category) {
      continue;
    }
    if (!IsRealizedClassOwner(surface, interface_decl.name)) {
      continue;
    }

    const std::string owner_identity = BuildCategoryOwnerIdentity(
        interface_decl.name, interface_decl.category_name);
    if (!seen_realized_category_interfaces.insert(owner_identity).second) {
      continue;
    }
    if (surface.category_implementations.find(owner_identity) ==
        surface.category_implementations.end()) {
      diagnostics.push_back(MakeDiag(
          interface_decl.line, interface_decl.column, "O3S219",
          "category merge failure: category interface '" +
              FormatObjcContainerName(interface_decl.name, true,
                                      interface_decl.category_name) +
              "' is missing category implementation for realized class '" +
              interface_decl.name + "'"));
      continue;
    }

    Objc3CategoryMergeOrderEntry entry;
    entry.class_name = interface_decl.name;
    entry.category_name = interface_decl.category_name;
    entry.owner_identity = owner_identity;
    entry.container_name = FormatObjcContainerName(interface_decl.name, true,
                                                   interface_decl.category_name);
    entry.line = interface_decl.line;
    entry.column = interface_decl.column;
    merge_order_by_class[entry.class_name].push_back(std::move(entry));
  }

  for (const auto &implementation_decl : ast.implementations) {
    if (!implementation_decl.has_category) {
      continue;
    }
    if (!IsRealizedClassOwner(surface, implementation_decl.name)) {
      continue;
    }

    const std::string owner_identity = BuildCategoryOwnerIdentity(
        implementation_decl.name, implementation_decl.category_name);
    if (surface.category_interfaces.find(owner_identity) ==
        surface.category_interfaces.end()) {
      diagnostics.push_back(MakeDiag(
          implementation_decl.line, implementation_decl.column, "O3S219",
          "category merge failure: category implementation '" +
              FormatObjcContainerName(implementation_decl.name, true,
                                      implementation_decl.category_name) +
              "' is missing category interface for realized class '" +
              implementation_decl.name + "'"));
    }
  }

  surface.category_merge_surfaces.clear();
  for (const auto &entry : merge_order_by_class) {
    const std::string &class_name = entry.first;
    Objc3CategoryMergeInfo merge_info;
    for (const Objc3CategoryMergeOrderEntry &order_entry : entry.second) {
      merge_info.category_owner_identities_in_merge_order.push_back(
          order_entry.container_name);
      const auto category_interface_it =
          surface.category_interfaces.find(order_entry.owner_identity);
      const auto category_implementation_it =
          surface.category_implementations.find(order_entry.owner_identity);
      if (category_interface_it == surface.category_interfaces.end() ||
          category_implementation_it ==
              surface.category_implementations.end()) {
        merge_info.deterministic = false;
        continue;
      }

      for (const auto &property_entry :
           category_interface_it->second.properties) {
        MergeCategoryPropertyCandidate(
            surface, class_name, order_entry, property_entry.first,
            property_entry.second, merge_info, diagnostics);
      }
      for (const auto &property_entry :
           category_implementation_it->second.properties) {
        MergeCategoryPropertyCandidate(
            surface, class_name, order_entry, property_entry.first,
            property_entry.second, merge_info, diagnostics);
      }
      for (const auto &method_entry : category_interface_it->second.methods) {
        MergeCategoryMethodCandidate(surface, class_name, order_entry,
                                     method_entry.first, method_entry.second,
                                     merge_info, diagnostics);
      }
      for (const auto &method_entry :
           category_implementation_it->second.methods) {
        MergeCategoryMethodCandidate(surface, class_name, order_entry,
                                     method_entry.first, method_entry.second,
                                     merge_info, diagnostics);
      }
    }

    surface.category_merge_surfaces.emplace(class_name, std::move(merge_info));
  }
}

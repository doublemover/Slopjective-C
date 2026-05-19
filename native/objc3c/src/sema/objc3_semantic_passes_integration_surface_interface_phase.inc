  for (const auto &interface_decl : ast.interfaces) {
    const bool is_category_interface = interface_decl.has_category;
    const std::string container_name = FormatObjcContainerName(
        interface_decl.name, interface_decl.has_category,
        interface_decl.category_name);
    const std::string container_label =
        interface_container_label(is_category_interface);

    auto interface_it = surface.interfaces.end();
    if (!is_category_interface) {
      interface_it = surface.interfaces.find(interface_decl.name);
    }
    if (!is_category_interface && interface_it != surface.interfaces.end()) {
      diagnostics.push_back(
          MakeDiag(interface_decl.line, interface_decl.column, "O3S200", "duplicate interface '" + interface_decl.name + "'"));
      continue;
    }

    Objc3InterfaceInfo interface_info;
    // completeness anchor: sema preserves deterministic interface
    // super-name and implementation-pairing inputs for class/metaclass graph
    // materialization in the frontend pipeline.
    // completion anchor: sema must preserve protocol/category/member
    // declaration inventories without reordering so export graph closure stays
    // deterministic for protocol/category/property/ivar packets.
    // freeze anchor: the same deterministic inventories remain the
    // semantic-consistency boundary inputs for fail-closed lane-B admission.
    // accessor legality anchor: interface/category-interface
    // property containers also own one fail-closed effective accessor selector
    // namespace so later executable accessors never inherit ambiguous getter
    // or setter bindings from source.
    interface_info.super_name = interface_decl.super_name;
    interface_info.adopted_protocols_lexicographic =
        BuildSortedUniqueStringsLocal(interface_decl.adopted_protocols);
    for (const auto &generic_param : interface_decl.generic_params) {
      interface_info.generic_parameter_names_source_order.push_back(
          generic_param.name);
      interface_info.generic_parameter_variance_source_order.push_back(
          generic_param.variance_spelling);
    }
    interface_info.objc_direct_members_declared =
        interface_decl.objc_direct_members_declared;
    interface_info.objc_final_declared = interface_decl.objc_final_declared;
    interface_info.objc_sealed_declared = interface_decl.objc_sealed_declared;
    if (interface_decl.objc_derive_declared) {
      const std::string derive_spelling =
          NormalizeMetaprogrammingRawDeriveToken(interface_decl.objc_derive_name);
      const std::string canonical_derive_name =
          NormalizeMetaprogrammingSupportedDeriveName(derive_spelling);
      if (is_category_interface) {
        diagnostics.push_back(MakeDiag(
            interface_decl.line, interface_decl.column, "O3S318",
            "derive expansion is only supported on primary interfaces, not category interface '" +
                container_name + "'"));
      } else if (canonical_derive_name.empty()) {
        diagnostics.push_back(MakeDiag(
            interface_decl.line, interface_decl.column, "O3S317",
            "unsupported derive '" + derive_spelling +
                "' on interface '" + container_name + "'"));
      } else {
        const std::string derived_selector =
            BuildMetaprogrammingDerivedSelectorForInventoryRow(canonical_derive_name);
        if (!derived_selector.empty()) {
          const bool selector_conflict = std::any_of(
              interface_decl.methods.begin(), interface_decl.methods.end(),
              [&derived_selector](const Objc3MethodDecl &method_decl) {
                return BuildMethodSelectorNormalizationContractInfo(method_decl)
                           .normalized_selector == derived_selector;
              });
          if (selector_conflict) {
            diagnostics.push_back(MakeDiag(
                interface_decl.line, interface_decl.column, "O3S319",
                "derive expansion selector conflict: interface '" +
                    container_name + "' already declares selector '" +
                    derived_selector + "'"));
          }
        }
      }
    }
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        getter_owner_states;
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        setter_owner_states;
    for (const auto &property_decl : interface_decl.properties) {
      ValidatePropertyTypeSuffixes(property_decl, container_name,
                                   container_label, diagnostics,
                                   &generic_interface_names);
      Objc3PropertyInfo property_info =
          BuildPropertyInfo(property_decl, container_name, container_label,
                            arc_mode_enabled,
                            diagnostics);
      const auto property_insert = interface_info.properties.emplace(property_decl.name, std::move(property_info));
      if (!property_insert.second) {
        diagnostics.push_back(
            MakeDiag(property_decl.line,
                     property_decl.column,
                     "O3S200",
                     "duplicate " + container_label + " property '" +
                         property_decl.name + "' in " + container_label +
                         " '" + container_name + "'"));
      } else {
        ValidatePropertyAccessorSelectorUniqueness(
            property_insert.first->second,
            property_decl.name,
            container_name,
            container_label,
            interface_info.properties,
            getter_owner_states,
            setter_owner_states,
            diagnostics);
      }
    }

    for (const auto &method_decl : interface_decl.methods) {
      const MethodSelectorNormalizationContractInfo selector_contract =
          BuildMethodSelectorNormalizationContractInfo(method_decl);
      ValidateMethodSelectorNormalizationContract(method_decl, container_name,
                                                 container_label,
                                                 selector_contract,
                                                 diagnostics);
      ValidateMethodReturnTypeSuffixes(method_decl, container_name,
                                       container_label, diagnostics,
                                       &generic_interface_names);
      ValidateMethodParameterTypeSuffixes(method_decl, container_name,
                                          container_label, diagnostics,
                                          &generic_interface_names);
      if (method_decl.objc_macro_declared ||
          method_decl.objc_macro_package_declared ||
          method_decl.objc_macro_provenance_declared) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S325",
            "macro expansion is currently limited to free functions; " +
                container_label + " selector in '" + container_name +
                "' is not sandbox-admitted"));
      }

      const std::string selector = selector_contract.normalized_selector;
      if (method_decl.has_body) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: " + container_label +
                                           " selector '" + selector + "' in '" +
                                           container_name +
                                           "' must not define a body"));
      }

      const auto method_insert =
          interface_info.methods.emplace(
              selector, BuildMethodInfo(method_decl, selector_contract,
                                        arc_mode_enabled,
                                        &generic_interface_names));
      if (!method_insert.second) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S200",
                                       "duplicate " + container_label +
                                           " selector '" + selector + "' in " +
                                           container_label + " '" +
                                           container_name + "'"));
        continue;
      }

      method_insert.first->second.effective_direct_dispatch =
          IsDispatchEffectivelyDirectMethod(method_insert.first->second,
                                         interface_info);

      if (!is_category_interface) {
        ++interface_implementation_summary.interface_method_symbols;
      }
    }

    if (!is_category_interface) {
      surface.interfaces.emplace(interface_decl.name, std::move(interface_info));
    } else {
      const std::string category_owner_identity =
          BuildCategoryOwnerIdentity(interface_decl.name,
                                     interface_decl.category_name);
      if (!surface.category_interfaces
               .emplace(category_owner_identity, std::move(interface_info))
               .second) {
        diagnostics.push_back(MakeDiag(interface_decl.line,
                                       interface_decl.column,
                                       "O3S200",
                                       "duplicate category interface '" +
                                           container_name + "'"));
      }
    }
  }

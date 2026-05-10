#include "sema/objc3_semantic_passes_integration_surface_id_class_sel_handoff.inc"
#include "sema/objc3_semantic_passes_integration_surface_function_builder.inc"

// lowering-handoff anchor: semantic integration plus type metadata
// handoff remain the canonical executable metadata graph source for typed
// lowering handoff packets and must stay deterministic before lowering claims
// readiness.
// typed-lowering anchor: the concrete lowering-ready metadata graph
// handoff continues to consume this semantic integration surface without
// reparsing syntax, so type/owner closure must remain deterministic here.
// debug-projection anchor: the lane-C manifest/IR inspection matrix
// replays this semantic integration surface verbatim before runtime section
// emission and runtime ingest packaging land.
// runtime-ingest packaging anchor: lane-D freezes one manifest
// transport boundary over this semantic integration surface before section
// emission/startup registration land, so later runtime ingest consumes the
// published graph rather than a reconstructed sema view.
Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(
    const Objc3ParsedProgram &program,
    bool allow_source_only_block_literals,
    bool allow_source_only_defer_statements,
    bool allow_source_only_error_runtime_surface,
    bool arc_mode_enabled,
    std::vector<std::string> &diagnostics) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  Objc3SemanticIntegrationSurface surface;
  std::unordered_map<std::string, int> resolved_global_values;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
      protocol_definitions =
          BuildProtocolSemanticDefinitions(ast, arc_mode_enabled, diagnostics);
  const std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
      generic_definitions =
          BuildInterfaceGenericDefinitions(ast, protocol_definitions,
                                           diagnostics);
  std::unordered_set<std::string> generic_interface_names;
  for (const auto &entry : generic_definitions) {
    if (!entry.second.parameters.empty()) {
      generic_interface_names.insert(entry.first);
    }
  }
  ValidateInterfaceGenericSpecializations(ast, generic_definitions,
                                          protocol_definitions, diagnostics);
  ValidateProtocolCompositionIdentifierBindings(ast, protocol_definitions,
                                                diagnostics);
  // diagnostic precision anchor: class interface/implementation
  // summaries exclude category containers so valid class-plus-category
  // programs do not degrade into duplicate class-owner diagnostics before the
  // runtime metadata blocker computes attachment collisions and ambiguity.
  interface_implementation_summary.declared_interfaces = static_cast<std::size_t>(
      std::count_if(ast.interfaces.begin(), ast.interfaces.end(),
                    [](const Objc3InterfaceDecl &decl) {
                      return !decl.has_category;
                    }));
  interface_implementation_summary.declared_implementations =
      static_cast<std::size_t>(
          std::count_if(ast.implementations.begin(), ast.implementations.end(),
                        [](const Objc3ImplementationDecl &decl) {
                          return !decl.has_category;
                        }));

  const auto interface_container_label = [](bool has_category) {
    return has_category ? "category interface" : "interface";
  };
  const auto implementation_container_label = [](bool has_category) {
    return has_category ? "category implementation" : "implementation";
  };

  for (const auto &global : ast.globals) {
    const bool duplicate_global = surface.globals.find(global.name) != surface.globals.end();
    if (duplicate_global) {
      diagnostics.push_back(MakeDiag(global.line, global.column, "O3S200", "duplicate global '" + global.name + "'"));
    } else {
      surface.globals.emplace(global.name, ValueType::I32);
    }
    int value = 0;
    if (!EvaluateObjc3ConstExpr(global.value.get(), value,
                                &resolved_global_values)) {
      diagnostics.push_back(
          MakeDiag(global.line, global.column, "O3S210", "global initializer must be constant expression"));
    } else if (!duplicate_global) {
      resolved_global_values.emplace(global.name, value);
    }
  }

  PopulateSemanticIntegrationSurfaceFunctions(
      ast, surface, generic_interface_names, arc_mode_enabled, diagnostics);

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

  for (const auto &implementation_decl : ast.implementations) {
    const bool is_category_implementation = implementation_decl.has_category;
    const std::string container_name = FormatObjcContainerName(
        implementation_decl.name, implementation_decl.has_category,
        implementation_decl.category_name);
    const std::string container_label =
        implementation_container_label(is_category_implementation);

    auto implementation_it = surface.implementations.end();
    if (!is_category_implementation) {
      implementation_it = surface.implementations.find(implementation_decl.name);
    }
    if (!is_category_implementation &&
        implementation_it != surface.implementations.end()) {
      diagnostics.push_back(MakeDiag(implementation_decl.line, implementation_decl.column, "O3S200",
                                     "duplicate implementation '" + implementation_decl.name + "'"));
      continue;
    }

    Objc3ImplementationInfo implementation_info;
    const Objc3InterfaceInfo *matched_interface = nullptr;
    if (!is_category_implementation) {
      const auto interface_it = surface.interfaces.find(implementation_decl.name);
      if (interface_it == surface.interfaces.end()) {
        diagnostics.push_back(MakeDiag(implementation_decl.line, implementation_decl.column, "O3S206",
                                       "type mismatch: missing interface declaration for implementation '" +
                                           implementation_decl.name + "'"));
      } else {
        implementation_info.has_matching_interface = true;
        matched_interface = &interface_it->second;
      }
    } else {
      const std::string category_owner_identity =
          BuildCategoryOwnerIdentity(implementation_decl.name,
                                     implementation_decl.category_name);
      const auto interface_it =
          surface.category_interfaces.find(category_owner_identity);
      if (interface_it != surface.category_interfaces.end()) {
        implementation_info.has_matching_interface = true;
        matched_interface = &interface_it->second;
      }
    }

    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        getter_owner_states;
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        setter_owner_states;
    for (const auto &property_decl : implementation_decl.properties) {
      ValidatePropertyTypeSuffixes(property_decl, container_name,
                                   container_label, diagnostics,
                                   &generic_interface_names);
      Objc3PropertyInfo property_info =
          BuildPropertyInfo(property_decl, container_name, container_label,
                            arc_mode_enabled,
                            diagnostics);
      const auto property_insert =
          implementation_info.properties.emplace(property_decl.name,
                                                std::move(property_info));
      if (!property_insert.second) {
        diagnostics.push_back(MakeDiag(
            property_decl.line, property_decl.column, "O3S200",
            "duplicate " + container_label + " property '" +
                property_decl.name + "' in " + container_label + " '" +
                container_name + "'"));
        continue;
      }
      ValidatePropertyAccessorSelectorUniqueness(
          property_insert.first->second,
          property_decl.name,
          container_name,
          container_label,
          implementation_info.properties,
          getter_owner_states,
          setter_owner_states,
          diagnostics);

      if (matched_interface == nullptr) {
        continue;
      }

      const auto interface_property_it =
          matched_interface->properties.find(property_decl.name);
      if (interface_property_it == matched_interface->properties.end()) {
        diagnostics.push_back(MakeDiag(property_decl.line,
                                       property_decl.column,
                                       "O3S206",
                                       "type mismatch: implementation property '" + property_decl.name + "' in '" +
                                           implementation_decl.name + "' is not declared in interface"));
        continue;
      }
      if (!IsCompatiblePropertySignature(interface_property_it->second, property_insert.first->second)) {
        if (!EmitImplementationPropertyReflectionAccessorCompatibilityDiagnostics(
                interface_property_it->second,
                property_insert.first->second,
                property_decl.name,
                implementation_decl.name,
                diagnostics)) {
          diagnostics.push_back(MakeDiag(
              property_decl.line,
              property_decl.column,
              "O3S206",
              "type mismatch: incompatible property signature for '" +
                  property_decl.name + "' in implementation '" +
                  implementation_decl.name + "'"));
        }
        continue;
      }
      OverlayAuthoritativeDefaultIvarBinding(property_insert.first->second,
                                             interface_property_it->second);
    }

    // property-synthesis enforcement anchor: matched class
    // implementations resolve default ivar bindings from interface-declared
    // properties first, then fold in optional implementation redeclarations
    // without making redeclaration a prerequisite for authoritative binding.
    if (!is_category_implementation && matched_interface != nullptr) {
      for (const auto &interface_property_entry : matched_interface->properties) {
        const Objc3PropertyInfo &interface_property = interface_property_entry.second;
        if (!HasResolvedDefaultIvarBinding(interface_property)) {
          diagnostics.push_back(MakeDiag(
              implementation_decl.line,
              implementation_decl.column,
              "O3S206",
              "type mismatch: property synthesis for '" +
                  interface_property_entry.first + "' in implementation '" +
                  implementation_decl.name +
                  "' lacks an authoritative default ivar binding"));
          continue;
        }

        const Objc3PropertyInfo *implementation_property =
            FindPropertyInImplementationInfo(implementation_info,
                                            interface_property_entry.first);
        if (implementation_property != nullptr &&
            (!HasResolvedDefaultIvarBinding(*implementation_property) ||
             !HasCompatibleResolvedDefaultIvarBinding(interface_property,
                                                     *implementation_property))) {
          diagnostics.push_back(MakeDiag(
              implementation_decl.line,
              implementation_decl.column,
              "O3S206",
              "type mismatch: property synthesis for '" +
                  interface_property_entry.first + "' in implementation '" +
                  implementation_decl.name +
                  "' drifted from the interface default ivar binding"));
        }
      }
    }

    for (const auto &method_decl : implementation_decl.methods) {
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
      if (!method_decl.has_body) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: " + container_label +
                                           " selector '" + selector + "' in '" +

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
    Objc3SemaLanguageProfile language_profile,
    bool allow_source_only_block_literals,
    bool allow_source_only_defer_statements,
    bool allow_source_only_error_runtime_surface,
    bool arc_mode_enabled,
    std::vector<std::string> &diagnostics) {
#include "sema/objc3_semantic_passes_integration_surface_builder_setup.inc"

#include "sema/objc3_semantic_passes_integration_surface_global_function_phase.inc"

#include "sema/objc3_semantic_passes_integration_surface_interface_phase.inc"

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
          method_decl.objc_macro_provenance_declared ||
          method_decl.objc_macro_cache_key_declared ||
          method_decl.objc_macro_sandbox_declared) {
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

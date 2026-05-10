static const Objc3InterfaceDecl *FindAstInterfaceDecl(
    const Objc3Program &ast,
    const std::string &name) {
  for (const auto &interface_decl : ast.interfaces) {
    if (!interface_decl.has_category && interface_decl.name == name) {
      return &interface_decl;
    }
  }
  return nullptr;
}

// inheritance-override-realization anchor: sema now turns the
// frozen rules into live fail-closed legality for realized classes.
// The parser still owns raw superclass spellings and member identities, but
// sema rejects missing/cyclic superclass closure, unrealized superclass
// implementations, selector-kind drift, and inherited method/property
// incompatibilities before runtime realization and concrete dispatch can
// consume the class surface.
static void ValidateInheritanceOverrideAndRealizationLegality(
    const Objc3Program &ast,
    const Objc3SemanticIntegrationSurface &surface,
    std::vector<std::string> &diagnostics) {
  for (const auto &implementation_decl : ast.implementations) {
    if (implementation_decl.has_category) {
      continue;
    }

    const auto interface_it = surface.interfaces.find(implementation_decl.name);
    if (interface_it == surface.interfaces.end()) {
      continue;
    }
    if (surface.implementations.find(implementation_decl.name) ==
        surface.implementations.end()) {
      continue;
    }

    const Objc3InterfaceDecl *interface_decl =
        FindAstInterfaceDecl(ast, implementation_decl.name);
    if (interface_decl == nullptr) {
      continue;
    }
    if (interface_it->second.super_name.empty()) {
      continue;
    }

    // legality anchor: fail closed on superclass finality/sealing
    // and on override chains that cross objc_final / objc_direct boundaries
    // before Part 9 lowering or runnable dispatch behavior is allowed to widen.
    const auto direct_super_it =
        surface.interfaces.find(interface_it->second.super_name);
    if (direct_super_it != surface.interfaces.end()) {
      if (direct_super_it->second.objc_final_declared) {
        diagnostics.push_back(MakeDiag(
            interface_decl->line, interface_decl->column, "O3S307",
            "dispatch-control semantics failed: interface '" +
                interface_decl->name +
                "' cannot inherit from objc_final superclass '" +
                direct_super_it->first + "'"));
      }
      if (direct_super_it->second.objc_sealed_declared) {
        diagnostics.push_back(MakeDiag(
            interface_decl->line, interface_decl->column, "O3S308",
            "dispatch-control semantics failed: interface '" +
                interface_decl->name +
                "' cannot inherit from objc_sealed superclass '" +
                direct_super_it->first + "'"));
      }
    }

    const Objc3SuperclassRealizationClosure closure =
        ResolveSuperclassRealizationClosure(surface, implementation_decl.name);
    if (closure.cycle_detected) {
      diagnostics.push_back(MakeDiag(
          interface_decl->line, interface_decl->column, "O3S220",
          "runtime realization failed: inheritance cycle detected for interface '" +
              interface_decl->name + "'"));
      continue;
    }
    if (closure.missing_super_interface) {
      diagnostics.push_back(MakeDiag(
          interface_decl->line, interface_decl->column, "O3S220",
          "runtime realization failed: interface '" + interface_decl->name +
              "' inherits from missing superclass '" +
              closure.failing_super_name + "'"));
      continue;
    }
    if (closure.missing_super_implementation) {
      diagnostics.push_back(MakeDiag(
          implementation_decl.line, implementation_decl.column, "O3S220",
          "runtime realization failed: implementation '" +
              implementation_decl.name +
              "' requires realized superclass implementation '" +
              closure.failing_super_name + "'"));
      continue;
    }

    for (const auto &method_decl : interface_decl->methods) {
      const MethodSelectorNormalizationContractInfo selector_contract =
          BuildMethodSelectorNormalizationContractInfo(method_decl);
      const auto method_it = interface_it->second.methods.find(
          selector_contract.normalized_selector);
      if (method_it == interface_it->second.methods.end()) {
        continue;
      }

      const Objc3InheritedMethodLookup inherited_method =
          FindSurfaceMethodInSuperChainDetailed(
              surface, implementation_decl.name,
              selector_contract.normalized_selector, true,
              method_decl.is_class_method);
      const Objc3InheritedMethodLookup inherited_method_any_kind =
          FindSurfaceMethodInSuperChainDetailed(
              surface, implementation_decl.name,
              selector_contract.normalized_selector, false,
              method_decl.is_class_method);

      if (inherited_method_any_kind.method != nullptr &&
          inherited_method_any_kind.method->is_class_method !=
              method_it->second.is_class_method) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S220",
            "runtime realization failed: selector '" +
                FormatMethodSelectorForDiagnostic(
                    selector_contract.normalized_selector,
                    method_it->second.is_class_method) +
                "' in interface '" + interface_decl->name +
                "' changes method kind relative to inherited selector '" +
                FormatMethodSelectorForDiagnostic(
                    selector_contract.normalized_selector,
                    inherited_method_any_kind.method->is_class_method) +
                "' in superclass '" +
                inherited_method_any_kind.owner_name + "'"));
        continue;
      }

      if (inherited_method.method != nullptr &&
          !IsCompatibleMethodSignature(*inherited_method.method,
                                       method_it->second)) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S220",
            "runtime realization failed: incompatible override signature for selector '" +
                FormatMethodSelectorForDiagnostic(
                    selector_contract.normalized_selector,
                    method_it->second.is_class_method) +
                "' in interface '" + interface_decl->name +
                "' relative to superclass '" +
                inherited_method.owner_name + "'"));
      }

      if (inherited_method.method != nullptr) {
        const auto inherited_owner_it =
            surface.interfaces.find(inherited_method.owner_name);
        if (inherited_owner_it != surface.interfaces.end()) {
          if (inherited_method.method->objc_final_declared) {
            diagnostics.push_back(MakeDiag(
                method_decl.line, method_decl.column, "O3S309",
                "dispatch-control semantics failed: selector '" +
                    FormatMethodSelectorForDiagnostic(
                        selector_contract.normalized_selector,
                        method_it->second.is_class_method) +
                    "' in interface '" + interface_decl->name +
                    "' cannot override objc_final superclass method in '" +
                    inherited_method.owner_name + "'"));
          }
          if (method_it->second.effective_direct_dispatch ||
              IsDispatchEffectivelyDirectMethod(*inherited_method.method,
                                             inherited_owner_it->second)) {
            diagnostics.push_back(MakeDiag(
                method_decl.line, method_decl.column, "O3S310",
                "dispatch-control semantics failed: selector '" +
                    FormatMethodSelectorForDiagnostic(
                        selector_contract.normalized_selector,
                        method_it->second.is_class_method) +
                    "' in interface '" + interface_decl->name +
                    "' cannot participate in an override chain that uses objc_direct dispatch"));
          }
        }
      }
    }

    for (const auto &property_decl : interface_decl->properties) {
      const auto property_it =
          interface_it->second.properties.find(property_decl.name);
      if (property_it == interface_it->second.properties.end()) {
        continue;
      }
      const Objc3InheritedPropertyLookup inherited_property =
          FindSurfacePropertyInSuperChainDetailed(surface,
                                                  implementation_decl.name,
                                                  property_decl.name);
      if (inherited_property.property != nullptr &&
          !IsCompatiblePropertySignature(*inherited_property.property,
                                         property_it->second)) {
        diagnostics.push_back(MakeDiag(
            property_decl.line, property_decl.column, "O3S220",
            "runtime realization failed: incompatible inherited property '" +
                property_decl.name + "' in interface '" +
                interface_decl->name + "' relative to superclass '" +
                inherited_property.owner_name + "'"));
      }
    }
  }
}

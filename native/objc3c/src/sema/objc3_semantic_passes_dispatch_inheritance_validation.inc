                                is_class_method);
}

static const Objc3PropertyInfo *ResolvePropertyInOwnerInterface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  const auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it == surface.interfaces.end()) {
    return nullptr;
  }
  const auto property_it = interface_it->second.properties.find(property_name);
  if (property_it == interface_it->second.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *ResolvePropertyInOwnerImplementation(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  const auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it == surface.implementations.end()) {
    return nullptr;
  }
  const auto property_it =
      implementation_it->second.properties.find(property_name);
  if (property_it == implementation_it->second.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *ResolvePropertyInCategoryInterface(
    const std::unordered_map<std::string, Objc3InterfaceInfo>
        &category_interfaces,
    const std::string &owner_identity,
    const std::string &property_name) {
  const auto interface_it = category_interfaces.find(owner_identity);
  if (interface_it == category_interfaces.end()) {
    return nullptr;
  }
  const auto property_it = interface_it->second.properties.find(property_name);
  if (property_it == interface_it->second.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *ResolvePropertyInCategoryImplementation(
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    const std::string &owner_identity,
    const std::string &property_name) {
  const auto implementation_it = category_implementations.find(owner_identity);
  if (implementation_it == category_implementations.end()) {
    return nullptr;
  }
  const auto property_it =
      implementation_it->second.properties.find(property_name);
  if (property_it == implementation_it->second.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *ResolvePropertyInMergedCategorySurface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  const auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it == surface.category_merge_surfaces.end()) {
    return nullptr;
  }
  const auto property_it =
      merge_it->second.merged_properties.find(property_name);
  if (property_it == merge_it->second.merged_properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *FindSurfacePropertyInSuperChain(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &property_name,
    bool &missing_base,
    bool &cycle_detected) {
  missing_base = false;
  cycle_detected = false;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    missing_base = true;
    return nullptr;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      cycle_detected = true;
      return nullptr;
    }
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      missing_base = true;
      return nullptr;
    }
    const auto property_it = super_it->second.properties.find(property_name);
    if (property_it != super_it->second.properties.end()) {
      return &property_it->second;
    }
    next_super = super_it->second.super_name;
  }
  return nullptr;
}

struct Objc3InheritedMethodLookup {
  const Objc3MethodInfo *method = nullptr;
  std::string owner_name;
  bool missing_base = false;
  bool cycle_detected = false;
};

static Objc3InheritedMethodLookup FindSurfaceMethodInSuperChainDetailed(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &selector,
    bool match_method_kind,
    bool is_class_method) {
  Objc3InheritedMethodLookup lookup;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    lookup.missing_base = true;
    return lookup;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      lookup.cycle_detected = true;
      return lookup;
    }
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      lookup.missing_base = true;
      lookup.owner_name = next_super;
      return lookup;
    }
    const auto method_it = super_it->second.methods.find(selector);
    if (method_it != super_it->second.methods.end() &&
        (!match_method_kind ||
         method_it->second.is_class_method == is_class_method)) {
      lookup.method = &method_it->second;
      lookup.owner_name = next_super;
      return lookup;
    }
    next_super = super_it->second.super_name;
  }
  return lookup;
}

struct Objc3InheritedPropertyLookup {
  const Objc3PropertyInfo *property = nullptr;
  std::string owner_name;
  bool missing_base = false;
  bool cycle_detected = false;
};

static Objc3InheritedPropertyLookup FindSurfacePropertyInSuperChainDetailed(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &property_name) {
  Objc3InheritedPropertyLookup lookup;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    lookup.missing_base = true;
    return lookup;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      lookup.cycle_detected = true;
      return lookup;
    }
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      lookup.missing_base = true;
      lookup.owner_name = next_super;
      return lookup;
    }
    const auto property_it = super_it->second.properties.find(property_name);
    if (property_it != super_it->second.properties.end()) {
      lookup.property = &property_it->second;
      lookup.owner_name = next_super;
      return lookup;
    }
    next_super = super_it->second.super_name;
  }
  return lookup;
}

struct Objc3SuperclassRealizationClosure {
  bool missing_super_interface = false;
  bool missing_super_implementation = false;
  bool cycle_detected = false;
  std::string failing_super_name;
};

static Objc3SuperclassRealizationClosure ResolveSuperclassRealizationClosure(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name) {
  Objc3SuperclassRealizationClosure closure;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    return closure;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      closure.cycle_detected = true;
      closure.failing_super_name = next_super;
      return closure;
    }
    const auto super_interface_it = surface.interfaces.find(next_super);
    if (super_interface_it == surface.interfaces.end()) {
      closure.missing_super_interface = true;
      closure.failing_super_name = next_super;
      return closure;
    }
    if (surface.implementations.find(next_super) == surface.implementations.end()) {
      closure.missing_super_implementation = true;
      closure.failing_super_name = next_super;
      return closure;
    }
    next_super = super_interface_it->second.super_name;
  }
  return closure;
}

static bool IsDispatchEffectivelyDirectMethod(const Objc3MethodInfo &method,
                                           const Objc3InterfaceInfo &owner) {
  return method.objc_direct_declared ||
         (owner.objc_direct_members_declared && !method.objc_dynamic_declared);
}

template <typename CallableDeclT>
static bool HasDispatchCallableDispatchIntentAttributes(
    const CallableDeclT &decl) {
  return decl.objc_direct_declared || decl.objc_final_declared ||
         decl.objc_dynamic_declared;
}

static bool HasDispatchContainerDispatchIntentAttributes(
    const Objc3InterfaceDecl &decl) {
  return decl.objc_direct_members_declared || decl.objc_final_declared ||
         decl.objc_sealed_declared;
}

template <typename CallableDeclT>
static void DiagnoseDispatchCallableDispatchIntentConflicts(
    const CallableDeclT &decl, const std::string &callable_label,
    std::vector<std::string> &diagnostics) {
  if (decl.objc_direct_declared && decl.objc_dynamic_declared) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S311",
        "dispatch-control semantics failed: " + callable_label +
            " cannot combine objc_direct and objc_dynamic"));
  }
  if (decl.objc_final_declared && decl.objc_dynamic_declared) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S312",
        "dispatch-control semantics failed: " + callable_label +
            " cannot combine objc_final and objc_dynamic"));
  }
}

static void ValidateDispatchDispatchIntentCompatibility(
    const Objc3Program &ast, std::vector<std::string> &diagnostics) {
  auto format_method_label =
      [](const Objc3MethodDecl &method, const std::string &owner_kind,
         const std::string &owner_name) {
        return "selector '" +
               FormatMethodSelectorForDiagnostic(
                   BuildMethodSelectorNormalizationContractInfo(method)
                       .normalized_selector,
                   method.is_class_method) +
               "' in " + owner_kind + " '" + owner_name + "'";
      };

  for (const auto &fn : ast.functions) {
    if (!HasDispatchCallableDispatchIntentAttributes(fn)) {
      continue;
    }
    DiagnoseDispatchCallableDispatchIntentConflicts(
        fn, "function '" + fn.name + "'", diagnostics);
    diagnostics.push_back(MakeDiag(
        fn.line, fn.column, "O3S313",
        "dispatch-control semantics failed: function '" + fn.name +
            "' cannot use Part 9 dispatch-control callable attributes outside Objective-C methods"));
  }

  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &method : protocol_decl.methods) {
      if (!HasDispatchCallableDispatchIntentAttributes(method)) {
        continue;
      }
      const std::string method_label =
          format_method_label(method, "protocol", protocol_decl.name);
      DiagnoseDispatchCallableDispatchIntentConflicts(method, method_label,
                                                   diagnostics);
      diagnostics.push_back(MakeDiag(
          method.line, method.column, "O3S314",
          "dispatch-control semantics failed: " + method_label +
              " cannot use Part 9 dispatch-control callable attributes"));
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    const std::string category_identity = interface_decl.name + "(" +
                                          interface_decl.category_name + ")";
    if (interface_decl.has_category &&
        HasDispatchContainerDispatchIntentAttributes(interface_decl)) {
      diagnostics.push_back(MakeDiag(
          interface_decl.line, interface_decl.column, "O3S316",
          "dispatch-control semantics failed: category '" +
              category_identity +
              "' cannot use objc_direct_members, objc_final, or objc_sealed container attributes"));
    }

    if (!interface_decl.has_category) {
      continue;
    }
    for (const auto &method : interface_decl.methods) {
      if (!HasDispatchCallableDispatchIntentAttributes(method)) {
        continue;
      }
      const std::string method_label =
          format_method_label(method, "category", category_identity);
      DiagnoseDispatchCallableDispatchIntentConflicts(method, method_label,
                                                   diagnostics);
      diagnostics.push_back(MakeDiag(
          method.line, method.column, "O3S315",
          "dispatch-control semantics failed: " + method_label +
              " cannot use Part 9 dispatch-control callable attributes"));
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    if (!implementation_decl.has_category) {
      for (const auto &method : implementation_decl.methods) {
        if (!HasDispatchCallableDispatchIntentAttributes(method)) {
          continue;
        }
        DiagnoseDispatchCallableDispatchIntentConflicts(
            method,
            format_method_label(method, "implementation",
                                implementation_decl.name),
            diagnostics);
      }
      continue;
    }

    const std::string category_identity = implementation_decl.name + "(" +
                                          implementation_decl.category_name + ")";
    for (const auto &method : implementation_decl.methods) {
      if (!HasDispatchCallableDispatchIntentAttributes(method)) {
        continue;
      }
      const std::string method_label =
          format_method_label(method, "category", category_identity);
      DiagnoseDispatchCallableDispatchIntentConflicts(method, method_label,
                                                   diagnostics);
      diagnostics.push_back(MakeDiag(
          method.line, method.column, "O3S315",
          "dispatch-control semantics failed: " + method_label +
              " cannot use Part 9 dispatch-control callable attributes"));
    }
  }
}

static void ValidateInteropInteropRuntimeParity(
    const Objc3Program &ast, std::vector<std::string> &diagnostics) {
  const auto format_function_label = [](const FunctionDecl &fn) {
    return "function '" + fn.name + "'";
  };
  const auto format_method_label =
      [](const Objc3MethodDecl &method, const std::string &owner_kind,
         const std::string &owner_name) {
        return "selector '" +
               FormatMethodSelectorForDiagnostic(method.selector,
                                                method.is_class_method) +
               "' in " + owner_kind + " '" + owner_name + "'";
      };
  const auto diagnose_import_requires_foreign =
      [&diagnostics](unsigned line, unsigned column,
                     const std::string &callable_label) {
        diagnostics.push_back(MakeDiag(
            line, column, "O3S332",
            "interop semantics failed: " + callable_label +
                " cannot use objc_import_module without objc_foreign"));
      };

  for (const auto &fn : ast.functions) {
    if (fn.objc_import_module_declared && !fn.objc_foreign_declared) {
      diagnose_import_requires_foreign(fn.line, fn.column,
                                       format_function_label(fn));
      continue;
    }
    if (fn.objc_foreign_declared && !fn.is_prototype) {
      diagnostics.push_back(MakeDiag(
          fn.line, fn.column, "O3S331",
          "interop semantics failed: " + format_function_label(fn) +
              " must use a declaration-only extern surface under objc_foreign"));
    }
  }

  const auto validate_declaration_method =
      [&diagnose_import_requires_foreign, &format_method_label](
          const Objc3MethodDecl &method_decl, const std::string &owner_kind,
          const std::string &owner_name) {
        if (method_decl.objc_import_module_declared &&
            !method_decl.objc_foreign_declared) {
          diagnose_import_requires_foreign(method_decl.line, method_decl.column,
                                           format_method_label(method_decl,
                                                               owner_kind,
                                                               owner_name));
        }
      };

  for (const auto &interface_decl : ast.interfaces) {
    const std::string owner_name = interface_decl.has_category
                                       ? interface_decl.name + "(" +
                                             interface_decl.category_name + ")"
                                       : interface_decl.name;
    const std::string owner_kind =
        interface_decl.has_category ? "category interface" : "interface";
    for (const auto &method_decl : interface_decl.methods) {
      validate_declaration_method(method_decl, owner_kind, owner_name);
    }
  }

  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      validate_declaration_method(method_decl, "protocol", protocol_decl.name);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    const std::string owner_name = implementation_decl.has_category
                                       ? implementation_decl.name + "(" +
                                             implementation_decl.category_name + ")"
                                       : implementation_decl.name;
    const std::string owner_kind =
        implementation_decl.has_category ? "category implementation"
                                         : "implementation";
    for (const auto &method_decl : implementation_decl.methods) {
      if (!HasInteropForeignOrImportAnnotations(method_decl)) {
        continue;
      }
      diagnostics.push_back(MakeDiag(
          method_decl.line, method_decl.column, "O3S333",
          "interop semantics failed: " +
              format_method_label(method_decl, owner_kind, owner_name) +
              " cannot use Part 11 foreign/import callable annotations on an implementation surface"));
    }
  }
}

static void ValidateInteropCppInteropInteractions(
    const Objc3Program &ast, std::vector<std::string> &diagnostics) {
  const auto format_function_label = [](const FunctionDecl &fn) {
    return "function '" + fn.name + "'";
  };
  const auto format_method_label =
      [](const Objc3MethodDecl &method, const std::string &owner_kind,
         const std::string &owner_name) {
        return "selector '" +
               FormatMethodSelectorForDiagnostic(method.selector,
                                                method.is_class_method) +
               "' in " + owner_kind + " '" + owner_name + "'";
      };
  const auto diagnose_cpp_interaction =
      [&diagnostics](unsigned line, unsigned column,
                     const std::string &callable_label, const char *code,
                     const char *message) {
        diagnostics.push_back(
            MakeDiag(line, column, code,
                     "interop semantics failed: " + callable_label + " " +
                         message));
      };

  const auto validate_callable = [&diagnose_cpp_interaction](const auto &decl,
                                                             unsigned line,
                                                             unsigned column,
                                                             const std::string &callable_label) {
    if (!HasInteropCppInteropAnnotations(decl)) {
      return;
    }
    if (HasInteropOwnershipInteractionSurface(decl)) {
      diagnose_cpp_interaction(
          line, column, callable_label, "O3S334",
          "cannot currently combine C++-facing interop annotations with ownership-managed callable surfaces");
    }
    if (decl.throws_declared) {
      diagnose_cpp_interaction(
          line, column, callable_label, "O3S335",
          "cannot currently combine C++-facing interop annotations with throws");
    }
    if (HasInteropAsyncInteractionSurface(decl)) {
      diagnose_cpp_interaction(
          line, column, callable_label, "O3S336",
          "cannot currently combine C++-facing interop annotations with async or objc_executor affinity");
    }
  };

  for (const auto &fn : ast.functions) {
    validate_callable(fn, fn.line, fn.column, format_function_label(fn));
  }

  for (const auto &interface_decl : ast.interfaces) {
    const std::string owner_name = interface_decl.has_category
                                       ? interface_decl.name + "(" +
                                             interface_decl.category_name + ")"
                                       : interface_decl.name;
    const std::string owner_kind =
        interface_decl.has_category ? "category interface" : "interface";
    for (const auto &method_decl : interface_decl.methods) {
      validate_callable(method_decl, method_decl.line, method_decl.column,
                        format_method_label(method_decl, owner_kind,
                                            owner_name));
    }
  }

  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      validate_callable(method_decl, method_decl.line, method_decl.column,
                        format_method_label(method_decl, "protocol",
                                            protocol_decl.name));
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    const std::string owner_name = implementation_decl.has_category
                                       ? implementation_decl.name + "(" +
                                             implementation_decl.category_name + ")"
                                       : implementation_decl.name;
    const std::string owner_kind =
        implementation_decl.has_category ? "category implementation"
                                         : "implementation";
    for (const auto &method_decl : implementation_decl.methods) {
      validate_callable(method_decl, method_decl.line, method_decl.column,
                        format_method_label(method_decl, owner_kind,
                                            owner_name));
    }
  }
}

static void ValidateInteropSwiftInteropIsolation(
    const Objc3Program &ast, std::vector<std::string> &diagnostics) {
  const auto format_function_label = [](const FunctionDecl &fn) {
    return "function '" + fn.name + "'";
  };
  const auto format_method_label =
      [](const Objc3MethodDecl &method, const std::string &owner_kind,
         const std::string &owner_name) {
        return "selector '" +
               FormatMethodSelectorForDiagnostic(method.selector,
                                                method.is_class_method) +
               "' in " + owner_kind + " '" + owner_name + "'";
      };
  const auto diagnose_swift_interaction =
      [&diagnostics](unsigned line, unsigned column,
                     const std::string &callable_label, const char *code,
                     const char *message) {
        diagnostics.push_back(
            MakeDiag(line, column, code,
                     "interop semantics failed: " + callable_label + " " +
                         message));
      };
  const auto validate_callable =
      [&diagnose_swift_interaction](const auto &decl, bool actor_owned,
                                    bool implementation_surface,
                                    unsigned line, unsigned column,
                                    const std::string &callable_label) {
        if (!HasInteropSwiftInteropAnnotations(decl)) {
          return;
        }
        if (decl.objc_swift_private_declared && !decl.objc_swift_name_declared) {
          diagnose_swift_interaction(
              line, column, callable_label, "O3S337",
              "cannot use objc_swift_private without objc_swift_name");
        }
        if (actor_owned) {
          diagnose_swift_interaction(
              line, column, callable_label, "O3S338",
              "cannot currently map Swift-facing metadata on actor-owned callable surfaces");
        }
        if (decl.objc_nonisolated_declared) {
          diagnose_swift_interaction(
              line, column, callable_label, "O3S339",
              "cannot currently map Swift-facing metadata on objc_nonisolated callable surfaces");
        }
        if (implementation_surface) {
          diagnose_swift_interaction(
              line, column, callable_label, "O3S340",
              "cannot currently publish Swift-facing metadata directly from implementation surfaces");
        }
      };
  const auto is_actor_interface =
      [&ast](const std::string &name) -> bool {
    const auto interface_it =
        std::find_if(ast.interfaces.begin(), ast.interfaces.end(),
                     [&name](const Objc3InterfaceDecl &interface_decl) {
                       return !interface_decl.has_category &&
                              interface_decl.name == name &&
                              interface_decl.is_actor;
                     });
    return interface_it != ast.interfaces.end();
  };

  for (const auto &fn : ast.functions) {
    validate_callable(fn, false, false, fn.line, fn.column, format_function_label(fn));
  }
  for (const auto &interface_decl : ast.interfaces) {
    const std::string owner_name = interface_decl.has_category
                                       ? interface_decl.name + "(" +
                                             interface_decl.category_name + ")"
                                       : interface_decl.name;
    const std::string owner_kind =
        interface_decl.has_category ? "category interface" : "interface";
    for (const auto &method_decl : interface_decl.methods) {
      validate_callable(method_decl, interface_decl.is_actor, false,
                        method_decl.line, method_decl.column,
                        format_method_label(method_decl, owner_kind,
                                            owner_name));
    }
  }
  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      validate_callable(method_decl, false, false, method_decl.line, method_decl.column,
                        format_method_label(method_decl, "protocol",
                                            protocol_decl.name));
    }
  }
  for (const auto &implementation_decl : ast.implementations) {
    const bool actor_owned =
        !implementation_decl.has_category &&
        is_actor_interface(implementation_decl.name);
    const std::string owner_name = implementation_decl.has_category
                                       ? implementation_decl.name + "(" +
                                             implementation_decl.category_name + ")"
                                       : implementation_decl.name;
    const std::string owner_kind =
        implementation_decl.has_category ? "category implementation"
                                         : "implementation";
    for (const auto &method_decl : implementation_decl.methods) {
      validate_callable(method_decl, actor_owned, true, method_decl.line,
                        method_decl.column,
                        format_method_label(method_decl, owner_kind,
                                            owner_name));
    }
  }
}

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

static const Objc3MethodInfo *ResolveDeclaredConformanceMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const Objc3MethodInfo *method =
      ResolveMethodInOwnerInterface(surface, owner_name, selector, is_class_method);
  if (method != nullptr) {
    return method;
  }
  method =
      ResolveMethodInOwnerImplementation(surface, owner_name, selector, is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInMergedCategorySurface(surface, owner_name, selector,
                                                is_class_method);
  if (method != nullptr) {
    return method;
  }
  bool missing_base = false;
  bool cycle_detected = false;
  return FindSurfaceMethodInSuperChain(surface, owner_name, selector,
                                       is_class_method, missing_base,
                                       cycle_detected);
}

static const Objc3MethodInfo *ResolveCategoryConformanceMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::unordered_map<std::string, Objc3InterfaceInfo> &category_interfaces,
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    const std::string &class_name,
    const std::string &category_name,
    const std::string &selector,
    bool is_class_method) {
  const std::string category_owner_identity =
      BuildCategoryOwnerIdentity(class_name, category_name);
  const Objc3MethodInfo *method = ResolveMethodInCategoryInterface(
      category_interfaces, category_owner_identity, selector, is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInCategoryImplementation(category_implementations,
                                                 category_owner_identity,
                                                 selector, is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInOwnerInterface(surface, class_name, selector,
                                         is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInOwnerImplementation(surface, class_name, selector,
                                              is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInMergedCategorySurface(surface, class_name, selector,
                                                is_class_method);
  if (method != nullptr) {
    return method;
  }
  bool missing_base = false;
  bool cycle_detected = false;
  return FindSurfaceMethodInSuperChain(surface, class_name, selector,
                                       is_class_method, missing_base,
                                       cycle_detected);
}

static const Objc3PropertyInfo *ResolveDeclaredConformanceProperty(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  const Objc3PropertyInfo *property =
      ResolvePropertyInOwnerInterface(surface, owner_name, property_name);
  if (property != nullptr) {
    return property;
  }
  property =
      ResolvePropertyInOwnerImplementation(surface, owner_name, property_name);
  if (property != nullptr) {
    return property;
  }
  property =
      ResolvePropertyInMergedCategorySurface(surface, owner_name, property_name);
  if (property != nullptr) {
    return property;
  }
  bool missing_base = false;
  bool cycle_detected = false;
  return FindSurfacePropertyInSuperChain(surface, owner_name, property_name,
                                         missing_base, cycle_detected);
}

static Objc3ResolvedMessageSendPropertyAccessor
ResolveConcreteOwnerPropertyAccessor(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_property) {
  Objc3ResolvedMessageSendPropertyAccessor resolution;
  auto consider = [&](const Objc3PropertyInfo *property) {
    if (property == nullptr || property->is_class != is_class_property) {
      return false;
    }
    if (property->effective_getter_selector == selector) {
      resolution.property = property;
      resolution.is_setter = false;
      return true;
    }
    if (property->effective_setter_available &&
        property->effective_setter_selector == selector) {
      resolution.property = property;
      resolution.is_setter = true;
      return true;
    }
    return false;
  };

  const auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it != surface.interfaces.end()) {
    for (const auto &entry : interface_it->second.properties) {
      if (consider(&entry.second)) {
        return resolution;
      }
    }
  }
  const auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it != surface.implementations.end()) {
    for (const auto &entry : implementation_it->second.properties) {
      if (consider(&entry.second)) {
        return resolution;
      }
    }
  }
  const auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it != surface.category_merge_surfaces.end()) {
    for (const auto &entry : merge_it->second.merged_properties) {
      if (consider(&entry.second)) {
        return resolution;
      }
    }
  }
  const auto owner_it = surface.interfaces.find(owner_name);
  if (owner_it == surface.interfaces.end()) {
    return resolution;
  }
  std::string next_super = owner_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty() && visited.insert(next_super).second) {
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      return resolution;
    }
    for (const auto &entry : super_it->second.properties) {
      if (consider(&entry.second)) {
        return resolution;
      }
    }
    next_super = super_it->second.super_name;
  }
  return resolution;
}

static const Objc3PropertyInfo *ResolveCategoryConformanceProperty(
    const Objc3SemanticIntegrationSurface &surface,
    const std::unordered_map<std::string, Objc3InterfaceInfo> &category_interfaces,
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    const std::string &class_name,
    const std::string &category_name,
    const std::string &property_name) {
  const std::string category_owner_identity =
      BuildCategoryOwnerIdentity(class_name, category_name);
  const Objc3PropertyInfo *property = ResolvePropertyInCategoryInterface(
      category_interfaces, category_owner_identity, property_name);
  if (property != nullptr) {
    return property;
  }
  property = ResolvePropertyInCategoryImplementation(category_implementations,
                                                     category_owner_identity,
                                                     property_name);
  if (property != nullptr) {
    return property;
  }
  property = ResolvePropertyInOwnerInterface(surface, class_name, property_name);
  if (property != nullptr) {
    return property;
  }
  property =
      ResolvePropertyInOwnerImplementation(surface, class_name, property_name);
  if (property != nullptr) {
    return property;
  }
  property = ResolvePropertyInMergedCategorySurface(surface, class_name,
                                                    property_name);
  if (property != nullptr) {
    return property;
  }
  bool missing_base = false;
  bool cycle_detected = false;
  return FindSurfacePropertyInSuperChain(surface, class_name, property_name,
                                         missing_base, cycle_detected);
}

static bool CollectProtocolRequirementClosure(
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &definitions,
    const std::string &protocol_name,
    std::unordered_set<std::string> &active_protocols,
    Objc3ProtocolRequirementClosure &closure) {
  const auto definition_it = definitions.find(protocol_name);
  if (definition_it == definitions.end()) {
    closure.status = Objc3ProtocolRequirementClosureStatus::MissingProtocol;
    closure.failing_protocol_name = protocol_name;
    return false;
  }
  const Objc3ProtocolSemanticDefinition &definition = definition_it->second;
  if (definition.is_forward_declaration) {
    closure.status = Objc3ProtocolRequirementClosureStatus::ForwardDeclaration;
    closure.failing_protocol_name = protocol_name;
    return false;
  }
  if (!active_protocols.insert(protocol_name).second) {
    closure.status = Objc3ProtocolRequirementClosureStatus::InheritanceCycle;
    closure.failing_protocol_name = protocol_name;
    return false;
  }

  for (const auto &inherited_protocol_name :
       definition.inherited_protocols_lexicographic) {
    if (!CollectProtocolRequirementClosure(definitions, inherited_protocol_name,
                                          active_protocols, closure)) {
      active_protocols.erase(protocol_name);
      return false;
    }
  }

  for (const auto &entry : definition.required_properties_by_name) {
    const std::string &property_name = entry.first;
    const Objc3ProtocolPropertyRequirementInfo &requirement = entry.second;
    const auto existing_it =
        closure.required_properties_by_name.find(property_name);
    if (existing_it != closure.required_properties_by_name.end()) {
      if (!IsCompatiblePropertySignature(existing_it->second->property,
                                         requirement.property)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.property_name;
        closure.conflicting_member_is_method = false;
        closure.conflicting_selector_is_class_method = false;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    const auto existing_optional_it =
        closure.optional_properties_by_name.find(property_name);
    if (existing_optional_it != closure.optional_properties_by_name.end() &&
        !IsCompatiblePropertySignature(existing_optional_it->second->property,
                                       requirement.property)) {
      closure.status = Objc3ProtocolRequirementClosureStatus::
          IncompatibleInheritedRequirement;
      closure.failing_protocol_name = protocol_name;
      closure.conflicting_member_name = requirement.property_name;
      closure.conflicting_member_is_method = false;
      closure.conflicting_selector_is_class_method = false;
      active_protocols.erase(protocol_name);
      return false;
    }
    closure.required_properties_by_name.emplace(property_name, &requirement);
  }

  for (const auto &entry : definition.optional_properties_by_name) {
    const std::string &property_name = entry.first;
    const Objc3ProtocolPropertyRequirementInfo &requirement = entry.second;
    const auto existing_required_it =
        closure.required_properties_by_name.find(property_name);
    if (existing_required_it != closure.required_properties_by_name.end()) {
      if (!IsCompatiblePropertySignature(existing_required_it->second->property,
                                         requirement.property)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.property_name;
        closure.conflicting_member_is_method = false;
        closure.conflicting_selector_is_class_method = false;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    const auto existing_optional_it =
        closure.optional_properties_by_name.find(property_name);
    if (existing_optional_it != closure.optional_properties_by_name.end()) {
      if (!IsCompatiblePropertySignature(existing_optional_it->second->property,
                                         requirement.property)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.property_name;
        closure.conflicting_member_is_method = false;
        closure.conflicting_selector_is_class_method = false;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    closure.optional_properties_by_name.emplace(property_name, &requirement);
  }

  for (const auto &entry : definition.required_methods_by_key) {
    const std::string &requirement_key = entry.first;
    const Objc3ProtocolRequirementInfo &requirement = entry.second;
    const auto existing_it =
        closure.required_methods_by_key.find(requirement_key);
    if (existing_it != closure.required_methods_by_key.end()) {
      if (!IsCompatibleMethodSignature(existing_it->second->method,
                                       requirement.method)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.selector;
        closure.conflicting_member_is_method = true;
        closure.conflicting_selector_is_class_method =
            requirement.method.is_class_method;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    const auto existing_optional_it =
        closure.optional_methods_by_key.find(requirement_key);
    if (existing_optional_it != closure.optional_methods_by_key.end() &&
        !IsCompatibleMethodSignature(existing_optional_it->second->method,
                                     requirement.method)) {
      closure.status = Objc3ProtocolRequirementClosureStatus::
          IncompatibleInheritedRequirement;
      closure.failing_protocol_name = protocol_name;
      closure.conflicting_member_name = requirement.selector;
      closure.conflicting_member_is_method = true;
      closure.conflicting_selector_is_class_method =
          requirement.method.is_class_method;
      active_protocols.erase(protocol_name);
      return false;
    }
    closure.required_methods_by_key.emplace(requirement_key, &requirement);
  }

  for (const auto &entry : definition.optional_methods_by_key) {
    const std::string &requirement_key = entry.first;
    const Objc3ProtocolRequirementInfo &requirement = entry.second;
    const auto existing_required_it =
        closure.required_methods_by_key.find(requirement_key);
    if (existing_required_it != closure.required_methods_by_key.end()) {
      if (!IsCompatibleMethodSignature(existing_required_it->second->method,
                                       requirement.method)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.selector;
        closure.conflicting_member_is_method = true;
        closure.conflicting_selector_is_class_method =
            requirement.method.is_class_method;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    const auto existing_optional_it =
        closure.optional_methods_by_key.find(requirement_key);
    if (existing_optional_it != closure.optional_methods_by_key.end()) {
      if (!IsCompatibleMethodSignature(existing_optional_it->second->method,
                                       requirement.method)) {
        closure.status = Objc3ProtocolRequirementClosureStatus::
            IncompatibleInheritedRequirement;
        closure.failing_protocol_name = protocol_name;
        closure.conflicting_member_name = requirement.selector;
        closure.conflicting_member_is_method = true;
        closure.conflicting_selector_is_class_method =
            requirement.method.is_class_method;
        active_protocols.erase(protocol_name);
        return false;
      }
      continue;
    }
    closure.optional_methods_by_key.emplace(requirement_key, &requirement);
  }

  active_protocols.erase(protocol_name);
  return true;
}

#include "sema/objc3_semantic_passes_protocol_message_resolution.inc"

static bool IsDispatchEffectivelyDirectMethod(const Objc3MethodInfo &method,
                                           const Objc3InterfaceInfo &owner) {
  return method.objc_direct_declared ||
         (owner.objc_direct_members_declared && !method.objc_dynamic_declared);
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

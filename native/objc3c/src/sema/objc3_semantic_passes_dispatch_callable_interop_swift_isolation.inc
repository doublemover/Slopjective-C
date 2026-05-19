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

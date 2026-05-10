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

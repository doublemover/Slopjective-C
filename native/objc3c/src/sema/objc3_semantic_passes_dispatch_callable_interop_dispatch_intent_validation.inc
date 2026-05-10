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

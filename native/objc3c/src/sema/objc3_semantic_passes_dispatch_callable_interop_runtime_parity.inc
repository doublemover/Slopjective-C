static void ValidateInteropInteropRuntimeParity(
    const Objc3Program &ast, std::vector<std::string> &diagnostics) {
  const auto has_unsafe_mixed_image_name = [](const std::string &name) {
    return name.empty() || name.find("..") != std::string::npos ||
           name.find('/') != std::string::npos ||
           name.find('\\') != std::string::npos ||
           name.find(':') != std::string::npos;
  };
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
  const auto diagnose_unsafe_mixed_image =
      [&diagnostics](unsigned line, unsigned column,
                     const std::string &callable_label,
                     const std::string &mixed_image_name) {
        diagnostics.push_back(MakeDiag(
            line == 0u ? 1u : line, column == 0u ? 1u : column, "O3S346",
            "package interop failed: " + callable_label +
                " uses unsafe objc_mixed_image name '" + mixed_image_name +
                "'; mixed-image names must be portable package image identifiers"));
      };

  for (const auto &fn : ast.functions) {
    if (fn.objc_import_module_declared && !fn.objc_foreign_declared) {
      diagnose_import_requires_foreign(fn.line, fn.column,
                                       format_function_label(fn));
      continue;
    }
    if (fn.objc_mixed_image_declared &&
        has_unsafe_mixed_image_name(fn.objc_mixed_image_name)) {
      diagnose_unsafe_mixed_image(fn.line, fn.column, format_function_label(fn),
                                  fn.objc_mixed_image_name);
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
      [&diagnose_import_requires_foreign, &diagnose_unsafe_mixed_image,
       &format_method_label, &has_unsafe_mixed_image_name](
          const Objc3MethodDecl &method_decl, const std::string &owner_kind,
          const std::string &owner_name) {
        if (method_decl.objc_import_module_declared &&
            !method_decl.objc_foreign_declared) {
          diagnose_import_requires_foreign(method_decl.line, method_decl.column,
                                           format_method_label(method_decl,
                                                               owner_kind,
                                                               owner_name));
        }
        if (method_decl.objc_mixed_image_declared &&
            has_unsafe_mixed_image_name(method_decl.objc_mixed_image_name)) {
          diagnose_unsafe_mixed_image(
              method_decl.line, method_decl.column,
              format_method_label(method_decl, owner_kind, owner_name),
              method_decl.objc_mixed_image_name);
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

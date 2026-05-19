  snapshot.token_count = token_count;
  snapshot.global_decl_count = ast.globals.size();
  snapshot.protocol_decl_count = ast.protocols.size();
  for (const auto &protocol_decl : ast.protocols) {
    snapshot.protocol_property_decl_count += protocol_decl.properties.size();
    snapshot.protocol_method_decl_count += protocol_decl.methods.size();
    for (const auto &method_decl : protocol_decl.methods) {
      if (method_decl.is_class_method) {
        ++snapshot.protocol_class_method_decl_count;
      } else {
        ++snapshot.protocol_instance_method_decl_count;
      }
    }
  }
  snapshot.interface_decl_count = ast.interfaces.size();
  for (const auto &interface_decl : ast.interfaces) {
    snapshot.interface_property_decl_count += interface_decl.properties.size();
    snapshot.interface_method_decl_count += interface_decl.methods.size();
    for (const auto &method_decl : interface_decl.methods) {
      if (method_decl.is_class_method) {
        ++snapshot.interface_class_method_decl_count;
      } else {
        ++snapshot.interface_instance_method_decl_count;
      }
    }
  }
  snapshot.implementation_decl_count = ast.implementations.size();
  for (const auto &implementation_decl : ast.implementations) {
    snapshot.implementation_property_decl_count += implementation_decl.properties.size();
    snapshot.implementation_method_decl_count += implementation_decl.methods.size();
    for (const auto &method_decl : implementation_decl.methods) {
      if (method_decl.is_class_method) {
        ++snapshot.implementation_class_method_decl_count;
      } else {
        ++snapshot.implementation_instance_method_decl_count;
      }
    }
  }
  snapshot.function_decl_count = ast.functions.size();
  snapshot.interface_category_decl_count = static_cast<std::size_t>(std::count_if(
      ast.interfaces.begin(),
      ast.interfaces.end(),
      [](const Objc3InterfaceDecl &interface_decl) { return interface_decl.has_category; }));
  snapshot.implementation_category_decl_count = static_cast<std::size_t>(std::count_if(
      ast.implementations.begin(),
      ast.implementations.end(),
      [](const Objc3ImplementationDecl &implementation_decl) { return implementation_decl.has_category; }));
  snapshot.function_prototype_count = static_cast<std::size_t>(std::count_if(
      ast.functions.begin(),
      ast.functions.end(),
      [](const FunctionDecl &function_decl) { return function_decl.is_prototype; }));
  snapshot.function_pure_count = static_cast<std::size_t>(std::count_if(
      ast.functions.begin(),
      ast.functions.end(),
      [](const FunctionDecl &function_decl) { return function_decl.is_pure; }));

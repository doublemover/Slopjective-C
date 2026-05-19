  void ParseObjcImplementationDeclPropertyMember(
      Objc3ImplementationDecl &decl) {
    Objc3PropertyDecl property;
    if (ParseObjcPropertyDecl(property)) {
      property.scope_owner_symbol = decl.scope_owner_symbol;
      property.scope_path_symbol =
          decl.scope_owner_symbol + "::" +
          BuildObjcPropertyScopePathSymbol(property);
      const std::string &storage_owner_symbol =
          decl.has_category ? decl.semantic_link_symbol
                            : decl.semantic_link_interface_symbol;
      AssignObjcExecutablePropertySourceModelFields(
          property, storage_owner_symbol, decl.properties.size(),
          !decl.has_category);
      decl.properties.push_back(std::move(property));
      return;
    }

    SynchronizeObjcContainer();
  }

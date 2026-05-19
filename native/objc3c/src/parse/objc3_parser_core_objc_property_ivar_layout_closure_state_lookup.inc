  Objc3InterfaceDecl *FindObjcInterfaceLayoutOwner(
      Objc3ParsedProgram &program, const std::string &name) const {
    if (name.empty()) {
      return nullptr;
    }
    for (auto &decl : MutableObjc3ParsedProgramAst(program).interfaces) {
      if (!decl.has_category && decl.name == name) {
        return &decl;
      }
    }
    return nullptr;
  }

  Objc3PropertyDecl *FindObjcPropertyDeclByName(
      std::vector<Objc3PropertyDecl> &properties,
      const std::string &name) const {
    for (auto &property : properties) {
      if (property.name == name) {
        return &property;
      }
    }
    return nullptr;
  }

  void FinalizeObjcProgramPropertyIvarLayoutClosure(
      Objc3ParsedProgram &program) {
    std::unordered_set<std::string> visiting;
    std::unordered_set<std::string> completed;
    for (auto &decl : MutableObjc3ParsedProgramAst(program).interfaces) {
      FinalizeObjcInterfacePropertyIvarLayoutClosure(program, decl, visiting,
                                                     completed);
    }

    for (auto &implementation :
         MutableObjc3ParsedProgramAst(program).implementations) {
      Objc3InterfaceDecl *interface_decl =
          FindObjcInterfaceLayoutOwner(program, implementation.name);
      if (implementation.has_category || interface_decl == nullptr) {
        for (auto &property : implementation.properties) {
          ClearObjcPropertyIvarLayoutClosure(property);
        }
      } else {
        for (auto &property : implementation.properties) {
          Objc3PropertyDecl *interface_property =
              FindObjcPropertyDeclByName(interface_decl->properties,
                                         property.name);
          if (interface_property == nullptr) {
            continue;
          }
          CopyObjcIvarLayoutClosure(property, *interface_property);
        }
      }
      FinalizeObjcPropertySynthesisIvarBindingPackets(
          implementation.properties,
          implementation.property_synthesis_symbols_lexicographic,
          implementation.ivar_binding_symbols_lexicographic);
    }
  }

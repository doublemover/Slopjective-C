    if (decl.has_category) {
      for (auto &property : decl.properties) {
        ClearObjcPropertyIvarLayoutClosure(property);
      }
      FinalizeObjcPropertySynthesisIvarBindingPackets(
          decl.properties, decl.property_synthesis_symbols_lexicographic,
          decl.ivar_binding_symbols_lexicographic);
      return true;
    }

    if (completed.count(decl.name) != 0u) {
      return true;
    }
    if (visiting.count(decl.name) != 0u) {
      diagnostics_.push_back(MakeDiag(
          decl.line, decl.column, "O3P150",
          "cyclic Objective-C interface inheritance cannot produce a stable ivar layout"));
      for (auto &property : decl.properties) {
        ClearObjcPropertyIvarLayoutClosure(property);
      }
      completed.insert(decl.name);
      return false;
    }

    visiting.insert(decl.name);

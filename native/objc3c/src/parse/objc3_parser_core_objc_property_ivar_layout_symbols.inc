  void AssignObjcPropertySynthesisIvarBindingSymbols(
      Objc3PropertyDecl &property,
      const std::string &synthesis_owner_symbol) {
    property.property_synthesis_symbol =
        synthesis_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property);
    property.ivar_binding_symbol =
        synthesis_owner_symbol + "::" + BuildObjcIvarBindingSymbol(property);
  }

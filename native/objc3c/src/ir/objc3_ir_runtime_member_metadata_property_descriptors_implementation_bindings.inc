static bool ResolveObjc3IRRuntimePropertyImplementationSymbols(
    const std::unordered_map<std::string, std::string>
        &implementation_method_symbols_by_owner_identity,
    const Objc3IRRuntimeMetadataPropertyBundle &bundle,
    Objc3IRRuntimePropertyImplementationSymbols &symbols,
    std::string &error) {
  if (!Objc3IRRuntimeMetadataPropertyBundleIsImplementationOwned(bundle)) {
    return true;
  }

  const auto getter_implementation_it =
      implementation_method_symbols_by_owner_identity.find(
          BuildSynthesizedInstanceMethodOwnerIdentity(
              bundle.declaration_owner_identity,
              bundle.effective_getter_selector));
  if (getter_implementation_it ==
      implementation_method_symbols_by_owner_identity.end()) {
    error = "missing synthesized accessor getter binding for property '" +
            bundle.property_name + "' in owner '" +
            bundle.declaration_owner_identity + "'";
    return false;
  }
  symbols.getter_implementation_symbol = getter_implementation_it->second;

  if (bundle.effective_setter_available) {
    const auto setter_implementation_it =
        implementation_method_symbols_by_owner_identity.find(
            BuildSynthesizedInstanceMethodOwnerIdentity(
                bundle.declaration_owner_identity,
                bundle.effective_setter_selector));
    if (setter_implementation_it ==
        implementation_method_symbols_by_owner_identity.end()) {
      error = "missing synthesized accessor setter binding for property '" +
              bundle.property_name + "' in owner '" +
              bundle.declaration_owner_identity + "'";
      return false;
    }
    symbols.setter_implementation_symbol = setter_implementation_it->second;
  }
  return true;
}

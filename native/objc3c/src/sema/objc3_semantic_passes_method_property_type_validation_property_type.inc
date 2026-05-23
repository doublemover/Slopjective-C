static void ValidatePropertyTypeSuffixes(const Objc3PropertyDecl &property,
                                         const std::string &owner_name,
                                         const std::string &owner_kind,
                                         std::vector<std::string> &diagnostics,
                                         const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  if (property.has_generic_suffix && !SupportsGenericPropertyTypeSuffix(property)) {
    std::string suffix = property.generic_suffix_text;
    if (suffix.empty()) {
      suffix = "<...>";
    }
    diagnostics.push_back(MakeDiag(property.generic_line,
                                   property.generic_column,
                                   "O3S206",
                                   "type mismatch: generic property type suffix '" + suffix +
                                       "' is unsupported for property '" + property.name + "' in " + owner_kind +
                                       " '" + owner_name + "'"));
  } else if (property.has_generic_suffix &&
             !ValidateGenericCollectionSpecializationSuffix(
                 property.object_pointer_type_spelling,
                 property.object_pointer_type_name,
                 property.generic_suffix_text, property.generic_line,
                 property.generic_column,
                 "property '" + property.name + "' in " + owner_kind + " '" +
                     owner_name + "' type annotation",
                 diagnostics)) {
    ValidateProtocolCompositionSuffix(property.generic_suffix_text,
                                     property.generic_line,
                                     property.generic_column,
                                     "property '" + property.name + "' in " + owner_kind + " '" + owner_name +
                                         "' type annotation",
                                     IsGenericInterfaceSpecializationSuffix(
                                         property.object_pointer_type_spelling,
                                         property.object_pointer_type_name,
                                         generic_interface_names),
                                     diagnostics);
  }
  if (!SupportsPointerPropertyTypeDeclarator(property)) {
    for (const auto &token : property.pointer_declarator_tokens) {
      diagnostics.push_back(MakeDiag(token.line,
                                     token.column,
                                     "O3S206",
                                     "type mismatch: unsupported property type declarator '" + token.text +
                                         "' for property '" + property.name + "' in " + owner_kind + " '" +
                                         owner_name + "'"));
    }
  }
  if (!SupportsNullabilityPropertyTypeSuffix(property)) {
    for (const auto &token : property.nullability_suffix_tokens) {
      diagnostics.push_back(MakeDiag(token.line,
                                     token.column,
                                     "O3S206",
                                     "type mismatch: unsupported property type suffix '" + token.text +
                                         "' for property '" + property.name + "' in " + owner_kind + " '" +
                                         owner_name + "'"));
    }
  }
  if (!SupportsOwnershipQualifierPropertyTypeSuffix(property)) {
    for (const auto &token : property.ownership_qualifier_tokens) {
      diagnostics.push_back(MakeDiag(token.line,
                                     token.column,
                                     "O3S206",
                                     "type mismatch: unsupported property ownership qualifier '" + token.text +
                                         "' for property '" + property.name + "' in " + owner_kind + " '" +
                                         owner_name + "'"));
    }
  }
}

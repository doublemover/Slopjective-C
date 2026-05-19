std::string SanitizeMetaprogrammingArtifactToken(const std::string &text) {
  std::string out;
  out.reserve(text.size());
  for (unsigned char ch : text) {
    if (std::isalnum(ch) != 0) {
      out.push_back(static_cast<char>(ch));
    } else {
      out.push_back('_');
    }
  }
  if (out.empty()) {
    out = "anonymous";
  }
  return out;
}

std::string BuildMetaprogrammingRuntimeClassOwnerIdentity(
    const std::string &class_name) {
  return "class:" + class_name;
}

std::string BuildMetaprogrammingRuntimeCategoryOwnerIdentity(
    const std::string &class_name, const std::string &category_name) {
  return "category:" + class_name + "(" + category_name + ")";
}

std::string BuildMetaprogrammingCategoryOwnerName(
    const std::string &class_name,
    const std::string &category_name) {
  return class_name + "(" + category_name + ")";
}

std::string BuildMetaprogrammingDerivedMethodFunctionSymbol(
    const std::string &implementation_name, const std::string &selector) {
  return "objc3_method_" +
         SanitizeMetaprogrammingArtifactToken(implementation_name) +
         "_instance_" + SanitizeMetaprogrammingArtifactToken(selector);
}

std::string BuildMetaprogrammingMacroArtifactSymbol(
    const std::string &function_name, const std::string &macro_name) {
  return "objc3_metaprogramming_macro_artifact_" +
         SanitizeMetaprogrammingArtifactToken(function_name) + "_" +
         SanitizeMetaprogrammingArtifactToken(macro_name);
}

std::string BuildMetaprogrammingPropertyBehaviorArtifactSymbol(
    const std::string &owner_kind, const std::string &owner_name,
    const std::string &property_name, const std::string &behavior_name) {
  return "objc3_metaprogramming_property_behavior_" +
         SanitizeMetaprogrammingArtifactToken(owner_kind) + "_" +
         SanitizeMetaprogrammingArtifactToken(owner_name) + "_" +
         SanitizeMetaprogrammingArtifactToken(property_name) + "_" +
         SanitizeMetaprogrammingArtifactToken(behavior_name);
}

std::string NormalizeMetaprogrammingNamedStringToken(std::string token) {
  token.erase(std::remove_if(token.begin(), token.end(),
                             [](unsigned char ch) {
                               return std::isspace(ch) != 0;
                             }),
              token.end());
  if (token.size() >= 2u && token.front() == '"' && token.back() == '"') {
    token = token.substr(1u, token.size() - 2u);
  }
  return token;
}

std::string NormalizeMetaprogrammingPropertyBehaviorNameForEmission(
    std::string token) {
  token = NormalizeMetaprogrammingNamedStringToken(std::move(token));
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  if (token == "observed") {
    return "Observed";
  }
  if (token == "projected") {
    return "Projected";
  }
  return {};
}

std::string NormalizeMetaprogrammingDeriveNameForEmission(std::string token) {
  token = NormalizeMetaprogrammingNamedStringToken(std::move(token));
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  if (token == "equality" || token == "equatable") {
    return "Equality";
  }
  if (token == "hash") {
    return "Hash";
  }
  if (token == "debugdescription") {
    return "DebugDescription";
  }
  return {};
}

bool IsSupportedMetaprogrammingMacroPackageNameForEmission(
    std::string package_name) {
  package_name = NormalizeMetaprogrammingNamedStringToken(std::move(package_name));
  std::transform(package_name.begin(), package_name.end(), package_name.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  return package_name == "std.metaprogramming" ||
         package_name == "std.metaprogramming.trace";
}

bool IsSupportedMetaprogrammingMacroProvenanceNameForEmission(
    std::string provenance_name) {
  provenance_name =
      NormalizeMetaprogrammingNamedStringToken(std::move(provenance_name));
  std::transform(provenance_name.begin(), provenance_name.end(),
                 provenance_name.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  constexpr std::string_view prefix = "sha256:";
  if (provenance_name.size() < prefix.size() ||
      provenance_name.compare(0u, prefix.size(), prefix) != 0) {
    return false;
  }
  const std::string_view digest(provenance_name.c_str() + prefix.size(),
                                provenance_name.size() - prefix.size());
  return digest.size() >= 6u && digest.size() <= 64u &&
         std::all_of(digest.begin(), digest.end(), [](char c) {
           return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
         });
}

bool IsMetaprogrammingPropertyBehaviorObjectLike(
    const Objc3PropertyDecl &property) {
  return property.id_spelling || property.instancetype_spelling ||
         property.object_pointer_type_spelling ||
         property.type == ValueType::ObjCId ||
         property.type == ValueType::ObjCClass ||
         property.type == ValueType::ObjCInstancetype ||
         property.type == ValueType::ObjCObjectPtr;
}

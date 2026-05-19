static const Objc3PropertyInfo *FindPropertyInImplementationInfo(
    const Objc3ImplementationInfo &implementation_info,
    const std::string &property_name) {
  const auto property_it = implementation_info.properties.find(property_name);
  if (property_it == implementation_info.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3SemanticPropertyTypeMetadata *FindPropertyInImplementationMetadata(
    const Objc3SemanticImplementationTypeMetadata &metadata,
    const std::string &property_name) {
  for (const auto &property_metadata : metadata.properties_lexicographic) {
    if (property_metadata.name == property_name) {
      return &property_metadata;
    }
  }
  return nullptr;
}

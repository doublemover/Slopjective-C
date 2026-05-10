std::string BuildMethodFamilySemanticsSymbol(
    const std::string &method_family_name,
    bool returns_retained_result,
    bool returns_related_result) {
  std::ostringstream out;
  out << "method-family:name=" << method_family_name
      << ";returns-retained=" << (returns_retained_result ? "true" : "false")
      << ";returns-related=" << (returns_related_result ? "true" : "false");
  return out.str();
}

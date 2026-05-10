std::string BuildNormalizedObjcSelector(
    const std::vector<Objc3MethodDecl::SelectorPiece> &pieces) {
  std::string normalized;
  for (const auto &piece : pieces) {
    normalized += piece.keyword;
    if (piece.has_parameter) {
      normalized += ":";
    }
  }
  return normalized;
}

std::string BuildTypedKeyPathLiteralProfile(
    const std::string &root_name,
    bool root_is_self,
    const std::vector<std::string> &components) {
  std::ostringstream out;
  out << "typed-keypath:root=" << (root_is_self ? "self" : root_name)
      << ";components=";
  for (std::size_t i = 0; i < components.size(); ++i) {
    if (i != 0u) {
      out << ".";
    }
    out << components[i];
  }
  return out.str();
}

std::string BuildAutoreleasePoolScopeSymbol(unsigned serial, unsigned depth) {
  std::ostringstream out;
  out << "autoreleasepool-scope:" << serial << ";depth=" << depth;
  return out.str();
}

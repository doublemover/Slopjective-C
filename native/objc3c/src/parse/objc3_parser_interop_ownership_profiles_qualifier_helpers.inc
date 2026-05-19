bool IsOwnershipQualifierSpelling(const std::string &text) {
  return text == "__strong" || text == "__weak" || text == "__autoreleasing" ||
         text == "__unsafe_unretained";
}

std::string BuildOwnershipQualifierSymbol(const std::string &spelling,
                                          bool is_return_type) {
  if (spelling.empty()) {
    return "";
  }
  return std::string(is_return_type ? "return-ownership-qualifier:"
                                    : "ownership-qualifier:") +
         spelling;
}

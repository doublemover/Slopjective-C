  bool AtIdentifierText(const char *text) const {
    return objc3c::parse::AtIdentifierText(tokens_, index_, text);
  }

  bool AtIdentifierTextOffset(std::size_t offset, const char *text) const {
    return objc3c::parse::AtIdentifierTextOffset(tokens_, index_, offset, text);
  }

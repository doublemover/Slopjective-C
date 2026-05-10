  bool ParseMacroCallableBridgeNamedStringPayload(
      const Token &attribute_name,
      const char *attribute_spelling,
      std::string &named_value_out) {
    return ParseNamedStringAttributePayload(
        attribute_name, attribute_spelling, named_value_out);
  }

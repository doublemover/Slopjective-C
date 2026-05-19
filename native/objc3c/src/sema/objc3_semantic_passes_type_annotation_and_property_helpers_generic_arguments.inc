static std::vector<std::string> ParseGenericArgumentSuffixTextSourceOrder(
    const std::string &suffix_text) {
  std::vector<std::string> arguments;
  if (suffix_text.size() < 2u || suffix_text.front() != '<' ||
      suffix_text.back() != '>') {
    return arguments;
  }
  const std::string inner = suffix_text.substr(1, suffix_text.size() - 2u);
  int depth = 0;
  std::size_t start = 0;
  for (std::size_t index = 0; index <= inner.size(); ++index) {
    const bool at_end = index == inner.size();
    const char c = at_end ? ',' : inner[index];
    if (!at_end) {
      if (c == '<') {
        ++depth;
      } else if (c == '>' && depth > 0) {
        --depth;
      }
    }
    if ((at_end || c == ',') && depth == 0) {
      const std::string token =
          objc3c::support::TrimAsciiWhitespace(
              inner.substr(start, index - start));
      if (!token.empty()) {
        arguments.push_back(token);
      }
      start = index + 1u;
    }
  }
  return arguments;
}

static std::vector<std::string> BuildGenericArgumentsLexicographic(
    const std::vector<std::string> &arguments_source_order) {
  std::vector<std::string> values = arguments_source_order;
  std::sort(values.begin(), values.end());
  return values;
}

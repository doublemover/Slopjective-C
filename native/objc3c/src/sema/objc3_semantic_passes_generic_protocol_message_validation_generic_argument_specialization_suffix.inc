    return false;
  }
  std::size_t cursor = object_name.size();
  while (cursor < trimmed.size() &&
         std::isspace(static_cast<unsigned char>(trimmed[cursor])) != 0) {
    ++cursor;
  }
  if (cursor >= trimmed.size() || trimmed[cursor] != '<') {
    return true;
  }
  int depth = 0;
  for (std::size_t index = cursor; index < trimmed.size(); ++index) {
    const char c = trimmed[index];
    if (c == '<') {
      ++depth;
    } else if (c == '>') {
      --depth;
      if (depth == 0) {
        generic_suffix_text = trimmed.substr(cursor, index - cursor + 1u);
        return true;
      }
      if (depth < 0) {
        return false;
      }
    }
  }
  return false;
}

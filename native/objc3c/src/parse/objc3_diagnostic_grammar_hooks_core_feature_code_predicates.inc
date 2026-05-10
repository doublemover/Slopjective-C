bool IsObjc3GrammarHookCode(const std::string &code) {
  if (!StartsWith(code, "O3P") || code.size() != 6u) {
    return false;
  }
  for (std::size_t i = 3u; i < code.size(); ++i) {
    const char c = code[i];
    if (c < '0' || c > '9') {
      return false;
    }
  }
  return true;
}

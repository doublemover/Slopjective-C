std::string DescribeParserDiagnosticToken(const Objc3LexToken &token) {
  if (token.kind == Objc3LexTokenKind::Eof) {
    return "end of file";
  }
  if (token.kind == Objc3LexTokenKind::Identifier) {
    return "identifier '" + token.text + "'";
  }
  if (token.kind == Objc3LexTokenKind::Number) {
    return "number '" + token.text + "'";
  }
  if (token.text.empty()) {
    return "token";
  }
  return "token '" + token.text + "'";
}

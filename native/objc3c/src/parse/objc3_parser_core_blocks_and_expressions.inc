  const std::vector<Token> &tokens_;
  std::size_t index_ = 0;
  std::vector<std::string> diagnostics_;
  std::vector<BlockLiteralSourceUseKind> block_literal_source_use_stack_;
  bool saw_module_declaration_ = false;
  bool block_failed_ = false;
  unsigned autoreleasepool_scope_depth_ = 0;
  unsigned autoreleasepool_scope_serial_ = 0;
  Objc3AstBuilder ast_builder_;
};

}  // namespace

Objc3ParserCoreResult ParseObjc3ProgramCore(const Objc3LexTokenStream &tokens) {
  Objc3Parser parser(tokens);
  Objc3ParserCoreResult result;
  result.program = parser.Parse();
  result.diagnostics = parser.TakeDiagnostics();
  return result;
}

}  // namespace objc3c::parse

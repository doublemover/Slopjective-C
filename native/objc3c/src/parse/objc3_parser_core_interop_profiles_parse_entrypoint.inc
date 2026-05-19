  Objc3ParsedProgram Parse() {
    Objc3ParsedProgram program = ast_builder_.BeginProgram();
    while (!At(TokenKind::Eof)) {
      ParseTopLevelDeclaration(program);
    }
    PublishProgramProfiles(program);
    return program;
  }

  std::vector<std::string> TakeDiagnostics() { return diagnostics_; }

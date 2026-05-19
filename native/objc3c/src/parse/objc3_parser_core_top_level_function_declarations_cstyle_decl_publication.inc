  void PublishTopLevelCStyleFunctionDecl(Objc3ParsedProgram &program,
                                         FunctionDecl &fn) {
    FinalizeTopLevelCStyleFunctionProfiles(fn);
    ast_builder_.AddFunctionDecl(program, std::move(fn));
  }

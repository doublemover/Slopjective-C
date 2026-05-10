  void ParseTopLevelCStyleFunctionDecl(Objc3ParsedProgram &program) {
    auto fn = std::make_unique<FunctionDecl>();

    if (!ParseTopLevelCStyleFunctionReturnType(*fn)) {
      SynchronizeTopLevel();
      return;
    }

    if (!ParseTopLevelCStyleFunctionNameAndScope(*fn)) {
      SynchronizeTopLevel();
      return;
    }

    if (!ParseTopLevelCStyleFunctionParameterList(*fn)) {
      SynchronizeTopLevel();
      return;
    }

    if (!ParseTopLevelCStyleFunctionPrototypeOrBody(program, *fn)) {
      SynchronizeTopLevel();
      return;
    }
  }

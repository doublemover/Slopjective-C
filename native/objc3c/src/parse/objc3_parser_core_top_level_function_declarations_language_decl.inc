  void ParseTopLevelFunctionDecl(Objc3ParsedProgram &program) {
    const std::optional<TopLevelFunctionQualifierState> qualifier_state =
        ParseTopLevelFunctionQualifierState();
    if (!qualifier_state.has_value()) {
      return;
    }

    auto fn = ParseFunction();
    if (fn == nullptr) {
      return;
    }

    fn->is_pure = qualifier_state->is_pure;
    fn->async_declared = qualifier_state->is_async;
    // ParseFunction finalizes semantic profiles before top-level qualifiers are
    // copied onto the declaration, so async callables need one final pass after
    // the real async bit is attached.
    FinalizeAsyncContinuationProfile(*fn);
    FinalizeAwaitSuspensionProfile(*fn);
    if (qualifier_state->is_extern && !fn->is_prototype) {
      diagnostics_.push_back(MakeDiag(fn->line, fn->column, "O3P104", "missing ';' after extern function declaration"));
      return;
    }

    ast_builder_.AddFunctionDecl(program, std::move(*fn));
  }

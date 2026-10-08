  void ParseTopLevelFunctionDecl(
      Objc3ParsedProgram &program,
      bool reification_marker_declared = false,
      unsigned reification_marker_line = 1,
      unsigned reification_marker_column = 1) {
    const std::optional<TopLevelFunctionQualifierState> qualifier_state =
        ParseTopLevelFunctionQualifierState();
    if (!qualifier_state.has_value()) {
      return;
    }

    auto fn = ParseFunction();
    if (fn == nullptr) {
      return;
    }

    fn->generic_callable_reification_marker_declared =
        reification_marker_declared;
    fn->generic_callable_reification_marker_line = reification_marker_line;
    fn->generic_callable_reification_marker_column = reification_marker_column;
    if (reification_marker_declared && fn->generic_params.empty()) {
      diagnostics_.push_back(MakeDiag(
          reification_marker_line, reification_marker_column, "O3P114",
          "declaration-scoped generic reification marker '@reify_generics' "
          "requires a generic callable declaration"));
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

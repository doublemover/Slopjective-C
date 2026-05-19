  std::unique_ptr<FunctionDecl> PublishFunctionPrototype(
      std::unique_ptr<FunctionDecl> fn,
      bool has_return_annotation) {
    fn->is_prototype = true;
    FinalizeFunctionSemanticProfiles(*fn, has_return_annotation);
    return fn;
  }

  std::unique_ptr<FunctionDecl> PublishFunctionBody(
      std::unique_ptr<FunctionDecl> fn,
      bool has_return_annotation) {
    fn->body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      SynchronizeTopLevel();
      return nullptr;
    }
    FinalizeFunctionSemanticProfiles(*fn, has_return_annotation);
    return fn;
  }

  std::unique_ptr<FunctionDecl> ParseFunctionTerminatorOrBody(
      std::unique_ptr<FunctionDecl> fn,
      bool has_return_annotation) {
    if (Match(TokenKind::Semicolon)) {
      return PublishFunctionPrototype(std::move(fn), has_return_annotation);
    }

    if (!At(TokenKind::LBrace)) {
      (void)DiagnoseMissingFunctionTerminatorOrBody();
      return nullptr;
    }

    return PublishFunctionBody(std::move(fn), has_return_annotation);
  }

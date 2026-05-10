  bool ParseCStyleFunctionParameters(FunctionDecl &fn) {
    if (At(TokenKind::RParen)) {
      return true;
    }

    std::size_t parameter_index = 0;
    while (true) {
      FuncParam param;
      const Token &type_token = Peek();
      param.line = type_token.line;
      param.column = type_token.column;

      const Objc3CStyleFunctionTypeParseResult parsed_type =
          ParseObjc3CStyleFunctionType(tokens_, index_, parameter_index == 0,
                                       type_token.line, type_token.column,
                                       "parameter", diagnostics_);
      if (!parsed_type.ok) {
        return false;
      }
      param.type = parsed_type.type;
      param.id_spelling = parsed_type.id_spelling;
      param.class_spelling = parsed_type.class_spelling;
      param.sel_spelling = parsed_type.sel_spelling;
      param.instancetype_spelling = parsed_type.instancetype_spelling;
      param.object_pointer_type_spelling = parsed_type.object_pointer_spelling;
      param.object_pointer_type_name = parsed_type.object_pointer_name;
      param.has_pointer_declarator = parsed_type.pointer_depth > 0u;
      param.pointer_declarator_depth = parsed_type.pointer_depth;

      if (parameter_index == 0 && param.type == ValueType::Void && At(TokenKind::RParen)) {
        return true;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P101",
                     "expected parameter identifier, found " + DescribeParserDiagnosticToken(token)));
        return false;
      }

      const Token name_token = Advance();
      param.name = name_token.text;
      param.line = name_token.line;
      param.column = name_token.column;
      param.typecheck_family_symbol = BuildObjcTypecheckParamFamilySymbol(param);
      fn.params.push_back(std::move(param));
      ++parameter_index;

      if (!Match(TokenKind::Comma)) {
        return true;
      }
      if (At(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P104",
                     "trailing ',' in C-style parameter list is not allowed"));
        return false;
      }
      if (!AtCStyleTypeLeadToken() && !At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100",
                     "expected parameter declaration after ',' in C-style parameter list, found " +
                         DescribeParserDiagnosticToken(token)));
        return false;
      }
    }
  }

#include "parse/objc3_parser_core_top_level_function_declarations.inc"

  bool AtIdentifierColon() const {
    return IsObjc3IdentifierColonStatementLead(tokens_, index_);
  }

  bool AtIdentifierAssignment() const {
    return IsObjc3IdentifierAssignmentStatementLead(tokens_, index_);
  }

  bool AtIdentifierUpdate() const {
    return IsObjc3IdentifierUpdateStatementLead(tokens_, index_);
  }

  bool AtPrefixUpdate() const {
    return IsObjc3PrefixUpdateStatementLead(tokens_, index_);
  }

  bool MatchAssignmentOperator(std::string &op) {
    if (!objc3c::parse::IsObjc3AssignmentOperatorToken(Peek().kind)) {
      return false;
    }
    op = objc3c::parse::Objc3AssignmentOperatorSpelling(Peek().kind);
    Advance();
    return true;
  }

  bool MatchUpdateOperator(std::string &op) {
    if (!objc3c::parse::IsObjc3UpdateOperatorToken(Peek().kind)) {
      return false;
    }
    op = objc3c::parse::Objc3UpdateOperatorSpelling(Peek().kind);
    Advance();
    return true;
  }

  void ParseModule(Objc3ParsedProgram &program) {
    // registration-descriptor/image-root source-surface and
    // frontend-closure anchor: module identity remains parser-owned here,
    // while the paired registration-descriptor and image-root prelude pragmas
    // are lexer-owned inputs that later frontend-manifest code resolves into
    // one bootstrap naming surface and emitted descriptor artifact boundary.
    const Token &name_token = Peek();
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P101", "invalid module identifier"));
      SynchronizeTopLevel();
      return;
    }
    const std::string module_name = Advance().text;
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P104", "missing ';' after module declaration"));
      SynchronizeTopLevel();
      return;
    }
    if (saw_module_declaration_) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3S200", "duplicate module '" + module_name + "'"));
      return;
    }
    saw_module_declaration_ = true;
    ast_builder_.SetModuleName(program, module_name);
  }

  std::unique_ptr<GlobalDecl> ParseGlobalLet() {
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          BuildObjc3InvalidDeclarationIdentifierDiagnostic(name_token));
      SynchronizeTopLevel();
      return nullptr;
    }
    auto decl = std::make_unique<GlobalDecl>(
        BuildObjc3GlobalLetDeclarationIdentity(Previous()));

    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
      SynchronizeTopLevel();
      return nullptr;
    }

    decl->value = ParseExpressionWithBlockLiteralSourceUse(
        BlockLiteralSourceUseKind::GlobalInitializer);
    if (decl->value == nullptr) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          BuildObjc3MissingSemicolonDiagnostic(token, "declaration"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  void CopyMethodReturnTypeFromFunctionDecl(const FunctionDecl &source, Objc3MethodDecl &target) {
    CopyObjc3MethodReturnTypeFromFunctionDecl(source, target);
  }

  void CopyPropertyTypeFromParam(const FuncParam &source, Objc3PropertyDecl &target) {
    CopyObjc3PropertyTypeFromParam(source, target);
  }

  bool AtThrowsClauseKeyword() const {
    return At(TokenKind::Identifier) && Peek().text == "throws";
  }

  bool AtAsyncClauseKeyword() const {
    return At(TokenKind::KwAsync);
  }

  bool AtIdentifierText(const char *text) const {
    return objc3c::parse::AtIdentifierText(tokens_, index_, text);
  }

  bool AtIdentifierTextOffset(std::size_t offset, const char *text) const {
    return objc3c::parse::AtIdentifierTextOffset(tokens_, index_, offset, text);
  }

  bool ParseOptionalThrowsClause(FunctionDecl &fn) {
    if (!AtThrowsClauseKeyword()) {
      return true;
    }
    const Token throws_token = Advance();
    if (fn.throws_declared) {
      diagnostics_.push_back(MakeDiag(throws_token.line, throws_token.column, "O3P181",
                                      "duplicate 'throws' declaration modifier"));
      return false;
    }
    fn.throws_declared = true;
    return true;
  }

  bool ParseOptionalThrowsClause(Objc3MethodDecl &method) {
    if (!AtThrowsClauseKeyword()) {
      return true;
    }
    const Token throws_token = Advance();
    if (method.throws_declared) {
      diagnostics_.push_back(MakeDiag(throws_token.line, throws_token.column, "O3P181",
                                      "duplicate 'throws' declaration modifier"));
      return false;
    }
    method.throws_declared = true;
    return true;
  }

  bool ParseOptionalAsyncClause(FunctionDecl &fn) {
    if (!AtAsyncClauseKeyword()) {
      return true;
    }
    const Token async_token = Advance();
    if (fn.async_declared) {
      diagnostics_.push_back(MakeDiag(async_token.line, async_token.column, "O3P181",
                                      "duplicate 'async' declaration modifier"));
      return false;
    }
    fn.async_declared = true;
    return true;
  }

  bool ParseOptionalAsyncClause(Objc3MethodDecl &method) {
    if (!AtAsyncClauseKeyword()) {
      return true;
    }
    const Token async_token = Advance();
    if (method.async_declared) {
      diagnostics_.push_back(MakeDiag(async_token.line, async_token.column, "O3P181",
                                      "duplicate 'async' declaration modifier"));
      return false;
    }
    method.async_declared = true;
    return true;
  }

  bool IsBorrowedQualifierSpelling(const std::string &text) const {
    return objc3c::parse::IsObjc3BorrowedQualifierSpelling(text);
  }

  std::string BuildReturnsBorrowedProfile(bool declared,
                                          std::size_t owner_index,
                                          bool return_borrowed_pointer_qualified) {
    return objc3c::parse::BuildObjc3ReturnsBorrowedProfile(
        declared, owner_index, return_borrowed_pointer_qualified);
  }

  bool ParseLocalStorageAttribute(LetStmt &stmt) {
    return objc3c::parse::ParseObjc3LocalStorageAttribute(
        tokens_, index_, diagnostics_, stmt);
  }

  bool ParseLocalStorageSugar(LetStmt &stmt) {
    return objc3c::parse::ParseObjc3LocalStorageSugar(
        tokens_, index_, diagnostics_, stmt);

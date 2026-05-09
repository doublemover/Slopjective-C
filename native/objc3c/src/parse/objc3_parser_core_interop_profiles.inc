class Objc3Parser {
 public:
  explicit Objc3Parser(const std::vector<Token> &tokens) : tokens_(tokens) {}

  using BlockLiteralSourceUseKind = Objc3BlockLiteralSourceUseKind;

  struct ScopedBlockLiteralSourceUse {
    explicit ScopedBlockLiteralSourceUse(std::vector<BlockLiteralSourceUseKind> &stack,
                                         BlockLiteralSourceUseKind kind)
        : stack_(stack) {
      stack_.push_back(kind);
    }

    ~ScopedBlockLiteralSourceUse() { stack_.pop_back(); }

   private:
    std::vector<BlockLiteralSourceUseKind> &stack_;
  };

  Objc3ParsedProgram Parse() {
    Objc3ParsedProgram program = ast_builder_.BeginProgram();
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::KwModule)) {
        ParseModule(program);
      } else if (Match(TokenKind::KwLet)) {
        auto decl = ParseGlobalLet();
        if (decl != nullptr) {
          ast_builder_.AddGlobalDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtInterface)) {
        auto decl = ParseObjcInterfaceDecl();
        if (decl != nullptr) {
          ast_builder_.AddInterfaceDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtImplementation)) {
        auto decl = ParseObjcImplementationDecl();
        if (decl != nullptr) {
          ast_builder_.AddImplementationDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtProtocol)) {
        auto decl = ParseObjcProtocolDecl();
        if (decl != nullptr) {
          ast_builder_.AddProtocolDecl(program, std::move(*decl));
        }
      } else if (IsObjc3ActorClassDeclarationLead(tokens_, index_)) {
        // source-surface anchor: `actor class` stays a contextual
        // parser form rather than a dedicated lexer keyword while lane-A
        // completes actor-member frontend admission.
        const Token actor_token = Advance();
        Advance();
        auto decl = ParseObjcActorInterfaceDecl(actor_token);
        if (decl != nullptr) {
          ast_builder_.AddInterfaceDecl(program, std::move(*decl));
        }
      } else if (AtIdentifierText("__attribute__")) {
        // source-completion anchor: prefixed Part 9 container
        // attributes are parser-owned wrappers around the existing interface /
        // actor container forms rather than a widened top-level grammar family.
        Objc3InterfaceDecl pending_container_attributes;
        if (!ParseOptionalContainerDispatchAttributes(
                pending_container_attributes)) {
          SynchronizeTopLevel();
          continue;
        }
        if (Match(TokenKind::KwAtInterface)) {
          auto decl = ParseObjcInterfaceDecl(&pending_container_attributes);
          if (decl != nullptr) {
            ast_builder_.AddInterfaceDecl(program, std::move(*decl));
          }
        } else if (IsObjc3ActorClassDeclarationLead(tokens_, index_)) {
          const Token actor_token = Advance();
          Advance();
          auto decl = ParseObjcActorInterfaceDecl(actor_token,
                                                  &pending_container_attributes);
          if (decl != nullptr) {
            ast_builder_.AddInterfaceDecl(program, std::move(*decl));
          }
        } else {
          const Token &token = Peek();
          diagnostics_.push_back(BuildObjc3UnsupportedTopLevelDiagnostic(token));
          SynchronizeTopLevel();
        }
      } else if (IsObjc3TopLevelFunctionQualifierLead(Peek().kind)) {
        ParseTopLevelFunctionDecl(program);
      } else if (AtRemovedOptionalTemplateAlias()) {
        RejectRemovedOptionalTemplateAlias(Peek());
        SynchronizeTopLevel();
      } else if (AtCStyleTopLevelFunctionDeclStart()) {
        ParseTopLevelCStyleFunctionDecl(program);
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(BuildObjc3UnsupportedTopLevelDiagnostic(token));
        SynchronizeTopLevel();
      }
    }
    FinalizeObjcProgramPropertyIvarLayoutClosure(program);
    ast_builder_.SetDraftSyntaxSurfaceSummary(
        program, BuildObjc3DraftSyntaxSurfaceSummary(program));
    return program;
  }

  std::vector<std::string> TakeDiagnostics() { return diagnostics_; }

 private:
  bool At(TokenKind kind) const { return AtTokenKind(tokens_, index_, kind); }

  const Token &Peek() const { return PeekToken(tokens_, index_); }

  const Token &Previous() const { return PreviousToken(tokens_, index_); }

  const Token &Advance() { return AdvanceToken(tokens_, index_); }

  bool Match(TokenKind kind) { return MatchTokenKind(tokens_, index_, kind); }

  bool AtRemovedOptionalTemplateAlias() const {
    return IsObjc3RemovedOptionalTemplateAliasLead(tokens_, index_);
  }

  bool RejectRemovedOptionalTemplateAlias(const Token &token) {
    diagnostics_.push_back(BuildObjc3RemovedOptionalTemplateAliasDiagnostic(token));
    return false;
  }

  bool AtCStyleTypeLeadToken() const {
    return IsObjc3CStyleTypeLeadToken(tokens_, index_);
  }

  bool AtCStyleTopLevelFunctionDeclStart() const {
    return IsObjc3CStyleFunctionDeclarationStart(tokens_, index_);
  }

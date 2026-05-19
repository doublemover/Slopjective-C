  explicit Objc3Parser(const std::vector<Token> &tokens) : tokens_(tokens) {}

  using BlockLiteralSourceUseKind = Objc3BlockLiteralSourceUseKind;

  struct ScopedBlockLiteralSourceUse {
    explicit ScopedBlockLiteralSourceUse(
        std::vector<BlockLiteralSourceUseKind> &stack,
        BlockLiteralSourceUseKind kind)
        : stack_(stack) {
      stack_.push_back(kind);
    }

    ~ScopedBlockLiteralSourceUse() { stack_.pop_back(); }

   private:
    std::vector<BlockLiteralSourceUseKind> &stack_;
  };

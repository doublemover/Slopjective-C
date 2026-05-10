        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P103", "expected 'case' or 'default' in match body"));
        return nullptr;

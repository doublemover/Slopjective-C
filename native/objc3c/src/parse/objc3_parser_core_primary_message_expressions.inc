#include "parse/objc3_parser_core_primary_message_expressions_primary_literals_identifiers.inc"
#include "parse/objc3_parser_core_primary_message_expressions_message_receiver_selector.inc"
#include "parse/objc3_parser_core_primary_message_expressions_dispatch_abi_lowering_annotations.inc"
#include "parse/objc3_parser_core_primary_message_expressions_receiver_metadata_annotations.inc"

  bool AtReservedMatchExpressionRecoveryBoundary(int paren_depth,
                                                 int brace_depth,
                                                 int bracket_depth) const {
    if (paren_depth != 0 || brace_depth != 0 || bracket_depth != 0) {
      return false;
    }
    switch (Peek().kind) {
      case TokenKind::Semicolon:
      case TokenKind::RBrace:
      case TokenKind::KwLet:
      case TokenKind::KwVar:
      case TokenKind::KwReturn:
      case TokenKind::KwIf:
      case TokenKind::KwGuard:
      case TokenKind::KwDefer:
      case TokenKind::KwDo:
      case TokenKind::KwMatch:
      case TokenKind::KwFor:
      case TokenKind::KwSwitch:
      case TokenKind::KwWhile:
      case TokenKind::KwBreak:
      case TokenKind::KwContinue:
      case TokenKind::KwAtAutoreleasePool:
        return true;
      default:
        return false;
    }
  }

  void ConsumeReservedMatchExpressionRemainder() {
    int paren_depth = 0;
    int brace_depth = 0;
    int bracket_depth = 0;
    while (!At(TokenKind::Eof) &&
           !AtReservedMatchExpressionRecoveryBoundary(paren_depth, brace_depth,
                                                      bracket_depth)) {
      const TokenKind kind = Peek().kind;
      Advance();
      switch (kind) {
        case TokenKind::LParen:
          ++paren_depth;
          break;
        case TokenKind::RParen:
          if (paren_depth > 0) {
            --paren_depth;
          }
          break;
        case TokenKind::LBrace:
          ++brace_depth;
          break;
        case TokenKind::RBrace:
          if (brace_depth > 0) {
            --brace_depth;
          }
          break;
        case TokenKind::LBracket:
          ++bracket_depth;
          break;
        case TokenKind::RBracket:
          if (bracket_depth > 0) {
            --bracket_depth;
          }
          break;
        default:
          break;
      }
    }
  }

  std::unique_ptr<Expr> ParsePrimary() {
    if (At(TokenKind::Hash)) {
      return ParseCollectionLiteralExpression();
    }

    bool matched_primary = false;
    auto primary = ParsePrimaryLiteralOrIdentifier(matched_primary);
    if (matched_primary) {
      return primary;
    }

    if (Match(TokenKind::LParen)) {
      auto expr = ParseExpression();
      if (expr == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after expression"));
        return nullptr;
      }
      return expr;
    }

    if (Match(TokenKind::LBracket)) {
      return ParseMessageSendExpression();
    }

    if (At(TokenKind::KwMatch)) {
      const Token match_token = Advance();
      diagnostics_.push_back(
          BuildObjc3ReservedMatchExpressionDiagnostic(match_token));
      ConsumeReservedMatchExpressionRemainder();
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in expression position"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in expression position"
                                                  : "unexpected qualifier 'async' in expression position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return nullptr;
    }

    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid expression"));
    return nullptr;
  }

  std::unique_ptr<Expr> ParseCollectionLiteralExpression() {
    const Token hash = Advance();
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P214",
          "collection literal requires '#array', '#map', or '#set'"));
      return nullptr;
    }
    const Token kind_token = Advance();
    Expr::CollectionLiteralKind kind = Expr::CollectionLiteralKind::None;
    if (kind_token.text == "array") {
      kind = Expr::CollectionLiteralKind::Array;
    } else if (kind_token.text == "map") {
      kind = Expr::CollectionLiteralKind::Map;
    } else if (kind_token.text == "set") {
      kind = Expr::CollectionLiteralKind::Set;
    } else {
      diagnostics_.push_back(MakeDiag(
          kind_token.line, kind_token.column, "O3P214",
          "unsupported collection literal '#" + kind_token.text +
              "'; expected '#array', '#map', or '#set'"));
      return nullptr;
    }
    if (!Match(TokenKind::LBracket)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P214",
          "missing '[' after collection literal prefix"));
      return nullptr;
    }

    std::vector<std::unique_ptr<Expr>> keys;
    std::vector<std::unique_ptr<Expr>> values;
    if (!At(TokenKind::RBracket)) {
      while (true) {
        auto first = ParseExpression();
        if (first == nullptr) {
          return nullptr;
        }
        if (kind == Expr::CollectionLiteralKind::Map) {
          if (!Match(TokenKind::Colon)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(
                token.line, token.column, "O3P214",
                "map literal entries require 'key: value'"));
            return nullptr;
          }
          auto value = ParseExpression();
          if (value == nullptr) {
            return nullptr;
          }
          keys.push_back(std::move(first));
          values.push_back(std::move(value));
        } else {
          values.push_back(std::move(first));
        }
        if (!Match(TokenKind::Comma)) {
          break;
        }
        if (At(TokenKind::RBracket)) {
          break;
        }
      }
    }
    if (!Match(TokenKind::RBracket)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P214",
          "missing ']' after collection literal"));
      return nullptr;
    }
    return BuildObjc3CollectionLiteralExpr(kind, hash, std::move(keys),
                                           std::move(values));
  }

  std::unique_ptr<Expr> ParseMessageSendExpression() {
    const Token open = Previous();
    auto message = std::make_unique<Expr>();
    message->kind = Expr::Kind::MessageSend;
    message->line = open.line;
    message->column = open.column;

    if (!ParseMessageReceiverAndOptionalSendMetadata(*message, open)) {
      return nullptr;
    }

    if (!ParseMessageSelectorAndArguments(*message)) {
      return nullptr;
    }

    ApplyMessageDispatchAbiLoweringAnnotations(*message);
    ApplyMessageReceiverMetadataAnnotations(*message);

    if (!Match(TokenKind::RBracket)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "missing ']' after message send expression"));
      return nullptr;
    }
    return message;
  }

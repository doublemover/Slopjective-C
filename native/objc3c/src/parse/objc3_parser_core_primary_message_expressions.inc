#include "parse/objc3_parser_core_primary_message_expressions_primary_literals_identifiers.inc"
#include "parse/objc3_parser_core_primary_message_expressions_message_receiver_selector.inc"
#include "parse/objc3_parser_core_primary_message_expressions_dispatch_abi_lowering_annotations.inc"
#include "parse/objc3_parser_core_primary_message_expressions_receiver_metadata_annotations.inc"

  std::unique_ptr<Expr> ParsePrimary() {
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

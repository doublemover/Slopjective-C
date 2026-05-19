  bool ParseObjcMethodParameterClause(FuncParam &param) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106", "missing '(' before Objective-C method parameter type"));
      return false;
    }
    if (!ParseParameterType(param)) {
      return false;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C method parameter type"));
      return false;
    }
    const Token &name = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name.line, name.column, "O3P101", "invalid Objective-C method parameter identifier"));
      return false;
    }
    param.name = Previous().text;
    param.line = Previous().line;
    param.column = Previous().column;
    return true;
  }

#include "parse/objc3_parser_core_objc_method_decl_parser.inc"

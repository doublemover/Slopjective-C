#include "token/objc3_token_kind.h"

namespace {

Objc3TokenKindClassification Keyword(Objc3LexTokenKind kind) {
  return Objc3TokenKindClassification{kind, true};
}

}  // namespace

Objc3TokenKindClassification ClassifyObjc3IdentifierToken(const std::string &text) {
  if (text == "module") {
    return Keyword(Objc3LexTokenKind::KwModule);
  }
  if (text == "let") {
    return Keyword(Objc3LexTokenKind::KwLet);
  }
  if (text == "var") {
    return Keyword(Objc3LexTokenKind::KwVar);
  }
  if (text == "fn") {
    return Keyword(Objc3LexTokenKind::KwFn);
  }
  if (text == "async") {
    return Keyword(Objc3LexTokenKind::KwAsync);
  }
  if (text == "pure") {
    return Keyword(Objc3LexTokenKind::KwPure);
  }
  if (text == "extern") {
    return Keyword(Objc3LexTokenKind::KwExtern);
  }
  if (text == "return") {
    return Keyword(Objc3LexTokenKind::KwReturn);
  }
  if (text == "if") {
    return Keyword(Objc3LexTokenKind::KwIf);
  }
  if (text == "else") {
    return Keyword(Objc3LexTokenKind::KwElse);
  }
  if (text == "guard") {
    return Keyword(Objc3LexTokenKind::KwGuard);
  }
  if (text == "defer") {
    return Keyword(Objc3LexTokenKind::KwDefer);
  }
  if (text == "do") {
    return Keyword(Objc3LexTokenKind::KwDo);
  }
  if (text == "await") {
    return Keyword(Objc3LexTokenKind::KwAwait);
  }
  if (text == "try") {
    return Keyword(Objc3LexTokenKind::KwTry);
  }
  if (text == "throw") {
    return Keyword(Objc3LexTokenKind::KwThrow);
  }
  if (text == "catch") {
    return Keyword(Objc3LexTokenKind::KwCatch);
  }
  if (text == "for") {
    return Keyword(Objc3LexTokenKind::KwFor);
  }
  if (text == "switch") {
    return Keyword(Objc3LexTokenKind::KwSwitch);
  }
  if (text == "match") {
    return Keyword(Objc3LexTokenKind::KwMatch);
  }
  if (text == "case") {
    return Keyword(Objc3LexTokenKind::KwCase);
  }
  if (text == "default") {
    return Keyword(Objc3LexTokenKind::KwDefault);
  }
  if (text == "while") {
    return Keyword(Objc3LexTokenKind::KwWhile);
  }
  if (text == "break") {
    return Keyword(Objc3LexTokenKind::KwBreak);
  }
  if (text == "continue") {
    return Keyword(Objc3LexTokenKind::KwContinue);
  }
  if (text == "i32") {
    return Keyword(Objc3LexTokenKind::KwI32);
  }
  if (text == "bool") {
    return Keyword(Objc3LexTokenKind::KwBool);
  }
  if (text == "BOOL") {
    return Keyword(Objc3LexTokenKind::KwBOOL);
  }
  if (text == "NSInteger") {
    return Keyword(Objc3LexTokenKind::KwNSInteger);
  }
  if (text == "NSUInteger") {
    return Keyword(Objc3LexTokenKind::KwNSUInteger);
  }
  if (text == "void") {
    return Keyword(Objc3LexTokenKind::KwVoid);
  }
  if (text == "id") {
    return Keyword(Objc3LexTokenKind::KwId);
  }
  if (text == "Class") {
    return Keyword(Objc3LexTokenKind::KwClass);
  }
  if (text == "SEL") {
    return Keyword(Objc3LexTokenKind::KwSEL);
  }
  if (text == "Protocol") {
    return Keyword(Objc3LexTokenKind::KwProtocol);
  }
  if (text == "instancetype") {
    return Keyword(Objc3LexTokenKind::KwInstancetype);
  }
  if (text == "true") {
    return Keyword(Objc3LexTokenKind::KwTrue);
  }
  if (text == "false") {
    return Keyword(Objc3LexTokenKind::KwFalse);
  }
  if (text == "nil") {
    return Keyword(Objc3LexTokenKind::KwNil);
  }
  return Objc3TokenKindClassification{};
}

Objc3TokenKindClassification ClassifyObjc3AtDirectiveToken(const std::string &directive) {
  if (directive == "interface") {
    return Keyword(Objc3LexTokenKind::KwAtInterface);
  }
  if (directive == "implementation") {
    return Keyword(Objc3LexTokenKind::KwAtImplementation);
  }
  if (directive == "protocol") {
    return Keyword(Objc3LexTokenKind::KwAtProtocol);
  }
  if (directive == "required") {
    return Keyword(Objc3LexTokenKind::KwAtRequired);
  }
  if (directive == "optional") {
    return Keyword(Objc3LexTokenKind::KwAtOptional);
  }
  if (directive == "property") {
    return Keyword(Objc3LexTokenKind::KwAtProperty);
  }
  if (directive == "keypath") {
    return Keyword(Objc3LexTokenKind::KwAtKeypath);
  }
  if (directive == "cleanup") {
    return Keyword(Objc3LexTokenKind::KwAtCleanup);
  }
  if (directive == "resource") {
    return Keyword(Objc3LexTokenKind::KwAtResource);
  }
  if (directive == "end") {
    return Keyword(Objc3LexTokenKind::KwAtEnd);
  }
  if (directive == "autoreleasepool") {
    return Keyword(Objc3LexTokenKind::KwAtAutoreleasePool);
  }
  return Objc3TokenKindClassification{};
}

Objc3LegacyLiteralAliasKind ClassifyObjc3LegacyLiteralAlias(const std::string &text) {
  if (text == "YES") {
    return Objc3LegacyLiteralAliasKind::Yes;
  }
  if (text == "NO") {
    return Objc3LegacyLiteralAliasKind::No;
  }
  if (text == "NULL") {
    return Objc3LegacyLiteralAliasKind::Null;
  }
  return Objc3LegacyLiteralAliasKind::None;
}

const char *Objc3LegacyLiteralAliasDiagnosticSpelling(Objc3LegacyLiteralAliasKind alias) {
  switch (alias) {
  case Objc3LegacyLiteralAliasKind::Yes:
    return "YES";
  case Objc3LegacyLiteralAliasKind::No:
    return "NO";
  case Objc3LegacyLiteralAliasKind::Null:
    return "NULL";
  case Objc3LegacyLiteralAliasKind::None:
  default:
    return "";
  }
}

const char *Objc3LegacyLiteralAliasCanonicalSpelling(Objc3LegacyLiteralAliasKind alias) {
  switch (alias) {
  case Objc3LegacyLiteralAliasKind::Yes:
    return "true";
  case Objc3LegacyLiteralAliasKind::No:
    return "false";
  case Objc3LegacyLiteralAliasKind::Null:
    return "nil";
  case Objc3LegacyLiteralAliasKind::None:
  default:
    return "";
  }
}

const char *Objc3LexTokenKindName(Objc3LexTokenKind kind) {
  switch (kind) {
  case Objc3LexTokenKind::Eof:
    return "eof";
  case Objc3LexTokenKind::Identifier:
    return "identifier";
  case Objc3LexTokenKind::Number:
    return "number";
  case Objc3LexTokenKind::String:
    return "string";
  case Objc3LexTokenKind::KwModule:
    return "module";
  case Objc3LexTokenKind::KwLet:
    return "let";
  case Objc3LexTokenKind::KwVar:
    return "var";
  case Objc3LexTokenKind::KwFn:
    return "fn";
  case Objc3LexTokenKind::KwAsync:
    return "async";
  case Objc3LexTokenKind::KwPure:
    return "pure";
  case Objc3LexTokenKind::KwExtern:
    return "extern";
  case Objc3LexTokenKind::KwReturn:
    return "return";
  case Objc3LexTokenKind::KwIf:
    return "if";
  case Objc3LexTokenKind::KwElse:
    return "else";
  case Objc3LexTokenKind::KwGuard:
    return "guard";
  case Objc3LexTokenKind::KwDefer:
    return "defer";
  case Objc3LexTokenKind::KwDo:
    return "do";
  case Objc3LexTokenKind::KwAwait:
    return "await";
  case Objc3LexTokenKind::KwTry:
    return "try";
  case Objc3LexTokenKind::KwThrow:
    return "throw";
  case Objc3LexTokenKind::KwCatch:
    return "catch";
  case Objc3LexTokenKind::KwFor:
    return "for";
  case Objc3LexTokenKind::KwSwitch:
    return "switch";
  case Objc3LexTokenKind::KwMatch:
    return "match";
  case Objc3LexTokenKind::KwCase:
    return "case";
  case Objc3LexTokenKind::KwDefault:
    return "default";
  case Objc3LexTokenKind::KwWhile:
    return "while";
  case Objc3LexTokenKind::KwBreak:
    return "break";
  case Objc3LexTokenKind::KwContinue:
    return "continue";
  case Objc3LexTokenKind::KwI32:
    return "i32";
  case Objc3LexTokenKind::KwBool:
    return "bool";
  case Objc3LexTokenKind::KwBOOL:
    return "BOOL";
  case Objc3LexTokenKind::KwNSInteger:
    return "NSInteger";
  case Objc3LexTokenKind::KwNSUInteger:
    return "NSUInteger";
  case Objc3LexTokenKind::KwVoid:
    return "void";
  case Objc3LexTokenKind::KwId:
    return "id";
  case Objc3LexTokenKind::KwClass:
    return "Class";
  case Objc3LexTokenKind::KwSEL:
    return "SEL";
  case Objc3LexTokenKind::KwProtocol:
    return "Protocol";
  case Objc3LexTokenKind::KwInstancetype:
    return "instancetype";
  case Objc3LexTokenKind::KwTrue:
    return "true";
  case Objc3LexTokenKind::KwFalse:
    return "false";
  case Objc3LexTokenKind::KwNil:
    return "nil";
  case Objc3LexTokenKind::KwAtInterface:
    return "@interface";
  case Objc3LexTokenKind::KwAtImplementation:
    return "@implementation";
  case Objc3LexTokenKind::KwAtProtocol:
    return "@protocol";
  case Objc3LexTokenKind::KwAtRequired:
    return "@required";
  case Objc3LexTokenKind::KwAtOptional:
    return "@optional";
  case Objc3LexTokenKind::KwAtProperty:
    return "@property";
  case Objc3LexTokenKind::KwAtKeypath:
    return "@keypath";
  case Objc3LexTokenKind::KwAtCleanup:
    return "@cleanup";
  case Objc3LexTokenKind::KwAtResource:
    return "@resource";
  case Objc3LexTokenKind::KwAtEnd:
    return "@end";
  case Objc3LexTokenKind::KwAtAutoreleasePool:
    return "@autoreleasepool";
  case Objc3LexTokenKind::LParen:
    return "(";
  case Objc3LexTokenKind::RParen:
    return ")";
  case Objc3LexTokenKind::LBracket:
    return "[";
  case Objc3LexTokenKind::RBracket:
    return "]";
  case Objc3LexTokenKind::LBrace:
    return "{";
  case Objc3LexTokenKind::RBrace:
    return "}";
  case Objc3LexTokenKind::Comma:
    return ",";
  case Objc3LexTokenKind::Colon:
    return ":";
  case Objc3LexTokenKind::Dot:
    return ".";
  case Objc3LexTokenKind::Semicolon:
    return ";";
  case Objc3LexTokenKind::Equal:
    return "=";
  case Objc3LexTokenKind::PlusEqual:
    return "+=";
  case Objc3LexTokenKind::MinusEqual:
    return "-=";
  case Objc3LexTokenKind::StarEqual:
    return "*=";
  case Objc3LexTokenKind::SlashEqual:
    return "/=";
  case Objc3LexTokenKind::PercentEqual:
    return "%=";
  case Objc3LexTokenKind::AmpersandEqual:
    return "&=";
  case Objc3LexTokenKind::PipeEqual:
    return "|=";
  case Objc3LexTokenKind::CaretEqual:
    return "^=";
  case Objc3LexTokenKind::LessLessEqual:
    return "<<=";
  case Objc3LexTokenKind::GreaterGreaterEqual:
    return ">>=";
  case Objc3LexTokenKind::PlusPlus:
    return "++";
  case Objc3LexTokenKind::MinusMinus:
    return "--";
  case Objc3LexTokenKind::EqualEqual:
    return "==";
  case Objc3LexTokenKind::Bang:
    return "!";
  case Objc3LexTokenKind::BangEqual:
    return "!=";
  case Objc3LexTokenKind::Less:
    return "<";
  case Objc3LexTokenKind::LessLess:
    return "<<";
  case Objc3LexTokenKind::LessEqual:
    return "<=";
  case Objc3LexTokenKind::Greater:
    return ">";
  case Objc3LexTokenKind::GreaterGreater:
    return ">>";
  case Objc3LexTokenKind::GreaterEqual:
    return ">=";
  case Objc3LexTokenKind::Ampersand:
    return "&";
  case Objc3LexTokenKind::Pipe:
    return "|";
  case Objc3LexTokenKind::Caret:
    return "^";
  case Objc3LexTokenKind::AndAnd:
    return "&&";
  case Objc3LexTokenKind::OrOr:
    return "||";
  case Objc3LexTokenKind::Question:
    return "?";
  case Objc3LexTokenKind::QuestionDot:
    return "?.";
  case Objc3LexTokenKind::QuestionQuestion:
    return "??";
  case Objc3LexTokenKind::Tilde:
    return "~";
  case Objc3LexTokenKind::Plus:
    return "+";
  case Objc3LexTokenKind::Minus:
    return "-";
  case Objc3LexTokenKind::Star:
    return "*";
  case Objc3LexTokenKind::Slash:
    return "/";
  case Objc3LexTokenKind::Percent:
    return "%";
  }
  return "unknown";
}

bool Objc3LexTokenKindIsKeyword(Objc3LexTokenKind kind) {
  switch (kind) {
  case Objc3LexTokenKind::KwModule:
  case Objc3LexTokenKind::KwLet:
  case Objc3LexTokenKind::KwVar:
  case Objc3LexTokenKind::KwFn:
  case Objc3LexTokenKind::KwAsync:
  case Objc3LexTokenKind::KwPure:
  case Objc3LexTokenKind::KwExtern:
  case Objc3LexTokenKind::KwReturn:
  case Objc3LexTokenKind::KwIf:
  case Objc3LexTokenKind::KwElse:
  case Objc3LexTokenKind::KwGuard:
  case Objc3LexTokenKind::KwDefer:
  case Objc3LexTokenKind::KwDo:
  case Objc3LexTokenKind::KwAwait:
  case Objc3LexTokenKind::KwTry:
  case Objc3LexTokenKind::KwThrow:
  case Objc3LexTokenKind::KwCatch:
  case Objc3LexTokenKind::KwFor:
  case Objc3LexTokenKind::KwSwitch:
  case Objc3LexTokenKind::KwMatch:
  case Objc3LexTokenKind::KwCase:
  case Objc3LexTokenKind::KwDefault:
  case Objc3LexTokenKind::KwWhile:
  case Objc3LexTokenKind::KwBreak:
  case Objc3LexTokenKind::KwContinue:
  case Objc3LexTokenKind::KwI32:
  case Objc3LexTokenKind::KwBool:
  case Objc3LexTokenKind::KwBOOL:
  case Objc3LexTokenKind::KwNSInteger:
  case Objc3LexTokenKind::KwNSUInteger:
  case Objc3LexTokenKind::KwVoid:
  case Objc3LexTokenKind::KwId:
  case Objc3LexTokenKind::KwClass:
  case Objc3LexTokenKind::KwSEL:
  case Objc3LexTokenKind::KwProtocol:
  case Objc3LexTokenKind::KwInstancetype:
  case Objc3LexTokenKind::KwTrue:
  case Objc3LexTokenKind::KwFalse:
  case Objc3LexTokenKind::KwNil:
  case Objc3LexTokenKind::KwAtInterface:
  case Objc3LexTokenKind::KwAtImplementation:
  case Objc3LexTokenKind::KwAtProtocol:
  case Objc3LexTokenKind::KwAtRequired:
  case Objc3LexTokenKind::KwAtOptional:
  case Objc3LexTokenKind::KwAtProperty:
  case Objc3LexTokenKind::KwAtKeypath:
  case Objc3LexTokenKind::KwAtCleanup:
  case Objc3LexTokenKind::KwAtResource:
  case Objc3LexTokenKind::KwAtEnd:
  case Objc3LexTokenKind::KwAtAutoreleasePool:
    return true;
  default:
    return false;
  }
}

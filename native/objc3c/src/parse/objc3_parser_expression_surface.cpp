#include "parse/objc3_parser_expression_surface.h"

namespace objc3c::parse {

bool IsObjc3AssignmentOperatorToken(Objc3LexTokenKind kind) {
  return kind == Objc3LexTokenKind::Equal ||
         kind == Objc3LexTokenKind::PlusEqual ||
         kind == Objc3LexTokenKind::MinusEqual ||
         kind == Objc3LexTokenKind::StarEqual ||
         kind == Objc3LexTokenKind::SlashEqual ||
         kind == Objc3LexTokenKind::PercentEqual ||
         kind == Objc3LexTokenKind::AmpersandEqual ||
         kind == Objc3LexTokenKind::PipeEqual ||
         kind == Objc3LexTokenKind::CaretEqual ||
         kind == Objc3LexTokenKind::LessLessEqual ||
         kind == Objc3LexTokenKind::GreaterGreaterEqual;
}

bool IsObjc3UpdateOperatorToken(Objc3LexTokenKind kind) {
  return kind == Objc3LexTokenKind::PlusPlus ||
         kind == Objc3LexTokenKind::MinusMinus;
}

const char *Objc3AssignmentOperatorSpelling(Objc3LexTokenKind kind) {
  switch (kind) {
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
  default:
    return "";
  }
}

const char *Objc3UpdateOperatorSpelling(Objc3LexTokenKind kind) {
  switch (kind) {
  case Objc3LexTokenKind::PlusPlus:
    return "++";
  case Objc3LexTokenKind::MinusMinus:
    return "--";
  default:
    return "";
  }
}

}  // namespace objc3c::parse

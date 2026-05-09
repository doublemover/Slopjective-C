#include "parse/objc3_parser_type_surface.h"

namespace objc3c::parse {

bool ClassifyObjc3CStyleBuiltinTypeSpelling(
    const std::string &spelling,
    Objc3CStyleTypeTokenClassification &classification) {
  classification = Objc3CStyleTypeTokenClassification{};
  if (spelling == "int" || spelling == "NSInteger" || spelling == "NSUInteger") {
    classification.recognized = true;
    classification.type = ValueType::I32;
    return true;
  }
  if (spelling == "bool" || spelling == "BOOL") {
    classification.recognized = true;
    classification.type = ValueType::Bool;
    return true;
  }
  if (spelling == "void") {
    classification.recognized = true;
    classification.type = ValueType::Void;
    return true;
  }
  if (spelling == "id") {
    classification.recognized = true;
    classification.type = ValueType::ObjCId;
    classification.id_spelling = true;
    return true;
  }
  if (spelling == "Class") {
    classification.recognized = true;
    classification.type = ValueType::ObjCClass;
    classification.class_spelling = true;
    return true;
  }
  if (spelling == "SEL") {
    classification.recognized = true;
    classification.type = ValueType::ObjCSel;
    classification.sel_spelling = true;
    return true;
  }
  if (spelling == "Protocol") {
    classification.recognized = true;
    classification.type = ValueType::ObjCProtocol;
    return true;
  }
  if (spelling == "instancetype") {
    classification.recognized = true;
    classification.type = ValueType::ObjCInstancetype;
    classification.instancetype_spelling = true;
    return true;
  }
  return false;
}

bool ClassifyObjc3CStyleBuiltinTypeToken(
    const Objc3LexToken &token,
    Objc3CStyleTypeTokenClassification &classification) {
  classification = Objc3CStyleTypeTokenClassification{};
  switch (token.kind) {
  case Objc3LexTokenKind::KwI32:
    classification.recognized = true;
    classification.type = ValueType::I32;
    return true;
  case Objc3LexTokenKind::KwBool:
  case Objc3LexTokenKind::KwBOOL:
    classification.recognized = true;
    classification.type = ValueType::Bool;
    return true;
  case Objc3LexTokenKind::KwVoid:
    classification.recognized = true;
    classification.type = ValueType::Void;
    return true;
  case Objc3LexTokenKind::KwId:
    classification.recognized = true;
    classification.type = ValueType::ObjCId;
    classification.id_spelling = true;
    return true;
  case Objc3LexTokenKind::KwClass:
    classification.recognized = true;
    classification.type = ValueType::ObjCClass;
    classification.class_spelling = true;
    return true;
  case Objc3LexTokenKind::KwSEL:
    classification.recognized = true;
    classification.type = ValueType::ObjCSel;
    classification.sel_spelling = true;
    return true;
  case Objc3LexTokenKind::KwProtocol:
    classification.recognized = true;
    classification.type = ValueType::ObjCProtocol;
    return true;
  case Objc3LexTokenKind::KwInstancetype:
    classification.recognized = true;
    classification.type = ValueType::ObjCInstancetype;
    classification.instancetype_spelling = true;
    return true;
  case Objc3LexTokenKind::KwNSInteger:
  case Objc3LexTokenKind::KwNSUInteger:
    classification.recognized = true;
    classification.type = ValueType::I32;
    return true;
  case Objc3LexTokenKind::Identifier:
    return ClassifyObjc3CStyleBuiltinTypeSpelling(token.text, classification);
  default:
    return false;
  }
}

bool IsObjc3CStyleTypeLeadToken(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  if (index >= tokens.size()) {
    return false;
  }
  Objc3CStyleTypeTokenClassification classification;
  return ClassifyObjc3CStyleBuiltinTypeToken(tokens[index], classification);
}

bool IsObjc3CStyleFunctionDeclarationStart(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  if (!IsObjc3CStyleTypeLeadToken(tokens, index)) {
    return false;
  }
  std::size_t cursor = index + 1u;
  while (cursor < tokens.size() && tokens[cursor].kind == Objc3LexTokenKind::Star) {
    ++cursor;
  }
  if (cursor >= tokens.size() || tokens[cursor].kind != Objc3LexTokenKind::Identifier) {
    return false;
  }
  ++cursor;
  return cursor < tokens.size() && tokens[cursor].kind == Objc3LexTokenKind::LParen;
}

}  // namespace objc3c::parse

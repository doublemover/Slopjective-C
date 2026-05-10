#include "parse/objc3_parser_attributes.h"

#include <algorithm>

#include "parse/objc3_parse_support.h"
#include "parse/objc3_parser_attribute_profiles.h"
#include "parse/objc3_parser_cursor.h"

namespace objc3c::parse {
namespace {

using Token = Objc3LexToken;
using TokenKind = Objc3LexTokenKind;
using objc3c::parse::support::MakeDiag;

class Objc3AttributeParser {
 public:
  Objc3AttributeParser(
      const std::vector<Token> &tokens,
      std::size_t &index,
      std::vector<std::string> &diagnostics)
      : tokens_(tokens), index_(index), diagnostics_(diagnostics) {}

  bool ParseOptionalContainerDispatchAttributes(Objc3InterfaceDecl &decl) {
    while (AtIdentifierText("__attribute__")) {
      Advance();
      if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P338",
            "malformed __attribute__ Objective-C container annotation"));
        return false;
      }

      do {
        if (!ParseSingleContainerDispatchAttribute(decl)) {
          return false;
        }
      } while (Match(TokenKind::Comma));

      if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P339",
            "missing '))' after Objective-C container attribute list"));
        return false;
      }
    }
    return true;
  }

  bool ParseOptionalCallableBridgeAttributes(FunctionDecl &decl) {
    return ParseOptionalCallableBridgeAttributesImpl(decl);
  }

  bool ParseOptionalCallableBridgeAttributes(Objc3MethodDecl &decl) {
    return ParseOptionalCallableBridgeAttributesImpl(decl);
  }

#include "parse/objc3_parser_attributes_local_storage_entrypoints.inc"

 private:
  bool At(TokenKind kind) const { return AtTokenKind(tokens_, index_, kind); }

  const Token &Peek() const { return PeekToken(tokens_, index_); }

  const Token &Advance() { return AdvanceToken(tokens_, index_); }

  bool Match(TokenKind kind) { return MatchTokenKind(tokens_, index_, kind); }

  bool AtIdentifierText(const char *text) const {
    return objc3c::parse::AtIdentifierText(tokens_, index_, text);
  }

#include "parse/objc3_parser_attributes_payload_helpers.inc"

#include "parse/objc3_parser_attributes_local_storage.inc"

#include "parse/objc3_parser_attributes_status_code_bridge.inc"

#include "parse/objc3_parser_attributes_callable_families.inc"

  bool ParseSingleContainerDispatchAttribute(Objc3InterfaceDecl &decl) {
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P333",
          "invalid Objective-C container attribute name"));
      return false;
    }

    const Token attribute_name = Advance();
    auto record_duplicate = [&](const char *code, const std::string &message) {
      diagnostics_.push_back(MakeDiag(
          attribute_name.line, attribute_name.column, code, message));
      return false;
    };

    if (attribute_name.text == "objc_direct_members") {
      if (decl.objc_direct_members_declared) {
        return record_duplicate(
            "O3P334", "duplicate objc_direct_members attribute");
      }
      decl.objc_direct_members_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_derive") {
      if (decl.objc_derive_declared) {
        return record_duplicate("O3P345", "duplicate objc_derive attribute");
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_derive", decl.objc_derive_name)) {
        return false;
      }
      decl.objc_derive_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_final") {
      if (decl.objc_final_declared) {
        return record_duplicate("O3P335", "duplicate objc_final attribute");
      }
      decl.objc_final_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_sealed") {
      if (decl.objc_sealed_declared) {
        return record_duplicate("O3P336", "duplicate objc_sealed attribute");
      }
      decl.objc_sealed_declared = true;
      return true;
    }

    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, "O3P337",
        "unsupported Objective-C container attribute '" + attribute_name.text +
            "'"));
    return false;
  }

#include "parse/objc3_parser_attributes_callable_bridge.inc"

  const std::vector<Token> &tokens_;
  std::size_t &index_;
  std::vector<std::string> &diagnostics_;
};

}  // namespace

bool ParseObjc3OptionalContainerDispatchAttributes(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    Objc3InterfaceDecl &decl) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseOptionalContainerDispatchAttributes(decl);
}

bool ParseObjc3OptionalCallableBridgeAttributes(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    FunctionDecl &decl) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseOptionalCallableBridgeAttributes(decl);
}

bool ParseObjc3OptionalCallableBridgeAttributes(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    Objc3MethodDecl &decl) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseOptionalCallableBridgeAttributes(decl);
}

bool ParseObjc3LocalStorageAttribute(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    LetStmt &stmt) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseLocalStorageAttribute(stmt);
}

bool ParseObjc3LocalStorageSugar(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    LetStmt &stmt) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseLocalStorageSugar(stmt);
}

}  // namespace objc3c::parse

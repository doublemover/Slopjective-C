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

  bool ParseLocalStorageAttribute(LetStmt &stmt) {
    if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P314",
          "malformed __attribute__ local annotation"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P315",
          "missing local annotation name after __attribute__(("));
      return false;
    }
    const Token attribute_name = Advance();
    if (attribute_name.text == "cleanup") {
      return ParseLocalCleanupAttribute(stmt, attribute_name);
    }
    if (attribute_name.text == "objc_resource") {
      return ParseLocalResourceAttribute(stmt, attribute_name);
    }
    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, "O3P316",
        "only cleanup and objc_resource are supported on local let bindings in this tranche"));
    return false;
  }

  bool ParseLocalStorageSugar(LetStmt &stmt) {
    if (Match(TokenKind::KwAtCleanup)) {
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P317",
            "missing '(' after @cleanup"));
        return false;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P318",
            "@cleanup requires cleanup function identifier"));
        return false;
      }
      stmt.cleanup_function_symbol = Advance().text;
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P319",
            "missing ')' after @cleanup payload"));
        return false;
      }
      stmt.cleanup_attribute_declared = true;
      stmt.cleanup_sugar_declared = true;
      stmt.cleanup_profile_is_normalized = true;
      stmt.cleanup_profile =
          BuildCleanupAttributeProfile(true, stmt.cleanup_function_symbol);
      return true;
    }
    if (Match(TokenKind::KwAtResource)) {
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P320",
            "missing '(' after @resource"));
        return false;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P321",
            "@resource requires cleanup function identifier"));
        return false;
      }
      stmt.resource_close_symbol = Advance().text;
      if (!Match(TokenKind::Comma)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P322",
            "missing ',' after @resource cleanup function"));
        return false;
      }
      if (!At(TokenKind::Identifier) || Peek().text != "invalid") {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P323",
            "@resource requires invalid clause"));
        return false;
      }
      Advance();
      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P324",
            "missing ':' after invalid in @resource"));
        return false;
      }
      const std::string invalid_expression = ParseAttributeArgumentText();
      if (invalid_expression.empty()) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P325",
            "@resource invalid expression must not be empty"));
        return false;
      }
      stmt.resource_invalid_expression = invalid_expression;
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P326",
            "missing ')' after @resource payload"));
        return false;
      }
      stmt.resource_attribute_declared = true;
      stmt.resource_sugar_declared = true;
      stmt.resource_profile_is_normalized = true;
      stmt.resource_profile = BuildResourceAttributeProfile(
          true, stmt.resource_close_symbol, stmt.resource_invalid_expression);
      return true;
    }
    return false;
  }

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

  template <typename TCallableDecl>
  bool ParseRetainableCFamilyCallableAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    const auto record_no_payload = [this, &decl, &attribute_name]() {
      decl.retainable_c_family_callable_attributes.push_back(attribute_name.text);
      decl.retainable_c_family_profile_is_normalized = true;
      decl.retainable_c_family_profile = BuildRetainableCFamilyCallableProfile(
          decl.retainable_c_family_callable_attributes,
          decl.retainable_c_family_names);
    };
    const auto record_family_payload =
        [this, &decl, &attribute_name]() -> bool {
          if (!Match(TokenKind::LParen)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(
                token.line, token.column, "O3P327",
                "missing '(' after retainable C-family callable attribute"));
            return false;
          }
          if (!At(TokenKind::Identifier)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(
                token.line, token.column, "O3P328",
                "retainable C-family callable attribute requires family identifier"));
            return false;
          }
          decl.retainable_c_family_callable_attributes.push_back(
              attribute_name.text);
          decl.retainable_c_family_names.push_back(Advance().text);
          if (!Match(TokenKind::RParen)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(
                token.line, token.column, "O3P329",
                "missing ')' after retainable C-family attribute payload"));
            return false;
          }
          decl.retainable_c_family_profile_is_normalized = true;
          decl.retainable_c_family_profile = BuildRetainableCFamilyCallableProfile(
              decl.retainable_c_family_callable_attributes,
              decl.retainable_c_family_names);
          return true;
        };

    if (attribute_name.text == "objc_family_retain" ||
        attribute_name.text == "objc_family_release" ||
        attribute_name.text == "objc_family_autorelease") {
      return record_family_payload();
    }
    if (attribute_name.text == "os_returns_retained" ||
        attribute_name.text == "os_returns_not_retained" ||
        attribute_name.text == "os_consumed" ||
        attribute_name.text == "cf_returns_retained" ||
        attribute_name.text == "cf_returns_not_retained" ||
        attribute_name.text == "cf_consumed" ||
        attribute_name.text == "ns_returns_retained" ||
        attribute_name.text == "ns_returns_not_retained" ||
        attribute_name.text == "ns_consumed") {
      record_no_payload();
      return true;
    }
    return false;
  }

  template <typename TCallableDecl>
  bool ParseDispatchIntentCallableAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    auto record_duplicate = [&](const char *code, const std::string &message) {
      diagnostics_.push_back(MakeDiag(
          attribute_name.line, attribute_name.column, code, message));
      return false;
    };

    if (attribute_name.text == "objc_direct") {
      if (decl.objc_direct_declared) {
        return record_duplicate("O3P330", "duplicate objc_direct attribute");
      }
      decl.objc_direct_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_final") {
      if (decl.objc_final_declared) {
        return record_duplicate("O3P331", "duplicate objc_final attribute");
      }
      decl.objc_final_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_dynamic") {
      if (decl.objc_dynamic_declared) {
        return record_duplicate("O3P332", "duplicate objc_dynamic attribute");
      }
      decl.objc_dynamic_declared = true;
      return true;
    }
    return false;
  }

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

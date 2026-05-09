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

  std::string ParseAttributeArgumentText() {
    std::string text;
    while (!At(TokenKind::Eof) &&
           !At(TokenKind::Comma) &&
           !At(TokenKind::RParen)) {
      text += Advance().text;
    }
    return text;
  }

  bool ParseNamedStringAttributePayload(
      const Token &attribute_name,
      const char *attribute_spelling,
      std::string &named_value_out) {
    (void)attribute_name;
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P340",
          std::string("missing '(' after ") + attribute_spelling + " attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "named") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P341",
          std::string(attribute_spelling) + " requires named(\"...\") payload"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P342",
          std::string("missing '(' after ") + attribute_spelling + " named payload"));
      return false;
    }
    if (!At(TokenKind::String)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P343",
          std::string(attribute_spelling) + " named payload requires string literal"));
      return false;
    }
    named_value_out = Advance().text;
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P344",
          std::string("missing '))' after ") + attribute_spelling + " named payload"));
      return false;
    }
    return true;
  }

  template <typename TCallableDecl>
  bool ParseExecutorAttributePayload(
      TCallableDecl &decl,
      const Token &attribute_token) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P287",
          "missing '(' after objc_executor attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P288",
          "invalid objc_executor payload"));
      return false;
    }

    const Token kind = Advance();
    decl.executor_affinity_declared = true;
    decl.executor_affinity_kind = kind.text;
    decl.executor_affinity_named = false;
    decl.executor_affinity_name.clear();

    if (kind.text == "main" || kind.text == "global") {
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P289",
            "missing ')' after objc_executor payload"));
        return false;
      }
      return true;
    }

    if (kind.text != "named") {
      diagnostics_.push_back(MakeDiag(
          attribute_token.line, attribute_token.column, "O3P290",
          "unsupported objc_executor payload '" + kind.text + "'"));
      return false;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P291",
          "missing '(' after objc_executor named payload"));
      return false;
    }
    if (!At(TokenKind::String)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P292",
          "objc_executor named payload requires string literal"));
      return false;
    }
    decl.executor_affinity_name = Advance().text;
    decl.executor_affinity_named = true;
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P293",
          "missing '))' after objc_executor named payload"));
      return false;
    }
    return true;
  }

  template <typename TCallableDecl>
  bool ParseAbiAlignmentAttributePayload(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P361",
          "missing '(' after objc_abi_align attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "bytes") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P362",
          "objc_abi_align requires bytes: integer payload"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::Colon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P363",
          "missing ':' after objc_abi_align bytes label"));
      return false;
    }
    if (!At(TokenKind::Number)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P364",
          "objc_abi_align bytes payload requires integer literal"));
      return false;
    }
    const Token bytes_token = Advance();
    std::size_t alignment = 0;
    try {
      alignment =
          static_cast<std::size_t>(std::stoull(bytes_token.text, nullptr, 0));
    } catch (...) {
      diagnostics_.push_back(MakeDiag(
          bytes_token.line, bytes_token.column, "O3P364",
          "objc_abi_align bytes payload requires integer literal"));
      return false;
    }
    if (alignment == 0u || (alignment & (alignment - 1u)) != 0u) {
      diagnostics_.push_back(MakeDiag(
          bytes_token.line, bytes_token.column, "O3P365",
          "objc_abi_align bytes payload must be a non-zero power of two"));
      return false;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P366",
          "missing ')' after objc_abi_align payload"));
      return false;
    }
    decl.objc_abi_align_declared = true;
    decl.objc_abi_alignment_bytes = alignment;
    (void)attribute_name;
    return true;
  }

  template <typename TCallableDecl>
  bool ParseReturnsBorrowedAttributePayload(TCallableDecl &decl) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P296",
          "missing '(' after objc_returns_borrowed attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "owner_index") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P297",
          "objc_returns_borrowed requires owner_index clause"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P298",
          "missing '=' after owner_index in objc_returns_borrowed"));
      return false;
    }
    if (!At(TokenKind::Number)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P299",
          "objc_returns_borrowed owner_index requires integer literal"));
      return false;
    }
    decl.objc_returns_borrowed_owner_index =
        static_cast<std::size_t>(std::max(0, std::stoi(Advance().text)));
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P300",
          "missing ')' after objc_returns_borrowed payload"));
      return false;
    }
    decl.objc_returns_borrowed_declared = true;
    return true;
  }

  bool ParseLocalCleanupAttribute(LetStmt &stmt, const Token &) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P301",
          "missing '(' after cleanup"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P302",
          "cleanup requires cleanup function identifier"));
      return false;
    }
    stmt.cleanup_function_symbol = Advance().text;
    if (!Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P303",
          "missing ')))' after cleanup local annotation"));
      return false;
    }
    stmt.cleanup_attribute_declared = true;
    stmt.cleanup_profile_is_normalized = true;
    stmt.cleanup_profile =
        BuildCleanupAttributeProfile(true, stmt.cleanup_function_symbol);
    return true;
  }

  bool ParseLocalResourceAttribute(
      LetStmt &stmt,
      const Token &attribute_name) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P304",
          "missing '(' after objc_resource"));
      return false;
    }
    bool saw_close = false;
    bool saw_invalid = false;
    while (!At(TokenKind::Eof) && !At(TokenKind::RParen)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P305",
            "invalid objc_resource clause label"));
        return false;
      }
      const Token clause = Advance();
      if (!Match(TokenKind::Equal)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P306",
            "missing '=' after objc_resource clause label"));
        return false;
      }
      const std::string value_text = ParseAttributeArgumentText();
      if (value_text.empty()) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P307",
            "objc_resource clause value must not be empty"));
        return false;
      }
      if (clause.text == "close") {
        if (saw_close) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P308",
              "duplicate objc_resource close clause"));
          return false;
        }
        stmt.resource_close_symbol = value_text;
        saw_close = true;
      } else if (clause.text == "invalid") {
        if (saw_invalid) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P309",
              "duplicate objc_resource invalid clause"));
          return false;
        }
        stmt.resource_invalid_expression = value_text;
        saw_invalid = true;
      } else {
        diagnostics_.push_back(MakeDiag(
            clause.line, clause.column, "O3P310",
            "unsupported objc_resource clause '" + clause.text + "'"));
        return false;
      }
      if (At(TokenKind::Comma)) {
        Advance();
      } else {
        break;
      }
    }
    if (!Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P311",
          "missing ')))' after objc_resource local annotation"));
      return false;
    }
    if (!saw_close || !saw_invalid) {
      diagnostics_.push_back(MakeDiag(
          attribute_name.line, attribute_name.column, "O3P312",
          "objc_resource requires close and invalid clauses"));
      return false;
    }
    stmt.resource_attribute_declared = true;
    stmt.resource_profile_is_normalized = true;
    stmt.resource_profile = BuildResourceAttributeProfile(
        true, stmt.resource_close_symbol, stmt.resource_invalid_expression);
    return true;
  }

  template <typename TCallableDecl>
  bool ParseStatusCodeBridgeAttributePayload(
      TCallableDecl &decl,
      const Token &attribute_token) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P271",
          "missing '(' after objc_status_code attribute"));
      return false;
    }

    bool saw_success = false;
    bool saw_error_type = false;
    bool saw_mapping = false;
    while (!At(TokenKind::Eof) && !At(TokenKind::RParen)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P272",
            "invalid objc_status_code clause label"));
        return false;
      }
      const Token clause = Advance();
      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P273",
            "missing ':' after objc_status_code clause label"));
        return false;
      }
      const std::string value_text = ParseAttributeArgumentText();
      if (value_text.empty()) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P274",
            "objc_status_code clause value must not be empty"));
        return false;
      }

      if (clause.text == "success") {
        if (saw_success) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P275",
              "duplicate objc_status_code success clause"));
          return false;
        }
        decl.objc_status_code_success_literal = value_text;
        saw_success = true;
      } else if (clause.text == "error_type") {
        if (saw_error_type) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P276",
              "duplicate objc_status_code error_type clause"));
          return false;
        }
        decl.objc_status_code_error_type_spelling = value_text;
        saw_error_type = true;
      } else if (clause.text == "mapping") {
        if (saw_mapping) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P277",
              "duplicate objc_status_code mapping clause"));
          return false;
        }
        decl.objc_status_code_mapping_symbol = value_text;
        saw_mapping = true;
      } else {
        diagnostics_.push_back(MakeDiag(
            clause.line, clause.column, "O3P278",
            "unsupported objc_status_code clause '" + clause.text + "'"));
        return false;
      }

      if (At(TokenKind::Comma)) {
        Advance();
      } else {
        break;
      }
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P279",
          "missing ')' after objc_status_code attribute payload"));
      return false;
    }

    if (!saw_success || !saw_error_type || !saw_mapping) {
      diagnostics_.push_back(MakeDiag(
          attribute_token.line, attribute_token.column, "O3P280",
          "objc_status_code requires success, error_type, and mapping clauses"));
      return false;
    }

    decl.objc_status_code_declared = true;
    return true;
  }

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

  template <typename TCallableDecl>
  bool ParseSingleCallableBridgeAttribute(TCallableDecl &decl) {
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P281",
          "invalid callable attribute name"));
      return false;
    }
    const Token attribute_name = Advance();
    if (attribute_name.text == "objc_nserror") {
      if (decl.objc_nserror_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P282",
            "duplicate objc_nserror attribute"));
        return false;
      }
      decl.objc_nserror_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_status_code") {
      if (decl.objc_status_code_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P283",
            "duplicate objc_status_code attribute"));
        return false;
      }
      return ParseStatusCodeBridgeAttributePayload(decl, attribute_name);
    }
    if (attribute_name.text == "objc_executor") {
      if (decl.executor_affinity_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P294",
            "duplicate objc_executor attribute"));
        return false;
      }
      return ParseExecutorAttributePayload(decl, attribute_name);
    }
    if (attribute_name.text == "objc_macro") {
      if (decl.objc_macro_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P346",
            "duplicate objc_macro attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro", decl.objc_macro_name)) {
        return false;
      }
      decl.objc_macro_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_package") {
      if (decl.objc_macro_package_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P347",
            "duplicate objc_macro_package attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_package",
              decl.objc_macro_package_name)) {
        return false;
      }
      decl.objc_macro_package_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_provenance") {
      if (decl.objc_macro_provenance_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P348",
            "duplicate objc_macro_provenance attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_provenance",
              decl.objc_macro_provenance_name)) {
        return false;
      }
      decl.objc_macro_provenance_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_cache_key") {
      if (decl.objc_macro_cache_key_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P367",
            "duplicate objc_macro_cache_key attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_cache_key",
              decl.objc_macro_cache_key_name)) {
        return false;
      }
      decl.objc_macro_cache_key_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_sandbox") {
      if (decl.objc_macro_sandbox_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P368",
            "duplicate objc_macro_sandbox attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_sandbox",
              decl.objc_macro_sandbox_name)) {
        return false;
      }
      decl.objc_macro_sandbox_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_foreign") {
      if (decl.objc_foreign_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P349",
            "duplicate objc_foreign attribute"));
        return false;
      }
      decl.objc_foreign_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_import_module") {
      if (decl.objc_import_module_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P350",
            "duplicate objc_import_module attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_import_module",
              decl.objc_import_module_name)) {
        return false;
      }
      decl.objc_import_module_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_swift_name") {
      if (decl.objc_swift_name_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P351",
            "duplicate objc_swift_name attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_swift_name", decl.objc_swift_name)) {
        return false;
      }
      decl.objc_swift_name_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_swift_private") {
      if (decl.objc_swift_private_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P352",
            "duplicate objc_swift_private attribute"));
        return false;
      }
      decl.objc_swift_private_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_cxx_name") {
      if (decl.objc_cxx_name_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P353",
            "duplicate objc_cxx_name attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_cxx_name", decl.objc_cxx_name)) {
        return false;
      }
      decl.objc_cxx_name_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_header_name") {
      if (decl.objc_header_name_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P354",
            "duplicate objc_header_name attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_header_name", decl.objc_header_name)) {
        return false;
      }
      decl.objc_header_name_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_export_header") {
      if (decl.objc_export_header_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P356",
            "duplicate objc_export_header attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_export_header",
              decl.objc_export_header_name)) {
        return false;
      }
      decl.objc_export_header_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_abi_align") {
      if (decl.objc_abi_align_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P357",
            "duplicate objc_abi_align attribute"));
        return false;
      }
      return ParseAbiAlignmentAttributePayload(decl, attribute_name);
    }
    if (attribute_name.text == "objc_foreign_type") {
      if (decl.objc_foreign_type_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P358",
            "duplicate objc_foreign_type attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_foreign_type",
              decl.objc_foreign_type_name)) {
        return false;
      }
      decl.objc_foreign_type_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_mixed_image") {
      if (decl.objc_mixed_image_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P359",
            "duplicate objc_mixed_image attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_mixed_image", decl.objc_mixed_image_name)) {
        return false;
      }
      decl.objc_mixed_image_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_package_entry") {
      if (decl.objc_package_entry_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P360",
            "duplicate objc_package_entry attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_package_entry",
              decl.objc_package_entry_name)) {
        return false;
      }
      decl.objc_package_entry_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_nonisolated") {
      if (decl.objc_nonisolated_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P295",
            "duplicate objc_nonisolated attribute"));
        return false;
      }
      decl.objc_nonisolated_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_returns_borrowed") {
      if (decl.objc_returns_borrowed_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P312",
            "duplicate objc_returns_borrowed attribute"));
        return false;
      }
      return ParseReturnsBorrowedAttributePayload(decl);
    }
    if (ParseRetainableCFamilyCallableAttribute(decl, attribute_name)) {
      return true;
    }
    if (ParseDispatchIntentCallableAttribute(decl, attribute_name)) {
      return true;
    }

    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, "O3P284",
        "unsupported callable attribute '" + attribute_name.text + "'"));
    return false;
  }

  template <typename TCallableDecl>
  bool ParseOptionalCallableBridgeAttributesImpl(TCallableDecl &decl) {
    while (AtIdentifierText("__attribute__")) {
      Advance();
      if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P285",
            "malformed __attribute__ callable annotation"));
        return false;
      }

      do {
        if (!ParseSingleCallableBridgeAttribute(decl)) {
          return false;
        }
      } while (Match(TokenKind::Comma));

      if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P286",
            "missing '))' after callable attribute list"));
        return false;
      }
    }
    return true;
  }

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

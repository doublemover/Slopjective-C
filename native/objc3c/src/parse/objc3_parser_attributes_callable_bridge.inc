  enum class CallableBridgeAttributeResult {
    kNotMatched,
    kParsed,
    kFailed,
  };

#include "parse/objc3_parser_attributes_callable_bridge_core.inc"

#include "parse/objc3_parser_attributes_callable_bridge_macro.inc"

#include "parse/objc3_parser_attributes_callable_bridge_interop.inc"

#include "parse/objc3_parser_attributes_callable_bridge_abi.inc"

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
    const auto core_result =
        ParseCoreCallableBridgeAttribute(decl, attribute_name);
    if (core_result != CallableBridgeAttributeResult::kNotMatched) {
      return core_result == CallableBridgeAttributeResult::kParsed;
    }

    const auto macro_result =
        ParseMacroCallableBridgeAttribute(decl, attribute_name);
    if (macro_result != CallableBridgeAttributeResult::kNotMatched) {
      return macro_result == CallableBridgeAttributeResult::kParsed;
    }

    const auto interop_result =
        ParseInteropCallableBridgeAttribute(decl, attribute_name);
    if (interop_result != CallableBridgeAttributeResult::kNotMatched) {
      return interop_result == CallableBridgeAttributeResult::kParsed;
    }

    const auto abi_result = ParseAbiCallableBridgeAttribute(decl, attribute_name);
    if (abi_result != CallableBridgeAttributeResult::kNotMatched) {
      return abi_result == CallableBridgeAttributeResult::kParsed;
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

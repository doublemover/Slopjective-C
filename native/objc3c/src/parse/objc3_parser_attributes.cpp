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

#include "parse/objc3_parser_attributes_container_dispatch_optional.inc"

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

#include "parse/objc3_parser_attributes_container_dispatch_single.inc"

#include "parse/objc3_parser_attributes_callable_bridge.inc"

  const std::vector<Token> &tokens_;
  std::size_t &index_;
  std::vector<std::string> &diagnostics_;
};

}  // namespace

#include "parse/objc3_parser_attributes_entrypoints.inc"

}  // namespace objc3c::parse

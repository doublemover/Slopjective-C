#include "parse/objc3_parser_token_metadata.h"

namespace objc3c::parse {

Objc3SemaTokenMetadata MakeObjc3ParserSemaTokenMetadata(
    Objc3SemaTokenKind kind,
    const Objc3LexToken &token) {
  return MakeObjc3SemaTokenMetadata(kind, token.text, token.line, token.column);
}

}  // namespace objc3c::parse

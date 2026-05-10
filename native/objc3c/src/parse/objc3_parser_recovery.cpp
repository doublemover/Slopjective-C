#include "parse/objc3_parser_recovery.h"

#include "parse/objc3_parser_cursor.h"
#include "parse/objc3_parser_recovery_boundaries.h"
#include "parse/objc3_parser_statement_surface.h"

namespace objc3c::parse {
namespace {

using TokenKind = Objc3LexTokenKind;

#include "parse/objc3_parser_recovery_token_helpers.inc"

}  // namespace

#include "parse/objc3_parser_recovery_top_level_synchronization.inc"
#include "parse/objc3_parser_recovery_function_tail_synchronization.inc"
#include "parse/objc3_parser_recovery_statement_synchronization.inc"

}  // namespace objc3c::parse

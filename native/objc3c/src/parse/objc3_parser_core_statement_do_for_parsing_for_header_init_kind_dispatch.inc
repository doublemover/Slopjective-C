        if (Match(TokenKind::KwLet)) {
#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_init_local_binding.inc"
        } else if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_init_assignment.inc"
        } else if (AtPrefixUpdate()) {
#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_init_prefix_update.inc"
        } else {
#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_init_expression.inc"
        }

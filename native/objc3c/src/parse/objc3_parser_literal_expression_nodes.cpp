#include "parse/objc3_parser_expression_nodes.h"

#include <utility>

#include "parse/objc3_parser_profile_helpers.h"

namespace objc3c::parse {

#include "parse/objc3_parser_literal_expression_nodes_scalar_literals.inc"
#include "parse/objc3_parser_literal_expression_nodes_identifier_literals.inc"
#include "parse/objc3_parser_literal_expression_nodes_typed_keypath_literals.inc"

}  // namespace objc3c::parse

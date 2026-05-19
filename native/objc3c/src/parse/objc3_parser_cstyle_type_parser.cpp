#include "parse/objc3_parser_cstyle_type_parser.h"

#include "diag/objc3_diag_utils.h"
#include "parse/objc3_parser_cursor.h"
#include "parse/objc3_parser_cstyle_type_classifier.h"
#include "parse/objc3_parser_declaration_surface.h"
#include "parse/objc3_parser_diagnostics.h"

namespace objc3c::parse {

#include "parse/objc3_parser_cstyle_type_parser_diagnostics.inc"
#include "parse/objc3_parser_cstyle_type_parser_spelling.inc"
#include "parse/objc3_parser_cstyle_type_parser_declarator.inc"
#include "parse/objc3_parser_cstyle_type_parser_entrypoints.inc"

}  // namespace objc3c::parse

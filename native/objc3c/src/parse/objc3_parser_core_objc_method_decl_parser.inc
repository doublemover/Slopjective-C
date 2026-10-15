#include "parse/objc3_parser_core_objc_method_decl_parser_signature_header.inc"
#include "parse/objc3_parser_core_objc_method_decl_parser_selector_assembly.inc"
#include "parse/objc3_parser_core_objc_method_decl_parser_trailing_attributes.inc"
#include "parse/objc3_parser_core_objc_method_decl_parser_finalization.inc"

  bool ParseObjcMethodDecl(
      Objc3MethodDecl &method,
      bool allow_body,
      bool reification_marker_declared = false,
      unsigned reification_marker_line = 1,
      unsigned reification_marker_column = 1) {
    if (!ParseObjcMethodMarker(method)) {
      return false;
    }
    method.generic_callable_reification_marker_declared =
        reification_marker_declared;
    method.generic_callable_reification_marker_line = reification_marker_line;
    method.generic_callable_reification_marker_column =
        reification_marker_column;

    if (!ParseObjcMethodReturnHeader(method)) {
      return false;
    }

    if (!ParseObjcMethodSelector(method)) {
      return false;
    }

    if (!ParseObjcMethodTrailingAttributes(method)) {
      return false;
    }

    return ParseObjcMethodTerminatorOrBody(method, allow_body);
  }

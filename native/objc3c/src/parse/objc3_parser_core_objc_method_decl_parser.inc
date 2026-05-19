#include "parse/objc3_parser_core_objc_method_decl_parser_signature_header.inc"
#include "parse/objc3_parser_core_objc_method_decl_parser_selector_assembly.inc"
#include "parse/objc3_parser_core_objc_method_decl_parser_trailing_attributes.inc"
#include "parse/objc3_parser_core_objc_method_decl_parser_finalization.inc"

  bool ParseObjcMethodDecl(Objc3MethodDecl &method, bool allow_body) {
    if (!ParseObjcMethodMarker(method)) {
      return false;
    }

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

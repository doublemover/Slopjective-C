#include "parse/objc3_parser_core_objc_declarations_function_return_type_parser_diagnostics_recovery.inc"
#include "parse/objc3_parser_core_objc_declarations_function_return_type_parser_qualifier_parsing.inc"
#include "parse/objc3_parser_core_objc_declarations_function_return_type_parser_spelling_capture.inc"
#include "parse/objc3_parser_core_objc_declarations_function_return_type_parser_profile_finalization.inc"

  bool ParseFunctionReturnType(FunctionDecl &fn) {
    ResetFunctionReturnTypeState(fn);

    if (!ParseFunctionReturnTypeQualifiers(fn)) {
      return false;
    }

    if (!ParseFunctionReturnTypeSpelling(fn)) {
      return false;
    }

    return FinalizeFunctionReturnTypeParsing(fn);
  }

#include "parse/objc3_parser_core_objc_declarations_function_return_suffixes_generic.inc"
#include "parse/objc3_parser_core_objc_declarations_function_return_suffixes_pointer_nullability.inc"
#include "parse/objc3_parser_core_objc_declarations_function_return_suffixes_ownership.inc"

  bool ParseFunctionReturnTypeSuffix(FunctionDecl &fn) {
    bool parsed_generic_suffix = false;
    while (true) {
      bool generic_suffix_failed = false;
      if (ParseFunctionReturnGenericSuffix(
              fn, parsed_generic_suffix, generic_suffix_failed)) {
        if (generic_suffix_failed) {
          return false;
        }
        parsed_generic_suffix = true;
        continue;
      }

      if (ParseFunctionReturnPointerDeclaratorSuffix(fn)) {
        continue;
      }

      if (ParseFunctionReturnNullabilitySuffix(fn)) {
        continue;
      }

      if (ParseFunctionReturnOwnershipQualifierSuffix(fn)) {
        continue;
      }

      break;
    }
    return true;
  }

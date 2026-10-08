  bool ParseParameterType(FuncParam &param) {
#include "parse/objc3_parser_core_objc_declarations_parameter_type_reset.inc"
#include "parse/objc3_parser_core_objc_declarations_parameter_type_qualifiers.inc"
#include "parse/objc3_parser_core_objc_declarations_parameter_type_spelling.inc"

    param.typecheck_family_symbol = BuildObjcTypecheckParamFamilySymbol(param);

    ParseParameterTypeSuffix(param);
    if (!FinalizeParameterValueOptionalType(param)) {
      return false;
    }
    if (!param.generic_suffix_terminated) {
      return false;
    }

#include "parse/objc3_parser_core_objc_declarations_parameter_type_ownership_profiles.inc"
#include "parse/objc3_parser_core_objc_declarations_parameter_type_semantic_profiles.inc"
#include "parse/objc3_parser_core_objc_declarations_parameter_type_borrowed_profile.inc"

    return true;
  }

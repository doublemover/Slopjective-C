#include "parse/objc3_parser_cstyle_type_parser.h"

#include "diag/objc3_diag_utils.h"
#include "parse/objc3_parser_cursor.h"
#include "parse/objc3_parser_cstyle_type_classifier.h"
#include "parse/objc3_parser_declaration_surface.h"
#include "parse/objc3_parser_diagnostics.h"

namespace objc3c::parse {

Objc3CStyleFunctionTypeParseResult ParseObjc3CStyleFunctionType(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    bool allow_void,
    unsigned diag_line,
    unsigned diag_column,
    const char *diagnostic_context,
    std::vector<std::string> &diagnostics) {
  Objc3CStyleFunctionTypeParseResult result;

  if (IsObjc3RemovedOptionalTemplateAliasLead(tokens, index)) {
    diagnostics.push_back(
        BuildObjc3RemovedOptionalTemplateAliasDiagnostic(PeekToken(tokens, index)));
    return result;
  }

  Objc3CStyleTypeTokenClassification classification;
  if (ClassifyObjc3CStyleBuiltinTypeToken(PeekToken(tokens, index), classification)) {
    result.type = classification.type;
    result.id_spelling = classification.id_spelling;
    result.class_spelling = classification.class_spelling;
    result.sel_spelling = classification.sel_spelling;
    result.instancetype_spelling = classification.instancetype_spelling;
    AdvanceToken(tokens, index);
  } else if (AtTokenKind(tokens, index, Objc3LexTokenKind::Identifier)) {
    const Objc3LexToken object_name = AdvanceToken(tokens, index);
    result.type = ValueType::ObjCObjectPtr;
    result.object_pointer_spelling = true;
    result.object_pointer_name = object_name.text;
  } else {
    diagnostics.push_back(MakeDiag(
        diag_line,
        diag_column,
        "O3P114",
        std::string("expected ") + diagnostic_context + " type"));
    return result;
  }

  while (MatchTokenKind(tokens, index, Objc3LexTokenKind::Star)) {
    ++result.pointer_depth;
  }

  if (result.pointer_depth > 0u) {
    if (result.type == ValueType::Void) {
      result.type = ValueType::ObjCObjectPtr;
      result.object_pointer_spelling = true;
      if (result.object_pointer_name.empty()) {
        result.object_pointer_name = "void";
      }
    } else if (result.type == ValueType::I32 || result.type == ValueType::Bool) {
      const char *pointer_type = result.type == ValueType::I32 ? "i32*" : "bool*";
      diagnostics.push_back(MakeDiag(
          diag_line,
          diag_column,
          "O3P114",
          std::string("unsupported pointer type in C-style declaration: unsupported pointer type '") +
              pointer_type + "' in C-style declaration"));
      return result;
    }
    if (!result.object_pointer_spelling &&
        !result.id_spelling &&
        !result.class_spelling &&
        !result.sel_spelling &&
        !result.instancetype_spelling) {
      result.object_pointer_spelling = true;
    }
    if (result.object_pointer_name.empty() &&
        AtTokenKind(tokens, index, Objc3LexTokenKind::Identifier)) {
      result.object_pointer_name = PeekToken(tokens, index).text;
    }
  }

  if (!allow_void && result.type == ValueType::Void) {
    diagnostics.push_back(MakeDiag(
        diag_line,
        diag_column,
        "O3P108",
        "void is only allowed as '(void)' in C-style parameter lists"));
    return result;
  }

  result.ok = true;
  return result;
}

}  // namespace objc3c::parse

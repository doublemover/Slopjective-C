#include "parse/objc3_parser_core_objc_declarations_function_entrypoint_entry_classification.inc"
#include "parse/objc3_parser_core_objc_declarations_function_entrypoint_diagnostics_recovery.inc"
#include "parse/objc3_parser_core_objc_declarations_function_entrypoint_callable_setup.inc"
#include "parse/objc3_parser_core_objc_declarations_function_entrypoint_parser_handoff.inc"
#include "parse/objc3_parser_core_objc_declarations_function_entrypoint_final_publication.inc"

  std::unique_ptr<FunctionDecl> ParseFunction() {
    auto fn = std::make_unique<FunctionDecl>();
    if (!RejectUnexpectedFunctionDeclarationQualifier("after 'fn'")) {
      return nullptr;
    }

    if (!ParseFunctionNameAndScope(*fn)) {
      return nullptr;
    }

    if (!ParseFunctionGenericParameterClause(*fn)) {
      return nullptr;
    }

    if (!ParseFunctionParameterList(*fn)) {
      return nullptr;
    }

    bool has_return_annotation = false;
    if (!ParseFunctionCallableTail(*fn, has_return_annotation)) {
      return nullptr;
    }

    return ParseFunctionTerminatorOrBody(std::move(fn), has_return_annotation);
  }

#include "parse/objc3_parser_core_block_literal_capture_list.inc"
#include "parse/objc3_parser_core_block_literal_signature_parameters.inc"
#include "parse/objc3_parser_core_block_literal_capture_body.inc"
#include "parse/objc3_parser_core_block_literal_storage_ownership.inc"
#include "parse/objc3_parser_core_block_literal_final_assembly.inc"

  std::unique_ptr<Expr> ParseBlockLiteralExpression() {
    const Token caret = Previous();
    auto block = std::make_unique<Expr>();
    block->kind = Expr::Kind::BlockLiteral;
    block->line = caret.line;
    block->column = caret.column;

    if (!ParseBlockLiteralExplicitCaptureList(*block)) {
      return nullptr;
    }

    std::vector<Objc3BlockParameterSourceModel> parameters;
    if (!ParseBlockLiteralParameterList(parameters)) {
      return nullptr;
    }

    if (!RequireBlockLiteralBodyStart()) {
      return nullptr;
    }

    auto body = ParseBlock();
    std::vector<std::string> parameter_names;
    bool deterministic_capture_set = true;
    ApplyBlockLiteralSignatureModel(*block, parameters, parameter_names);
    ApplyBlockLiteralCaptureBodyModel(
        *block, body, parameter_names, deterministic_capture_set);
    ApplyBlockLiteralInvokeAbiAssembly(*block);
    ApplyBlockLiteralStorageOwnershipAnnotations(*block);
    ApplyBlockLiteralFinalAssembly(*block);
    block->block_body = std::move(body);
    return block;
  }

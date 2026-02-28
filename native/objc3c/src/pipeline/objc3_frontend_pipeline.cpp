#include "pipeline/objc3_frontend_pipeline.h"

#include <unordered_map>
#include <utility>
#include <vector>

#include "lex/objc3_lexer.h"
#include "parse/objc3_ast_builder_contract.h"
#include "sema/objc3_semantic_passes.h"

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  Objc3Lexer lexer(source);
  std::vector<Objc3LexToken> tokens = lexer.Run(result.stage_diagnostics.lexer);

  Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);
  result.program = std::move(parse_result.program);
  result.stage_diagnostics.parser = std::move(parse_result.diagnostics);
  Objc3Program &program_ast = MutableObjc3ParsedProgramAst(result.program);

  if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {
    result.integration_surface = BuildSemanticIntegrationSurface(result.program, result.stage_diagnostics.semantic);
    Objc3SemanticValidationOptions semantic_options;
    semantic_options.max_message_send_args = options.lowering.max_message_send_args;
    ValidateSemanticBodies(result.program, result.integration_surface, semantic_options,
                           result.stage_diagnostics.semantic);
    ValidatePureContractSemanticDiagnostics(result.program, result.integration_surface.functions,
                                            result.stage_diagnostics.semantic);
  }

  program_ast.diagnostics.reserve(result.stage_diagnostics.lexer.size() + result.stage_diagnostics.parser.size() +
                                  result.stage_diagnostics.semantic.size());
  program_ast.diagnostics.insert(program_ast.diagnostics.end(), result.stage_diagnostics.lexer.begin(),
                                 result.stage_diagnostics.lexer.end());
  program_ast.diagnostics.insert(program_ast.diagnostics.end(), result.stage_diagnostics.parser.begin(),
                                 result.stage_diagnostics.parser.end());
  program_ast.diagnostics.insert(program_ast.diagnostics.end(), result.stage_diagnostics.semantic.begin(),
                                 result.stage_diagnostics.semantic.end());
  return result;
}

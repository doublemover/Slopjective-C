#include "pipeline/objc3_frontend_pipeline.h"

#include <unordered_map>
#include <utility>
#include <vector>

#include "lex/objc3_lexer.h"
#include "parse/objc3_ast_builder_contract.h"
#include "sema/objc3_sema_pass_manager.h"

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  Objc3LexerOptions lexer_options;
  lexer_options.language_version = options.language_version;
  lexer_options.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy
                                         ? Objc3LexerCompatibilityMode::kLegacy
                                         : Objc3LexerCompatibilityMode::kCanonical;
  lexer_options.migration_assist = options.migration_assist;
  Objc3Lexer lexer(source, lexer_options);
  std::vector<Objc3LexToken> tokens = lexer.Run(result.stage_diagnostics.lexer);
  const Objc3LexerMigrationHints &lexer_hints = lexer.MigrationHints();
  result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;
  result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;
  result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;

  Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);
  result.program = std::move(parse_result.program);
  result.stage_diagnostics.parser = std::move(parse_result.diagnostics);

  if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {
    Objc3SemanticValidationOptions semantic_options;
    semantic_options.max_message_send_args = options.lowering.max_message_send_args;

    Objc3SemaPassManagerInput sema_input;
    sema_input.program = &result.program;
    sema_input.validation_options = semantic_options;
    sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;

    Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);
    result.integration_surface = std::move(sema_result.integration_surface);
    result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;
    result.sema_parity_surface = sema_result.parity_surface;
    if (result.stage_diagnostics.semantic.empty() && !sema_result.diagnostics.empty()) {
      result.stage_diagnostics.semantic = std::move(sema_result.diagnostics);
    }
  }

  TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);
  return result;
}

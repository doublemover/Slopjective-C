#include "pipeline/frontend_pipeline_stage_runner.h"

#include <utility>

#include "lex/objc3_lexer.h"
#include "parse/objc3_ast_builder_contract.h"
#include "parse/objc3_diagnostics_bus.h"

namespace {

void CopyLanguageVersionPragmaContract(
    const Objc3LexerLanguageVersionPragmaContract &source,
    Objc3FrontendLanguageVersionPragmaContract &target) {
  target.seen = source.seen;
  target.directive_count = source.directive_count;
  target.duplicate = source.duplicate;
  target.non_leading = source.non_leading;
  target.first_line = source.first_line;
  target.first_column = source.first_column;
  target.last_line = source.last_line;
  target.last_column = source.last_column;
}

void CopyNamedIdentifierPragmaContract(
    const Objc3LexerNamedIdentifierPragmaContract &source,
    Objc3FrontendNamedIdentifierPragmaContract &target) {
  target.seen = source.seen;
  target.directive_count = source.directive_count;
  target.duplicate = source.duplicate;
  target.non_leading = source.non_leading;
  target.first_line = source.first_line;
  target.first_column = source.first_column;
  target.last_line = source.last_line;
  target.last_column = source.last_column;
  target.identifier = source.identifier;
}

void CopyBootstrapRegistrationSourceContract(
    const Objc3LexerBootstrapRegistrationSourceContract &source,
    Objc3FrontendBootstrapRegistrationSourcePragmaContract &target) {
  CopyNamedIdentifierPragmaContract(source.registration_descriptor,
                                    target.registration_descriptor);
  CopyNamedIdentifierPragmaContract(source.image_root, target.image_root);
}

}  // namespace

std::vector<Objc3LexToken> RunObjc3FrontendLexStage(
    const std::string &source, const Objc3FrontendOptions &options,
    Objc3FrontendPipelineResult &result) {
  Objc3LexerOptions lexer_options;
  lexer_options.language_version = options.language_version;
  lexer_options.language_profile = Objc3LexerLanguageProfile::kCanonical;
  Objc3Lexer lexer(source, lexer_options);
  std::vector<Objc3LexToken> tokens =
      lexer.Run(result.stage_diagnostics.lexer);

  const Objc3LexerMigrationHints &lexer_hints = lexer.MigrationHints();
  result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;
  result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;
  result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;

  CopyLanguageVersionPragmaContract(lexer.LanguageVersionPragmaContract(),
                                    result.language_version_pragma_contract);
  CopyBootstrapRegistrationSourceContract(
      lexer.BootstrapRegistrationSourceContract(),
      result.bootstrap_registration_source_pragma_contract);
  return tokens;
}

void RunObjc3FrontendParseStageIfLexClean(
    const std::vector<Objc3LexToken> &tokens,
    Objc3FrontendPipelineResult &result) {
  if (!result.stage_diagnostics.lexer.empty()) {
    return;
  }
  Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);
  result.program = std::move(parse_result.program);
  result.stage_diagnostics.parser = std::move(parse_result.diagnostics);
  result.parser_contract_snapshot = parse_result.contract_snapshot;
}

void TransportObjc3FrontendPipelineDiagnostics(
    Objc3FrontendPipelineResult &result) {
  TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics,
                                           result.program);
}

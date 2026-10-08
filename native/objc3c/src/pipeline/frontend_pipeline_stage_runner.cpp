#include "pipeline/frontend_pipeline_stage_runner.h"

#include <utility>

#include "lex/objc3_lexer.h"
#include "pipeline/frontend_pipeline_result_handoff.h"
#include "parse/objc3_ast_builder_contract.h"
#include "parse/objc3_diagnostics_bus.h"
#include "pipeline/frontend_pipeline_pragma_contracts.h"

namespace {

Objc3LexerLanguageProfile Objc3LexerLanguageProfileFromFrontendProfile(
    Objc3FrontendLanguageProfile profile) {
  switch (profile) {
    case Objc3FrontendLanguageProfile::kCanonical:
      return Objc3LexerLanguageProfile::kCanonical;
    case Objc3FrontendLanguageProfile::kStrict:
      return Objc3LexerLanguageProfile::kStrict;
    case Objc3FrontendLanguageProfile::kStrictConcurrency:
      return Objc3LexerLanguageProfile::kStrictConcurrency;
  }
  return Objc3LexerLanguageProfile::kCanonical;
}

}  // namespace

std::vector<Objc3LexToken> RunObjc3FrontendLexStage(
    const std::string &source, const Objc3FrontendOptions &options,
    Objc3FrontendPipelineResult &result) {
  Objc3LexerOptions lexer_options;
  lexer_options.language_version = options.language_version;
  lexer_options.language_profile =
      Objc3LexerLanguageProfileFromFrontendProfile(options.language_profile);
  Objc3Lexer lexer(source, lexer_options);
  std::vector<Objc3LexToken> tokens =
      lexer.Run(result.stage_diagnostics.lexer);

  CaptureObjc3FrontendCanonicalLiteralRejections(
      result, lexer.CanonicalLiteralRejectionCounts());

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

#include "pipeline/frontend_pipeline_stage_sequence.h"

#include "pipeline/frontend_pipeline_stage_runner.h"

std::vector<Objc3LexToken> RunObjc3FrontendLexParseStageSequence(
    const std::string &source,
    const Objc3FrontendOptions &options,
    Objc3FrontendPipelineResult &result) {
  std::vector<Objc3LexToken> tokens =
      RunObjc3FrontendLexStage(source, options, result);
  RunObjc3FrontendParseStageIfLexClean(tokens, result);
  return tokens;
}

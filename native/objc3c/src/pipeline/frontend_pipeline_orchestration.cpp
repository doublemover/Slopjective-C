#include "pipeline/objc3_frontend_pipeline.h"

#include <utility>
#include <vector>

#include "pipeline/frontend_pipeline_orchestration_owners.h"
#include "pipeline/frontend_pipeline_result_handoff.h"
#include "pipeline/frontend_pipeline_sema_stage_runner.h"
#include "pipeline/frontend_pipeline_stage_runner.h"
#include "pipeline/frontend_pipeline_stage_sequence.h"

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(
    const std::string &source,
    const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  std::vector<Objc3LexToken> tokens =
      RunObjc3FrontendLexParseStageSequence(source, options, result);
  objc3_frontend_pipeline_orchestration::PopulateSourceReadinessSummaries(
      result, options, tokens);
  objc3_frontend_pipeline_orchestration::RefreshProtocolSymbolReadiness(result);

  const bool allow_error_handling_error_runtime_surface = true;
  if (ShouldRunObjc3FrontendSemaStage(result)) {
    Objc3SemaPassManagerResult sema_result = RunObjc3FrontendSemaStage(
        result, options, allow_error_handling_error_runtime_surface);
    AdoptObjc3FrontendSemaResult(result, std::move(sema_result));
    objc3_frontend_pipeline_orchestration::RefreshProtocolSymbolReadiness(result);
  }

  objc3_frontend_pipeline_orchestration::PopulateSemanticModelSummaries(
      result, allow_error_handling_error_runtime_surface);
  objc3_frontend_pipeline_orchestration::PopulateRuntimeExportReadiness(
      result, options);
  objc3_frontend_pipeline_orchestration::PublishRuntimeExportDiagnostics(result);
  objc3_frontend_pipeline_orchestration::PublishTerminalPhaseResults(result,
                                                                     options);
  TransportObjc3FrontendPipelineDiagnostics(result);
  return result;
}

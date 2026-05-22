#include "pipeline/objc3_frontend_pipeline.h"

#include <utility>
#include <vector>

#include "pipeline/frontend_pipeline_orchestration_owners.h"
#include "pipeline/frontend_pipeline_result_handoff.h"
#include "pipeline/frontend_pipeline_sema_stage_runner.h"
#include "pipeline/frontend_pipeline_stage_runner.h"
#include "pipeline/frontend_pipeline_stage_sequence.h"

namespace {

bool ShouldEnableObjc3NativeErrorRuntimeSurface(
    const Objc3FrontendOptions &options) {
  return options.allow_live_error_runtime_surface || options.emit_ir ||
         options.emit_object;
}

}  // namespace

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(
    const std::string &source,
    const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;
  Objc3FrontendOptions effective_options = options;
  effective_options.allow_live_error_runtime_surface =
      ShouldEnableObjc3NativeErrorRuntimeSurface(options);

  std::vector<Objc3LexToken> tokens =
      RunObjc3FrontendLexParseStageSequence(source, effective_options, result);
  objc3_frontend_pipeline_orchestration::PopulateSourceReadinessSummaries(
      result, effective_options, tokens);
  objc3_frontend_pipeline_orchestration::RefreshProtocolSymbolReadiness(result);

  const bool allow_error_handling_error_runtime_surface =
      effective_options.allow_live_error_runtime_surface;
  if (ShouldRunObjc3FrontendSemaStage(result)) {
    Objc3SemaPassManagerResult sema_result = RunObjc3FrontendSemaStage(
        result, effective_options, allow_error_handling_error_runtime_surface);
    AdoptObjc3FrontendSemaResult(result, std::move(sema_result));
    objc3_frontend_pipeline_orchestration::RefreshProtocolSymbolReadiness(result);
  }

  objc3_frontend_pipeline_orchestration::PopulateSemanticModelSummaries(
      result, allow_error_handling_error_runtime_surface);
  objc3_frontend_pipeline_orchestration::PopulateRuntimeExportReadiness(
      result, effective_options);
  objc3_frontend_pipeline_orchestration::PublishRuntimeExportDiagnostics(result);
  objc3_frontend_pipeline_orchestration::PublishTerminalPhaseResults(result,
                                                                     effective_options);
  TransportObjc3FrontendPipelineDiagnostics(result);
  return result;
}

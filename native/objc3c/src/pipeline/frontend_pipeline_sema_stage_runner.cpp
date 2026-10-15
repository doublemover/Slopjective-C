#include "pipeline/frontend_pipeline_sema_stage_runner.h"

#include <utility>

#include "parse/objc3_parse_support.h"
#include "pipeline/dispatch_surface_classification.h"
#include "pipeline/frontend_pipeline_result_handoff.h"
#include "sema/objc3_sema_pass_manager.h"

bool ShouldRunObjc3FrontendSemaStage(
    const Objc3FrontendPipelineResult &result) {
  return result.stage_diagnostics.lexer.empty() &&
         result.stage_diagnostics.parser.empty();
}

Objc3SemaPassManagerResult RunObjc3FrontendSemaStage(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options,
    bool allow_error_handling_error_runtime_surface) {
  NormalizeProgramDispatchSurfaceClassification(
      MutableObjc3ParsedProgramAst(result.program));

  Objc3SemanticValidationOptions semantic_options;
  semantic_options.max_message_send_args = options.lowering.max_message_send_args;
  // Source-only frontend runs may admit block literals through sema, but
  // native emit paths must fail closed until runnable block lowering lands.
  semantic_options.allow_source_only_block_literals =
      !options.emit_ir && !options.emit_object;
  // Defer statements are admitted for native validation because executable
  // cleanup insertion is owned downstream in the IR path.
  semantic_options.allow_source_only_defer_statements = true;
  semantic_options.allow_source_only_error_runtime_surface =
      allow_error_handling_error_runtime_surface;
  semantic_options.arc_mode_enabled =
      options.arc_mode == Objc3FrontendArcMode::kEnabled;

  Objc3SemaPassManagerInput sema_input;
  PopulateObjc3FrontendSemaInputHandoff(
      result, options, semantic_options, sema_input);

  Objc3SemaPassManagerResult sema_result =
      RunObjc3SemaPassManager(sema_input);
  if (result.stage_diagnostics.semantic.empty() &&
      !sema_result.diagnostics.empty()) {
    result.stage_diagnostics.semantic = std::move(sema_result.diagnostics);
  }
  return sema_result;
}
